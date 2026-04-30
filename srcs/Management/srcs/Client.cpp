/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nofanizz <nofanizz@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/30 10:35:02 by mvachon           #+#    #+#             */
/*   Updated: 2026/04/30 15:39:28 by nofanizz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"
#include "Request.hpp"
#include "WebServer.hpp"
#include "HttpExceptions.hpp"
#include <iostream>
#include <poll.h>
#include <sys/stat.h>

#include "CgiManager.hpp"

Client::Client(int fd, const ServerConfig &config) : _config(config)
{
	_fd = fd;
	_requestEnded = false;
	_closedStatus = false;
	_events = POLLIN;
	_timedOut = false;
	_cgi = false;
	_startTime = std::time(NULL);
	WebServer::pollFdCreation(_fd, this);
}

void Client::PollInHandler()
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
				std::cout << "IS CGI\n";
				_events = POLLOUT;
				_response.setIsCgi(true);
				CgiManager *cgi = new CgiManager(*this, "website/cgi/hello.py");
				(void)cgi;
				_cgi = true;
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
		//_requestEnded = true;
		int errorCode = e.getStatusCode();
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
		// _isCgi = false;
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

	std::string full = header.str() + body;
	size_t sent = 0;
	while (sent < full.size())
	{
		ssize_t n = send(_fd, full.c_str() + sent, full.size() - sent, 0);
		if (n <= 0)
			break;
		sent += n;
	}

	_events = 0;
	_closedStatus = true;
}

void Client::PollOutHandler()
{
	if (_cgi)
		return;
	std::string full = _response.getFullResponse();
	size_t sent = 0;
	while (sent < full.size())
	{
		ssize_t n = send(_fd, full.c_str() + sent, full.size() - sent, 0);
		if (n <= 0)
			break;
		sent += n;
	}

	_events = 0;
	_closedStatus = true;
}

void Client::setCgiOutput(const std::string &output) {
	_response.setBody(output);
	_events = POLLOUT;
	_cgi = false;
}