#ifndef I2C_LCD_CONTROLLER_HPP
#define I2C_LCD_CONTROLLER_HPP

#include <string>
#include "Timer.hpp"
#include "propertiesclass.h"

// const std::string DEFAULT_IDLE_MSG = "EMOVIS";
const std::string DEFAULT_IDLE_MSG = GlobalProperties::Get().LCD_DEFAULT_IDLE_MESSAGE;

/// Class which represents I2C HD44780 16x2 LCD Screen Controller
class I2C_LCD
{
public:
	I2C_LCD() = delete;
	/**
	 * @brief Create new I2C_LCD object
	 * @param i2c_bus RPi I2C bus (check the RPi documentation)
	*/
	I2C_LCD(unsigned int i2c_bus);
	~I2C_LCD();

	/**
	 * @brief Set time after which temporary messages will revert to `idleMessage`
	 * @param messageLingerTime_ms Time duration which temporary message stays displayed on LCD Screen (in milliseconds)
	 * @param idleMessage Permanent message which will be shown after `messageLingerTime_ms` expires.
	*/
	void SetMessageLingerTime_ms(unsigned int messageLingerTime_ms, const std::string& idleMessage = DEFAULT_IDLE_MSG);
	void ClearDisplay();
	void ClearDisplayAndDisplayDefaultMsg();
	void PutToLCD_Permanently(const std::string& message);

	/**
	* Put temporary `message` to LCD Screen. Message duration time defined by `SetMessageLingerTime_ms()`, \n
	* after which Default message will be permanently displayed \n
	* \see SetMessageLingerTime_ms()
	**/
	void PutToLCD_wTimeout(const std::string& message);

private:

	unsigned int m_i2c_bus;
	unsigned int m_i2c_device_handle;

	unsigned int m_messageLingerTime_ms;

	const std::string m_PREDEFINED_defaultIdleMessage = DEFAULT_IDLE_MSG;
	std::string m_idleMessage;

	Timer m_messageTimer;
	TimerCallbackFunctor<I2C_LCD>* m_pTimerTimeoutCallback;

	static const uint8_t m_i2c_lcd_address;

	typedef enum : uint8_t
	{
		clearDisplay_cmd = 0x01,
		cursorHome_cmd = 0x02,
		shiftCursorRight_cmd = 0x14 
	}enuCommands;

	typedef enum : uint8_t
	{
		command = 0x0,
		data = 0x1,

		read = 0x2,
		write = 0x0,

		enable = 0x4,
		backlight = 0x8
	} enuFlags;


	void SendCommand(enuCommands command);
	void SendCharacter(char character);

	void SendUpperNibbleCommand(uint8_t rawData);
	void SendUpperNibbleData(uint8_t rawData);

	void initializeLCD();
	void initializeTimer();

	void goToSecondLine();

	void clearDisplay_TimerCallback(void*);

	void PutToLCD(const std::string& message);
};

#endif