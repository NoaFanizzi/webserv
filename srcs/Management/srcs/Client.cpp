/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nofanizz <nofanizz@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/30 10:35:02 by mvachon           #+#    #+#             */
/*   Updated: 2026/02/27 14:31:09 by nofanizz         ###   ########.fr       */
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
	_closedStatus = false;
	_events = POLLIN;
	WebServer::pollFdCreation(_fd, this);
}

void Client::PollInHandler()
{	
	_request.readRaw(_fd, _closedStatus, _rawRequest);
	try
	{
		if (_request.isValid(_rawRequest) == true) {
			_request.parse(_rawRequest, _config);
			_response.setRequest(_request);

			_response.generate(_config);
			_events = POLLOUT;
			std::cout << _rawRequest << std::endl;
			std::cout << "===============================" << std::endl;
		}
		/* code */
	}
	catch(const HttpException& e)
	{
		int errorCode = e.getStatusCode();
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
		std::cerr << "Wesh error non HTTP" << e.what() << std::endl; 
	}
	
}

void Client::PollOutHandler() {
	
	std::string full = _response.getFullResponse();
	send(_fd, full.c_str(), full.size(), 0);

	_events = 0;
	_closedStatus = true;
}

