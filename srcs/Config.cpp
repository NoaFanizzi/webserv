/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mvachon <mvachon@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/22 20:22:15 by mvachon           #+#    #+#             */
/*   Updated: 2026/01/23 02:37:06 by mvachon          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Config.hpp"

Config::Config()
{
    _keys[0] = "port";
    _keys[1] = "host";
    _keys[2] = "root";
    _keys[3] = "index";
    _keys[4] = "error_page";
    _keys[5] = "client_max_body_size";
    _keys[6] = "autoindex";

}

void Config::printServers()
{
    std::cout << "\n=== SERVERS CONFIG ===\n" << std::endl;
    for (size_t i = 0; i < _servers.size(); ++i)
    {
        const ServerConfig &srv = _servers[i];
        std::cout << "Server " << i + 1 << ":\n";
        std::cout << "  Port: " << srv.port << std::endl;
        std::cout << "  Host: " << srv.host << std::endl;
        std::cout << "  Root: " << srv.root << std::endl;
        std::cout << "  Index: " << srv.index << std::endl;
        std::cout << "  autoindex: " << (srv.autoindex == true ? "yes" : "no") << std::endl;
        std::cout << "  Error pages:\n";
        for (size_t j = 0; j < srv.error_page.size(); ++j)
        {
            std::cout << "    Code: " << srv.error_page[j].index
                      << ", Path: " << srv.error_page[j].path << std::endl;
        }
        std::cout << "  Max client body size: " << srv.client_max_body_size << std::endl;

        std::cout << "---------------------------\n";
    }
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
        server.autoindex = (value == "1");
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
                if (str == _keys[k] && brace_level == 1)
                    parseServer(server, _keys[k], _fileContent[*i], j);
            }
        }
        (*i)++;
    }
}

int Config::getInfo()
{
    for (size_t i = 0; i < _fileContent.size(); i++)
    {
        if (!_fileContent[i].empty() && _fileContent[i][0] == "server")
            newServer(&i);
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
