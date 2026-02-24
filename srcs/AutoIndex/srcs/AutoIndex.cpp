/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   AutoIndex.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nofanizz <nofanizz@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/05 15:01:52 by nofanizz          #+#    #+#             */
/*   Updated: 2026/02/24 17:25:14 by nofanizz         ###   ########.fr       */
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
	"<!DOCTYPE html>"
	"<html lang='en'>"
	"<head>"
	"<meta charset='UTF-8'/>"
	"<title>Index</title>"
	"<style>"
	"body {"
	"  font-family: monospace;"
	"  background: #1a1a1a;"
	"  color: #ccc;"
	"  margin: 0;"
	"  height: 100vh;"
	"  display: flex;"
	"  justify-content: center;"
	"  align-items: center;"
	"}"
	".container {"
	"  background: #222;"
	"  padding: 40px;"
	"  border-radius: 8px;"
	"  box-shadow: 0 0 25px rgba(0,0,0,0.6);"
	"  width: 700px;"
	"  max-width: 90%;"
	"}"
	"h1 {"
	"  color: #fff;"
	"  font-size: 24px;"
	"  margin-bottom: 30px;"
	"  border-bottom: 1px solid #333;"
	"  padding-bottom: 15px;"
	"  text-align: center;"
	"}"
	"table {"
	"  width: 100%;"
	"  border-collapse: collapse;"
	"}"
	"th {"
	"  color: #777;"
	"  font-size: 13px;"
	"  text-transform: uppercase;"
	"  letter-spacing: .1em;"
	"  padding: 12px 16px;"
	"  text-align: left;"
	"}"
	"td {"
	"  padding: 14px 16px;"
	"  border-top: 1px solid #2a2a2a;"
	"  font-size: 15px;"
	"}"
	"a {"
	"  color: #6af;"
	"  text-decoration: none;"
	"}"
	"a:hover {"
	"  text-decoration: underline;"
	"}"
	".size {"
	"  color: #777;"
	"}"
	".delete-button {"
	"background-color: red;"
	"border: none;"
	"color: white;"
	"padding: 15px 32px;"
	"text-align: center;"
	"text-decoration: none;"
	"display: inline-block;"
	"font-size: 16px;"
	"cursor: pointer;"
	"}"
	".dir a { color: #b8d839ff; font-weight: bold; }"
	".file a { color: #6af; }"
	"</style>"
	"<script>"
		"document.addEventListener('DOMContentLoaded', () => {"
		"  const deleteButtons = document.querySelectorAll('.delete-button');"
		"  deleteButtons.forEach(button => {"
		"    button.addEventListener('click', handleDeleteClick);"
		"  });"
		"});"
		"async function handleDeleteClick(event) {"
		"  const button = event.currentTarget;"
		"  const id = button.dataset.id;"
		"const ok = confirm('Are you sure that you want to delete this file ?');"
		"if (!ok) {"
			"return;"
		"}"
		"  const response = await fetch(`${id}`, {"
		"    method: 'DELETE'"
		"  });"
		"  if (response.ok) {"
		"    button.closest('tr')?.remove();"
		"    alert(\"File deleted\");"
		"  }"
		"}"
	"</script>"


	"</head>"
	"<body>"
	"<div class='container'>"
	"<h1>Index</h1>"
	"<table>"
	"<thead><tr><th>Name</th><th>Date</th><th>Size</th></tr></thead>"
	"<tbody>";

std::string AutoIndex::_template =
	"<tr class='{{ TYPE }}'>"
	"<td><a href={{ URL }}>{{ NAME }}</a></td>"
	"<td class='size'>{{ DATE }}</td>"
	"<td class='size'>{{ WEIGHT }}</td>"
	"<td class='delete-button' data-id={{ DELETE }}>DELETE</td>"
	"</tr>";

std::string AutoIndex::_footer =
	"</tbody></table>"
	"</div>"
	"</body></html>";

AutoIndex::AutoIndex(const std::string &root, const std::string &location) :
	_root(root), _location(location) {}
									
void AutoIndex::replaceName(std::string &newTemplate, struct dirent &sdir)
{
	const std::string name = sdir.d_name;
	std::string test;
	newTemplate.find("dir") != std::string::npos ? test = "📁 " : test = "";
	newTemplate.replace(newTemplate.find("{{ NAME }}"), 10, test +  name);
}


void AutoIndex::replaceLink(std::string &newTemplate, struct dirent &sdir)
{
    std::string fLocation = _location;
    
    if (!fLocation.empty() && fLocation[fLocation.length() - 1] != '/')
        fLocation += "/";
    
    fLocation += sdir.d_name;
    newTemplate.replace(newTemplate.find("{{ URL }}"), 9, fLocation);
    newTemplate.replace(newTemplate.find("{{ DELETE }}"), 12, fLocation);
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

std::string AutoIndex::replaceTemplate(struct dirent &sdir, const std::string &rPath)
{
	std::string newTemplate = _template;
	struct stat file;

	std::string filepath = rPath + "/" + sdir.d_name;
	if(stat(filepath.c_str(), &file) == -1)
		exit(-10); // TODO check error
		
	if (S_ISDIR(file.st_mode))
		newTemplate.replace(newTemplate.find("{{ TYPE }}"), 10, "dir");
	else
		newTemplate.replace(newTemplate.find("{{ TYPE }}"), 10, "file");

	replaceName(newTemplate, sdir);
	replaceLink(newTemplate, sdir);
	replaceDate(newTemplate, file);
	replaceWeight(newTemplate, file);

	return newTemplate;
}

void AutoIndex::addNewRow(struct dirent &sdir, const std::string &rPath)
{
	std::string newRow = replaceTemplate(sdir, rPath);
	_content.append(newRow);
	
}

std::string AutoIndex::initAutoIndex(const std::string &rPath)
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
	dr = opendir(rPath.c_str());
	if(dr)
	{
		while((sdir = readdir(dr)) != NULL)
		{
			// std::cout << sdir->d_name << std::endl;
			addNewRow(*sdir, rPath);
		}
	}
	_content.append(_footer);
	closedir(dr);
	return(_content);
}