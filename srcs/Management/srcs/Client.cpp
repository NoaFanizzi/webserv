/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mvachon <mvachon@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/30 10:35:02 by mvachon           #+#    #+#             */
/*   Updated: 2026/03/25 08:00:24 by mvachon          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"
#include "Request.hpp"
#include "WebServer.hpp"
#include "HttpExceptions.hpp"
#include <iostream>
#include <poll.h>
#include <sys/stat.h>

Client::Client(int fd, const ServerConfig &config) : _config(config) {
	_fd = fd;
	_requestEnded = false;
	_closedStatus = false;
	_events = POLLIN;
	_timedOut = false;
	_startTime = std::time(NULL);
	WebServer::pollFdCreation(_fd, this);
}

void Client::PollInHandler()
{	
	// if(_requestEnded)
	// 	return;
	_request.readRaw(_fd, _closedStatus, _rawRequest);\
	_startTime = std::time(NULL);
	try
	{
		if (_request.isValid(_rawRequest) == true) {
			//_requestEnded = true;
			_request.parse(_rawRequest, _config);
			_response.setRequest(_request);

			_response.generate(_config);
			_events = POLLOUT;
			std::cout << _rawRequest << std::endl;
			std::cout << "===============================" << std::endl;
		}
	}
	catch(const HttpException& e)
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
	catch(const std::exception &e)
	{
		//_requestEnded = true;
		std::cerr << e.what() << std::endl; 
	}
	
}

void Client::onTimeout()
{
	if (_timedOut)
		return;
	_timedOut = true;
    std::string body = _response.getErrorPageContent(408, _config);

    std::ostringstream header;
    header << "HTTP/1.0 408 Request Timeout\r\n"
           << "Content-Type: text/html; charset=UTF-8\r\n"
           << "Content-Length: " << body.size() << "\r\n"
           << "Connection: close\r\n"
           << "\r\n";

    std::string full = header.str() + body;
    send(_fd, full.c_str(), full.size(), 0);

    _events = 0;
    _closedStatus = true;
}

void Client::PollOutHandler() {
	
	std::string full = _response.getFullResponse();
	send(_fd, full.c_str(), full.size(), 0);

	_events = 0;
	_closedStatus = true;
}

