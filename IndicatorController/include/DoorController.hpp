#ifndef DOOR_CONTROLLER_HPP
#define DOOR_CONTROLLER_HPP

#include "Timer.hpp"

/// Class which represents electronic lock
class Door
{
public:
	Door() = delete;

	/**
	 * @brief Create new Door object
	 * @param pinBCM Door `SIGNAL` pin (Broadcomm/BCM pin numbering)
	 * @param doorOpenTime_ms Time duration which the door stays unlocked (in milliseconds)
	*/
	Door(unsigned int pinBCM, unsigned int doorOpenTime_ms);
	~Door();

	void OpenDoors();

private:
	unsigned int m_pinBCM;
	unsigned int m_doorOpenTime_ms;

	Timer m_tmrDoor;
	TimerCallbackFunctor<Door>* m_pTimeoutCallback;

	void closeTheDoor_timerCallback(void*);
};

#endif