/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mvachon <mvachon@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/22 20:22:15 by mvachon           #+#    #+#             */
/*   Updated: 2026/02/24 14:22:39 by mvachon          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Config.hpp"
#include <iostream>
#include <sstream>

Config::Config()
{
    initServerKeys();
    initLocationKeys();
}

void Config::initServerKeys()
{
    _keysServer[0] = "port";
    _keysServer[1] = "host";
    _keysServer[2] = "root";
    _keysServer[3] = "index";
    _keysServer[4] = "error_page";
    _keysServer[5] = "client_max_body_size";
    _keysServer[6] = "autoindex";
}

void Config::initLocationKeys()
{
    _keysLocation[0] = "path";
    _keysLocation[1] = "root";
    _keysLocation[2] = "index";
    _keysLocation[3] = "autoindex";
    _keysLocation[4] = "allow_methods";
}

bool Config::setFile(std::string doc)
{
    
    try
    {
        std::string content = readFile(doc);
        _fileContent = splitLinesWords(content);
        parseConfigFile();
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

int Config::parseConfigFile()
{
    size_t i = 0;
    while (i < _fileContent.size())
    {
        if (!_fileContent[i].empty() && _fileContent[i][0] == "server")
            parseServerBlock(&i);
        else if (!_fileContent[i].empty() && _fileContent[i][0] != "}" && _fileContent[i][0] != "server")
            throw Exception("unexpected token -> " + _fileContent[i][0]);
        else
            i++;
    }
    
    validateDuplicatePorts();
    printServers();
    return 1;
}

void Config::validateDuplicatePorts()
{
    if (_servers.size() <= 1)
        return;

    for (size_t j = 0; j < _servers.size(); j++)
    {
        for (size_t k = j + 1; k < _servers.size(); k++)
        {
            if (_servers[k].port == _servers[j].port && 
                _servers[k].host == _servers[j].host)
            {
                std::ostringstream oss;
                oss << "Duplicate port -> " << _servers[j].port;
                throw Exception(oss.str());
            }
        }
    }
}

const std::vector<ServerConfig>& Config::getServers() const
{
    return _servers;
}

