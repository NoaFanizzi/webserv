#pragma once

#include "Request.hpp"
#include "AManager.hpp"
#include <string>
#include <vector>

#include "Client.hpp"

class CgiManager
{
private:
	// variables
	std::vector<std::string> _env;
	const Request &_request;
	std::string &_scriptPath;
	std::string &_interpreter;
	Client &_client;

	int _fdIn[2];
	int _fdOut[2];
	pid_t _pid;

	// function
	void buildEnv();
	char **envToCharArray() const; // used by function execv

public:
	// constructor
	CgiManager(Client &client, std::string &scriptPath, std::string &interpreter);
	~CgiManager() {}

	// static function — returns interpreter path if CGI, empty string otherwise
	static std::string getCgiInterpreter(const std::string &path, const Request &req);

	// function
	bool start();
};
