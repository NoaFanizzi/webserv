/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   AutoIndex.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nofanizz <nofanizz@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/05 15:01:52 by nofanizz          #+#    #+#             */
/*   Updated: 2026/02/14 14:54:10 by nofanizz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "AutoIndex.hpp"
#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sstream>
#include <ctime>
#include <stdlib.h>


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
								"</head>"
								"<body>"
									"<section>"
										"<table>"
										"<tr>"
											"<td>Name</td>"
											"<td>Date</td>"
											"<td>Size</td>"
										"</tr>";

std::string AutoIndex::_template = "<tr>"
									"<td><a href={{ URL }}>{{ NAME }}</td>"
									"<td>{{ DATE }}</td>"
									"<td>{{ WEIGHT }}</td>"
									"</tr>";

std::string AutoIndex::_footer = "</table>"
									"</section>"
									"</body>"
									"</html>";


AutoIndex::AutoIndex(const std::string &root, const std::string &location) :
	_root(root), _location(location) {}
									
void AutoIndex::replaceName(std::string &newTemplate, struct dirent &sdir)
{
	newTemplate.replace(newTemplate.find("{{ NAME }}"), 10, sdir.d_name);
}

void AutoIndex::replaceLink(std::string &newTemplate, struct dirent &sdir)
{
	std::string fLocation;
	
	fLocation = _location + sdir.d_name;
	newTemplate.replace(newTemplate.find("{{ URL }}"), 9, fLocation);
}

void AutoIndex::replaceDate(std::string &newTemplate, struct stat &file)
{
	struct tm * timeinfo = std::localtime(&file.st_mtime);

	char buffer[80];
	std::strftime(buffer, 80, "%Y-%m-%d %H:%M", timeinfo);
	std::string dateStr(buffer);

	newTemplate.replace(newTemplate.find("{{ DATE }}"), 10, dateStr);
}

void AutoIndex::replaceWeight(std::string &newTemplate, struct stat &file)
{
	std::stringstream ssweight;
	off_t temp_weight = (file.st_size / 1024);
	if (temp_weight < 1000)
		ssweight << temp_weight << " KB";
	else
		ssweight << (temp_weight / 1024) << " MB";
	newTemplate.replace(newTemplate.find("{{ WEIGHT }}"), 12, ssweight.str());
}

std::string AutoIndex::replaceTemplate(struct dirent &sdir)
{
	std::string newTemplate = _template;
	struct stat file;

	std::string filepath = _root + "/" + sdir.d_name;
	if(stat(filepath.c_str(), &file) == -1)
		exit(-10); //TODO Check les trucs d'erreur
	replaceName(newTemplate, sdir);
	replaceLink(newTemplate, sdir);
	replaceDate(newTemplate, file);
	replaceWeight(newTemplate, file);

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
	dr = opendir(_root.c_str());
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