/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   LocationParser.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mvachon <mvachon@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/28 10:30:00 by mvachon           #+#    #+#             */
/*   Updated: 2026/01/28 10:19:41 by mvachon          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Config.hpp"

static const size_t LOCATION_KEYS_COUNT = 5;

void Config::parseLocationBlock(ServerConfig &server, size_t *i, size_t *j)
{
    LocationConfig location;
    size_t brace_level = 0;

    if (*j + 1 >= _fileContent[*i].size())
        throw Exception("No path specified for location");
        
    location.path = _fileContent[*i][*j + 1];
    
    if (location.path.empty() || location.path == "{")
        throw Exception("No argument found for the location path");

    (*j) += 2;
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
                for (size_t k = 0; k < LOCATION_KEYS_COUNT; k++)
                {
                    if (token == _keysLocation[k])
                        parseLocationDirective(location, token, _fileContent[*i], *j);
                }
            }
        }
        *j = 0;
        (*i)++;
    }
    throw Exception("Bad closing brace");
}

void Config::parseLocationDirective(LocationConfig &location, const std::string &key, 
                                     const std::vector<std::string> &line, size_t j)
{
    if (j + 1 >= line.size())
        throw Exception("No argument found for -> " + key);

    std::string value = line[j + 1];
    
    if (key == "root" || key == "autoindex")
    {      
        if (line.size() > 2)
        {
            if (line[2] != ";" || line.size() > 3)
                throw Exception("Too much value for -> " + key);
        }
    }
    
    if (!value.empty() && value[value.size() - 1] == ';')
        value = value.substr(0, value.size() - 1);
    
    if (key == "path")
        location.path = value;
    else if (key == "root")
        location.root = value;
    else if (key == "index")
        location.index = value;
    else if (key == "autoindex")
        parseLocationAutoindex(location, value);
    else if (key == "allow_methods")
        parseAllowMethods(location, line, j);
}

void Config::parseLocationAutoindex(LocationConfig &location, const std::string &value)
{
    if (value == "on" || value == "off")
        location.autoindex = (value == "on");
    else
        throw Exception("Invalid value for autoindex -> " + value);
}

void Config::parseAllowMethods(LocationConfig &location, const std::vector<std::string> &line, size_t j)
{
    for (size_t x = j + 1; x < line.size(); x++)
    {
        std::string method = line[x];
        
        if (method == ";")
            break;
            
        if (!method.empty() && method[method.size() - 1] == ';')
            method = method.substr(0, method.size() - 1);
            
        if (method != "GET" && method != "POST" && method != "DELETE")
            throw Exception("The method is not valid [GET-POST-DELETE] -> " + method);
            
        if (!method.empty())
            location.allowed_methods.push_back(method);
    }
}