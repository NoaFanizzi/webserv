#include "Request.hpp"
#include <string>
#include <sstream>
struct ServerConfig;

class Response {
  private:
	// variables
	std::string _statusCode;
	std::string _statusText;
	std::string _header;
	std::string _body;
	Request	*_request;
	std::string _finalPath;
	std::map<int, std::string> _errorPages;
	std::map<std::string, std::string> _mimeTypes;
	bool _isCgi;

	// functions
	std::string checkUrl(const ServerConfig &config);
	std::string readFile(const std::string &path);
	std::string buildHeader(size_t contentLength, const std::string &statusCode,
	const std::string &statusText);
	
	void setMimes();
	void setErrorPages();
	
  public:
	// constructor
	Response() : _isCgi(false) {};
	void setRequest(Request &request);
	std::string getErrorPageContent(int code, const ServerConfig &config);
	void setStatusCode(const std::string &code) { _statusCode = code; }
	void setStatusText(const std::string &text) { _statusText = text; }
	void setHeader(const std::string &header) { _header = header; }
	void setFinalPath(const std::string &finalPath) { _finalPath = finalPath; }
	void setBody(const std::string &body) { _body = body; }
	void setIsCgi(bool value) { _isCgi = value; }
	void buildErrorHeader();
	// functions
	void generate(const ServerConfig &config);
	std::string getFullResponse() { 
		if (_isCgi)
			return _body;
		return _header + _body; 
	}
};
