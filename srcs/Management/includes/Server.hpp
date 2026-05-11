/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nofanizz <nofanizz@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/22 11:35:23 by nofanizz          #+#    #+#             */
/*   Updated: 2026/05/05 12:18:59 by nofanizz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
#define SERVER_HPP

#include <netinet/in.h>
#include <vector>

#include "AManager.hpp"
#include "Config.hpp"

class Server : public AManager {
  private:
	// variables
	struct sockaddr_in _servAddr;
	const std::vector<ServerConfig> _serverConfigs;

	// functions
	int createSocket();
	void createSocketAdress(const ServerConfig &serverconfig);
	void bindClient(int fd);

  public:
	// constructor
	Server(const std::vector<ServerConfig> &configs);
	~Server();

	// functions
	void pollInHandler();
	void onTimeout();

	// getters
	sockaddr_in getSockddr_in() const { return _servAddr; }
};

#endif
