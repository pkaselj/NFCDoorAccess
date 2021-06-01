#include "DoorController.hpp"

#include <unistd.h>

#include <pigpio.h>

#include "Kernel.hpp"

const unsigned int DOOR_OPEN_PIN_STATE = PI_LOW;
const unsigned int DOOR_CLOSED_PIN_STATE = PI_HIGH;
const unsigned int DOOR_PIN_PUD = PI_PUD_UP;

Door::Door(unsigned int pinBCM, unsigned int doorOpenTime_ms)
	: m_pinBCM(pinBCM), m_doorOpenTime_ms(doorOpenTime_ms), m_tmrDoor("door"), // TODO rand name to prevent clashes
	m_pTimeoutCallback(new TimerCallbackFunctor<Door>(this, &Door::closeTheDoor_timerCallback))
{
	int pinStatus = gpioSetMode(m_pinBCM, PI_OUTPUT);
	if (pinStatus < 0)
	{
		Kernel::Fatal_Error("Door invalid pin: " + std::to_string(m_pinBCM));
	}

	gpioSetPullUpDown(m_pinBCM, DOOR_PIN_PUD);
	gpioWrite(m_pinBCM, DOOR_CLOSED_PIN_STATE);

	m_tmrDoor.setTimeout_ms(doorOpenTime_ms);
	m_tmrDoor.setTimeoutCallback(m_pTimeoutCallback);
}

Door::~Door()
{
	delete m_pTimeoutCallback;
	m_tmrDoor.Stop(); // ?
	closeTheDoor_timerCallback(nullptr); //?
}

void Door::OpenDoors()
{
	gpioWrite(m_pinBCM, DOOR_OPEN_PIN_STATE);
	//usleep(m_doorOpenTime_ms * 1000);
	//gpioWrite(m_pinBCM, PI_LOW);

	m_tmrDoor.Reset();
}

void Door::closeTheDoor_timerCallback(void*)
{
	gpioWrite(m_pinBCM, DOOR_CLOSED_PIN_STATE);
}