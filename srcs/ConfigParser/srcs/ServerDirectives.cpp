/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerDirectives.cpp                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mvachon <mvachon@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/28 10:30:00 by mvachon           #+#    #+#             */
/*   Updated: 2026/03/26 12:30:45 by mvachon          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Config.hpp"
#include <sstream>
#include <ostream>

void Config::parseServerDirective(ServerConfig &server, const std::string &key, 
                                   const std::vector<std::string> &line, size_t j)
{
    if (j + 1 >= line.size())
        throw Exception("No argument found for -> " + key);

    std::string value = extractValue(line, j, key);
    
    if (key == "port")
        parsePort(server, value);
    else if (key == "host")
        parseHost(server, value);
    else if (key == "root")
        server.root = value;
    else if (key == "index")
        server.index = value;
    else if (key == "client_max_body_size")
        parseClientMaxBodySize(server, value);
    else if (key == "autoindex")
        parseAutoindex(server, value);
    else if (key == "allow_methods")
        parseAllowMethods(server.allowed_methods, line, j);
    else if (key == "error_page")
        parseErrorPage(server, line, j);
    else if (key == "upload_dir")
        server.upload_dir = value;
    else if (key == "server_name")
        parseServerName(server, line, j);
}

void Config::parseServerName(ServerConfig &server, const std::vector<std::string> &line, size_t j)
{
    bool semicolon = false;

    for (size_t x = j + 1; x < line.size(); x++)
    {
        std::string name = line[x];

        if (name == ";")
        {
            semicolon = true;
            break;
        }
        if (!name.empty() && name[name.size() - 1] == ';')
        {
            name = name.substr(0, name.size() - 1);
            semicolon = true;
        }
        if (!name.empty())
            server.server_names.push_back(name);
        if (semicolon)
            break;
    }
    if (!semicolon)
        throw Exception("No semicolon on the line -> server_name");
}

std::string Config::extractValue(const std::vector<std::string> &line, size_t j, 
                                  const std::string &key)
{
    bool semicolon = false;
    std::string value = line[j + 1];
    if (_keysServer->find(key) && key != "error_page" && key != "allow_methods" && key != "server_name")
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

    if (!semicolon && key != "error_page" && key != "allow_methods" && key != "server_name")
        throw Exception("No semicolon on the line -> " + key);

    if (value.empty() && key != "error_page" && key != "allow_methods")
        throw Exception("Empty value for -> " + key);

    return value;
}

void Config::parseHost(ServerConfig &server, const std::string &value)
{
    if (value == "localhost")
    {
        server.host = value;
        return;
    }

    std::istringstream iss(value);
    std::string token;
    int parts = 0;

    while (std::getline(iss, token, '.'))
    {
        if (token.empty() || token.size() > 3)
            throw Exception("Invalid host format -> " + value);
        for (size_t i = 0; i < token.size(); i++)
            if (!isdigit(token[i]))
                throw Exception("Invalid host format -> " + value);
        std::istringstream num(token);
        int n;
        num >> n;
        if (n < 0 || n > 255)
            throw Exception("Invalid host format -> " + value);
        parts++;
    }
    if (parts != 4)
        throw Exception("Invalid host format -> " + value);
    server.host = value;
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

    bool semicolon = false;
    if (!path.empty() && path[path.size() - 1] == ';')
    {
        path = path.substr(0, path.size() - 1);
        semicolon = true;
    }
    if (!semicolon)
        throw Exception("No semicolon on the line -> error_page");

    ErrorPage page;
    page.index = static_cast<int>(code);
    page.path = path;

    server.error_page.push_back(page);
}
