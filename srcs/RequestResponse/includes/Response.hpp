#include "Request.hpp"
#include <string>

struct ServerConfig;

class Response {
  private:
	// variables
	std::string _statusCode;
	std::string _statusText;
	std::string _header;
	std::string _body;
	Request &_request;
	std::map<int, std::string> _errorPages;
	std::map<std::string, std::string> _mimeTypes;
	bool _isCgi;

	// functions
	std::string checkUrl(const ServerConfig &config);
	std::string readFile(const std::string &path);
	std::string getErrorPageContent(int code, const ServerConfig &config);
	std::string buildHeader(size_t contentLength, const std::string &statusCode,
	                        const std::string &statusText);

	void setMimes();
	void setErrorPages();

  public:
	// constructor
	Response(Request &request);

	// functions
	void generate(const ServerConfig &config);
	std::string getFullResponse() { 
		if (_isCgi)
			return _body;
		return _header + _body; 
	}
};
