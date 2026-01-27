/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mvachon <mvachon@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/22 20:24:07 by mvachon           #+#    #+#             */
/*   Updated: 2026/01/27 18:28:39 by mvachon          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CONFIG_HPP
#define CONFIG_HPP

#include <fcntl.h>
#include <vector>
#include <unistd.h>
#include <iostream>
#include <sstream>
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
    int                    port;
    std::string             host;
    std::string             root;
    std::string             index;
    std::vector<ErrorPage>  error_page;
    long                  client_max_body_size;
    bool                    autoindex;
    std::vector<LocationConfig> locations;
};


class Config
{
    private:
        std::vector <std::vector<std::string> > _fileContent;
        std::vector<ServerConfig> _servers;
        std::string _keysServer[7];
        std::string _keysLocation[5];
    public:
    int setFile(std::string doc);
    int getInfo();
    void parseServer(ServerConfig &server, const std::string &key, const std::vector<std::string> &line, size_t j);
    void printServers();
    void parseLocation(ServerConfig &server, size_t *i, size_t *j);
    void parseKeyLocation(LocationConfig &location, const std::string &key, const std::vector<std::string> &line, size_t j); 
    void newServer(size_t *i);
    // bool validateServer();
    Config();
    // ~Config();
};

#endif