/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ManageAll.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nofanizz <nofanizz@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/22 12:37:43 by nofanizz          #+#    #+#             */
/*   Updated: 2026/01/31 11:16:35 by nofanizz         ###   ########.fr       */
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
		static bool error400;
		static bool error405;
		static bool error408;
		static std::vector <struct pollfd> _pollfds;
		static std::map <int, AManager *> _managers;
		static size_t	getPollFdSize();
		static AManager* getManager(int fd);
		static void updateStatus();

	public:
		static void pollFdCreation(const int &fd, AManager *manager);
		struct pollfd& getPollFd(int index);
		static void	loop();
		static bool GetError408() {return error408;}
		static bool GetError400() {return error400;}
		static bool GetError405() {return error405;}
		static void SetError400(bool error) {error400 = error;}
		static void SetError405(bool error) {error405 = error;}
		static void SetError408(bool error) {error408 = error;}
};

#endif