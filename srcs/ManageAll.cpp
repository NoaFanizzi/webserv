

#include "ManageAll.hpp"

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
		if(poll_value < 0)
		{
			perror("poll error");
			exit(1);
		}
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


