/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nofanizz <nofanizz@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/31 11:01:51 by nofanizz          #+#    #+#             */
/*   Updated: 2026/02/21 12:56:26 by nofanizz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Request.hpp"
#include "HttpExceptions.hpp"
#include "Config.hpp"
#include <cstdlib>
#include <fcntl.h>
#include <iostream>
#include <sstream>
#include <sys/socket.h>
#include <unistd.h>

void Request::readRaw(int &fd, bool &closedStatus,
                             std::string &request) {
	char buffer[BUFFER_SIZE];
	int n = recv(fd, buffer, sizeof(buffer) - 1, 0);

	if (n <= 0) {
		closedStatus = true;
		return;
	}
	buffer[n] = '\0';
	request.append(buffer, n);
}

void Request::parseContentLength(const std::string &req) {
	size_t pos = req.find("Content-Length:");
	if (pos == std::string::npos) {
		_contentLengthBody = 0;
		return;
	}

	pos += 15;
	while (pos < req.size() && req[pos] == ' ')
		pos++;

	size_t end = req.find("\r\n", pos);
	if (end == std::string::npos)
		return;

	_contentLengthBody = std::atoi(req.substr(pos, end - pos).c_str());
}

void Request::parseWebKitForm(const std::string &headers) {
	size_t ct = headers.find("Content-Type:");
	if (ct == std::string::npos)
		return;

	size_t lineEnd = headers.find("\r\n", ct);
	if (lineEnd == std::string::npos)
		throw Http400Exception();

	std::string line = headers.substr(ct, lineEnd - ct);

	size_t b = line.find("boundary=");
	if (b == std::string::npos)
		throw Http400Exception();

	_webKitForm = line.substr(b + 9);
}

bool Request::isValid(const std::string &req) {
	size_t header_end = req.find("\r\n\r\n");
	if (header_end == std::string::npos)
		return false;

	if (_method.empty()) {
		if (req.compare(0, 3, "GET") == 0)
			_method = "GET";
		else if (req.compare(0, 4, "POST") == 0)
			_method = "POST";
	}
	if (_method == "GET")
		return true;

	if (_method == "POST") {
		if (_contentLengthBody == static_cast<size_t>(-1)) {
			parseContentLength(req);
			parseWebKitForm(req);
		}
		size_t body_size = req.size() - (header_end + 4);
		return body_size >= static_cast<size_t>(_contentLengthBody);
	}

	return false;
}

void Request::checkRequest() {
	if (_method != "GET" && _method != "POST" && _method != "DELETE")
		throw Http405Exception();
	if (_version != "HTTP/1.1" && _version != "HTTP/1.0")
		throw Http400Exception();
	if (_path.empty())
		throw Http400Exception();
}

// helper function for parsing
static std::map<std::string, std::string>
separateHeaders(std::vector<std::string> &docRequest) {
	std::map<std::string, std::string> headers;

	for (size_t i = 1; i < docRequest.size(); ++i) {
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

std::vector<std::string> split(const std::string &str, std::string delimiters)
{
    std::vector<std::string> result;
    size_t             start = 0;
    size_t             end   = 0;
    std::string         token;

    while (end != std::string::npos)
    {
        end = str.find_first_of(delimiters, start);
        token = str.substr(start, end - start);
        if (!token.empty())
            result.push_back(token);
        start = (end == std::string::npos) ? end : end + 1;
    }
    return result;
}

void printSplitDebug(const std::vector<std::string> &v)
{
	for (size_t i = 0; i < v.size(); ++i) {
		std::cout << "[" << i << "] ";
		for (size_t j = 0; j < v[i].size(); ++j) {
			if (v[i][j] == '\r')
				std::cout << "\\r";
			else
				std::cout << v[i][j];
		}
		std::cout << std::endl;
	}
}

void Request::printDebug() const
{
	std::cout << "===== BODY DEBUG =====" << std::endl;
	std::cout << "Nb body parts: " << _bodyRequests.size() << std::endl;

	for (size_t i = 0; i < _bodyRequests.size(); ++i) {
		std::cout << "\n[Part " << i << "]" << std::endl;
		std::cout << "Type     : " << _bodyRequests[i].getType() << std::endl;
		std::cout << "Filename : " << _bodyRequests[i].getFilename()
		          << std::endl;
		std::cout << "Body :\n" << std::endl;
		std::cout << _bodyRequests[i].getBody() << std::endl;
	}

	std::cout << "======================" << std::endl;
}

void Request::parsePostMethod(const std::string &request, size_t body_start)
{
	std::vector<std::string> parts = split(request.substr(body_start, _contentLengthBody), "--" + _webKitForm);

	for (size_t i = 0; i + 1 < parts.size(); ++i) {
		std::string &part = parts[i];

		std::string type;
		std::string filename;
		std::string body;

		size_t pos = part.find("Content-Type:");
		if (pos != std::string::npos) {
			size_t end = part.find("\r\n", pos);
			type = part.substr(pos + 13, end - (pos + 13));
		}
		pos = part.find("filename=");
		if (pos != std::string::npos) {
			size_t end = part.find("\r\n", pos);
			filename = part.substr(pos + 9, end - (pos + 9));
			if (filename.size() >= 2 && filename[0] == '"' &&
			    filename[filename.size() - 1] == '"')
				filename = filename.substr(1, filename.size() - 2);
		}

		pos = part.find("\r\n\r\n");
		if (pos != std::string::npos)
			body = part.substr(pos + 4);
		if (filename.empty())
			continue;
		_bodyRequests.push_back(BodyRequest(body, filename, type));
		int fd = open(("upload/" + filename).c_str(),
		              O_WRONLY | O_CREAT | O_TRUNC, 0644);
		if (fd < 0)
			throw Http500Exception();
		write(fd, body.c_str(), body.size());
		close(fd);
	}
}

// parse request
void Request::parse(const std::string &request)
{
	std::string line;
	std::vector<std::string> docRequest;

	size_t body_start = request.find("\r\n\r\n");
	if (body_start == std::string::npos)
		return;
	body_start += 4; // 4 = \r\n\r\n

	if (_method == "POST" && _contentLengthBody > 0)
		parsePostMethod(request, body_start);
	else
		_bodyRequests.clear();

	for (size_t i = 0; i < request.size(); ++i) {
		char c = request[i];
		if (c == '\n') {
			docRequest.push_back(line);
			line.clear();
		} else if (c != '\r')
			line.push_back(c);
	}
	if (!line.empty())
		docRequest.push_back(line);

	if (docRequest.empty())
		return;

	_headers = separateHeaders(docRequest);

	std::istringstream iss(docRequest[0]);
	iss >> _method >> _path >> _version;

	size_t queryCheck = _path.find("?"); // checks for queries
	if (queryCheck != std::string::npos) {
		std::string newPath = _path.substr(0, queryCheck);
		_query = _path.substr(queryCheck + 1);
		_path = newPath;
	}

	checkRequest();
	// PrintDebug();
}

std::string Request::getHeaders(const std::string toGet) const
{
	std::map<std::string, std::string>::const_iterator it;
	it = _headers.find(toGet);
	if (it != _headers.end())
		return it->second;
	return "";
}

void Request::setCurrentLocations(const ServerConfig &serverConfig)
{
	size_t i = 0;
	size_t j = 0;
	std::vector<std::string> vPath;
	std::string slashed;
	std::vector<LocationConfig> serverLocations = serverConfig.locations;
	std::string concatened;
	
	std::cout << "JE SUIS DANS SETCURRENTLOCATIONS" << std::endl;
	vPath = split(_path, "/");
	while (i < vPath.size()) {
		concatened = concatened + '/' + vPath[i];
		slashed = concatened + '/';
		j = 0;
		while (j < serverLocations.size()) {
			if (concatened == serverLocations[j].path)
			{
				_currentLocations.push_back(serverLocations[j]);
				std::cout << "AAAAAAAAAAAAAAAAA_currentLocations = " << concatened << std::endl;
			}
			else if (slashed == serverLocations[j].path)
			{
				_currentLocations.push_back(serverLocations[j]);
				std::cout << "AAAAAAAAAAAAAAAAAAAAAAAA_currentLocations = " << slashed << std::endl;
				
			}
			j++;
		}
		i++;
	}
}
