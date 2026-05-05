/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   LocationParser.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mvachon <mvachon@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/28 10:30:00 by mvachon           #+#    #+#             */
/*   Updated: 2026/03/26 12:23:43 by mvachon          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Config.hpp"
#include <cstdlib>

static const size_t LOCATION_KEYS_COUNT = 8;

void Config::parseLocationBlock(ServerConfig &server, size_t *i, size_t *j)
{
    LocationConfig location;
    size_t brace_level = 0;

    if (*j + 1 >= _fileContent[*i].size())
        throw Exception("No path specified for location");
        
    location.path = _fileContent[*i][*j + 1];
    
    if (location.path.empty() || location.path == "{")
        throw Exception("No argument found for the location path");
    if (location.path[0] != '/')
        throw Exception("Location path must start with '/': " + location.path);

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

    if (key == "allow_methods")
    {
        parseAllowMethods(location.allowed_methods, line, j);
        return;
    }

    if (key == "cgi_pass")
    {
        if (j + 2 >= line.size())
            throw Exception("cgi_pass requires an extension and an executable path");
        std::string ext = line[j + 1];
        std::string executable = line[j + 2];
        bool semicolon = false;
        if (!executable.empty() && executable[executable.size() - 1] == ';')
        {
            executable = executable.substr(0, executable.size() - 1);
            semicolon = true;
        }
        else if (j + 3 < line.size() && line[j + 3] == ";")
            semicolon = true;
        if (!semicolon)
            throw Exception("No semicolon on the line -> cgi_pass");
        if (ext.empty() || ext[0] != '.')
            throw Exception("cgi_pass extension must start with '.' -> " + ext);
        if (executable.empty())
            throw Exception("cgi_pass executable path is empty");
        location.cgi_pass[ext] = executable;
        return;
    }

    if (key == "return")
    {
        if (j + 2 >= line.size())
            throw Exception("return directive requires a code and a url");
        std::string codeStr = line[j + 1];
        std::string url = line[j + 2];
        bool semicolon = false;
        if (!url.empty() && url[url.size() - 1] == ';')
        {
            url = url.substr(0, url.size() - 1);
            semicolon = true;
        }
        else if (j + 3 < line.size() && line[j + 3] == ";")
            semicolon = true;
        if (!semicolon)
            throw Exception("No semicolon on the line -> return");
        for (size_t k = 0; k < codeStr.size(); k++)
            if (!std::isdigit(codeStr[k]))
                throw Exception("return directive: invalid code -> " + codeStr);
        location.redirectCode = std::atoi(codeStr.c_str());
        location.redirectUrl = url;
        return;
    }

    bool semicolon = false;
    std::string value = line[j + 1];

    if (line.size() > 2)
    {
        if (line[2] != ";" || line.size() > 3)
            throw Exception("Too much value for -> " + key);
        else
            semicolon = true;
    }

    if (!value.empty() && value[value.size() - 1] == ';')
    {
        value = value.substr(0, value.size() - 1);
        semicolon = true;
    }

    if (!semicolon)
        throw Exception("No semicolon on the line -> " + key);

    if (value.empty())
        throw Exception("Empty value for -> " + key);

    if (key == "path")
        location.path = value;
    else if (key == "root")
        location.root = value;
    else if (key == "index")
        location.index = value;
    else if (key == "upload_dir")
        location.upload_dir = value;
    else if (key == "autoindex")
        parseLocationAutoindex(location, value);
}

void Config::parseLocationAutoindex(LocationConfig &location, const std::string &value)
{
    if (value == "on" || value == "off")
        location.autoindex = (value == "on");
    else
        throw Exception("Invalid value for autoindex -> " + value);
}

void Config::parseAllowMethods(std::vector<std::string> &allowed_methods, const std::vector<std::string> &line, size_t j)
{
    bool semicolon = false;

    for (size_t x = j + 1; x < line.size(); x++)
    {
        std::string method = line[x];

        if (method == ";")
        {
            semicolon = true;
            break;
        }

        if (!method.empty() && method[method.size() - 1] == ';')
        {
            method = method.substr(0, method.size() - 1);
            semicolon = true;
        }

        if (method != "GET" && method != "POST" && method != "DELETE")
            throw Exception("The method is not valid [GET-POST-DELETE] -> " + method);

        if (!method.empty())
            allowed_methods.push_back(method);

        if (semicolon)
            break;
    }

    if (!semicolon)
        throw Exception("No semicolon on the line -> allow_methods");
}