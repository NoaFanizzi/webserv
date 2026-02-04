/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ManageAll.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mvachon <mvachon@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/22 12:37:43 by nofanizz          #+#    #+#             */
/*   Updated: 2026/02/04 17:08:36 by mvachon          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MANAGEALL_H
#define MANAGEALL_H

#include "Client.hpp"
#include "Server.hpp"
#include "AManager.hpp"
#include "Config.hpp"
#include "Request.hpp"
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