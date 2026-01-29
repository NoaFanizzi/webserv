/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mvachon <mvachon@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/22 11:47:07 by nofanizz          #+#    #+#             */
/*   Updated: 2026/01/29 14:14:47 by mvachon          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"
#include "RequestParser.hpp"
#include "ManageAll.hpp"
#include <fstream>
#include <sstream>

Client::Client(int fd, const ServerConfig &config): _config(config)
{
    _fd = fd;
    _closedStatus = false;
    _events = POLLIN;
    ManageAll::pollFdCreation(_fd, this);
}


void Client::PollInHandler()
{
    char buffer[4096];
    int n = recv(_fd, buffer, sizeof(buffer) - 1, 0);
    
    if (n <= 0) {
        _closedStatus = true;
        return;
    }
	
    buffer[n] = '\0';
    _request.append(buffer, n);
    _events = POLLOUT;
    _RequestParser.ParseRequest(_request);
}

std::string readFileTest(const std::string& path)
{
    std::ifstream file(path.c_str());
    if (!file.is_open())
        return "";

    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

#include "Http404Exception.hpp"
#include "Http403Exception.hpp"

std::string Client::CheckUrl()
{
    std::string path;

    if (_RequestParser.GetUrl() == "/")
        path = _config.root + "/" + _config.index;
    else
        path = _config.root + _RequestParser.GetUrl();

    if (access(path.c_str(), F_OK) != 0)
        throw Http404Exception();
        
    if (access(path.c_str(), R_OK) != 0)
        throw Http403Exception();

    return path;
}


void Client::PollOutHandler()
{
    std::string body;
    std::string statusCode = "200";
    std::string statusText = "OK";

    try {
        std::string path = CheckUrl();
        body = readFileTest(path);
    }
    catch (const HttpException& e) {
        body = readFileTest(e.getPage());

        std::ostringstream oss;
        oss << e.getStatusCode();
        statusCode = oss.str();
        statusText = e.getStatusText();
    }

    std::string header = GetHeaderResponse(body.size(), statusCode, statusText);

    send(_fd, (header + body).c_str(), header.size() + body.size(), 0);

    _events = 0;
    _closedStatus = true;
}
