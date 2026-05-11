#ifndef CLIENT_HPP
#define CLIENT_HPP

#include "AManager.hpp"
#include "Config.hpp"
#include "Request.hpp"
#include "Response.hpp"
#include <string>
#include <vector>

class Client : public AManager {
  private:
	// variables
	const std::vector<ServerConfig> _configs;
	ServerConfig _activeConfig;
	std::string _rawRequest;
	std::string _sendBuffer;
	size_t _sendOffset;
	Request _request;
	bool _requestEnded;
	bool _timedOut;
	bool _cgi;
	// functions

	Response _response;

  public:
	// constructor
	Client(int fd, const std::vector<ServerConfig> &configs);
	Client() {};
	~Client() {};

	// functions
	bool isTimeout(time_t timeNow);
	void onTimeout();
	void updateRequest(std::string &buffer, int n);
	void handleRequestReception();
	void pollInHandler();
	void pollOutHandler();

	// getter
	Request &getRequest() { return _request; }
	std::string GetRawRequest() { return _rawRequest; }

	// setter
	void setCgiOutput(const std::string &output, int error);
};

#endif
