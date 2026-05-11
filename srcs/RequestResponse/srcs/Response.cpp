#include "HttpExceptions.hpp"
#include "AutoIndex.hpp"
#include "CgiManager.hpp"
#include "Client.hpp"
#include <fstream>
#include <sys/stat.h>

void Response::setRequest(Request &req)
{
	_request = &req;
	_isCgi = false;
	_finalAutoIndex = false;
	setMimes();
	setErrorPages();
}

void Response::buildErrorHeader()
{
	_header = buildHeader(_body.size(), _statusCode, _statusText);
}

std::string Response::getErrorPageContent(int code,
										  const ServerConfig &config)
{
	for (size_t i = 0; i < config.error_page.size(); i++)
	{
		if (config.error_page[i].index == code)
		{
			std::string path;
			path = config.root + "/" + config.error_page[i].path;
			struct stat st;

			if (stat(path.c_str(), &st) == 0 && (st.st_mode & S_IRUSR))
				return readFile(path);
		}
	}

	std::map<int, std::string>::iterator it = _errorPages.find(code);
	if (it != _errorPages.end())
		return it->second;

	return _errorPages[404];
}

std::string Response::readFile(const std::string &path)
{
	if (access(path.c_str(), R_OK))
		throw Http403Exception();

	std::ifstream file(path.c_str());
	if (!file.is_open())
		return "";

	std::stringstream buffer;
	buffer << file.rdbuf();
	return buffer.str();
}

int check_dir(const std::string &full_path)
{
	struct stat path_stat;
	stat(full_path.c_str(), &path_stat);
	if (S_ISDIR(path_stat.st_mode))
		return (1);
	return (0);
}

std::string Response::checkUrl(const ServerConfig &config)
{
	const std::vector<LocationConfig> &locs = _request->getCurrentLocations();
	std::string reqPath = _request->getPath();
	std::string path;

	if (!locs.empty() && !locs.back().root.empty())
	{
		const LocationConfig &loc = locs.back();
		std::string subPath = reqPath.substr(std::min(loc.path.size(), reqPath.size()));
		std::string locRoot = loc.root;
		if (!locRoot.empty() && locRoot[0] == '/')
			locRoot = "." + locRoot;
		path = locRoot;
		if (!subPath.empty())
		{
			if (path[path.size() - 1] != '/' && subPath[0] != '/')
				path += '/';
			path += subPath;
		}
	}
	else
	{
		std::vector<std::string> tempPath = split(reqPath, "/");
		path = config.root;
		size_t i = 0;
		while (i < tempPath.size())
		{
			if (tempPath[i] == "..")
			{
				tempPath.erase(tempPath.begin() + i);
				if (i > 0)
					tempPath.erase(tempPath.begin() + i - 1);
				if (i > 0)
					i--;
			}
			else
				i++;
		}
		i = 0;
		while (i < tempPath.size())
		{
			path = path + '/';
			path = path + tempPath[i];
			i++;
		}
	}

	std::cout << "------------------------------------------------" << std::endl;
	_finalAutoIndex = false;
	_redirectLocation = "";

	if (_request->getPath() == "/")
		path = config.root;

	if (access(path.c_str(), F_OK) == -1)
		throw Http404Exception();

	if (check_dir(path))
	{
		const std::string &reqPath = _request->getPath();
		if (reqPath[reqPath.size() - 1] != '/')
		{
			_redirectLocation = reqPath + "/";
			return "";
		}
		if (path[path.size() - 1] != '/')
			path += "/";

		const std::vector<LocationConfig> &idxLocs = _request->getCurrentLocations();
		const std::string &indexFile = (!idxLocs.empty() && !idxLocs.back().index.empty())
										   ? idxLocs.back().index
										   : config.index;
		std::string indexPath = path + indexFile;

		if (access(indexPath.c_str(), F_OK) != -1)
			return indexPath;

		const std::vector<LocationConfig> &autoLocs = _request->getCurrentLocations();
		bool autoindex;
		if (!autoLocs.empty() && autoLocs.back().autoindex != -1)
			autoindex = autoLocs.back().autoindex == 1;
		else
			autoindex = config.autoindex;
		if (autoindex)
		{
			_finalAutoIndex = true;
			return path;
		}
		throw Http404Exception();
	}

	return path;
}

