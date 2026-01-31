#include "CgiHandler.hpp"
#include <cstring>
#include <unistd.h>
#include <sys/wait.h>

// constructor
CgiHandler::CgiHandler(const Request &req, const std::string &scriptPath)
	: _request(req), _scriptPath(scriptPath) {}

CgiHandler::~CgiHandler() {}

// getter
std::string CgiHandler::getOutput() const { return _output; }

// static function
bool CgiHandler::isCgi(std::string path)
{
	// TODO add file format that we could manage
	// 2 file formats is the minimum for bonus

	// TODO find the .py/.php before the `?` if there is one
	if (path.rfind(".py") != std::string::npos ||
		path.rfind(".php") != std::string::npos)
		return true;
	return false;
}

// functions
void CgiHandler::buildEnv()
{
	_env.push_back("REQUEST_METHOD=" + _request.method);
	_env.push_back("SCRIPT_FILENAME=" + _scriptPath);
	_env.push_back("SCRIPT_NAME=" + _request.path);
	//_env.push_back("QUERY_STRING=" + _request.query); // TODO: parse for query
	_env.push_back("CONTENT_LENGTH=" + headerFind(_request.headers, "Content-Lenght"));
	_env.push_back("CONTENT_TYPE=" + headerFind(_request.headers, "Content-Type"));
	_env.push_back("SERVER_PROTOCOL=HTTP/1.1");
	_env.push_back("GATEWAY_INTERFACE=CGI/1.1");
}

char **CgiHandler::envToCharArray()
{
	char **envp = new char *[_env.size() + 1];
	if (!envp)
		return NULL;
	for (int i = 0; i < _env.size(); i++)
	{
		envp[i] = strdup(_env[i].c_str());
		if (!envp[i])
		{
			while (--i != -1)
				delete envp[i];
			delete envp;
			return NULL;
		}
	}
	return envp;
}

// execute the cgi script
// dup STDIN to pass it the request body if method == POST
// return the output of the executed script
bool CgiHandler::execute()
{
	buildEnv();

	int fdIn[2];
	int fdOut[2];
	pid_t pid;

	if (pipe(fdIn) < 0 || pipe(fdOut) < 0)
		return false;

	if ((pid = fork()) < 0)
	{
		close(fdIn[0]);
		close(fdIn[1]);
		close(fdOut[0]);
		close(fdOut[1]);

		return false;
	}

	if (pid == 0)
	{
		if (dup2(fdIn[0], STDIN_FILENO) < 0)
		{
			close(fdIn[1]);
			close(fdOut[0]);
			exit (false);
		}
		if (dup2(fdOut[1], STDOUT_FILENO) < 0)
		{
			close(fdIn[1]);
			close(fdOut[0]);
			exit (false);
		}
		close(fdIn[1]);
		close(fdOut[0]);

		char *argv[] = {(char *)_scriptPath.c_str(), NULL};
		execve(_scriptPath.c_str(), argv, envToCharArray());
		perror("execve failed: ");
		exit (1);
	}

	close(fdIn[0]);
	close(fdOut[1]);

	if (_request.method == "POST") // write the request body in STDIN
	{
		const std::string& body = _request.body;
		if (!body.empty())
			write(fdIn[1], body.c_str(), body.size());
	}
	close(fdIn[1]);

	char buffer[1024];
	ssize_t readSize;
	while ((readSize = read(fdOut[0], buffer, sizeof(buffer) - 1)) > 0)
	{
		buffer[readSize] = 0;
		_output += buffer;
	}

	close(fdOut[0]);
	waitpid(pid, NULL, 0);
	return true;
}
