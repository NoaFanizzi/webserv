/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ManageAll.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nofanizz <nofanizz@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/22 12:37:43 by nofanizz          #+#    #+#             */
/*   Updated: 2026/01/22 13:53:47 by nofanizz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MANAGEALL_H
#define MANAGEALL_H

#include "Client.hpp"
#include "Server.hpp"

class ManageAll
{
	private:
		static std::vector <struct pollfd> _pollfds;
		static std::vector<Server> _vServ;
		std::vector<Client> _vClient;
	public:
		ManageAll();
		static struct pollfd pollFdCreation(int src_fd);
		static void addServer(Server &server);
};

#endif