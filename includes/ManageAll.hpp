/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ManageAll.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nofanizz <nofanizz@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/22 12:37:43 by nofanizz          #+#    #+#             */
/*   Updated: 2026/01/22 12:49:48 by nofanizz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MANAGEALL_H
#define MANAGEALL_H

#include "Client.hpp"
#include "Server.hpp"

class ManageAll
{
	private:
		std::vector <pollfd> _pollfds;
		std::vector<Server> _vServ;
		std::vector<Client> _vClient;
	public:
		ManageAll();
};

#endif