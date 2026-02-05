/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mvachon <mvachon@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/31 11:01:00 by nofanizz          #+#    #+#             */
/*   Updated: 2026/02/05 08:58:27 by mvachon          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef REQUEST_HPP
#define REQUEST_HPP

#include <string>
#include <vector>
#include <map>

#define BUFFER_SIZE 5

class BodyRequest
{
private:
    std::string _body;
    std::string _filename;
    std::string _type;

public:
    BodyRequest() {}
    BodyRequest(const std::string& body,
                const std::string& filename,
                const std::string& type)
        : _body(body), _filename(filename), _type(type) {}

    const std::string& getBody() const { return _body; }
    const std::string& getFilename() const { return _filename; }
    const std::string& getType() const { return _type; }
};


class Request
{
private:
	std::string _method;  // GET POST DELETE
	std::string _path;	  // file
	std::string _query;	  // args for cgi script
	std::string _version; // HTTP/1.1 usually
	size_t _contentLengthBody;
	std::string _WebKitForm;
	std::map<std::string, std::string> _headers; // headers
	std::vector<BodyRequest> _bodyRequests;		 // body content

public:
	Request() : _contentLengthBody(-1) {}
	~Request() {}

	void CheckRequest();
	void Parse(const std::string &raw);
	void PrintDebug() const;
	bool IsComplete(const std::string &req);
	void parseContentLength(const std::string &req);
	void parseWebKitForm(const std::string &req);
	void ParsePostMethod(const std::string &request, size_t body_start);
	void RequestReading(int &fd, bool &closedStatus, std::string &request);

	std::string GetPath() const { return _path; }
	std::string GetMethod() const { return _method; }
	std::string GetVersion() const { return _version; }
	std::string GetHeaders(const std::string toGet) const;

	void SetPath(const std::string &str) { _path = str; }
};

std::vector<std::string> split(const std::string &s, const std::string &delim);

#endif
