/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mvachon <mvachon@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/22 11:47:07 by nofanizz          #+#    #+#             */
/*   Updated: 2026/01/28 20:26:56 by mvachon          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"
#include "RequestParser.hpp"
#include "ManageAll.hpp"
#include <fstream>
#include <sstream>

Client::Client(int fd)
{
	_closedStatus = false;
	_fd = fd;
	ManageAll::pollFdCreation(_fd, this);
	_events = POLLIN;
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

void Client::PollOutHandler()
{
    std::string body = readFileTest("website/index.html");
    std::ostringstream oss;
    oss << body.size();

    std::string header =
        "HTTP/1.0 200 OK\r\n"
        "Content-Type: text/html; charset=UTF-8\r\n"
        "Content-Length: " + oss.str() + "\r\n"
        "Connection: close\r\n"
        "\r\n";


    std::string response = header + body;

    send(_fd, response.c_str(), response.size(), 0);
    _events = 0;
    _closedStatus = true;
}
