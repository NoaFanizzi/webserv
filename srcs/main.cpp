/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mvachon <mvachon@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/20 12:31:03 by nofanizz          #+#    #+#             */
/*   Updated: 2026/01/31 14:24:32 by mvachon          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ManageAll.hpp"
#include "Config.hpp"

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
		ManageAll::loop();
	}
	catch (const std::exception &e)
	{
		std::cerr << e.what() << '\n';
	}
}
