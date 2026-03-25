#include "WebServer.hpp"
#include "AManager.hpp"
#include <poll.h>
#include <stdlib.h>

std::vector <struct pollfd> WebServer::_pollfds;
std::map <int, AManager *> WebServer::_managers;
bool WebServer::_firstLoopRequest = true;

void WebServer::pollFdCreation(const int &fd, AManager *manager) {
	struct pollfd pollfd;

	pollfd.fd = fd;
	pollfd.events = 0;
	pollfd.revents = 0;
	_pollfds.push_back(pollfd);
	_managers.insert(std::make_pair(fd, manager));
}

void WebServer::updateStatus() {

	time_t now = std::time(NULL);
	for (size_t i = 0; i < _pollfds.size(); ++i) {
		AManager *manager = getManager(_pollfds[i].fd);
		_pollfds[i].events = manager->getEvents();
		_pollfds[i].revents = 0;
		if (manager->isTimeout(now))
		{
			manager->onTimeout();
			manager->setEvents(POLLOUT);
		}
	}
}

AManager *WebServer::getManager(int fd) {
	std::map<int, AManager *>::iterator it = _managers.find(fd);
	if (it != _managers.end())
		return (it->second);
	return (NULL);
}

void WebServer::destroy()
{
	for(size_t i = 0; i < _pollfds.size(); i++)
	{
		const int fd =_pollfds[i].fd;
		AManager *current = getManager(fd);
		_managers.erase(fd);
		close(fd);
		delete current;
	}
}

void WebServer::run() {
	_firstLoopRequest = true;
	while (true) {
		updateStatus();
		
		int poll_value = poll(&_pollfds[0], _pollfds.size(), 1000);
		if (poll_value < 0) {
			perror("poll error");
			exit(1);
		}
		for (size_t i = 0; i < _pollfds.size() && poll_value; i++) {
			AManager *current = getManager(_pollfds[i].fd);
			if (!current || !_pollfds[i].revents)
        		continue;
			poll_value--;
			if (!current)
				continue;
			if (_pollfds[i].revents & POLLIN)
				current->PollInHandler();
			if (_pollfds[i].revents & POLLOUT)
				current->PollOutHandler();
			if (current->getClosedStatus()) {
				_managers.erase(_pollfds[i].fd);
				_pollfds.erase(_pollfds.begin() + i);
				close(current->getFd());
				delete current;
				i--;
			}
		}
	}
}

