/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mvachon <mvachon@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/24 11:58:11 by mvachon           #+#    #+#             */
/*   Updated: 2026/02/25 17:21:55 by mvachon          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Config.hpp"
#include "WebServer.hpp"
#include "Server.hpp"
#include <iostream>
#include <csignal>


void signalHandler(int signal) {
    if (signal == SIGINT) {
		std::cout << std::endl;
		WebServer::destroy();
        throw Exception("Ctrl + C detected");
    }
}

int main(int ac, char **av)
{
	if (!av[1] && ac != 2)
	{
		std::cout << "No config file" << std::endl;
		return 1;
	}
    signal(SIGINT, SIG_IGN);
	Config config;
	if (!config.setFile(av[1]))
		return 1;
	
	try
	{
		const std::vector<ServerConfig> &SavedServers = config.getServers();
		for (size_t i = 0; i < SavedServers.size(); i++)
			new Server(SavedServers[i]);
		std::signal(SIGINT, signalHandler);
		WebServer::run();
	}
	catch (const std::exception &e)
	{
		std::cerr << e.what() << std::endl;
	}
}