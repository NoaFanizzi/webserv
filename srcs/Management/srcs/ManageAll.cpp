/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ManageAll.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nofanizz <nofanizz@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/30 09:27:09 by mvachon           #+#    #+#             */
/*   Updated: 2026/01/31 11:30:31 by nofanizz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ManageAll.hpp"
#include "HttpExceptions.hpp"
#include "iostream"
#include <sys/socket.h>
#include <vector>
#include <netinet/in.h>
#include <iostream>
#include <poll.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

std::vector<struct pollfd> ManageAll::_pollfds;
std::map <int, AManager *> ManageAll::_managers;
bool ManageAll::error408 = false;
bool ManageAll::error405 = false;
bool ManageAll::error400 = false;


void ManageAll::pollFdCreation(const int &fd, AManager *manager)
{
	struct pollfd pollfd;

	pollfd.fd = fd;
	pollfd.events = 0;
	pollfd.revents = 0;
	_pollfds.push_back(pollfd);
	_managers.insert(std::make_pair(fd, manager));
}


void ManageAll::updateStatus()
{
	for(size_t i = 0; i < _pollfds.size(); ++i)
	{
		AManager *manager = getManager(_pollfds[i].fd);
		_pollfds[i].events = manager->getEvents();
		// _pollfds[i].revents = 0;
	}
}


struct pollfd&	ManageAll::getPollFd(int index)
{
	return(_pollfds[index]);
}


size_t	ManageAll::getPollFdSize()
{
	return(_pollfds.size());
}

AManager* ManageAll::getManager(int fd)
{
	std::map<int, AManager *>::iterator it = _managers.find(fd);
	if(it != _managers.end())
		return(it->second);
	return(NULL);
}

void	ManageAll::loop()
{
	size_t i;
	while(true)
	{
		updateStatus();
		int poll_value = poll(&_pollfds[0], _pollfds.size(), 1000);
		if (poll_value < 0)
		{
			perror("poll error");
			exit(1);
		}
		poll_value == 0 ? error408 = true : error408 = false;
		i = 0;
		while(i < _pollfds.size() && poll_value)
		{
			if(!_pollfds[i].revents)
			{
				i++;
				continue;
			}
			poll_value--;
			AManager *current = getManager(_pollfds[i].fd);
			if (!current)
			{
				i++;
				continue;
			}
			if(_pollfds[i].revents & POLLIN)
				current->PollInHandler();
			if(_pollfds[i].revents & POLLOUT)
				current->PollOutHandler();
			if (current->getClosedStatus())
			{
				_managers.erase(_pollfds[i].fd);
				_pollfds.erase(_pollfds.begin() + i);
				delete current;
				i--;
			}
			i++;
		}
	}
}


