#ifndef WEBSERVER_H
#define WEBSERVER_H

#include <stdio.h> // for size_t
#include <map>
#include <vector>

class AManager;

class WebServer
{
	private:
		// variables
		static std::vector <struct pollfd> _pollfds;
		static std::map <int, AManager *> _managers;
		static void updateStatus();
		static size_t	getPollFdSize();
		static AManager* getManager(int fd);

	public:
		//functions
		static void pollFdCreation(const int &fd, AManager *manager);
		static void	run();

};

#endif
