#ifndef CLIENT_HPP
#define CLIENT_HPP

#include "AManager.hpp"
#include "Config.hpp"
#include "Request.hpp"
#include "Response.hpp"
#include <string>

class Client : public AManager {
  private:
	// variables
	const ServerConfig _config;
	std::string _rawRequest;
	Request _request;
	bool _firstLoopRequets;
	bool _requestEnded;
	bool _timedOut;
	bool _cgi;
	// functions

	Response _response;

  public:
	// constructor
	Client(int fd, const ServerConfig &config);
	Client() {};
	~Client() {};

	// functions
	bool isTimeout(time_t timeNow);
	void onTimeout();
	void updateRequest(std::string &buffer, int n);
	void handleRequestReception();
	void PollInHandler();
	void PollOutHandler();

	// getter
	Request &getRequest() { return _request; }
	std::string GetRawRequest() { return _rawRequest; }

	// setter
	void setCgiOutput(const std::string &output, int error);
};

#endif
