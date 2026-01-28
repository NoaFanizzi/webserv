/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nofanizz <nofanizz@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/22 11:47:07 by nofanizz          #+#    #+#             */
/*   Updated: 2026/01/28 15:20:10 by nofanizz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"
#include "ManageAll.hpp"

Client::Client(int fd)
{
	_closedStatus = false;
	_fd = fd;
	ManageAll::pollFdCreation(_fd, this);
	_events = POLLIN;
}


void Client::PollInHandler()
{
    char buffer[4096];
    int n = recv(_fd, buffer, sizeof(buffer) - 1, 0);
	std::cerr << "READING" << std::endl;
    
    if (n <= 0) {
        _closedStatus = true;
        return;
    }
	
    buffer[n] = '\0';
    _request.append(buffer, n);
    _events = POLLOUT;
}

void	Client::PollOutHandler()
{
	std::string response =
    "HTTP/1.0 200 OK\r\n"
    "Content-Type: text/plain\r\n"
    "Content-Length: 13\r\n"
    "\r\n"
    "Hello, world!";

	send(_fd, response.c_str(), response.length(), 0);
	std::cerr << "WESH0" << std::endl;
	_events = 0;
	_closedStatus = true;
}
