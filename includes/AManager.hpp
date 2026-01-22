/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   AManager.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nofanizz <nofanizz@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/22 16:11:39 by nofanizz          #+#    #+#             */
/*   Updated: 2026/01/22 18:03:41 by nofanizz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef AMANAGER_HPP
#define AMANAGER_HPP

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

class AManager
{
	protected:
		struct pollfd _fd;
	public:
		AManager();
		~AManager();
		virtual void PollInHandler() = 0;
		virtual void PollOutHandler() = 0;
};

AManager::AManager()
{
}

AManager::~AManager()
{
}


#endif