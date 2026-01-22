/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nofanizz <nofanizz@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/22 11:41:15 by nofanizz          #+#    #+#             */
/*   Updated: 2026/01/22 13:31:37 by nofanizz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ManageAll.hpp"

Server::Server()
{
	createSocket();
	createSocketAdress();
	bindClient();
	_data = ManageAll::pollFdCreation(_listen_fd);
}

int Server::getListen_fd() const
{
	return(_listen_fd);
}

sockaddr_in Server::getSockddr_in() const
{
	return(_servaddr);
}

struct pollfd Server::getPollFd() const
{
	return(_data);
}

void	Server::createSocket()
{
	_listen_fd = socket(AF_INET, SOCK_STREAM, 0);
}

void Server::createSocketAdress()
{
	_servaddr.sin_family = AF_INET;
	_servaddr.sin_addr.s_addr = INADDR_ANY;
	_servaddr.sin_port = htons(8080);
}

void Server::bindClient()
{
	bind(_listen_fd, (sockaddr *)&_servaddr, sizeof(_servaddr));
}


