#include "CgiManager.hpp"

#include "WebServer.hpp"
#include <cerrno>
#include <cstring>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>
#include <poll.h>

// constructor
CgiManager::CgiManager(Client &client, const std::string &scriptPath)
	: _request(client.getRequest()),
	  _scriptPath(scriptPath),
	  _client(client),
	  _pid(-1),
	  _stdinFd(-1),
	  _timedOut(false) {
	_fd = -1;
	_closedStatus = false;
	_events = POLLIN;
	_startTime = std::time(NULL);
	if (!start()) {
		// TODO manage error500
		_client.setCgiOutput("", 500);
		return;
	}
	fcntl(_fd, F_SETFL, O_NONBLOCK);
	WebServer::pollFdCreation(_fd, this);
}

CgiManager::~CgiManager() {
}

// static function
bool CgiManager::isCgi(const std::string &path) {
	if (path.size() >= 3 && path.compare(path.size() - 3, 3, ".py") == 0)
		return true;
	if (path.size() >= 4 && path.compare(path.size() - 4, 4, ".php") == 0)
		return true;
	return false;
}

static void freeEnv(char **env) {	
	for (size_t i = 0; env[i] != NULL; i++)
	{
		free(env[i]);
	}
	delete[] env;
}

// functions
void CgiManager::buildEnv() {
	_env.push_back("REQUEST_METHOD=" + _request.getMethod());
	_env.push_back("SCRIPT_FILENAME=" + _scriptPath);
	_env.push_back("SCRIPT_NAME=" + _request.getPath());
	_env.push_back("QUERY_STRING=" + _request.getQuery());
	_env.push_back("CONTENT_LENGTH=" + _request.getHeaders("Content-Length"));
	_env.push_back("CONTENT_TYPE=" + _request.getHeaders("Content-Type"));
	_env.push_back("SERVER_PROTOCOL=HTTP/1.1");
	_env.push_back("GATEWAY_INTERFACE=CGI/1.1");
}

char **CgiManager::envToCharArray() const {
	char **envp = new char *[_env.size() + 1];

	int i = 0;
	for (; i < static_cast<int>(_env.size()); i++) {
		envp[i] = strdup(_env[i].c_str());
		if (!envp[i]) {
			while (--i != -1)
				free(envp[i]);
			delete[] envp;
			return NULL;
		}
	}
	envp[i] = NULL;
	return envp;
}

// execute the cgi script
bool CgiManager::start() {
	buildEnv();

	int fdIn[2];
	int fdOut[2];

	if (pipe(fdIn) < 0 || pipe(fdOut) < 0)
		return false;

	_pid = fork();

	if (_pid < 0) {
		close(fdIn[0]);
		close(fdIn[1]);
		close(fdOut[0]);
		close(fdOut[1]);
		return false;
	}

	if (_pid == 0) {
		dup2(fdIn[0], STDIN_FILENO);
		dup2(fdOut[1], STDOUT_FILENO);

		close(fdIn[0]);
		close(fdIn[1]);
		close(fdOut[0]);
		close(fdOut[1]);

		char **envp = envToCharArray();
		char *argv[] = {
			const_cast<char *>(_scriptPath.c_str()),
			NULL
		};
		execve(argv[0], argv, envp);
		std::cerr << _scriptPath;
		perror(": ");
		freeEnv(envp);
		(*this).~CgiManager();
		WebServer::destroy();
		exit(127);
	}

	close(fdIn[0]);
	close(fdOut[1]);

	_stdinFd = fdIn[1];
	_fd = fdOut[0];

	// TODO demande a mat comment get le body
	//const std::string &body = _client.getRequest().getBody();
	//if (!body.empty()) {
	//	ssize_t written = write(_stdinFd, body.c_str(), body.size());
	//	(void)written;
	//}
	close(_stdinFd);
	_stdinFd = -1;

	return true;
}

void CgiManager::PollInHandler() {
	if (_timedOut) {
		return ;
	}
	char buffer[4096];

	const ssize_t readSize = read(_fd, buffer, sizeof(buffer));

	if (readSize == 0)
	{
		_client.setCgiOutput("", 501);
		_events = 0;
		_closedStatus = true;
	}
	if (readSize > 0) {
		_output.append(buffer, readSize);
		return;
	}
	if (readSize < 0) {
		_client.setCgiOutput("", 500);
		_events = 0;
		_closedStatus = true;
		return;
	}

	if (_pid > 0) {
		int status;
		const pid_t ret = waitpid(_pid, &status, WNOHANG);

		if (ret == 0)
			return;
		if (ret < 0) {
			_client.setCgiOutput("", 500);
			_events = 0;
			_closedStatus = true;
			_pid = -1;
			return;
		}

		if (WIFEXITED(status)) {
			const int code = WEXITSTATUS(status);

			if (code != 0) {
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

void CgiManager::PollOutHandler() {
	_closedStatus = true;
	_events = 0;
}

void CgiManager::onTimeout() {
	if (_timedOut)
		return ;
	if (_pid != -1) {
		kill(_pid, SIGKILL);
		waitpid(_pid, NULL, 0);
		_pid = -1;
	}
	_client.setCgiOutput("", 504);
	_events = 0;
	_closedStatus = true;
	_timedOut = true;
}
