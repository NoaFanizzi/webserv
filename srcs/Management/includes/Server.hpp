/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nofanizz <nofanizz@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/22 11:35:23 by nofanizz          #+#    #+#             */
/*   Updated: 2026/05/04 13:42:48 by nofanizz         ###   ########.fr       */
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
	void pollInHandler();
	void onTimeout();

	// getters
	sockaddr_in getSockddr_in() const { return _servAddr; }
	const ServerConfig getConfig() const { return _serverConfig; }
};

#endif
