/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mvachon <mvachon@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/20 12:31:03 by nofanizz          #+#    #+#             */
/*   Updated: 2026/01/30 13:49:44 by mvachon          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ManageAll.hpp"
#include "Config.hpp"

int	main(int ac, char **av)
{
	Config config;
	(void)ac;
	if (av[1])
		config.setFile(av[1]);
	else
	{
		std::cout << "No config file" << std::endl;
		return 1;
	}

	const std::vector<ServerConfig> &SavedServers= config.getServers();
	for(size_t i = 0; i < SavedServers.size(); i++)
		new Server (SavedServers[i]);
	ManageAll::loop();

}
