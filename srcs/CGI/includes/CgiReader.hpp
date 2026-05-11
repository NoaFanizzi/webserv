#ifndef CGI_READER_HPP
#define CGI_READER_HPP

#include "AManager.hpp"
#include "Client.hpp"
#include <string>

class CgiReader : public AManager
{
private:
	std::string _output;
	Client &_client;
	pid_t _pid;
	bool _timedOut;

public:
	CgiReader(Client &client, pid_t pid, int fd);
	~CgiReader() {};

	void pollInHandler();
	void pollOutHandler() {};

	void onTimeout();
};

#endif
