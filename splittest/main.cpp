/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nofanizz <nofanizz@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/09 16:11:33 by nofanizz          #+#    #+#             */
/*   Updated: 2026/02/09 16:40:51 by nofanizz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <vector>
#include <string>
#include <iostream>

std::vector<std::string> Split(const std::string &str, const std::string &charset)
{
    size_t returnedPos = 0;
    int savedPos = 0;
	size_t i = 0;
	size_t j = 0;
    std::string current;
    std::vector<std::string>strVector;

    while(i < str.size())
	{
		j = 0;
        while(j < charset.size())
		{
            returnedPos = str.find(charset, savedPos);
            if(returnedPos != std::string::npos)
            {
				if(returnedPos - savedPos != 0)
				{
                	current = str.substr(savedPos, returnedPos - savedPos);
               		strVector.push_back(current);
				}
                savedPos = returnedPos + 1;
				break;
            }
			j++;
        }
		i++;
    }
	if(str.size() - savedPos != 0)
	{
		current = str.substr(savedPos, str.size() - savedPos);
		strVector.push_back(current);
	}
	return(strVector);
}

void display_tab(std::vector<std::string>strVector)
{
	size_t	i;

	i = 0;
	while(i < strVector.size())
	{
		std::cout << strVector[i] << std::endl;
		i++;
	}
}


int main(int argc, char **argv)
{
	std::string aaa = argv[1];
	std::string bbb = argv[2];
	(void)argc;
	display_tab(Split(aaa, bbb));
}