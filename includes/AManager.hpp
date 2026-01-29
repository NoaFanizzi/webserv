/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   AManager.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mvachon <mvachon@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/22 16:11:39 by nofanizz          #+#    #+#             */
/*   Updated: 2026/01/29 08:38:08 by mvachon          ###   ########.fr       */
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
		int 	_fd;
		bool 	_closedStatus;
		short 	_events;

	public:
		virtual ~AManager() {};
		virtual void PollInHandler() = 0;
		virtual void PollOutHandler() {};

		bool getClosedStatus() {
			return _closedStatus;
		}

		short getEvents() {
			return _events;
		}

		void setEvents(short events)
		{
			_events = events;
		}
};



#endif