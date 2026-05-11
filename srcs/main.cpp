/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nofanizz <nofanizz@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/24 11:58:11 by mvachon           #+#    #+#             */
/*   Updated: 2026/05/11 16:11:31 by nofanizz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Config.hpp"
#include "WebServer.hpp"
#include "Server.hpp"
#include <iostream>
#include <csignal>
#include <map>
#include <utility>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <cstring>

typedef std::map<std::pair<std::string, int>, std::vector<ServerConfig> > ServerGroups;

void signalHandler(int)
{
	// if (signal == SIGINT) {
	std::cout << std::endl;
	WebServer::destroy();
	throw Exception("Ctrl + C detected");
	// }
}

static bool isBindable(const std::string &host, int port)
{
	int testFd = socket(AF_INET, SOCK_STREAM, 0);
	if (testFd == -1)
		return false;

	int opt = 1;
	setsockopt(testFd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

	struct addrinfo hints, *res;
	std::memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;

	bool bindable = false;
	if (getaddrinfo(host.c_str(), NULL, &hints, &res) == 0)
	{
		struct sockaddr_in addr;
		std::memset(&addr, 0, sizeof(addr));
		addr.sin_family = AF_INET;
		addr.sin_addr = ((struct sockaddr_in *)res->ai_addr)->sin_addr;
		addr.sin_port = htons(port);
		freeaddrinfo(res);
		bindable = (bind(testFd, (struct sockaddr *)&addr, sizeof(addr)) == 0);
	}
	close(testFd);
	return bindable;
}

static ServerGroups buildFinalGroups(const ServerGroups &groups)
{
	ServerGroups finalGroups;

	for (ServerGroups::const_iterator it = groups.begin(); it != groups.end(); ++it)
	{
		const std::string &host = it->first.first;
		int port = it->first.second;

		if (host == "0.0.0.0")
		{
			finalGroups[it->first].insert(finalGroups[it->first].end(), it->second.begin(), it->second.end());
			continue;
		}

		std::pair<std::string, int> wildcardKey = std::make_pair(std::string("0.0.0.0"), port);
		if (groups.count(wildcardKey))
		{
			for (size_t i = 0; i < it->second.size(); i++)
				finalGroups[wildcardKey].push_back(it->second[i]);
			continue;
		}

		if (isBindable(host, port))
		{
			finalGroups[it->first].insert(finalGroups[it->first].end(), it->second.begin(), it->second.end());
			continue;
		}

		std::cerr << "Warning: cannot bind to " << host << ":" << port << ", server ignored" << std::endl;
	}
	return finalGroups;
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
			ServerGroups groups;
			for (size_t i = 0; i < SavedServers.size(); i++)
				groups[std::make_pair(SavedServers[i].host, SavedServers[i].port)].push_back(SavedServers[i]);

			ServerGroups finalGroups = buildFinalGroups(groups);
			for (ServerGroups::iterator it = finalGroups.begin(); it != finalGroups.end(); ++it)
				new Server(it->second);
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