std::string Response::buildHeader(size_t contentLength,
								  const std::string &statusCode,
								  const std::string &statusText)
{
	std::ostringstream oss;
	oss << contentLength;

	std::string ContentType = "application/octet-stream";

	if (!_finalPath.empty())
	{
		size_t pos = _finalPath.rfind('.');
		if (pos != std::string::npos && pos != _finalPath.size() - 1)
		{
			std::string ext = _finalPath.substr(pos);
			std::map<std::string, std::string>::iterator it =
				_mimeTypes.find(ext);
			if (it != _mimeTypes.end())
				ContentType = it->second;
		}
	}
	// const std::string rescode = statusCode != "301"  ? "301"  : statusCode;
	std::string header;
	header = "HTTP/1.1 " + statusCode + " " + statusText + "\r\n" +
			 "Content-Type: " + ContentType + "; charset=UTF-8\r\n" +
			 "Content-Length: " + oss.str() + "\r\n" +
			 //  "Location:" + _request->getPath()+"\r\n" +
			 "Connection: close\r\n" +
			 "\r\n";

	return header;
}

void Response::generate(const ServerConfig &config)
{

	_statusCode = "200";
	_statusText = "OK";
	_isCgi = false;

	// try
	// {
	const std::vector<LocationConfig> &locs = _request->getCurrentLocations();

	if (!locs.empty() && locs.back().redirectCode != 0 && _request->getMethod() == "GET")
	{
		const LocationConfig &loc = locs.back();
		std::ostringstream code;
		code << loc.redirectCode;
		_statusCode = code.str();
		_statusText = "Moved Permanently";
		_body = "";
		_header = "HTTP/1.1 " + _statusCode + " " + _statusText + "\r\n"
																  "Location: " +
				  loc.redirectUrl + "\r\n"
									"Content-Length: 0\r\n"
									"Connection: close\r\n"
									"\r\n";
		return;
	}

	const std::vector<std::string> *methods = NULL;
	if (!locs.empty() && !locs.back().allowed_methods.empty())
		methods = &locs.back().allowed_methods;
	else if (!config.allowed_methods.empty())
		methods = &config.allowed_methods;
	if (methods)
	{
		bool allowed = false;
		for (size_t i = 0; i < methods->size(); i++)
		{
			if ((*methods)[i] == _request->getMethod())
			{
				allowed = true;
				break;
			}
		}
		if (!allowed)
			throw Http405Exception();
	}
	_finalPath = checkUrl(config);
	if (!_redirectLocation.empty())
	{
		_statusCode = "301";
		_statusText = "Moved Permanently";
		_body = "";
		_header = "HTTP/1.1 301 Moved Permanently\r\n"
				  "Location: " +
				  _redirectLocation + "\r\n"
									  "Content-Length: 0\r\n"
									  "Connection: close\r\n"
									  "\r\n";
		return;
	}

	if (_request->getMethod() == "DELETE")
	{
		if (access(_finalPath.c_str(), F_OK) != 0)
			throw Http404Exception();
		if (access(_finalPath.c_str(), W_OK) != 0)
			throw Http403Exception();
		if (std::remove(_finalPath.c_str()) != 0)
			throw Http500Exception();
		_statusCode = "200";
		_statusText = "OK";
		_body = "{\"message\": \"File deleted successfully\"}";
		_request->setPath(".json");
		_header = buildHeader(_body.size(), _statusCode, _statusText);
		return;
	}
	else if (_finalAutoIndex == true && check_dir(_finalPath) == 1)
	{
		AutoIndex indexation(_finalPath, _request->getPath());
		_body = indexation.initAutoIndex(_finalPath);
		_finalPath = ".html";
	}
	else
		_body = readFile(_finalPath);
	// } catch (const HttpException &e)
	// {
	// 	int errorCode = e.getStatusCode();
	// 	std::ostringstream oss;
	// 	oss << errorCode;

	// 	_statusCode = oss.str();
	// 	_statusText = e.getStatusText();
	// 	_body = getErrorPageContent(errorCode, config);
	// 	_finalPath = ".html";
	// 	_isCgi = false;
	// }

	_request->setPath(_finalPath);
	if (!_isCgi)
		_header = buildHeader(_body.size(), _statusCode, _statusText);
}

