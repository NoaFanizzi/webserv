#include "CgiWriter.hpp"
#include "WebServer.hpp"
#include <iostream>
#include <poll.h>
#include <fcntl.h>

CgiWriter::CgiWriter(const int fd, const std::string body) : _timedOut(false), _body(body), _written(0)
{
	std::cout << "CGI BODY=" << _body << std::endl;
	_fd = fd;
	_fullSize = body.size();
	_closedStatus = false;
	_events = POLLOUT;
	_startTime = std::time(NULL);
	WebServer::pollFdCreation(_fd, this);
}

void CgiWriter::pollOutHandler() {

	if (_timedOut)
		return ;
	ssize_t toSend;
	if (_fullSize - _written > WRITEBUFFSIZE)
		toSend = WRITEBUFFSIZE;
	else
		toSend = _fullSize - _written;

	const char *to_write = _body.data() + _written;
	std::cerr << to_write << std::endl;
	ssize_t bytesWriten = write(_fd, to_write, toSend);
	if (bytesWriten == -1) {
		perror("write: ");
		_events = 0;
		_closedStatus = true;
		return ;
	}
	_written += bytesWriten;
	if (_written == _fullSize) {
		std::cout << "all" << std::endl;
		_events = 0;
		_closedStatus = true;
	}
}

void CgiWriter::onTimeout() {
	_closedStatus = true;
	_events = 0;
}
