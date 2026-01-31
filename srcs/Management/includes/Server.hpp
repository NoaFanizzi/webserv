/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mvachon <mvachon@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/22 11:35:23 by nofanizz          #+#    #+#             */
/*   Updated: 2026/01/29 18:13:57 by mvachon          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
#define SERVER_HPP

#include "iostream"
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
#include <unistd.h>

#include <map>
#include "AManager.hpp"
#include "Config.hpp"

class	Server : public AManager
{
	private:
		struct sockaddr_in _servaddr;
		int createSocket();
		void createSocketAdress(const ServerConfig &serverconfig);
		void 			bindClient(int fd);
		const ServerConfig	&_serverconfig;

	public:
		//constructor
		Server(const ServerConfig &servconfig);
		~Server();
		//getters
		int 			getListen_fd() const;
		sockaddr_in 	getSockddr_in() const;
		const ServerConfig getConfig() const {return _serverconfig;}

		void PollInHandler();
		
		
};

#endif