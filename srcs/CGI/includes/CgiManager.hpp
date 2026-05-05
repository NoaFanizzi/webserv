#pragma once

#include "Request.hpp"
#include "AManager.hpp"
#include <string>
#include <vector>

#include "Client.hpp"

class CgiManager : public AManager {
private:
	// variables
	std::vector<std::string> _env;
	const Request &_request;
	std::string _scriptPath;
	std::string _output;
	Client &_client;

	pid_t _pid;
	int _stdinFd;
	bool _timedOut;

	// function
	void buildEnv();
	char **envToCharArray() const; // used by function execv

public:
	// constructor
	CgiManager(Client &client, const std::string &scriptPath, const bool writter);
	CgiManager(Client &client, const std::string &scriptPath);
	~CgiManager();

	// static function
	static bool isCgi(const std::string& path);

	// function
	bool start();

	void pollInHandler();
	void pollOutHandler();

	void onTimeout();

	// getter
	std::string getOutput() const { return _output; }
};
