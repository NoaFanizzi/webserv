/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nofanizz <nofanizz@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/22 11:32:07 by nofanizz          #+#    #+#             */
/*   Updated: 2026/01/22 12:06:43 by nofanizz         ###   ########.fr       */
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

class Client
{
	private:
		std::string request[1024];
	public:
		void updateRequest(std::string &buffer, int n);
		void handleRequestReception();		
};

#endif