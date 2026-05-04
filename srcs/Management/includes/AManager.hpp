#ifndef AMANAGER_HPP
#define AMANAGER_HPP

#include <sys/socket.h>
#include <unistd.h>
#include <ctime>

class AManager {
  protected:
	// variables
	int _fd;
	bool _closedStatus;
	short _events;
	time_t _startTime;

  public:
	// destructor
	virtual ~AManager() {};

	// functions
	virtual void pollInHandler() = 0;
	virtual void pollOutHandler() {};
	virtual bool isTimeout(time_t timeNow) {return (timeNow - _startTime > 5);}

	// getter
	time_t getStartTime() {return _startTime;}
	bool getClosedStatus() { return _closedStatus; }
	short getEvents() { return _events; }
	int getFd(void) { return _fd; }

	// setter
	void setEvents(short events) { _events = events; }
	void setStartTime(const time_t timeNow) { _startTime = timeNow; }
	virtual void onTimeout() {
		_closedStatus = true;
	}
};

#endif