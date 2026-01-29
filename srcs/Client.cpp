

#include "Client.hpp"
#include "RequestParser.hpp"
#include "ManageAll.hpp"
#include "Http404Exception.hpp"
#include "Http403Exception.hpp"
#include <fstream>
#include <sstream>

Client::Client(int fd, const ServerConfig &config): _config(config)
{
    _fd = fd;
    _closedStatus = false;
    _events = POLLIN;
    ManageAll::pollFdCreation(_fd, this);
    setErrorPages();
}


void Client::PollInHandler()
{
    _RequestParser.RequestReading(_fd, _closedStatus, _request);
     if(_RequestParser.IsComplete(_request) == true)
     {
         _events = POLLOUT;
         _RequestParser.ParseRequest(_request);
        }
}

std::string readFileTest(const std::string& path)
{
    if (access(path.c_str(), R_OK) != 0)
        throw Http403Exception();
    std::ifstream file(path.c_str());
    if (!file.is_open())
        return "";

    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

std::string Client::getErrorPage(int code)
{
    for (size_t i = 0; i < _config.error_page.size(); i++)
    {
        if (_config.error_page[i].index == code)
        {
            std::string path = _config.root + "/" + _config.error_page[i].path;
            if (access(path.c_str(), F_OK) != 0)
                return "404";
            if (access(path.c_str(), R_OK) != 0)
                return "403";
            return path;
        }
    }
    return "404";
}


std::string Client::CheckUrl()
{
    std::string path;

    if (_RequestParser.GetUrl() == "/")
        path = _config.root + "/" + _config.index;
    else
        path = _config.root + _RequestParser.GetUrl();

    if (access(path.c_str(), F_OK) != 0)
        throw Http404Exception();
        
    if (access(path.c_str(), R_OK) != 0)
        throw Http403Exception();

    return path;
}

void Client::PollOutHandler()
{
    std::string body;
    std::string statusCode = "200";
    std::string statusText = "OK";

    try{
        std::string path = CheckUrl();
        body = readFileTest(path);
    }
    catch (const HttpException& e)
    {
        std::string errorPagePath = getErrorPage(e.getStatusCode());
        if (errorPagePath == "404")
            body = _errorPages[404];
        else if (errorPagePath == "403")
            body = _errorPages[403];
        else
            body = readFileTest(errorPagePath);
        std::ostringstream oss;
        oss << e.getStatusCode();
        statusCode = oss.str();
        statusText = e.getStatusText();
    }

    std::string header = GetHeaderResponse(body.size(), statusCode, statusText);
    send(_fd, (header + body).c_str(), header.size() + body.size(), 0);

    _events = 0;
    _closedStatus = true;
}
