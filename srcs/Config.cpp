/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mvachon <mvachon@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/22 20:22:15 by mvachon           #+#    #+#             */
/*   Updated: 2026/01/23 05:43:49 by mvachon          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Config.hpp"

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
        return;

    std::string value = line[j + 1];
    
    if (!value.empty() && value[value.size() - 1] == ';')
        value = value.substr(0, value.size() - 1);

    if (key == "port")
        server.port = value;
    else if (key == "host")
        server.host = value;
    else if (key == "root")
        server.root = value;
    else if (key == "index")
        server.index = value;
    else if (key == "client_max_body_size")
        server.client_max_body_size = value;
    else if (key == "autoindex")
        server.autoindex = (value == "on");
    else if (key == "error_page")
    {
        if (j + 2 < line.size())
        {
            std::string code = line[j + 1];
            std::string path = line[j + 2];
            if (!path.empty() && path[path.size() - 1] == ';')
                path = path.substr(0, path.size() - 1);

            ErrorPage page;
            page.index = code;
            page.path = path;

            server.error_page.push_back(page);
        }
    }
}

void Config::parseKeyLocation(LocationConfig &location, const std::string &key, const std::vector<std::string> &line, size_t j)
{
    if (j + 1 >= line.size())
        return;

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
        location.autoindex = (value == "on");
    else if (key == "allow_methods")
    {
        for (size_t x = j + 1; x < line.size(); x++)
        {
            std::string method = line[x];
            location.allowed_methods.push_back(method);
        }
    }
}

void Config::parseLocation(ServerConfig &server, size_t *i, size_t *j)
{
    LocationConfig location;
    size_t brace_level = 0;

    location.path = _fileContent[*i][*j + 1];
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
            if (token == "}")
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
            if (str == "}")
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
        result.push_back(words);
    }
    return result;
}


std::string readFile(std::string doc)
{
    int fd = open(doc.c_str(), O_RDONLY);
    if (fd < 0)
        std::cout << "Error : Can't open the file" << std::endl;
    std::string _fileContent;
    char buffer[1024];
    ssize_t bytes;
    
    while ((bytes = read(fd, buffer, sizeof(buffer))) > 0)
        _fileContent.append(buffer, bytes);
    close(fd);
    if (bytes < 0)
        std::cout << "Error : Read issues" << std::endl;
    return _fileContent;
}

int Config::setFile(std::string doc)
{
    std::string content = readFile(doc);
    _fileContent = splitLinesWords(content);
    getInfo();

    return 0;
}
