/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ManageAll.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nofanizz <nofanizz@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/22 12:37:43 by nofanizz          #+#    #+#             */
/*   Updated: 2026/01/28 16:49:00 by nofanizz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MANAGEALL_H
#define MANAGEALL_H

#include "Client.hpp"
#include "Server.hpp"
#include "AManager.hpp"
#include "Config.hpp"
#include <map> 

class ManageAll
{
	private:
		static std::vector <struct pollfd> _pollfds;
		static std::map <int, AManager *> _managers;
		static size_t	getPollFdSize();
		static AManager* getManager(int fd);
		static void updateStatus();

	public:
		static void pollFdCreation(const int &fd, AManager *manager);
		
		struct pollfd& getPollFd(int index);
		static void	loop();
};

#endif