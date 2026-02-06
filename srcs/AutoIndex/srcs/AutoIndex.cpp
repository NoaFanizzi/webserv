/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   AutoIndex.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nofanizz <nofanizz@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/05 15:01:52 by nofanizz          #+#    #+#             */
/*   Updated: 2026/02/06 17:32:22 by nofanizz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "AutoIndex.hpp"
#include "ManageAll.hpp"
#include <sys/types.h>
#include <dirent.h>


std::string AutoIndex::_header =
								"<!doctype html>"
								"<html lang='fr'>"
								"<style>"
									"table {"
									"margin: auto;"
									"}"
									"td {"
									"border: solid;"
									"}"
								"</style>"

								"<head>"
									"<meta charset='utf-8' />"
									"<title>Titre de la page</title>"
									"<link rel='stylesheet' href='style.css' />"
									"<script src='script.js'></script>"
								"</head>"
								"<body>"
									"<section>"
										"<table>"
										"<tr>"
											"<td>FILENAME</td>"
											"<td>2</td>"
											"<td>3</td>"
										"</tr>";

std::string AutoIndex::_template = "<tr>"
									"<td>{{ NAME }}</td>"
									"<td>{{ DATE }}</td>"
									"<td>{{ WEIGHT }}</td>"
									"</tr>";

std::string AutoIndex::_footer = "</table>"
									"</section>"
									"</body>"
									"</html>";


std::string AutoIndex::replaceTemplate(struct dirent &sdir)
{
	std::string newTemplate = _template;

	newTemplate.replace(newTemplate.find("{{ NAME }}"), 10, sdir.d_name);
	newTemplate.replace(newTemplate.find("{{ DATE }}"), 10, sdir.d_name);
	newTemplate.replace(newTemplate.find("{{ WEIGHT }}"), 12, sdir.d_name);
	return(newTemplate);
}

void AutoIndex::addNewRow(struct dirent &sdir)
{
	std::string newRow = replaceTemplate(sdir);
	_content.append(newRow);
	
}

std::string AutoIndex::initAutoIndex()
{

// Begin
//    Declare a pointer dr to the DIR type.
//    Declare another pointer en of the dirent structure.
//    Call opendir() function to open all file in present directory.
//    Initialize dr pointer as dr = opendir(".").
//    If(dr)
//       while ((en = readdir(dr)) != NULL)
//          print all the file name using en->d_name.
//       call closedir() function to close the directory.
// End.

	DIR *dr;
	
	struct dirent* sdir;

	_content = _header;
	std::cout << "DIRRRRRR" << std::endl;
	dr = opendir("website");
	if(dr)
	{
		while((sdir = readdir(dr)) != NULL)
		{
			std::cout << sdir->d_name << std::endl;
			addNewRow(*sdir);
		}
	}
	_content.append(_footer);
	return(_content);
}