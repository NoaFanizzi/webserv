
#include "Client.hpp"
#include "Request.hpp"
#include "WebServer.hpp"
#include "HttpExceptions.hpp"
#include <iostream>
#include <poll.h>
#include <sys/stat.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <cstring>

#include "CgiManager.hpp"

static std::string extractHostFromRaw(const std::string &raw)
{
	std::string lower;
	size_t headerEnd = raw.find("\r\n\r\n");
	std::string headers = raw.substr(0, headerEnd);
	lower.resize(headers.size());
	for (size_t i = 0; i < headers.size(); i++)
		lower[i] = static_cast<char>(std::tolower(static_cast<unsigned char>(headers[i])));

	size_t pos = lower.find("\r\nhost:");
	if (pos == std::string::npos)
	{
		// Try first line in case Host is the first header
		pos = lower.find("host:");
		if (pos != 0)
			return "";
		pos = 5;
	}
	else
		pos += 7;

	while (pos < headers.size() && headers[pos] == ' ')
		pos++;
	size_t end = headers.find("\r\n", pos);
	if (end == std::string::npos)
		end = headers.size();
	std::string host = headers.substr(pos, end - pos);
	// Strip port suffix if present
	size_t colon = host.rfind(':');
	if (colon != std::string::npos)
		host = host.substr(0, colon);
	return host;
}

static std::string resolveToIp(const std::string &hostname)
{
	struct addrinfo hints, *res;
	std::memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	if (getaddrinfo(hostname.c_str(), NULL, &hints, &res) != 0)
		return hostname;
	char ip[INET_ADDRSTRLEN];
	inet_ntop(AF_INET, &((struct sockaddr_in *)res->ai_addr)->sin_addr, ip, sizeof(ip));
	freeaddrinfo(res);
	return std::string(ip);
}

static const ServerConfig &selectActiveConfig(const std::vector<ServerConfig> &configs, const std::string &hostname)
{
	std::string resolvedIp = resolveToIp(hostname);

	for (size_t i = 0; i < configs.size(); i++)
	{
		const std::vector<std::string> &names = configs[i].server_names;

		if (configs[i].host == hostname || configs[i].host == resolvedIp)
			return configs[i];
		for (size_t j = 0; j < names.size(); j++)
		{
			if (names[j] == hostname)
				return configs[i];
		}
	}
	return configs[0];
}

Client::Client(int fd, const std::vector<ServerConfig> &configs) : _configs(configs), _activeConfig(configs[0])
{
	_fd = fd;
	_sendOffset = 0;
	_requestEnded = false;
	_closedStatus = false;
	_events = POLLIN;
	_timedOut = false;
	_startTime = std::time(NULL);
	_cgi = false;
	WebServer::pollFdCreation(_fd, this);
}

void Client::pollInHandler()
{
	if (_requestEnded)
		return;
	_request.readRaw(_fd, _closedStatus, _rawRequest);
	_startTime = std::time(NULL);
	try
	{
		if (_request.isValid(_rawRequest, _activeConfig) == true)
		{
			_activeConfig = selectActiveConfig(_configs, extractHostFromRaw(_rawRequest));
			_requestEnded = true;
			_request.parse(_rawRequest, _activeConfig);
			_response.setRequest(_request);
			_response.checkAllowedMethods(_activeConfig);

			std::string interpreter = CgiManager::getCgiInterpreter(_request.getPath(), _request);
			if (!interpreter.empty()) {
				const std::vector<LocationConfig> &locs = _request.getCurrentLocations();
				std::string root = _activeConfig.root;
				if (!locs.empty() && !locs.back().root.empty())
					root = locs.back().root;
				std::string realPath = root + _request.getPath();
				CgiManager cgi(*this, realPath, interpreter);
				_events = 0;
				_response.setIsCgi(true);
				_cgi = true;
				_startTime = std::time(NULL);
			}
			else {
				_response.generate(_activeConfig);
				_events = POLLOUT;
			}
			std::cout << _rawRequest << std::endl;
			std::cout << "===============================" << std::endl;
		}
	}
	catch (const HttpException &e)
	{
		const int errorCode = e.getStatusCode();
		std::cout << "error code = " << errorCode << std::endl;
		std::ostringstream oss;
		oss << errorCode;
		_request.setPath(".html");
		_response.setRequest(_request);
		_response.setIsCgi(false);
		_response.setStatusCode(oss.str());
		_response.setStatusText(e.getStatusText());
		_response.setBody(_response.getErrorPageContent(errorCode, _activeConfig));
		_response.setFinalPath(".html");
		_response.buildErrorHeader();
		_events = POLLOUT;
	}
	catch (const std::exception &e)
	{
		_request.setPath(".html");
		_response.setRequest(_request);
		_response.setIsCgi(false);
		_response.setStatusCode("500");
		_response.setStatusText("Internal Server Error");
		_response.setBody(_response.getErrorPageContent(500, _activeConfig));
		_response.setFinalPath(".html");
		_response.buildErrorHeader();
		_events = POLLOUT;
	}
}

bool Client::isTimeout(time_t timeNow) {
	if (_cgi)
		return false;
	return (timeNow - _startTime > 5);
}

void Client::onTimeout()
{
	if (_timedOut)
		return;
	_timedOut = true;
	std::string body = _response.getErrorPageContent(408, _activeConfig);
	std::ostringstream header;
	header << "HTTP/1.1 408 Request Timeout\r\n"
		   << "Content-Type: text/html; charset=UTF-8\r\n"
		   << "Content-Length: " << body.size() << "\r\n"
		   << "Connection: close\r\n"
		   << "\r\n";
	_sendBuffer = header.str() + body;
	_sendOffset = 0;
	_events = POLLOUT;
}

void Client::pollOutHandler()
{
	if (_sendBuffer.empty())
	{
		_sendBuffer = _response.getFullResponse();
		_sendOffset = 0;
	}

	const ssize_t n = send(_fd, _sendBuffer.c_str() + _sendOffset, _sendBuffer.size() - _sendOffset, 0);
	if (n > 0)
		_sendOffset += static_cast<size_t>(n);
	else
	{
		_events = 0;
		_closedStatus = true;
		return;
	}

	if (_sendOffset >= _sendBuffer.size())
	{
		_events = 0;
		_closedStatus = true;
	}
}

void Client::setCgiOutput(const std::string &output, const int error) {
	if (error) {
		std::ostringstream oss;
		oss << error;
		_response.setIsCgi(false);
		_request.setPath(".html");
		_response.setRequest(_request);
		_response.setStatusCode(oss.str());
		if (error == 504)
			_response.setStatusText("Gateway Timeout");
		else
			_response.setStatusText("Internal Server Error");
		_response.setBody(_response.getErrorPageContent(error, _activeConfig));
		_response.setFinalPath(".html");
		_response.buildErrorHeader();
	}
	else
		_response.setBody(output);
	_events = POLLOUT;
}
