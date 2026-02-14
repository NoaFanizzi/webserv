/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nofanizz <nofanizz@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/30 10:35:02 by mvachon           #+#    #+#             */
/*   Updated: 2026/02/14 15:31:43 by nofanizz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"
#include "Request.hpp"
#include "Response.hpp"
#include "WebServer.hpp"
#include <iostream>
#include <poll.h>
#include <sys/stat.h>

Client::Client(int fd, const ServerConfig &config) : _config(config) {
	_fd = fd;
	_closedStatus = false;
	_events = POLLIN;
	WebServer::pollFdCreation(_fd, this);
}

void Client::PollInHandler() {
	_request.readRaw(_fd, _closedStatus, _rawRequest);
	
	
	if (_request.isValid(_rawRequest) == true) {
		_request.parse(_rawRequest);
		_events = POLLOUT;
		std::cout << _rawRequest << std::endl;
		std::cout << "===============================" << std::endl;
	}
}

void Client::PollOutHandler() {
	Response response(_request);

	response.generate(_config);

	std::string full = response.getFullResponse();
	send(_fd, full.c_str(), full.size(), 0);

	_events = 0;
	_closedStatus = true;
}

