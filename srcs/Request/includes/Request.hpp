/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mvachon <mvachon@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/31 11:01:00 by nofanizz          #+#    #+#             */
/*   Updated: 2026/01/31 15:34:08 by mvachon          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef REQUEST_HPP
#define REQUEST_HPP

#include <string>
#include <vector>
#include <map>

#define BUFFER_SIZE 50

class Request
{
private:
	std::string _method;						 // GET POST DELETE
	std::string _path;							 // file
	std::string _query;							 // args for cgi script
	std::string _version;						 // HTTP/1.1 usually
	std::map<std::string, std::string> _headers; // headers
	std::string _body;							 // body content

public:
	Request() {}
	~Request() {}

	void CheckRequest();
	void Parse(const std::string &raw);

	bool IsComplete(std::string &request);
	void RequestReading(int &fd, bool &closedStatus, std::string &request);

	std::string GetPath() const { return _path; }
	std::string GetMethod() const { return _method; }
	std::string GetVersion() const { return _version; }
	std::string GetHeaders(const std::string toGet) const;
	std::string GetBody() const { return _body; }

	void SetPath(std::string &str) { _path = str; }
};

#endif
