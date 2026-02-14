#ifndef CLIENT_HPP
#define CLIENT_HPP

#include "AManager.hpp"
#include "Config.hpp"
#include "Request.hpp"
#include <string>

class Client : public AManager {
  private:
	// variables
	const ServerConfig _config;
	std::string _rawRequest;
	Request _request;

	// functions
	bool _requestcomplete;

  public:
	// constructor
	Client(int fd, const ServerConfig &config);
	Client() {};
	~Client() {};

	// functions
	void updateRequest(std::string &buffer, int n);
	void handleRequestReception();
	void PollInHandler();
	void PollOutHandler();

	// getter
	std::string GetRawRequest() { return _rawRequest; }
};

#endif
