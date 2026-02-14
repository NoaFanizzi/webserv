#ifndef AMANAGER_HPP
#define AMANAGER_HPP

#include <sys/socket.h>
#include <unistd.h>

class AManager {
  protected:
	// variables
	int _fd;
	bool _closedStatus;
	short _events;

  public:
	// destructor
	virtual ~AManager() {};

	// functions
	virtual void PollInHandler() = 0;
	virtual void PollOutHandler() {};

	// getter
	bool getClosedStatus() { return _closedStatus; }
	short getEvents() { return _events; }
	int getFd(void) { return _fd; }

	// setter
	void setEvents(short events) { _events = events; }
};

#endif
