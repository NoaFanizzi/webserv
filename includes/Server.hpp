/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nofanizz <nofanizz@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/22 11:35:23 by nofanizz          #+#    #+#             */
/*   Updated: 2026/01/22 13:26:06 by nofanizz         ###   ########.fr       */
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

class	Server
{
	private:
		int _listen_fd;
		struct sockaddr_in _servaddr;
		struct pollfd _data;
		void createSocket();
		void createSocketAdress();
	public:
		//constructor
		Server();
		//getters
		int 			getListen_fd() const;
		sockaddr_in 	getSockddr_in() const;
		struct pollfd 	getPollFd() const;
		void 			bindClient();
		
		
};

#endif