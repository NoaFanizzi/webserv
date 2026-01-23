/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mvachon <mvachon@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/22 20:22:15 by mvachon           #+#    #+#             */
/*   Updated: 2026/01/23 08:48:21 by mvachon          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Config.hpp"
#include <cstdlib>

Config::Config()
{
    _keysServer[0] = "port";
    _keysServer[1] = "host";
    _keysServer[2] = "root";
    _keysServer[3] = "index";
    _keysServer[4] = "error_page";
    _keysServer[5] = "client_max_body_size";
    _keysServer[6] = "autoindex";

    _keysLocation[0] = "path";
    _keysLocation[1] = "root";
    _keysLocation[2] = "index";
    _keysLocation[3] = "autoindex";
    _keysLocation[4] = "allow_methods";

}

void Config::parseServer(ServerConfig &server, const std::string &key, const std::vector<std::string> &line, size_t j)
{
    if (j + 1 >= line.size())
        throw Exception("No argument found for: " + key);

    std::string value = line[j + 1];
    
    if (!value.empty() && value[value.size() - 1] == ';')
        value = value.substr(0, value.size() - 1);
    char* end;
    if (key == "port")
    {
        server.port = std::strtol(value.c_str(), &end, 10);
        if (*end != 0)
		    throw Exception("Invalid configuration: " + key);
	    else if (server.port < 0 || 65535 < server.port)
		    throw Exception("Invalid port range [0-65535]: " + key);
    }
    else if (key == "host")
        server.host = value;
    else if (key == "root")
        server.root = value;
    else if (key == "index")
        server.index = value;
    else if (key == "client_max_body_size")
    {
        server.client_max_body_size = std::strtoll(value.c_str(), &end, 10);
        if (*end != 0)
            throw Exception("Invalid client_max_body_size: " + value);
        if (server.client_max_body_size < 0)
            throw Exception("client_max_body_size must be positive");
    }
    else if (key == "autoindex")
    {
        if (value != "on" && value != "off")
            throw Exception("Invalid value for autoindex :" + value);
        server.autoindex = (value == "on");
    }
    else if (key == "error_page")
    {
        if (j + 2 >= line.size())
            throw Exception("error_page requires code and path");
        std::string path = line[j + 2];
        long code = (line[j + 1].c_str(), &end, 10);
        if (!path.empty() && path[path.size() - 1] == ';')
            path = path.substr(0, path.size() - 1);
        if (*end != 0)
            throw Exception("Invalid error_page number: " + value);
        if (code < 400 || code > 599)
            throw Exception("Value of error_page out of range [400-599]");
        ErrorPage page;
        page.index = code;
        page.path = path;
        server.error_page.push_back(page);
    }
}

void Config::parseKeyLocation(LocationConfig &location, const std::string &key, const std::vector<std::string> &line, size_t j)
{
    if (j + 1 >= line.size())
        throw Exception("No argument found for: " + key);

    std::string value = line[j + 1];
    
    if (!value.empty() && value[value.size() - 1] == ';')
        value = value.substr(0, value.size() - 1);

    if (key == "path")
        location.path = value;
    else if (key == "root")
        location.root = value;
    else if (key == "index")
        location.index = value;
    else if (key == "autoindex")
    {
        if (value == "on" || value == "off")
            location.autoindex = (value == "on");
        else
            throw Exception("Invalid value for autoindex: " + value);
    }
    else if (key == "allow_methods")
    {
        for (size_t x = j + 1; x < line.size(); x++)
        {
            std::string method = line[x];
            if (method == ";")
                break;
            if (!method.empty() && method[method.size() - 1] == ';')
                method = method.substr(0, method.size() - 1);
            if (method != "GET" && method != "POST" && method != "DELETE")
                throw Exception("The method is not valid [GET-POST-DELETE]");
            if (!method.empty())
                location.allowed_methods.push_back(method);
        }
    }
}

void Config::parseLocation(ServerConfig &server, size_t *i, size_t *j)
{
    LocationConfig location;
    size_t brace_level = 0;

    location.path = _fileContent[*i][*j + 1];
    if (location.path.empty() || location.path == "{")
        throw Exception("No argument found for the location path ");

    (*j)+=2;
    while (*i < _fileContent.size())
    {
        for (; *j < _fileContent[*i].size(); (*j)++)
        {
            std::string token = _fileContent[*i][*j];

            if (token == "{")
            { 
                brace_level++;
                continue;
            }
            else if (token == "}")
            {
                brace_level--;
                if (brace_level == 0)
                {
                    server.locations.push_back(location);
                    (*j)++;
                    return;
                }
                continue;
            }
            if (brace_level == 1)
            {
                for (size_t k = 0; k < 5; k++)
                    if (token == _keysLocation[k])
                        parseKeyLocation(location, token, _fileContent[*i], *j);
            }
        }
        *j = 0;
        (*i)++;
    }
}


void Config::newServer(size_t *i)
{
    ServerConfig server;
    size_t brace_level = 0;

    while (*i < _fileContent.size())
    {
        for (size_t j = 0; j < _fileContent[*i].size(); j++)
        {
            std::string str = _fileContent[*i][j];

            if (str == "{")
            {
                brace_level++;
                continue;
            }
            else if (str == "}")
            {
                brace_level--;
                if (brace_level == 0)
                {
                    _servers.push_back(server);
                    return;
                }
                continue;
            }
            for (size_t k = 0; k < 7; k++)
            {
                if (str == _keysServer[k] && brace_level == 1)
                    parseServer(server, _keysServer[k], _fileContent[*i], j);
            }
            if (str == "location" && brace_level == 1)
            {
                parseLocation(server, i, &j);
                break;
            }
        }
        (*i)++;
    }
}

int Config::getInfo()
{
    size_t i = 0;
    while (i < _fileContent.size())
    {
        if (!_fileContent[i].empty() && _fileContent[i][0] == "server")
            newServer(&i);
        else
            i++;
    }
    printServers();
    return 1;
}

std::vector< std::vector<std::string> > splitLinesWords(const std::string &content)
{
    std::vector< std::vector<std::string> > result;
    std::istringstream ss(content);
    std::string line;

    while (std::getline(ss, line))
    {
        std::vector<std::string> words;
        std::istringstream lineStream(line);
        std::string word;
        while (lineStream >> word)
            words.push_back(word);
        if (!words.empty())
            result.push_back(words);
    }
    return result;
}


std::string readFile(std::string doc)
{
    int fd = open(doc.c_str(), O_RDONLY);
    if (fd < 0)
        throw Exception("Error : can't open the file");
    std::string _fileContent;
    char buffer[1024];
    ssize_t bytes;
    
    while ((bytes = read(fd, buffer, sizeof(buffer))) > 0)
        _fileContent.append(buffer, bytes);
    close(fd);
    if (bytes < 0)
        throw Exception("Error : read issue");
    return _fileContent;
}

int Config::setFile(std::string doc)
{
    try
    {
        std::string content = readFile(doc);
        _fileContent = splitLinesWords(content);
        getInfo();
        return 1;
    }
    catch (const Exception& e)
    {
        std::cout << "Configuration Error: " << e.what() << std::endl;
        return 0;
    }
    catch (const std::exception& e)
    {
        std::cout << "Unexpected Error: " << e.what() << std::endl;
        return 0;
    }
}
