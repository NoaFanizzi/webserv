/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nofanizz <nofanizz@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/22 20:24:07 by mvachon           #+#    #+#             */
/*   Updated: 2026/01/28 16:21:08 by nofanizz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CONFIG_HPP
#define CONFIG_HPP

#include <fcntl.h>
#include <vector>
#include <unistd.h>
#include <iostream>
#include <sstream>
#include <fstream>
#include <string>

class Exception : public std::exception {
    private:
        std::string _message;
    public:
        explicit Exception(const std::string& message) : _message(message) {}
        virtual ~Exception() throw() {}
        virtual const char* what() const throw() {
            return _message.c_str();
        }
};

struct LocationConfig
{
    std::string path;
    std::string root;
    std::string index;
    bool autoindex;     
    std::vector<std::string> allowed_methods;
    LocationConfig() : autoindex(false){}
};

struct ErrorPage
{
    int index;
    std::string path;
};

struct ServerConfig
{
    ServerConfig();
    int                         port;
    std::string                 host;
    std::string                 root;
    std::string                 index;
    std::vector<ErrorPage>      error_page;
    long long                   client_max_body_size;
    bool                        autoindex;
    std::vector<LocationConfig> locations;
};

class Config
{
    private:
        std::vector<std::vector<std::string> > _fileContent;
        std::vector<ServerConfig> _servers;
        std::string _keysServer[7];
        std::string _keysLocation[5];

        void initServerKeys();
        void initLocationKeys();

        void parseServerBlock(size_t *i);
        void parseServerDirective(ServerConfig &server, const std::string &key, 
                                  const std::vector<std::string> &line, size_t j);
        std::string extractValue(const std::vector<std::string> &line, size_t j, 
                                 const std::string &key);
        
        void parsePort(ServerConfig &server, const std::string &value);
        void parseClientMaxBodySize(ServerConfig &server, const std::string &value);
        void parseAutoindex(ServerConfig &server, const std::string &value);
        void parseErrorPage(ServerConfig &server, const std::vector<std::string> &line, size_t j);

        void parseLocationBlock(ServerConfig &server, size_t *i, size_t *j);
        void parseLocationDirective(LocationConfig &location, const std::string &key, 
                                    const std::vector<std::string> &line, size_t j);
        void parseLocationAutoindex(LocationConfig &location, const std::string &value);
        void parseAllowMethods(LocationConfig &location, const std::vector<std::string> &line, size_t j);

        void validateDuplicatePorts();
        
        int parseConfigFile();

    public:
        Config();
        int setFile(std::string doc);
        const std::vector<ServerConfig>& getServers() const;
        void printServers();
};

void validateServerConfig(const ServerConfig& server);
std::vector<std::vector<std::string> > splitLinesWords(const std::string &content);
std::string readFile(const std::string& doc);

#endif