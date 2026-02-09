/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nofanizz <nofanizz@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/30 10:35:02 by mvachon           #+#    #+#             */
/*   Updated: 2026/02/09 13:47:49 by nofanizz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"
#include "AutoIndex.hpp"
#include "Request.hpp"
#include "ManageAll.hpp"
#include "HttpExceptions.hpp"
#include "CgiHandler.hpp"
#include <fstream>
#include <sstream>
#include <sys/stat.h>

std::string Client::GetHeaderResponse(size_t contentLength, std::string StatusCode, std::string StatusText)
{
    std::ostringstream oss;
    oss << contentLength;


    std::string ContentType = "application/octet-stream";
    std::string url = _Request.GetPath();   

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
        "HTTP/1.0 " + StatusCode + " " + StatusText + "\r\n"
        "Content-Type: " + ContentType + "; charset=UTF-8\r\n"
        "Content-Length: " + oss.str() + "\r\n"
        "Connection: close\r\n"
        "\r\n";
    return header;
}

Client::Client(int fd, const ServerConfig &config): _config(config)
{
    _fd = fd;
    _closedStatus = false;
    _events = POLLIN;
    ManageAll::pollFdCreation(_fd, this);
    setErrorPages();
    SetMimes();
}

void Client::PollInHandler()
{
    _Request.RequestReading(_fd, _closedStatus, _request);
    
        if(_Request.IsComplete(_request) == true)
        {
            _events = POLLOUT;
            std::cout << _request << std::endl;
            std::cout << "===============================" << std::endl;
        }
}

std::string Client::readFileClient(const std::string& path)
{
    if (access(path.c_str(), R_OK) != 0)
        return _errorPages[403];
    
    std::ifstream file(path.c_str());
    if (!file.is_open())
        return "";

    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

std::string Client::CheckUrl()
{
    std::string path;

    if (_Request.GetPath() == "/")
        path = _config.root + "/" + _config.index;      
    else
        path = _config.root + _Request.GetPath();
        
    struct stat st;
    if (stat(path.c_str(), &st) != 0)
        throw Http404Exception();
        
    if (!(st.st_mode & S_IRUSR))
        throw Http403Exception();
    
    return path;
}

std::string Client::getErrorPageContent(int code)
{
    for (size_t i = 0; i < _config.error_page.size(); i++)
    {
        if (_config.error_page[i].index == code)
        {
            std::string path = _config.root + "/" + _config.error_page[i].path;
            struct stat st;
            
            if (stat(path.c_str(), &st) == 0 && (st.st_mode & S_IRUSR))
                return readFileClient(path);
        }
    }
    
    std::map<int, std::string>::iterator it = _errorPages.find(code);
    if (it != _errorPages.end())
        return it->second;
    
    return _errorPages[404];
}

void Client::PollOutHandler()
{
    std::string body;
    std::string statusCode = "200";
    std::string statusText = "OK";
    std::string finalPath;

    try {
        _Request.Parse(_request);
        finalPath = CheckUrl();
        
        if(_config.autoindex == true)
        {
            AutoIndex Indexation = AutoIndex(_config.root, _Request.GetPath());
            body = Indexation.initAutoIndex();
        }
		else if (CgiHandler::isCgi(finalPath)) // puts the cgi in body if isCgi()
		{
			CgiHandler cgi(_Request, finalPath);
			cgi.execute();
			body = cgi.getOutput();
		}
		else
        	body = readFileClient(finalPath);
    }
    catch (const HttpException& e)
    {
        int errorCode = e.getStatusCode();
        std::ostringstream oss;
        oss << errorCode;
        statusCode = oss.str();
        statusText = e.getStatusText();
        
        body = getErrorPageContent(errorCode);
        finalPath = ".html";
    }
    
    _Request.SetPath(finalPath);
	if (CgiHandler::isCgi(finalPath)) // if isCgi() send the output of the script
	{
		send(_fd, body.c_str(), body.size(), 0);
	}
	else
	{
    	std::string header = GetHeaderResponse(body.size(), statusCode, statusText);
    	send(_fd, (header + body).c_str(), header.size() + body.size(), 0);
	}
    _events = 0;
    _closedStatus = true;
    
}

void Client::SetMimes()
{
    mimeTypes[".html"] = "text/html";
    mimeTypes[".htm"]  = "text/html";
    mimeTypes[".css"]  = "text/css";
    mimeTypes[".js"]   = "application/javascript";
    mimeTypes[".json"] = "application/json";
    mimeTypes[".xml"]  = "application/xml";
    mimeTypes[".txt"]  = "text/plain";
    mimeTypes[".csv"]  = "text/csv";
    mimeTypes[".cpp"]  = "text/x-c++src";

    mimeTypes[".png"]  = "image/png";
    mimeTypes[".jpg"]  = "image/jpeg";
    mimeTypes[".jpeg"] = "image/jpeg";
    mimeTypes[".gif"]  = "image/gif";
    mimeTypes[".bmp"]  = "image/bmp";
    mimeTypes[".svg"]  = "image/svg+xml";
    mimeTypes[".webp"] = "image/webp";
    mimeTypes[".ico"]  = "image/x-icon";

    mimeTypes[".mp3"]  = "audio/mpeg";
    mimeTypes[".wav"]  = "audio/wav";
    mimeTypes[".ogg"]  = "audio/ogg";
    mimeTypes[".aac"]  = "audio/aac";
    mimeTypes[".flac"] = "audio/flac";

    mimeTypes[".mp4"]  = "video/mp4";
    mimeTypes[".webm"] = "video/webm";
    mimeTypes[".ogv"]  = "video/ogg";
    mimeTypes[".avi"]  = "video/x-msvideo";
    mimeTypes[".mov"]  = "video/quicktime";

    mimeTypes[".zip"]  = "application/zip";
    mimeTypes[".tar"]  = "application/x-tar";
    mimeTypes[".gz"]   = "application/gzip";
    mimeTypes[".7z"]   = "application/x-7z-compressed";
    mimeTypes[".rar"]  = "application/vnd.rar";

    mimeTypes[".ttf"]  = "font/ttf";
    mimeTypes[".otf"]  = "font/otf";
    mimeTypes[".woff"] = "font/woff";
    mimeTypes[".woff2"]= "font/woff2";

    mimeTypes[".pdf"]  = "application/pdf";
    mimeTypes[".bin"]  = "application/octet-stream";
    mimeTypes[".exe"]  = "application/octet-stream";
}
