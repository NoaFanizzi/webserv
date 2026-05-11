/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nofanizz <nofanizz@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/22 20:22:15 by mvachon           #+#    #+#             */
/*   Updated: 2026/05/11 14:32:35 by nofanizz         ###   ########.fr       */
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
    _keysServer[7] = "allowed_methods";
    _keysServer[8] = "upload_dir";
    _keysServer[9] = "server_name";
}

void Config::initLocationKeys()
{
    _keysLocation[0] = "path";
    _keysLocation[1] = "root";
    _keysLocation[2] = "index";
    _keysLocation[3] = "autoindex";
    _keysLocation[4] = "allowed_methods";
    _keysLocation[5] = "return";
    _keysLocation[6] = "upload_dir";
    _keysLocation[7] = "cgi_pass";
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
    
    validateVirtualHosts();
    printServers();
    return 1;
}

void Config::validateVirtualHosts()
{
    if (_servers.size() <= 1)
        return;

    for (size_t j = 0; j < _servers.size(); j++)
    {
        for (size_t k = j + 1; k < _servers.size(); k++)
        {
            if (_servers[k].port != _servers[j].port)
                continue;
            // Same port — only allowed if server_names differ
            const std::vector<std::string> &names_j = _servers[j].server_names;
            const std::vector<std::string> &names_k = _servers[k].server_names;
            for (size_t a = 0; a < names_j.size(); a++)
            {
                for (size_t b = 0; b < names_k.size(); b++)
                {
                    if (names_j[a] == names_k[b])
                    {
                        std::ostringstream oss;
                        oss << "Duplicate server_name \"" << names_j[a]
                            << "\" on port " << _servers[j].port;
                        throw Exception(oss.str());
                    }
                }
            }
            if (names_j.empty() && names_k.empty())
            {
                std::ostringstream oss;
                oss << "Duplicate server on port " << _servers[j].port
                    << " without server_name";
                throw Exception(oss.str());
            }
        }
    }
}

const std::vector<ServerConfig>& Config::getServers() const
{
    return _servers;
}

