/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ManageAll.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nofanizz <nofanizz@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/22 12:43:22 by nofanizz          #+#    #+#             */
/*   Updated: 2026/01/22 13:31:42 by nofanizz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ManageAll.hpp"

ManageAll::ManageAll()
{
	
}

struct pollfd ManageAll::pollFdCreation(int src_fd)
{
	struct pollfd poll;
	poll.fd = src_fd;
	poll.events = POLLIN;
	poll.revents = 0;
	
	_pollfds.push_back(poll);
	return poll;
}
