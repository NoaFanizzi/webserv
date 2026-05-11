#include "CgiManager.hpp"
#include "WebServer.hpp"
#include "CgiWriter.hpp"
#include "CgiReader.hpp"
#include "HttpExceptions.hpp"
#include <cerrno>
#include <cstring>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>
#include <poll.h>

// constructor
CgiManager::CgiManager(Client &client, std::string &scriptPath, std::string &interpreter)
	: _request(client.getRequest()),
	  _scriptPath(scriptPath),
	  _interpreter(interpreter),
	  _client(client),
	  _pid(-1) {

	if (access(_scriptPath.c_str(), F_OK) == -1)
		throw Http404Exception();
	if (pipe(_fdIn) == -1) {
		throw Http500Exception();
	}
	if (pipe(_fdOut) == -1) {
		close(_fdIn[0]);
		close(_fdIn[1]);
		throw Http500Exception();
	}
	if (fcntl(_fdOut[0], F_SETFL, O_NONBLOCK) == -1 ||
		fcntl(_fdIn[1], F_SETFL, O_NONBLOCK) == -1 ||
		!start()) {
		close(_fdIn[0]);
		close(_fdIn[1]);
		close(_fdOut[0]);
		close(_fdOut[1]);
		throw Http500Exception();
	}
}

// Returns the interpreter path if the request maps to a CGI script, empty string otherwise
std::string CgiManager::getCgiInterpreter(const std::string &path, const Request &req)
{
	size_t dot = path.rfind('.');
	if (dot == std::string::npos)
		return "";
	std::string ext = path.substr(dot);

	const std::vector<LocationConfig> &locs = req.getCurrentLocations();
	if (!locs.empty()) {
		std::map<std::string, std::string>::const_iterator it = locs.back().cgi_pass.find(ext);
		if (it != locs.back().cgi_pass.end())
			return it->second;
	}
	return "";
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

	if ((_pid = fork()) == -1)
		return false;

	if (_pid == 0) {
		dup2(_fdIn[0], STDIN_FILENO);
		dup2(_fdOut[1], STDOUT_FILENO);

		close(_fdIn[0]);
		close(_fdIn[1]);
		close(_fdOut[0]);
		close(_fdOut[1]);

		char **envp = envToCharArray();
		char *argv[] = {
			const_cast<char *>(_interpreter.c_str()),
			const_cast<char *>(_scriptPath.c_str()),
			NULL
		};
		execve(argv[0], argv, envp);
		perror(_scriptPath.c_str());
		freeEnv(envp);
		WebServer::destroy();
		_scriptPath.~basic_string();
		_interpreter.~basic_string();
		exit(127);
	}

	close(_fdIn[0]);
	close(_fdOut[1]);

	new CgiReader(_client, _pid, _fdOut[0]);
	if (_client.getRequest().getMethod() == "POST")
		new CgiWriter(_fdIn[1], _client.getRequest().getBody());
	else
		close(_fdIn[1]);

	return true;
}
