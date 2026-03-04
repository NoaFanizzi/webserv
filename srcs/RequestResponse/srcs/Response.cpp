#include "Config.hpp"
#include "HttpExceptions.hpp"
#include "Request.hpp"
#include "AManager.hpp"
#include "WebServer.hpp"
#include "AutoIndex.hpp"
#include "CgiManager.hpp"
#include "Client.hpp"
#include <fstream>
#include <sstream>
#include <sys/stat.h>
#include <cstdio>
#include <iostream>
#include <sys/stat.h>
#include <cerrno>
#include "vector"

static bool _finalAutoIndex;

void Response::setRequest(Request &req) {
	_request = &req;
	_isCgi = false;
	setMimes();
	setErrorPages();
}

void Response::buildErrorHeader() {
    _header = buildHeader(_body.size(), _statusCode, _statusText);
}

std::string Response::getErrorPageContent(int code,
                                          const ServerConfig &config) {
	for (size_t i = 0; i < config.error_page.size(); i++) {
		if (config.error_page[i].index == code) {
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

std::string Response::readFile(const std::string &path) {
	if (access(path.c_str(), R_OK))
		throw Http403Exception();

	std::ifstream file(path.c_str());
	if (!file.is_open())
		return "";

	std::stringstream buffer;
	buffer << file.rdbuf();
	return buffer.str();
}

int	check_dir(const std::string &full_path)
{
	struct stat path_stat;
	stat(full_path.c_str(), &path_stat);
	if (S_ISDIR(path_stat.st_mode))
		return(1);
	return(0);
}

std::string Response::checkUrl(const ServerConfig &config)
{

	std::vector<std::string> tempPath = split(_request->getPath(), "/");
	std::string path = config.root;
	size_t	i = 0;
	while(i < tempPath.size())
	{
		if(tempPath[i] == "..")
			tempPath.erase(tempPath.begin() + i);
		else
			i++;
	}
	i = 0;
	while(i < tempPath.size())
	{
		path = path + '/';
		path = path + tempPath[i];
		i++;
	}

	std::cout << "------------------------------------------------" << std::endl;
	std::cout << "PAFF = " << path << std::endl;
    _finalAutoIndex = false;

    if (_request->getPath() == "/")
        path = config.root + "/" + config.index;

    if (access(path.c_str(), F_OK) == -1)
        throw Http404Exception();

    if (check_dir(path))
    {
        if (path[path.size() - 1] != '/')
            path += "/";

        std::string indexPath = path + config.index;

        if (access(indexPath.c_str(), F_OK) != -1)
            return indexPath;

        if (config.autoindex)
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
                                  const std::string &statusText) {
	std::ostringstream oss;
	oss << contentLength;
	
	std::string ContentType = "application/octet-stream";
	std::string url = _request->getPath();

	if (!url.empty()) {
		size_t pos = url.rfind('.');
		if (pos != std::string::npos && pos != url.size() - 1) {
			std::string ext = url.substr(pos);
			std::map<std::string, std::string>::iterator it =
			    _mimeTypes.find(ext);
			if (it != _mimeTypes.end())
				ContentType = it->second;
		}
	}
	std::cout << _request->getPath() << std::endl;
	// const std::string rescode = statusCode != "301"  ? "301"  : statusCode;
	std::string header;
	header = "HTTP/1.0 " + statusCode + " " + statusText + "\r\n" +
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
	std::cout << "JE SUIS DANS RESPONSE GENERATEEEEE" << std::endl;
		_request->setCurrentLocations(config);
		_finalPath = checkUrl(config);
		if (CgiManager::isCgi(_finalPath))
		{
			CgiManager cgi(*_request, _finalPath);
			if (!cgi.execute())
				throw Http500Exception();
			_body = cgi.getOutput();
			_isCgi = true; // CGI outputs already include headers
		}
		else if (_request->getMethod() == "DELETE")
		{
			std::cout << _finalPath.c_str() << std::endl;
			if (std::remove(_finalPath.c_str()) != 0) {
				if (errno == EACCES)
					throw Http403Exception();
				else
					throw Http404Exception();
			}
			_statusCode = "200";
			_statusText = "OK";
			_body = "{\"message\": \"File deleted successfully\"}";
			_request->setPath(".json");
			_header = buildHeader(_body.size(), _statusCode, _statusText);
			std::cout << "HEADERRRRRR = " <<_header << std::endl;
			return;
		}
		else if (_finalAutoIndex == true && check_dir(config.root + _request->getPath()) == 1)
		{
			AutoIndex indexation(config.root, _request->getPath());
			_body = indexation.initAutoIndex(config.root + _request->getPath());
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

void Response::setMimes() {
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
