/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nofanizz <nofanizz@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/22 11:32:07 by nofanizz          #+#    #+#             */
/*   Updated: 2026/01/22 18:08:12 by nofanizz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLIENT_HPP
#define CLIENT_HPP

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

#include "AManager.hpp"
#include "ManageAll.hpp"

class Client : public AManager
{
	private:
		std::string request[1024];
	public:
		Client(int fd);
		void	updateRequest(std::string &buffer, int n);
		void	handleRequestReception();		
		void	PollInHandler();
		void	PollOutHandler();
};

#endif