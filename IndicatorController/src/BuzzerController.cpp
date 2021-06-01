#include "BuzzerController.hpp"

#include <unistd.h>

#include <pigpio.h>

#include "Kernel.hpp"
#include "propertiesclass.h"

const unsigned int pingDuration_ms = GlobalProperties::Get().BUZZER_PING_DURATION_MS;

Buzzer::Buzzer(unsigned int pinBCM)
	:	 m_pinBCM(pinBCM)
{
	int pinStatus = gpioSetMode(m_pinBCM, PI_OUTPUT);
	if (pinStatus < 0)
	{
		Kernel::Fatal_Error("Buzzer invalid pin: " + std::to_string(m_pinBCM));
	}

	gpioSetPullUpDown(m_pinBCM, PI_PUD_DOWN);
	gpioWrite(m_pinBCM, PI_LOW);
}

Buzzer::~Buzzer()
{
}

void Buzzer::SignalSuccess()
{
	gpioWrite(m_pinBCM, PI_HIGH);
	usleep(5 * pingDuration_ms * 1000);

	gpioWrite(m_pinBCM, PI_LOW);
	usleep(5 * pingDuration_ms * 1000);

	gpioWrite(m_pinBCM, PI_HIGH);
	usleep(5 * pingDuration_ms * 1000);

	gpioWrite(m_pinBCM, PI_LOW);
	//usleep(5 * pingDuration_ms * 1000);

}

void Buzzer::SignalFailure()
{
	gpioWrite(m_pinBCM, PI_HIGH);
	usleep(5 * pingDuration_ms * 1000);

	gpioWrite(m_pinBCM, PI_LOW);
	usleep(5 * pingDuration_ms * 1000);

	gpioWrite(m_pinBCM, PI_HIGH);
	usleep(5 * 5 * pingDuration_ms * 1000);

	gpioWrite(m_pinBCM, PI_LOW);
	//usleep(5 * pingDuration_ms * 1000);
}

void Buzzer::SignalPing()
{
	gpioWrite(m_pinBCM, PI_HIGH);
	usleep(2 * pingDuration_ms * 1000);

	gpioWrite(m_pinBCM, PI_LOW);
	//usleep(2 * pingDuration_ms * 1000);

}
