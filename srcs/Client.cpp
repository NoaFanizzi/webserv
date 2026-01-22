/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nofanizz <nofanizz@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/22 11:47:07 by nofanizz          #+#    #+#             */
/*   Updated: 2026/01/22 18:09:16 by nofanizz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"

Client::Client(int fd)
{
	_fd.fd = fd;	
	_fd.events = POLLIN;
	_fd.revents = 0;
	ManageAll::pollFdCreation(_fd);
}

void Client::updateRequest(std::string &buffer, int n)
{
	this->request->append(buffer, n);
}

void	Client::PollInHandler()
{
	
}

void	Client::PollOutHandler()
{
	
}
