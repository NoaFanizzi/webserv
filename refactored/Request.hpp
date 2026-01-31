#pragma once

#include <string>
#include <map>

// struct to store the request infos after parsing
// (`Request RequestParser::Parse()`)
struct Request
{
	std::string method;		// GET POST DELETE
	std::string path;		// file
	std::string query;		// args for cgi script
	std::string version;	// HTTP/1.1 usually
	std::map<std::string, std::string> headers;	//headers
	std::string body;		// body content
};

// helper function to find a header
std::string headerFind(const std::map<std::string, std::string> headers, const std::string toFind)
{
	std::map<std::string, std::string>::const_iterator it = headers.find(toFind);
	if (it != headers.end())
		return it->second;
	return "";
}
