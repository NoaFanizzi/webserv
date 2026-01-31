/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerParser.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mvachon <mvachon@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/28 10:30:00 by mvachon           #+#    #+#             */
/*   Updated: 2026/01/28 10:50:30 by mvachon          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Config.hpp"

static const size_t SERVER_KEYS_COUNT = 7;

void Config::parseServerBlock(size_t *i)
{
    ServerConfig server;
    size_t brace_level = 0;
    
    if (_fileContent[*i].size() != 2 || _fileContent[*i][1] != "{")
        throw Exception("Missing opening brace after server");

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
                    validateServerConfig(server);
                    _servers.push_back(server);
                    return;
                }
                continue;
            }
            
            bool known = false;
            for (size_t k = 0; k < SERVER_KEYS_COUNT; k++)
            {
                if (str == _keysServer[k])
                {
                    known = true;
                    parseServerDirective(server, str, _fileContent[*i], j);
                }
            }
            
            if (brace_level == 1)
            {
                if (str == "location")
                {
                    parseLocationBlock(server, i, &j);
                    break;
                }
                else if (!known && str == _fileContent[*i][0])
                {
                    throw Exception("Unknown directive in server block -> " + str);
                }
            }
            else if (brace_level >= 2)
                throw Exception("Bad closing brace");
        }
        (*i)++;
    }
    throw Exception("Bad closing brace");
}

void validateServerConfig(const ServerConfig& server)
{
    if (server.port == -1)
        throw Exception("Port is not set");
    if (server.host.empty())
        throw Exception("Host is not set");
    if (server.root.empty())
        throw Exception("root is not set");
    if (server.index.empty())
        throw Exception("index is not set");
    if (server.client_max_body_size == -1)
        throw Exception("client_max_body_size is not set");
}

ServerConfig::ServerConfig()
{
    port = -1;
    client_max_body_size = -1;
    autoindex = false;
}