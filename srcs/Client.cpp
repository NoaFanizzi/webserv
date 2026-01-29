/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mvachon <mvachon@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/22 11:47:07 by nofanizz          #+#    #+#             */
/*   Updated: 2026/01/29 11:17:13 by mvachon          ###   ########.fr       */
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

std::string Client::CheckUrl()
{
    if (access((_config.root + "/" + _config.index).c_str(), F_OK))
        return (_config.root + "/" + _config.index);
    return "website/error404NotFound.html";
}

void Client::PollOutHandler()
{
    std::string path = _config.root + "/" + _config.index; 
    std::string body = readFileTest(path);
    std::string statusCode = "200";
    std::string statusText = "OK";
    
    if (body.empty() || _RequestParser.GetUrl() != "/") {
        body = readFileTest("website/error404NotFound.html");
        statusCode = "404";
        statusText = "Not Found";
    }

    std::string header = GetHeaderResponse(body.size(), statusCode, statusText);

    std::string response = header + body;

    send(_fd, response.c_str(), response.size(), 0);
    _events = 0;
    _closedStatus = true;
}
