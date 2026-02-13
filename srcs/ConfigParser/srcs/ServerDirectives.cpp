/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerDirectives.cpp                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mvachon <mvachon@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/28 10:30:00 by mvachon           #+#    #+#             */
/*   Updated: 2026/01/28 11:14:18 by mvachon          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Config.hpp"
#include <sstream>

void Config::parseServerDirective(ServerConfig &server, const std::string &key, 
                                   const std::vector<std::string> &line, size_t j)
{
    if (j + 1 >= line.size())
        throw Exception("No argument found for -> " + key);

    std::string value = extractValue(line, j, key);
    
    if (key == "port")
        parsePort(server, value);
    else if (key == "host")
        server.host = value;
    else if (key == "root")
        server.root = value;
    else if (key == "index")
        server.index = value;
    else if (key == "client_max_body_size")
        parseClientMaxBodySize(server, value);
    else if (key == "autoindex")
        parseAutoindex(server, value);
    else if (key == "error_page")
        parseErrorPage(server, line, j);
}

std::string Config::extractValue(const std::vector<std::string> &line, size_t j, 
                                  const std::string &key)
{
    bool semicolon = false;
    std::string value = line[j + line.size() - 1];

    if (_keysServer->find(key) && key != "error_page")
    {
        if (line.size() > 2)
        {
            if (line[2] != ";" || line.size() > 3)
                throw Exception("Too much value for -> " + key);
            else
                semicolon = true;
        }
    }

    if (!value.empty() && value[value.size() - 1] == ';')
    {
        value = value.substr(0, value.size() - 1);
        semicolon = true;
    }

    if (!semicolon && key != "error_page")
        throw Exception("No semicolon on the line -> " + key);

    return value;
}

void Config::parsePort(ServerConfig &server, const std::string &value)
{
    std::istringstream iss(value);
    long long tmp;
    
    if (!(iss >> tmp) || !iss.eof())
        throw Exception("Invalid port value -> " + value);

    if (tmp < 0 || tmp > 65535)
        throw Exception("Invalid port range [0-65535] -> " + value);

    server.port = static_cast<int>(tmp);
}

void Config::parseClientMaxBodySize(ServerConfig &server, const std::string &value)
{
    std::istringstream iss(value);
    long long tmp;
    
    if (!(iss >> tmp) || !iss.eof())
        throw Exception("Invalid client_max_body_size -> " + value);
    
    if (tmp < 0)
        throw Exception("client_max_body_size must be positive -> " + value);

    server.client_max_body_size = tmp;
}

void Config::parseAutoindex(ServerConfig &server, const std::string &value)
{
    if (value == "on" || value == "off")
        server.autoindex = (value == "on");
    else
        throw Exception("Invalid value for autoindex [on-off] -> " + value);
}

void Config::parseErrorPage(ServerConfig &server, const std::vector<std::string> &line, size_t j)
{
    if (j + 2 >= line.size())
        throw Exception("error_page requires code and path");

    std::string path = line[j + line.size() - 1];
    std::istringstream iss(line[j + line.size() - 2]);
    long long code;
    
    if (!(iss >> code) || !iss.eof())
        throw Exception("Invalid error_page code -> " + line[j + 1]);

    if (code < 400 || code > 599)
        throw Exception("error_page out of range [400-599] -> " + line[j + 1]);

    if (!path.empty() && path[path.size() - 1] == ';')
        path = path.substr(0, path.size() - 1);

    ErrorPage page;
    page.index = static_cast<int>(code);
    page.path = path;

    server.error_page.push_back(page);
}
