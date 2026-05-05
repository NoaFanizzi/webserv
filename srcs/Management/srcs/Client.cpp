
#include "Client.hpp"
#include "Request.hpp"
#include "WebServer.hpp"
#include "HttpExceptions.hpp"
#include <iostream>
#include <poll.h>
#include <sys/stat.h>

#include "CgiManager.hpp"
#include <cerrno>

Client::Client(int fd, const ServerConfig &config) : _config(config)
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
		if (_request.isValid(_rawRequest, _config) == true)
		{
			_requestEnded = true;
			_request.parse(_rawRequest, _config);
			_response.setRequest(_request);

			if (CgiManager::isCgi(_request.getPath())) {
				 // TODO vrai path
				std::string realPath = "website" + _request.getPath();
				if (access(realPath.c_str(), F_OK) == -1)
					throw Http404Exception();
				if (access(realPath.c_str(), X_OK) == -1)
					throw Http403Exception();
				_events = 0;
				_response.setIsCgi(true);
				_cgi = true;
				new CgiManager(*this, realPath);
				_startTime = std::time(NULL);
			}
			else {
				_response.generate(_config);
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
		_response.setStatusCode(oss.str());
		_response.setStatusText(e.getStatusText());
		_response.setBody(_response.getErrorPageContent(errorCode, _config));
		_response.setFinalPath(".html");
		_response.buildErrorHeader();
		_events = POLLOUT;
	}
	catch (const std::exception &e)
	{
		_request.setPath(".html");
		_response.setRequest(_request);
		_response.setStatusCode("500");
		_response.setStatusText("Internal Server Error");
		_response.setBody(_response.getErrorPageContent(500, _config));
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
	std::string body = _response.getErrorPageContent(408, _config);
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
	else if (n < 0 && errno != EAGAIN && errno != EWOULDBLOCK)
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
		_response.setBody(_response.getErrorPageContent(error, _config));
		_response.setFinalPath(".html");
		_response.buildErrorHeader();
	}
	else
		_response.setBody(output);
	_events = POLLOUT;
}
