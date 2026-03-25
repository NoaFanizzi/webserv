/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mvachon <mvachon@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/31 11:01:51 by nofanizz          #+#    #+#             */
/*   Updated: 2026/03/25 12:38:38 by mvachon          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Request.hpp"
#include "HttpExceptions.hpp"
#include "Config.hpp"
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <sstream>
#include <sys/socket.h>

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
	std::string value = req.substr(pos, end - pos);
	if(value.empty())
		throw Http400Exception();
	for(size_t i = 0; i < value.size(); i++)
	{
		if(!std::isdigit(value[i]))
			throw Http400Exception();
	}
	char *lastchar;
	long length = strtol(value.c_str(), &lastchar, 10);
	if(*lastchar != '\0' || length < 0)
		throw Http400Exception();
	_contentLengthBody = static_cast<size_t>(length);
}

void Request::parseWebKitForm(const std::string &headers) {
    size_t ct = headers.find("Content-Type:");
    if (ct == std::string::npos)
        return;

    size_t lineEnd = headers.find("\r\n", ct);
    if (lineEnd == std::string::npos)
        return;

    std::string line = headers.substr(ct, lineEnd - ct);

    size_t b = line.find("boundary=");
    if (b == std::string::npos)
        return;

    _webKitForm = line.substr(b + 9);
}

bool Request::isValid(const std::string &req) {
	size_t header_end = req.find("\r\n\r\n");
	if (header_end == std::string::npos)
		return false;
	if (_method.empty()) {
		if (req.compare(0, 4, "GET ") == 0)
			_method = "GET";
		else if (req.compare(0, 5, "POST ") == 0)
			_method = "POST";
		else if (req.compare(0, 7, "DELETE ") == 0)
			_method = "DELETE";
	}
	if (_method == "GET" || _method == "DELETE")
		return true;
	if (_method == "POST") {
		if (_contentLengthBody == static_cast<size_t>(-1)) {
			parseContentLength(req);
			parseWebKitForm(req);
		}
		size_t body_size = req.size() - (header_end + 4);
		return body_size >= static_cast<size_t>(_contentLengthBody);
	}
	//TODO
	// if (_path.size() > 2048)
    // 	throw Http414Exception();
	return false;
}

void Request::checkRequest() {
	if (_method != "GET" && _method != "POST" && _method != "DELETE")
		throw Http405Exception();
	if (_version != "HTTP/1.1")
		throw Http400Exception();
	if (_headers.find("Host") == _headers.end())
		throw Http400Exception();
	if(_method == "POST" && _headers.find("Content-Length") == _headers.end())
		throw Http411Exception(); // TODO c'est une exception 411 qu'i lfaut faire
	if (_path.empty())
		throw Http400Exception();
}

size_t	findSpaceLength(size_t pos, std::string line)
{
	size_t count = pos;
	while(line[pos] && isspace(line[pos]))
		pos++;
	count = pos - count;
	return(count);
}

size_t	findValueLength(size_t pos, std::string line)
{
	size_t count = pos;
	while(line[pos] && !(isspace(line[pos])))
		pos++;
	count = pos - count;
	return(count);
}

int	keyCheck(const std::string &key)
{
	size_t	i = 0;
	while(key[i])
		i++;
	if(i != key.size())
		return(1);
	return(0);
}

// helper function for parsing
static std::map<std::string, std::string>
separateHeaders(std::vector<std::string> &docRequest) {
    std::map<std::string, std::string> headers;

    for (size_t i = 1; i < docRequest.size(); ++i) {
        std::string line = docRequest[i];

        // 1. Nettoyage rapide du \r final s'il existe
        if (!line.empty() && line[line.size() - 1] == '\r')
            line.erase(line.size() - 1);

        if (line.empty())
            break;

        size_t colonPos = line.find(':');
        if (colonPos == std::string::npos)
            continue;

        // 2. Extraire la clé
        std::string key = line.substr(0, colonPos);
		if(isspace(key[key.size() - 1]))
			throw (Http400Exception());
        // 3. Extraire la valeur et trimmer les espaces au début et à la fin
        std::string value = line.substr(colonPos + 1);
        size_t first = value.find_first_not_of(" \t");
        if (first == std::string::npos)
		{
			throw (Http400Exception());
            //value = "";
        }
		else
		{
            size_t last = value.find_last_not_of(" \t");
            value = value.substr(first, (last - first + 1));
        }
        headers[key] = value;
    }
    return headers;
}

std::vector<std::string> split(const std::string &str, const std::string &delimiter)
{
    std::vector<std::string> result;
    size_t start = 0;
    size_t end = 0;

    while ((end = str.find(delimiter, start)) != std::string::npos)
    {
        std::string token = str.substr(start, end - start);
        if (!token.empty())
            result.push_back(token);
        start = end + delimiter.size();
    }
    std::string last = str.substr(start);
    if (!last.empty())
        result.push_back(last);
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
		std::ofstream ofs(("upload/" + filename).c_str(), std::ios::binary | std::ios::trunc);
		if (!ofs.is_open())
			throw Http500Exception();
		ofs.write(body.c_str(), body.size());
		if (ofs.fail())
			throw Http500Exception();
	}
}

// parse request
void Request::parse(const std::string &request, const ServerConfig &config)
{
	std::string line;
	std::vector<std::string> docRequest;

	size_t body_start = request.find("\r\n\r\n");
	if (body_start == std::string::npos)
		return;
	body_start += 4; // \r\n\r\n
	if (_method == "POST" && static_cast<long long>(_contentLengthBody) <= config.client_max_body_size )
		parsePostMethod(request, body_start);
	else
	{
		_bodyRequests.clear();
		if(static_cast<long long>(_contentLengthBody) > config.client_max_body_size)
			throw Http413Exception();
	}

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
	
	vPath = split(_path, "/");
	while (i < vPath.size()) {
		concatened = concatened + '/' + vPath[i];
		slashed = concatened + '/';
		j = 0;
		while (j < serverLocations.size()) {
			if (concatened == serverLocations[j].path)
				_currentLocations.push_back(serverLocations[j]);
			else if (slashed == serverLocations[j].path)
				_currentLocations.push_back(serverLocations[j]);
			j++;
		}
		i++;
	}
}
