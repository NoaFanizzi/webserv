/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mvachon <mvachon@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/22 20:24:07 by mvachon           #+#    #+#             */
/*   Updated: 2026/01/23 01:46:24 by mvachon          ###   ########.fr       */
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


struct LocationConfig
{
    std::string path;
    std::string root;
    std::string index;
    bool autoindex;
    std::vector<std::string> allowed_methods;
};

struct ErrorPage
{
    std::string index;
    std::string path;
};

struct ServerConfig
{
    std::string             port;
    std::string             host;
    std::string             root;
    std::string             index;
    std::vector<ErrorPage>  error_page;
    std::string                  client_max_body_size;
    bool                    autoindex;

    std::vector<LocationConfig> locations;
};


class Config
{
    private:
        std::vector <std::vector<std::string> > _fileContent;
        std::vector<ServerConfig> _servers;
        std::string _keys[7];
    public:
    int setFile(std::string doc);
    int getInfo();
    void parseServer(ServerConfig &server, const std::string &key, const std::vector<std::string> &line, size_t j);
    void printServers();
    void newServer(size_t *i);
    // bool validateServer();
    Config();
    // ~Config();
};

#endif