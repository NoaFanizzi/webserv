/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mvachon <mvachon@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/22 11:32:07 by nofanizz          #+#    #+#             */
/*   Updated: 2026/01/29 19:05:44 by mvachon          ###   ########.fr       */
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
#include "Config.hpp"
#include "stdbool.h"

class Client : public AManager
{
	private:
		std::string _request;
		RequestParser _RequestParser;
		std::map<int, std::string> _errorPages;
		const ServerConfig _config;
		bool _requestcomplete;
	public:
		Client(int fd, const ServerConfig &config);
		Client() {};
		~Client() {};

		void	updateRequest(std::string &buffer, int n);
		std::string getErrorPage(int code);
		std::string 	CheckUrl();
		void	handleRequestReception();		
		void	PollInHandler();
		void 	setErrorPages();
		void	PollOutHandler();
        std::string GetHeaderResponse(size_t contentLength, std::string, std::string);
		std::string GetRequest() {return _request;}
};



#endif