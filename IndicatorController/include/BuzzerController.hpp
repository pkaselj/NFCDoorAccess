#ifndef BUZZER_CONTROLLER_HPP
#define BUZZER_CONTROLLER_HPP

/// Class which represents Active buzzer
class Buzzer
{
public:
	Buzzer() = delete;
	/**
	 * @brief Create new Buzzer object
	 * @param pinBCM Buzzer `SIGNAL` pin (Broadcomm/BCM pin numbering)
	*/
	Buzzer(unsigned int pinBCM);
	~Buzzer();

	void SignalSuccess();
	void SignalFailure();
	void SignalPing();

private:
	unsigned int m_pinBCM;
};

#endif