/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mvachon <mvachon@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/22 20:22:15 by mvachon           #+#    #+#             */
/*   Updated: 2026/01/27 18:22:42 by mvachon          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Config.hpp"
#include <limits.h>

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


static int	check_overflow(long long res, int digit, int sign)
{
	if (sign == 1)
	{
		if (res > LLONG_MAX / 10)
			return (1);
		if (res == LLONG_MAX / 10 && digit > LLONG_MAX % 10)
			return (1);
	}
	else
	{
		if (res < LLONG_MIN / 10)
			return (1);
		if (res == LLONG_MIN / 10 && -digit < LLONG_MIN % 10)
			return (1);
	}
	return (0);
}

int	strToInt(const char *s, long long *out)
{
	size_t		i = 0;
	int			sign = 1;
	long long	res = 0;

	if (!s || !*s)
		return (0);

	while (s[i] == ' ' || (s[i] >= 9 && s[i] <= 13))
		i++;

	if (s[i] == '+' || s[i] == '-')
	{
		if (s[i] == '-')
			sign = -1;
		i++;
	}

	if (s[i] < '0' || s[i] > '9')
		return (0);

	while (s[i] >= '0' && s[i] <= '9')
	{
		int digit = s[i] - '0';

		if (check_overflow(res, digit, sign))
			return (0);

		res = res * 10 + digit * sign;
		i++;
	}

	if (s[i] != '\0')
		return (0);

	*out = res;
	return (1);
}


void Config::parseServer(ServerConfig &server,const std::string &key,const std::vector<std::string> &line,size_t j)
{
    if (j + 1 >= line.size())
        throw Exception("No argument found for -> " + key);

    std::string value = line[j + 1];

    if (!value.empty() && value[value.size() - 1] == ';')
        value = value.substr(0, value.size() - 1);

    if (key == "port")
    {
        long long tmp;

        if (!strToInt(value.c_str(), &tmp))
            throw Exception("Invalid port ->" + value);

        if (tmp < 0 || tmp > 65535)
            throw Exception("Invalid port range [0-65535] -> " + value);

        server.port = static_cast<int>(tmp);
    }

    else if (key == "host")
        server.host = value;

    else if (key == "root")
        server.root = value;

    else if (key == "index")
        server.index = value;

    else if (key == "client_max_body_size")
    {
        long long tmp;

        if (!strToInt(value.c_str(), &tmp))
            throw Exception("Invalid client_max_body_size -> " + value);
        if (tmp < 0)
            throw Exception("client_max_body_size must be positive -> " + value);

        server.client_max_body_size = tmp;
    }

    else if (key == "autoindex")
    {
        if (value == "on" || value == "off")
            server.autoindex = (value == "on");
        else
            throw Exception("Invalid value for autoindex -> " + value);
    }

    else if (key == "error_page")
    {
        if (j + 2 >= line.size())
            throw Exception("error_page requires code and path");

        std::string path = line[j + 2];
        long long code;

        if (!strToInt(line[j + 1].c_str(), &code))
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
}


void Config::parseKeyLocation(LocationConfig &location, const std::string &key, const std::vector<std::string> &line, size_t j)
{
    if (j + 1 >= line.size())
        throw Exception("No argument found for -> " + key);

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
            throw Exception("Invalid value for autoindex -> " + value);
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
                throw Exception("The method is not valid [GET-POST-DELETE] -> " + method);
            if (!method.empty())
                location.allowed_methods.push_back(method);
        }
    }
}

void Config::parseLocation(ServerConfig &server, size_t *i, size_t *j)
{
    LocationConfig location;
    size_t brace_level = 0;

    if (*j + 1 >= _fileContent[*i].size())
        throw Exception("No path specified for location");
        
    location.path = _fileContent[*i][*j + 1];
    
    if (location.path.empty() || location.path == "{")
        throw Exception("No argument found for the location path");

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
                {
                    if (token == _keysLocation[k])
                        parseKeyLocation(location, token, _fileContent[*i], *j);
                }
            }
        }
        *j = 0;
        (*i)++;
    }
    throw Exception("Bad closing brace");
}

void validateServerConfig(const ServerConfig server)
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

void Config::newServer(size_t *i)
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
            for (size_t k = 0; k < 7; k++)
            {
                if (str == _keysServer[k])
                {
                    known = true;
                    parseServer(server, str, _fileContent[*i], j);
                }
            }
            if (brace_level == 1)
            {
                if (str == "location")
                {
                    parseLocation(server, i, &j);
                    break;
                }
                else if (!known && str == _fileContent[*i][0])
                {
                    throw Exception("Unknown directive in server block: " + str);
                }
            }
            else if (brace_level >= 2)
                throw Exception("Bad closing brace");
        }
        (*i)++;
    }
    throw Exception("Bad closing brace");
}

int Config::getInfo()
{
    size_t i = 0;
    while (i < _fileContent.size())
    {
        if (!_fileContent[i].empty() && _fileContent[i][0] == "server")
            newServer(&i);
        else if (!_fileContent[i].empty() && _fileContent[i][0] != "}" && _fileContent[i][0] != "server")
            throw Exception("unexpected token -> " + _fileContent[i][0]);
        else
            i++;
    }
    printServers();
    return 1;
}

std::vector<std::vector<std::string> > splitLinesWords(const std::string &content)
{
    std::vector<std::vector<std::string> > result;
    std::vector<std::string> lineWords;
    std::string word;

    for (size_t i = 0; i < content.size(); ++i)
    {
        char c = content[i];

        if (c == ' ' || c == '\t')
        {
            if (!word.empty())
            {
                lineWords.push_back(word);
                word.clear();
            }
        }
        else if (c == '\n' || c == '\r')
        {
            if (!word.empty())
            {
                lineWords.push_back(word);
                word.clear();
            }
            if (!lineWords.empty())
            {
                result.push_back(lineWords);
                lineWords.clear();
            }
        }
        else if (c == '{' || c == '}' || c == ';')
        {
            if (!word.empty())
            {
                lineWords.push_back(word);
                word.clear();
            }
            std::string special(1, c);
            lineWords.push_back(special);
        }
        else
            word += c;
    }
    if (!word.empty())
        lineWords.push_back(word);
    if (!lineWords.empty())
        result.push_back(lineWords);

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
        throw Exception("Error : can't read the file");
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
