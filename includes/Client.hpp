/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mvachon <mvachon@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/22 11:32:07 by nofanizz          #+#    #+#             */
/*   Updated: 2026/01/28 16:50:31 by mvachon          ###   ########.fr       */
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
#include "RequestParser.hpp"

class Client : public AManager
{
	private:
		std::string _request;
		RequestParser _RequestParser;
	public:
		Client(int fd);
		Client() {};
		~Client() {};

		void	updateRequest(std::string &buffer, int n);
		void	handleRequestReception();		
		void	PollInHandler();
		void	PollOutHandler();
		std::string GetRequest() {return _request;}
};



#endif