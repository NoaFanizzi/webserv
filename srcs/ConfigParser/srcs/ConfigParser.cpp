/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigParser.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mvachon <mvachon@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/28 10:30:00 by mvachon           #+#    #+#             */
/*   Updated: 2026/01/28 10:18:49 by mvachon          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Config.hpp"
#include <sstream>
#include <fstream>

std::vector<std::vector<std::string> > splitLinesWords(const std::string &content)
{
    std::vector<std::vector<std::string> > result;
    std::istringstream contentStream(content);
    std::string line;

    while (std::getline(contentStream, line))
    {
        std::vector<std::string> lineWords;
        std::istringstream lineStream(line);
        std::string word;
        
        while (lineStream >> word)
            lineWords.push_back(word);
        
        if (!lineWords.empty())
            result.push_back(lineWords);
    }

    return result;
}

std::string readFile(const std::string& doc)
{
    std::ifstream file(doc.c_str());
    if (!file.is_open())
        throw Exception("Error : can't open the file");
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    
    if (file.bad())
    {
        file.close();
        throw Exception("Error : can't read the file");
    }
    
    file.close();
    return buffer.str();
}
