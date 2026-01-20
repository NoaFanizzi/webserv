/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nofanizz <nofanizz@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/20 12:31:03 by nofanizz          #+#    #+#             */
/*   Updated: 2026/01/20 15:30:00 by nofanizz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <sys/socket.h>
#include <vector>
#include <netinet/in.h>
#include <iostream>
#include <poll.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <arpa/inet.h>

int	main(void)
{
	//1. Je cree mon serveur et je listen

	//1.a. Je cree le socket du serveur
	int listen_fd = socket(AF_INET, SOCK_STREAM, 0);
	
	//1.b. Je cree et fill la structure stockadress_in
	struct sockaddr_in servaddr;
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = INADDR_ANY;
	servaddr.sin_port = htons(8080);
	
	//1.c. Je bind pour associer mon socket avec l'ip et le port
	bind(listen_fd, (sockaddr *)&servaddr, sizeof(servaddr));
	
	//2.a. Je cree la struct pollfd;
	std::vector<struct pollfd> poll_fds;
	
	//2.b. Je cree la structure poll_fd du serveur et je pushback dans poll_fds;
	struct pollfd server;
	server.fd = listen_fd;
	server.events = POLLIN;
	server.revents = 0;
	poll_fds.push_back(server);

	//2.c. je listen
	listen(listen_fd, 4096);
	
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
			if(poll_fds[i].revents & POLLIN)
			{
				if(poll_fds[i].fd == listen_fd)
				{
					//le client essaye de se connecter
					struct sockaddr_in clientaddr;
					socklen_t addr_len = sizeof(clientaddr);
					int client_fd = accept(listen_fd, (sockaddr *)&clientaddr, &addr_len);
					std::cout << "New client connected !" << std::endl;
					std::cout << "INFORMATIONS :" << std::endl;
					std::cout << "Client FD: " << client_fd << std::endl;
					std::cout << "Client IP: " << inet_ntoa(clientaddr.sin_addr) << std::endl;
					std::cout << "Client PORT: " << clientaddr.sin_port << std::endl;
					
					struct pollfd client;
					client.fd = client_fd;
					client.events = POLLIN;
					client.revents = 0;
					poll_fds.push_back(client);
				}
				else
				{
					std::cout << "Signal received" << std::endl;
					//TODO fix cette condition
					// char buffer[1024];
					// int n = recv(poll_fds[i].fd, buffer, 1024, 0);
					
					// if(n > 0)
					// {
					// 	std::cout << "Received" 
					// 			  <<  n 
					// 			  << " bytes from client "
					// 			  << poll_fds[i].fd
					// 			  << std::endl;
					// 	std::cout << "Content: " << std::string(buffer, n) << std::endl;
					// 	std::string response = "HTTP/1.1 is ok";
					// 	send(poll_fds[i].fd, response.c_str(), response.length(), 0);
					// 	close(poll_fds[i].fd);
					// 	poll_fds.erase(poll_fds.begin() + i);
					// }
					// je recois les donnees du client
				}
				poll_value--;
			}
			i++;
		}
	}
}