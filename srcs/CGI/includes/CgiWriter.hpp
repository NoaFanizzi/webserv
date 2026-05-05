//
// Created by lmarcucc on 5/4/26.
//

#ifndef CGI_WRITER_HPP
#define CGI_WRITER_HPP

#include "AManager.hpp"
#include <string>

#ifndef WRITEBUFFSIZE
#define WRITEBUFFSIZE 2
#endif

class CgiWriter : public AManager
{
private:
	bool _timedOut;
	const std::string _body;
	ssize_t _fullSize;
	ssize_t _written;
public:
	CgiWriter(const int _fd, const std::string body);
	~CgiWriter() {};

	void PollInHandler();
	void PollOutHandler();

	void onTimeout();
};

#endif // CGI_WRITER_HPP
