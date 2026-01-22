/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nofanizz <nofanizz@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/22 11:41:15 by nofanizz          #+#    #+#             */
/*   Updated: 2026/01/22 18:05:47 by nofanizz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ManageAll.hpp"

Server::Server()
{
	createSocket();
	createSocketAdress();
	bindClient();
	listen(_fd.fd, 4096);
	ManageAll::pollFdCreation(_fd);
	ManageAll::addServer(*this);
}

int Server::getListen_fd() const
{
	return(_fd.fd);
}

sockaddr_in Server::getSockddr_in() const
{
	return(_servaddr);
}

struct pollfd Server::getPollFd() const
{
	return(_fd);
}

void	Server::createSocket()
{
	_fd.fd = socket(AF_INET, SOCK_STREAM, 0);
	_fd.events= POLLIN;
	_fd.revents = 0;
}

void Server::createSocketAdress()
{
	_servaddr.sin_family = AF_INET;
	_servaddr.sin_addr.s_addr = INADDR_ANY;
	_servaddr.sin_port = htons(8080);
}

void Server::bindClient()
{
	bind(_fd.fd, (sockaddr *)&_servaddr, sizeof(_servaddr));
}

void	Server::PollInHandler()
{
	struct sockaddr_in clientaddr;
	socklen_t addr_len = sizeof(clientaddr);
	int client_fd = accept(_fd.fd, (sockaddr *)&clientaddr, &addr_len);
	std::cout << "New client connected !" << std::endl;
	std::cout << "INFORMATIONS :" << std::endl;
	std::cout << "Client FD: " << client_fd << std::endl;
	std::cout << "Client IP: " << inet_ntoa(clientaddr.sin_addr) << std::endl;
	std::cout << "Client PORT: " << clientaddr.sin_port << std::endl;
	Client nclient(client_fd);
}

void	Server::AManager::PollOutHandler()
{
	
}



