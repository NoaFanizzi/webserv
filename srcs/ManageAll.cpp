/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ManageAll.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nofanizz <nofanizz@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/22 12:43:22 by nofanizz          #+#    #+#             */
/*   Updated: 2026/01/22 13:57:14 by nofanizz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ManageAll.hpp"

std::vector<struct pollfd> ManageAll::_pollfds;
std::vector<Server> ManageAll::_vServ;

ManageAll::ManageAll()
{
	
}

struct pollfd ManageAll::pollFdCreation(int src_fd)
{
	struct pollfd poll;
	poll.fd = src_fd;
	poll.events = POLLIN;
	poll.revents = 0;
	
	_pollfds.push_back(poll);
	return poll;
}

void	ManageAll::addServer(Server &server)
{
	_vServ.push_back(server);
}
