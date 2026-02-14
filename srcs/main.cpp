#include "Config.hpp"
#include "WebServer.hpp"
#include "Server.hpp"
#include <iostream>

int main(int ac, char **av)
{
	if (!av[1] && ac != 2)
	{
		std::cout << "No config file" << std::endl;
		return 1;
	}

	Config config;
	config.setFile(av[1]);
	try
	{
		const std::vector<ServerConfig> &SavedServers = config.getServers();
		for (size_t i = 0; i < SavedServers.size(); i++)
			new Server(SavedServers[i]);
		WebServer::run();
	}
	catch (const std::exception &e)
	{
		std::cerr << e.what() << '\n';
	}
}