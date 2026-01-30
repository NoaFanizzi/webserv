/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RequestParser.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mvachon <mvachon@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/28 18:56:53 by mvachon           #+#    #+#             */
/*   Updated: 2026/01/30 18:01:19 by mvachon          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"
#include "RequestParser.hpp"
#include "ManageAll.hpp"

std::map<std::string, std::string> RequestParser::SeparateHeaders(std::vector<std::string> &docRequest)
{
    std::map<std::string, std::string> headers;

    for (size_t i = 1; i < docRequest.size(); ++i)
    {
        std::string &line = docRequest[i];
        
        if (line.empty())
            break;
        
        size_t colonPos = line.find(':');
        if (colonPos == std::string::npos)
            continue;
        
        std::string key = line.substr(0, colonPos);
        std::string value = line.substr(colonPos + 1);
        
        size_t start = value.find_first_not_of(" \t");
        if (start != std::string::npos)
            value = value.substr(start);
        
        headers[key] = value;
    }

    return headers;
}

void RequestParser::CheckRequest()
{
    if (_method != "GET" && _method != "POST" && _method != "DELETE")
        ManageAll::SetError405(true);
    if (_version != "HTTP/1.1" && _version != "HTTP/1.0")
        ManageAll::SetError400(true);
}

void RequestParser::printRequest(std::vector<std::string> docRequest, std::map<std::string, std::string> &headers)
{
    (void)docRequest;
    (void)headers;
    for (size_t i = 0; i < docRequest.size(); i++)
        std::cout << docRequest[i] << std::endl;

    // std::cout << " ____________________________\n" << std::endl;
    
    // std::cout << "|  === REQUEST LINE ===" << std::endl;
    // std::cout << "|  Method: " << _method << std::endl;
    // std::cout << "|  URL: " << _url << std::endl;
    // std::cout << "|  Version: " << _version << std::endl;
    
    // std::cout << "|\n|  === HEADERS ===" << std::endl;
    // std::map<std::string, std::string>::iterator it;
    // for (it = headers.begin(); it != headers.end(); ++it) {
    //     std::cout << "|  "<<it->first << ": " << it->second << std::endl;
    // }
    // std::cout << " __________________________________________"<< std::endl;
    // std::cout << std::endl;
}

std::string Client::GetHeaderResponse(size_t contentLength, std::string StatusCode, std::string StatusText)
{
    std::ostringstream oss;
    oss << contentLength;

    std::string ContentType = "application/octet-stream";
    std::string url = _RequestParser.GetUrl();

    if (!url.empty()) {
        size_t pos = url.rfind('.');
        if (pos != std::string::npos && pos != url.size() - 1) { 
            std::string ext = url.substr(pos);
            std::map<std::string, std::string>::iterator it = mimeTypes.find(ext);
            if (it != mimeTypes.end())
                ContentType = it->second;
        }
    }
    std::string header =
        "HTTP/1.1 " + StatusCode + " " + StatusText + "\r\n"
        "Content-Type: " + ContentType + "; charset=UTF-8\r\n"
        "Content-Length: " + oss.str() + "\r\n"
        "Connection: closed\r\n"
        "\r\n";
    return header;
}

void RequestParser::ParseRequest(const std::string& request)
{
    std::string line;
    std::vector<std::string> docRequest;
    
    for (size_t i = 0; i < request.size(); ++i)
    {
        char c = request[i];
        if (c == '\n') {
            docRequest.push_back(line);
            line.clear(); 
        } else if (c != '\r') {
            line.push_back(c);
        }
    }

    if (!line.empty())
        docRequest.push_back(line);

    if (docRequest.empty())
        return;
    std::map<std::string, std::string> headers = SeparateHeaders(docRequest);

    std::istringstream iss(docRequest[0]);
    iss >> _method >> _url >> _version;
    printRequest(docRequest, headers);
    CheckRequest();
}
bool    RequestParser::IsComplete(std::string &request)
{
    if(request.find("\r\n\r\n", 0) != std::string::npos)
        return(true);
    return(false);
}

void RequestParser::RequestReading(int &fd, bool &closedStatus, std::string &request)
{
    char buffer[BUFFER_SIZE];
    int n = recv(fd, buffer, sizeof(buffer) - 1, 0);
    
    if (n <= 0) {
        closedStatus = true;
        return;
    }
    buffer[n] = '\0';
    request.append(buffer, n);
}
