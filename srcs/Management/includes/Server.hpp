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

#include <netinet/in.h>

#include "AManager.hpp"
#include "Config.hpp"

class Server : public AManager {
  private:
	// variables
	struct sockaddr_in _servAddr;
	const ServerConfig &_serverConfig;

	// functions
	int createSocket();
	void createSocketAdress(const ServerConfig &serverconfig);
	void bindClient(int fd);

  public:
	// constructor
	Server(const ServerConfig &servconfig);
	~Server();

	// functions
	void PollInHandler();

	// getters
	sockaddr_in getSockddr_in() const { return _servAddr; }
	const ServerConfig getConfig() const { return _serverConfig; }
};

#endif
