#ifndef REQUEST_HPP
#define REQUEST_HPP

#include <map>
#include <string>
#include <vector>
#include "Config.hpp"

#define BUFFER_SIZE 5

class BodyRequest {
  private:
	// variables
	std::string _body;
	std::string _filename;
	std::string _type;

  public:
	// constructor
	BodyRequest() {}
	BodyRequest(const std::string &body, const std::string &filename,
	            const std::string &type)
	    : _body(body), _filename(filename), _type(type) {}

	// getter
	const std::string &getBody() const { return _body; }
	const std::string &getFilename() const { return _filename; }
	const std::string &getType() const { return _type; }
};

class Request {
  private:
	std::string _method;  // GET POST DELETE
	std::string _path;    // file
	std::string _query;   // args for cgi script
	std::string _version; // HTTP/1.1 usually
	size_t _contentLengthBody;
	std::string _webKitForm;
	std::map<std::string, std::string> _headers; // headers
	std::vector<BodyRequest> _bodyRequests;      // body content
	std::vector<LocationConfig> _currentLocations; // matching locations

  public:
	Request() : _contentLengthBody(-1) {}
	~Request() {}

	void readRaw(int &fd, bool &closedStatus, std::string &request);
	bool isValid(const std::string &req);
	void checkRequest();
	void parse(const std::string &request, const ServerConfig &config);
	void printDebug() const;
	void parseContentLength(const std::string &req);
	void parseWebKitForm(const std::string &req);
	void parsePostMethod(const std::string &request, size_t body_start);
	void setCurrentLocations(const ServerConfig &serverConfig);

	std::string getMethod() const { return _method; }
	std::string getPath() const { return _path; }
	std::string getQuery() const { return _query; }
	std::string getVersion() const { return _version; }
	std::string getHeaders(const std::string toGet) const;
	const std::vector<LocationConfig> &getCurrentLocations() const { return _currentLocations; }

	void setPath(const std::string &str) { _path = str; }
};

std::vector<std::string> split(const std::string &str, const std::string &delimiter);

#endif