std::string Response::getFullResponse()
{
	if (!_isCgi)
		return _header + _body;

	// Find the blank line separating CGI headers from body (CRLF or LF)
	std::string cgiHeaderBlock;
	std::string cgiBody;
	size_t sep = _body.find("\r\n\r\n");
	if (sep != std::string::npos) {
		cgiHeaderBlock = _body.substr(0, sep);
		cgiBody = _body.substr(sep + 4);
	} else {
		sep = _body.find("\n\n");
		if (sep != std::string::npos) {
			cgiHeaderBlock = _body.substr(0, sep);
			cgiBody = _body.substr(sep + 2);
		} else {
			cgiBody = _body;
		}
	}

	// Parse CGI headers: extract Status:, keep the rest
	std::string statusCode = "200";
	std::string statusText = "OK";
	std::string filteredHeaders;
	bool hasContentLength = false;

	size_t pos = 0;
	while (pos < cgiHeaderBlock.size()) {
		size_t end = cgiHeaderBlock.find('\n', pos);
		std::string line;
		if (end == std::string::npos) {
			line = cgiHeaderBlock.substr(pos);
			pos = cgiHeaderBlock.size();
		} else {
			line = cgiHeaderBlock.substr(pos, end - pos);
			pos = end + 1;
		}
		if (!line.empty() && line[line.size() - 1] == '\r')
			line.erase(line.size() - 1);
		if (line.empty())
			continue;

		size_t colon = line.find(':');
		std::string key = (colon != std::string::npos) ? line.substr(0, colon) : line;
		for (size_t i = 0; i < key.size(); i++)
			key[i] = std::tolower((unsigned char)key[i]);

		if (key == "status" && colon != std::string::npos) {
			std::string val = line.substr(colon + 1);
			size_t start = val.find_first_not_of(" \t");
			if (start != std::string::npos) {
				val = val.substr(start);
				size_t space = val.find(' ');
				if (space != std::string::npos) {
					statusCode = val.substr(0, space);
					statusText = val.substr(space + 1);
				} else {
					statusCode = val;
				}
			}
		} else {
			if (key == "content-length")
				hasContentLength = true;
			filteredHeaders += line + "\r\n";
		}
	}

	std::ostringstream response;
	response << "HTTP/1.1 " << statusCode << " " << statusText << "\r\n";
	response << filteredHeaders;
	if (!hasContentLength)
		response << "Content-Length: " << cgiBody.size() << "\r\n";
	response << "\r\n";
	response << cgiBody;

	return response.str();
}

void Response::setMimes()
{
	_mimeTypes[".html"] = "text/html";
	_mimeTypes[".htm"] = "text/html";
	_mimeTypes[".css"] = "text/css";
	_mimeTypes[".js"] = "application/javascript";
	_mimeTypes[".json"] = "application/json";
	_mimeTypes[".xml"] = "application/xml";
	_mimeTypes[".txt"] = "text/plain";
	_mimeTypes[".csv"] = "text/csv";
	_mimeTypes[".cpp"] = "text/x-c++src";

	_mimeTypes[".png"] = "image/png";
	_mimeTypes[".jpg"] = "image/jpeg";
	_mimeTypes[".jpeg"] = "image/jpeg";
	_mimeTypes[".gif"] = "image/gif";
	_mimeTypes[".bmp"] = "image/bmp";
	_mimeTypes[".svg"] = "image/svg+xml";
	_mimeTypes[".webp"] = "image/webp";
	_mimeTypes[".ico"] = "image/x-icon";

	_mimeTypes[".mp3"] = "audio/mpeg";
	_mimeTypes[".wav"] = "audio/wav";
	_mimeTypes[".ogg"] = "audio/ogg";
	_mimeTypes[".aac"] = "audio/aac";
	_mimeTypes[".flac"] = "audio/flac";

	_mimeTypes[".mp4"] = "video/mp4";
	_mimeTypes[".webm"] = "video/webm";
	_mimeTypes[".ogv"] = "video/ogg";
	_mimeTypes[".avi"] = "video/x-msvideo";
	_mimeTypes[".mov"] = "video/quicktime";

	_mimeTypes[".zip"] = "application/zip";
	_mimeTypes[".tar"] = "application/x-tar";
	_mimeTypes[".gz"] = "application/gzip";
	_mimeTypes[".7z"] = "application/x-7z-compressed";
	_mimeTypes[".rar"] = "application/vnd.rar";

	_mimeTypes[".ttf"] = "font/ttf";
	_mimeTypes[".otf"] = "font/otf";
	_mimeTypes[".woff"] = "font/woff";
	_mimeTypes[".woff2"] = "font/woff2";

	_mimeTypes[".pdf"] = "application/pdf";
	_mimeTypes[".bin"] = "application/octet-stream";
	_mimeTypes[".exe"] = "application/octet-stream";
}
