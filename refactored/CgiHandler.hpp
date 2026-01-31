#pragma once

#include <string>
#include <vector>
#include "Request.hpp"

class CgiHandler
{
private:
	// variables
	std::vector<std::string> _env;
	const Request &_request;
	std::string _scriptPath;
	std::string _output;

	// function
	void buildEnv();
	char **envToCharArray(); // used by function execv

public:
	// constructor
	CgiHandler(const Request &req, const std::string &scriptPath);
	~CgiHandler();

	// static function
	static bool isCgi(std::string path);

	// function
	bool execute();

	// getter
	std::string getOutput() const;
};
