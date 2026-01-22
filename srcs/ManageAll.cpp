/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ManageAll.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nofanizz <nofanizz@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/22 12:43:22 by nofanizz          #+#    #+#             */
/*   Updated: 2026/01/22 18:10:42 by nofanizz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ManageAll.hpp"

std::vector<struct pollfd&> ManageAll::_pollfds;
std::vector<Server> ManageAll::_vServ;

ManageAll::ManageAll()
{
	
}

void ManageAll::pollFdCreation(struct pollfd &poll)
{
	_pollfds.push_back(poll);
}

void	ManageAll::addToManager(Server &server)
{
	_pollfds.push_back(server);
}

struct pollfd&	ManageAll::getPollFd(int index)
{
	return(_pollfds[index]);
}

Server& ManageAll::getServer(int index)
{
	return(_vServ[index]);
}


