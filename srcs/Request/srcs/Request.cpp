/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nofanizz <nofanizz@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/31 11:01:51 by nofanizz          #+#    #+#             */
/*   Updated: 2026/01/31 11:27:18 by nofanizz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Request.hpp"
#include <sstream>
#include <sys/socket.h>

void Request::RequestReading(int &fd, bool &closedStatus, std::string &request)
{
    char buffer[BUFFER_SIZE];
    int n = recv(fd, buffer, sizeof(buffer) - 1, 0);

    if (n <= 0) {
        closedStatus = true;
        return;
    }
    buffer[n] = '\0';
    request.append(buffer, n);
}
bool    Request::IsComplete(std::string &request)
{
    if(request.find("\r\n\r\n", 0) != std::string::npos)
        return(true);
    return(false);
}

// split lines and stores them into a Vector<string>
// remove the `/n` at the end of each
// keeps empty lines "" for better request understanding
static std::vector<std::string> splitLines(const std::string &request)
{
	std::vector<std::string> allLines;
	std::string line;

	for (size_t i = 0; i < request.size(); ++i)
	{
		if (request[i] == '\n')
		{
			allLines.push_back(line);
			line.clear();
		}
		else if (request[i] != '\r')
			line.push_back(request[i]);
	}
	if (!line.empty())
		allLines.push_back(line);
	return allLines;
}

// Parse the raw request and return a struct Request
// TODO: parse for queries
Request Request::Parse(const std::string &raw)
{
	Request req;
	std::vector<std::string> lines = splitLines(raw); // separate each line

	if (lines.empty())
		return req;

	std::istringstream iss(lines[0]);
	iss >> req.method >> req.path >> req.version; // `>>` split a chaque espace
	// put each args into the corresponding Request variable

	// Headers
	size_t i = 1; // index declared here so we can keep it even after the forloop
	for (; i < lines.size(); ++i)
	{
		if (lines[i].empty()) // if line empty break and go below to body
			break;
		size_t colon = lines[i].find(':'); // find `:` separation
		if (colon == std::string::npos) // if no `:` character goes to next line
			continue;

		std::string key = lines[i].substr(0, colon); // take only the var name
		std::string val = lines[i].substr(colon + 1); // take the value (with the space before)

		size_t start = val.find_first_not_of(" \t");
		// Find the first character in val that is not a space and not a tab

		if (start != std::string::npos) // check if character has been found
		{
			val = val.substr(start);
			// if yes, cut the space or tab before the value
		}
		req.headers[key] = val; // stores key and value into the <map> headers
	}

	// Body
	++i; // move index to first body line
	for (; i < lines.size(); ++i)
	{
		// add remaining line into body and put a `/n` at the end of each except the last one
		req.body += lines[i];
		if (i + 1 < lines.size())
			req.body += "\n";
	}

	return req;
}
