#include "CgiReader.hpp"
#include "WebServer.hpp"
#include <poll.h>
#include <sys/wait.h>

CgiReader::CgiReader(Client &client, pid_t pid, int fd)
	: _client(client), _pid(pid), _timedOut(false)
{
	_fd = fd;
	_closedStatus = false;
	_events = POLLIN;
	_startTime = std::time(NULL);
	WebServer::pollFdCreation(_fd, this);
}

void CgiReader::pollInHandler()
{
	if (_timedOut)
	{
		return;
	}
	char buffer[4096];
	const ssize_t readSize = read(_fd, buffer, sizeof(buffer));

	if (readSize > 0)
	{
		_output.append(buffer, readSize);
		return;
	}
	if (readSize < 0)
	{
		_client.setCgiOutput("", 500);
		_events = 0;
		_closedStatus = true;
		return;
	}

	if (_pid > 0)
	{
		int status;
		const pid_t ret = waitpid(_pid, &status, WNOHANG);

		if (ret == 0)
			return;
		if (ret < 0)
		{
			_client.setCgiOutput("", 500);
			_events = 0;
			_closedStatus = true;
			_pid = -1;
			return;
		}

		if (WIFEXITED(status))
		{
			const int code = WEXITSTATUS(status);

			if (code != 0)
			{
				_client.setCgiOutput("", 500);
				_events = 0;
				_closedStatus = true;
				return;
			}
		}
	}

	_client.setCgiOutput(_output, 0);

	_pid = -1;
	_events = 0;
	_closedStatus = true;
}

void CgiReader::onTimeout()
{
	if (_timedOut)
		return;
	if (_pid != -1)
	{
		kill(_pid, SIGKILL);
		waitpid(_pid, NULL, 0);
		_pid = -1;
	}
	_client.setCgiOutput("", 504);
	_events = 0;
	_closedStatus = true;
	_timedOut = true;
}
