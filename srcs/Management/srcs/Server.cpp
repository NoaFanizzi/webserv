#include "Server.hpp"
#include "Client.hpp"
#include "WebServer.hpp"
#include <cstring> // Pour memset
#include <iostream>
#include <netdb.h> // Obligatoire pour getaddrinfo
#include <poll.h>

Server::Server(const ServerConfig &serverconfig) : _serverConfig(serverconfig) {
	_closedStatus = false;
	_fd = createSocket();
	createSocketAdress(serverconfig);
	int opt = 1;
	setsockopt(_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
	bindClient(_fd);
	listen(_fd, 4096);
	WebServer::pollFdCreation(_fd, this);
	_events = POLLIN;
}

Server::~Server() {
	if (_fd != -1)
		close(_fd);
}

int Server::createSocket() {
	int fd;
	if ((fd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		close(fd);
		throw std::runtime_error("Socket error");
	}
	return fd;
}

void Server::createSocketAdress(const ServerConfig &serverconfig) {
	struct addrinfo hints, *res;
	int status;
	std::memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;

	if ((status = getaddrinfo(serverconfig.host.c_str(), NULL, &hints, &res)) !=
	    0) {
		std::cerr << "getaddrinfo error: " << gai_strerror(status) << std::endl;
		throw std::runtime_error("Failed to resolve host IP");
	}
	
	_servAddr.sin_family = AF_INET;
	_servAddr.sin_addr = ((struct sockaddr_in *)res->ai_addr)->sin_addr;
	_servAddr.sin_port = htons(serverconfig.port);
	freeaddrinfo(res);
}

void Server::bindClient(int fd) {
	if (bind(fd, (sockaddr *)&_servAddr, sizeof(_servAddr)) == -1) {
		close(fd);
		throw std::runtime_error("Bind failed");
	}
}

void Server::PollInHandler(){
    struct sockaddr_in clientaddr;
    socklen_t addr_len = sizeof(clientaddr);
    int client_fd = accept(_fd, (sockaddr *)&clientaddr, &addr_len);
    if (client_fd == -1) {
        perror("accept failed");
        return;
    }
    fcntl(client_fd, F_SETFL, O_NONBLOCK);
    new Client(client_fd, _serverConfig);
}
