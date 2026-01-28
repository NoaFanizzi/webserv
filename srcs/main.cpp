/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nofanizz <nofanizz@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/20 12:31:03 by nofanizz          #+#    #+#             */
/*   Updated: 2026/01/28 15:07:50 by nofanizz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ManageAll.hpp"
#include "Config.hpp"

int	main(int ac, char **av)
{
	// Config test;
	// (void)ac;
	// if (av[1])
	// 	test.setFile(av[1]);
	// else
	// {
	// 	std::cout << "No config file" << std::endl;
	// 	return 1;
	// }
	(void)ac;
	(void)av;
	
	new Server();
	ManageAll::loop();
}