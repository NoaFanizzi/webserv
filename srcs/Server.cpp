

#include "ManageAll.hpp"
#include "fcntl.h"

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>      // Obligatoire pour getaddrinfo
#include <cstring>      // Pour memset
#include <stdexcept>    // Pour std::runtime_error (optionnel, pour gérer l'erreur)

Server::Server(const ServerConfig &serverconfig) : _serverconfig(serverconfig)
{
	_closedStatus = false;
	_fd = createSocket();
	createSocketAdress(serverconfig);
	int opt = 1;
	setsockopt(_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
	bindClient(_fd);
	listen(_fd, 4096);
	ManageAll::pollFdCreation(_fd, this);
	_events = POLLIN;
}

Server::~Server()
{
	if (_fd != -1)
		close(_fd);
}

int Server::getListen_fd() const
{
	return(_fd);
}

sockaddr_in Server::getSockddr_in() const
{
	return(_servaddr);
}

int	Server::createSocket()
{
	int fd;
	if ((fd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
		std::cerr << "Socket Error" << std::endl;

	return fd;
}

void Server::createSocketAdress(const ServerConfig &serverconfig)
{
    struct addrinfo hints, *res;
    int status;

    std::memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;

    if ((status = getaddrinfo(serverconfig.host.c_str(), NULL, &hints, &res)) != 0) {
        std::cerr << "getaddrinfo error: " << gai_strerror(status) << std::endl;
        throw std::runtime_error("Failed to resolve host IP");
    }

    _servaddr.sin_family = AF_INET;
    _servaddr.sin_addr = ((struct sockaddr_in *)res->ai_addr)->sin_addr;
    _servaddr.sin_port = htons(serverconfig.port);

    freeaddrinfo(res);
}

void Server::bindClient(int fd)
{
	if (bind(fd, (sockaddr *)&_servaddr, sizeof(_servaddr)) == -1)
		std::cerr << "Open Socket : bind failed" << std::endl;
	//TODO close/exit if cerr (means multiples serv at the same time)
}

void	Server::PollInHandler()
{
	struct sockaddr_in clientaddr;
	socklen_t addr_len = sizeof(clientaddr);
	int client_fd = accept(_fd, (sockaddr *)&clientaddr, &addr_len);
	fcntl(client_fd, F_SETFL, O_NONBLOCK);
	new Client(client_fd, _serverconfig);
}


