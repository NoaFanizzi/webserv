/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nofanizz <nofanizz@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/22 11:35:23 by nofanizz          #+#    #+#             */
/*   Updated: 2026/01/28 14:42:14 by nofanizz         ###   ########.fr       */
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

class	Server : public AManager
{
	private:
		struct sockaddr_in _servaddr;
		int createSocket();
		void createSocketAdress();
		void 			bindClient(int fd);


	public:
		//constructor
		Server();
		~Server() {};
		//getters
		int 			getListen_fd() const;
		sockaddr_in 	getSockddr_in() const;

		void PollInHandler();
		void PollOutHandler();
		
		
};

#endif