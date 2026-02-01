/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mvachon <mvachon@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/31 11:01:51 by nofanizz          #+#    #+#             */
/*   Updated: 2026/01/31 12:22:59 by mvachon          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Request.hpp"
#include "ManageAll.hpp"
#include <sstream>
#include <sys/socket.h>
#include <iostream>

void Request::RequestReading(int &fd, bool &closedStatus, std::string &request)
{
	char buffer[BUFFER_SIZE];
	int n = recv(fd, buffer, sizeof(buffer) - 1, 0);

	if (n <= 0)
	{
		closedStatus = true;
		return;
	}
	buffer[n] = '\0';
	request.append(buffer, n);
}

bool Request::IsComplete(std::string &request)
{
	if (request.find("\r\n\r\n", 0) != std::string::npos)
		return (true);
	return (false);
}

void Request::CheckRequest()
{
	if (_method != "GET" && _method != "POST" && _method != "DELETE")
		ManageAll::SetError405(true);
	if (_version != "HTTP/1.1" && _version != "HTTP/1.0")
		ManageAll::SetError400(true);
}
// helper function for parsing
static std::map<std::string, std::string> SeparateHeaders(std::vector<std::string> &docRequest)
{
	std::map<std::string, std::string> headers;

	for (size_t i = 1; i < docRequest.size(); ++i)
	{
		std::string &line = docRequest[i];

		if (line.empty())
			break;

		size_t colonPos = line.find(':');
		if (colonPos == std::string::npos)
			continue;

		std::string key = line.substr(0, colonPos);
		std::string value = line.substr(colonPos + 1);

		size_t start = value.find_first_not_of(" \t");
		if (start != std::string::npos)
			value = value.substr(start);

		headers[key] = value;
	}

	return headers;
}

// parse request
void Request::Parse(const std::string &request)
{
	std::string line;
	std::vector<std::string> docRequest;

	for (size_t i = 0; i < request.size(); ++i)
	{
		char c = request[i];
		if (c == '\n')
		{
			docRequest.push_back(line);
			line.clear();
		}
		else if (c != '\r')
		{
			line.push_back(c);
		}
	}

	if (!line.empty())
		docRequest.push_back(line);

	if (docRequest.empty())
		return;

	_headers = SeparateHeaders(docRequest);

	std::istringstream iss(docRequest[0]);
	iss >> _method >> _path >> _version;

	size_t queryCheck = _path.find("?"); // checks for queries
	if (queryCheck != std::string::npos)
	{
		std::string newPath = _path.substr(0, queryCheck);
		_query = _path.substr(queryCheck + 1);
		_path = newPath;
	}

	CheckRequest();
}

std::string Request::GetHeaders(const std::string toGet) const
{
	std::map<std::string, std::string>::const_iterator it;
	it = _headers.find(toGet);
	if (it != _headers.end())
		return it->second;
	return "";
}
