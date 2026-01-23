/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mvachon <mvachon@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/20 12:31:03 by nofanizz          #+#    #+#             */
/*   Updated: 2026/01/23 02:38:15 by mvachon          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

// #include "ManageAll.hpp"
#include "Config.hpp"
int	main(int ac, char **av)
{
	Config test;
	(void)ac;
	if (av[1])
		test.setFile(av[1]);
	else
	{
		std::cout << "No config file" << std::endl;
		return 1;
	}
	ManageAll manager;
	Server server;
	
	size_t	i;
	
	//3.0. Je lance ma boucle
	while(true)
	{
		int poll_value = poll(&poll_fds[0], poll_fds.size(), -1);
		if(poll_value < 0)
		{
			perror("poll error");
			exit(1);
		}
		i = 0;
		while(i < poll_fds.size() && poll_value != 0)
		{
			if(!poll_fds[i].revents)
			{
				i++;
				continue;
			}
			if(manager.getPollFd(i).revents & POLLIN)
			{
				if(manager.getPollFd(i).fd == manager.getServer(i).getListen_fd())
				{
					//le client essaye de se connecter
					initClientConnection();
					
					
					

					//la je call ManagePollfdcreation
					struct pollfd client;
					client.fd = client_fd;	
					client.events = POLLIN;
					client.revents = 0;
					poll_fds.push_back(client);
				}
				else
				{
					handleRequestReception();
					std::cout << "Signal received" << std::endl;
					//TODO fix cette condition
					char buffer[1024];
					int n = recv(poll_fds[i].fd, buffer, 1024, 0);

					
					if(n > 0)
					{
						std::cout << "Received" 
								  <<  n 
								  << " bytes from client "
								  << poll_fds[i].fd
								  << std::endl;
						std::cout << "Content: " << std::string(buffer, n) << std::endl;
						std::string response = "HTTP/1.1 is ok";
						//parse la requete
						send(poll_fds[i].fd, response.c_str(), response.length(), 0);
						close(poll_fds[i].fd);
						poll_fds.erase(poll_fds.begin() + i);
					}
				}
				poll_value--;
			}
			i++;
		}
	}
}