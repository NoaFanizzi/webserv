#pragma once

#include <string>
#include <vector>
#include <map>

#define BUFFER_SIZE 2

class Request
{
	private:
			std::string method;		// GET POST DELETE
			std::string path;		// file
			std::string query;		// args for cgi script
			std::string version;	// HTTP/1.1 usually
			std::map<std::string, std::string> headers;	//headers
			std::string body;		// body content
			std::vector<std::string> _rawRequest;
	public:
			Request Parse(const std::string &raw);
        	bool 	IsComplete(std::string &request);
        	void 	RequestReading(int &fd, bool &closedStatus, std::string &request);

			//getters
			std::string GetPath() { return(this->path);}

			//setters
			void SetPath(std::string &str) { path = str;}
};
