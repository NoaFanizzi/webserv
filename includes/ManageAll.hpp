/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ManageAll.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nofanizz <nofanizz@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/22 12:37:43 by nofanizz          #+#    #+#             */
/*   Updated: 2026/01/22 18:10:53 by nofanizz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MANAGEALL_H
#define MANAGEALL_H

#include "Client.hpp"
#include "Server.hpp"
#include "AManager.hpp"

class ManageAll
{
	private:
		static std::vector <struct pollfd&> _pollfds;
		static std::map <int, AManager&> _managers;
	public:
		ManageAll();
		static void ManageAll::pollFdCreation(struct pollfd &poll);
		static void addToManager(Server &server);
		struct pollfd& getPollFd(int index);
		Server& getServer(int index);
};

#endif