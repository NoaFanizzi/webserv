/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nofanizz <nofanizz@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/24 11:58:11 by mvachon           #+#    #+#             */
/*   Updated: 2026/05/05 12:20:19 by nofanizz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Config.hpp"
#include "WebServer.hpp"
#include "Server.hpp"
#include <iostream>
#include <csignal>
#include <map>
#include <utility>

void signalHandler(int)
{
	// if (signal == SIGINT) {
	std::cout << std::endl;
	WebServer::destroy();
	throw Exception("Ctrl + C detected");
	// }
}

int main(int ac, char **av)
{
	if (ac != 2)
	{
		std::cerr << "Usage: ./webserv <config>" << std::endl;
		return 1;
	}
	signal(SIGINT, SIG_IGN);
	signal(SIGQUIT, SIG_IGN);
	signal(SIGTERM, SIG_IGN);
	try
	{
		{
			Config config;
			if (!config.setFile(av[1]))
				return 1;
			const std::vector<ServerConfig> &SavedServers = config.getServers();
			std::map<int, std::vector<ServerConfig> > groups;
			for (size_t i = 0; i < SavedServers.size(); i++)
				groups[SavedServers[i].port].push_back(SavedServers[i]);
			for (std::map<int, std::vector<ServerConfig> >::iterator it = groups.begin(); it != groups.end(); ++it)
			{
				std::vector<ServerConfig> &grp = it->second;
				// Si les hosts diffèrent dans le groupe, binder sur 0.0.0.0
				const std::string &firstHost = grp[0].host;
				bool mixedHosts = false;
				for (size_t i = 1; i < grp.size(); i++)
					if (grp[i].host != firstHost) { mixedHosts = true; break; }
				if (mixedHosts)
					for (size_t i = 0; i < grp.size(); i++)
						grp[i].host = "0.0.0.0";
				new Server(grp);
			}
			
		}
		std::signal(SIGINT, signalHandler);
		std::signal(SIGQUIT, signalHandler);
		std::signal(SIGTERM, signalHandler);
		WebServer::run();
	}
	catch (const std::exception &e)
	{
		std::cerr << e.what() << std::endl;
		WebServer::destroy();
	}
}