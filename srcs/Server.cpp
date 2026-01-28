/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nofanizz <nofanizz@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/22 11:41:15 by nofanizz          #+#    #+#             */
/*   Updated: 2026/01/28 15:26:21 by nofanizz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ManageAll.hpp"
#include "fcntl.h"

Server::Server()
{
	_closedStatus = false;
	_fd = createSocket();
	createSocketAdress();
	int opt = 1;
	setsockopt(_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
	bindClient(_fd);
	listen(_fd, 4096);
	ManageAll::pollFdCreation(_fd, this);
	_events = POLLIN;
}

int Server::getListen_fd() const
{
	return(_fd);
}

sockaddr_in Server::getSockddr_in() const
{
	return(_servaddr);
}

int	Server::createSocket()
{
	int fd = socket(AF_INET, SOCK_STREAM, 0);

	return fd;
}

void Server::createSocketAdress()
{
	_servaddr.sin_family = AF_INET;
	_servaddr.sin_addr.s_addr = INADDR_ANY;
	_servaddr.sin_port = htons(8090);
}

void Server::bindClient(int fd)
{
	bind(fd, (sockaddr *)&_servaddr, sizeof(_servaddr));
}

void	Server::PollInHandler()
{
	struct sockaddr_in clientaddr;
	socklen_t addr_len = sizeof(clientaddr);
	std::cerr << "WESH Ya quelqun" << std::endl;
	int client_fd = accept(_fd, (sockaddr *)&clientaddr, &addr_len);
	fcntl(client_fd, F_SETFL, O_NONBLOCK);
	new Client(client_fd);
}

void	Server::PollOutHandler()
{
	
}



