#include "I2C_LCD_Controller.hpp"

#include <unistd.h>

#include <pigpio.h>

#include "Kernel.hpp"
#include "propertiesclass.h"


const uint8_t I2C_LCD::m_i2c_lcd_address = GlobalProperties::Get().LCD_I2C_ADDRESS;
const unsigned int defaultMessageLingerTime_ms = GlobalProperties::Get().LCD_DEFULT_MSG_DISPLAY_TIME_MS;
const unsigned int interCommandWaitTime_us = GlobalProperties::Get().LCD_INTER_COMMANDS_WAIT_TIME_US;

I2C_LCD::I2C_LCD(unsigned int i2c_bus)
	: m_i2c_bus(i2c_bus),
	m_i2c_device_handle(-1),
	m_messageLingerTime_ms(defaultMessageLingerTime_ms),
	m_idleMessage(m_PREDEFINED_defaultIdleMessage),
	m_messageTimer("LCD_Screen_" + std::to_string(std::rand()%1000)), // TODO? rand name
	m_pTimerTimeoutCallback(nullptr)
{
	
	m_i2c_device_handle = i2cOpen(m_i2c_bus, m_i2c_lcd_address, 0);

	if (m_i2c_device_handle < 0)
	{
		Kernel::Fatal_Error("Error while trying to open LCD I2C device. Device handle: " + std::to_string(m_i2c_device_handle));
	}

	initializeTimer();

	initializeLCD();

	ClearDisplayAndDisplayDefaultMsg();
}

void I2C_LCD::initializeTimer()
{
	m_pTimerTimeoutCallback = new TimerCallbackFunctor<I2C_LCD>(this, &I2C_LCD::clearDisplay_TimerCallback);

	m_messageTimer.setTimeout_ms(m_messageLingerTime_ms);
	m_messageTimer.setTimeoutCallback(m_pTimerTimeoutCallback);
}

I2C_LCD::~I2C_LCD()
{
	i2cClose(m_i2c_device_handle);
	delete m_pTimerTimeoutCallback;
}

void I2C_LCD::ClearDisplay()
{
	m_messageTimer.Stop();
	SendCommand(enuCommands::clearDisplay_cmd);
}

void I2C_LCD::ClearDisplayAndDisplayDefaultMsg()
{
	m_idleMessage = m_PREDEFINED_defaultIdleMessage;
	ClearDisplay();
	PutToLCD_Permanently(m_idleMessage);
}

void I2C_LCD::SetMessageLingerTime_ms(unsigned int messageLingerTime_ms, const std::string& idleMessage)
{
	m_messageLingerTime_ms = messageLingerTime_ms;

	if (idleMessage != DEFAULT_IDLE_MSG)
	{
		m_idleMessage = idleMessage;
	}

}

void I2C_LCD::PutToLCD_wTimeout(const std::string& message)
{
	m_messageTimer.Stop();
	PutToLCD(message);
	m_messageTimer.Start(); // Timer to clearScreen
}

void I2C_LCD::PutToLCD_Permanently(const std::string& message)
{
	m_messageTimer.Stop();

	// So that message can be brought back after any temporary message
	m_idleMessage = message;

	PutToLCD(message);
}

void I2C_LCD::SendCommand(enuCommands command)
{
	SendUpperNibbleCommand(command);
	SendUpperNibbleCommand(command << 4);
}

void I2C_LCD::SendCharacter(char character)
{
	uint8_t rawCharacter = (uint8_t)character;

	/*
	std::stringstream stream;
	stream << "Character:"  << character << " == " << std::uppercase << std::setw(2) << std::setfill('0') << std::hex << (int)rawCharacter;
	Kernel::Trace(stream.str());
	*/

	SendUpperNibbleData(rawCharacter);
	SendUpperNibbleData(rawCharacter << 4);
}

void I2C_LCD::SendUpperNibbleCommand(uint8_t rawData)
{
	rawData &= 0xF0; // set lower nibble to 0

	i2cWriteByte(m_i2c_device_handle, rawData | enuFlags::backlight | enuFlags::command);
	usleep(interCommandWaitTime_us);

	i2cWriteByte(m_i2c_device_handle, rawData | enuFlags::backlight | enuFlags::command | enuFlags::enable);
	usleep(interCommandWaitTime_us);

	i2cWriteByte(m_i2c_device_handle, rawData | enuFlags::backlight | enuFlags::command);
	usleep(interCommandWaitTime_us);
}

void I2C_LCD::SendUpperNibbleData(uint8_t rawData)
{
	rawData &= 0xF0; // set lower nibble to 0

	i2cWriteByte(m_i2c_device_handle, rawData | enuFlags::backlight | enuFlags::data);
	usleep(interCommandWaitTime_us);

	i2cWriteByte(m_i2c_device_handle, rawData | enuFlags::backlight | enuFlags::data | enuFlags::enable);
	usleep(interCommandWaitTime_us);

	i2cWriteByte(m_i2c_device_handle, rawData | enuFlags::backlight | enuFlags::data);
	usleep(interCommandWaitTime_us);
}

void I2C_LCD::initializeLCD()
{
	const uint8_t initSequence[14] =
	{
	  0x30,
	  0x30,
	  0x30,
	  0x20,
	  0x20,
	  0x80,
	  0x00,
	  0xC0,
	  0x00,
	  0x10,
	  0x00,
	  0x60,
	  0x00,
	  0x20,
	};

	for (size_t i = 0; i < sizeof(initSequence) / sizeof(uint8_t); i++)
	{
		SendUpperNibbleCommand(initSequence[i]);
	}
}

void I2C_LCD::goToSecondLine()
{
	SendCommand(enuCommands::cursorHome_cmd);

	const int charsFromBeginningToNewLine = 40;
	for (int i = 0; i < charsFromBeginningToNewLine; i++)
	{
		SendCommand(enuCommands::shiftCursorRight_cmd);
	}
}

void I2C_LCD::clearDisplay_TimerCallback(void* uu)
{
	ClearDisplay();
	PutToLCD_Permanently(m_idleMessage);
	
}

void I2C_LCD::PutToLCD(const std::string& message)
{
	std::string firstLine = message;
	std::string secondLine = "";

	const int charsPerLine = 16;
	if (firstLine.length() > charsPerLine)
	{
		firstLine = message.substr(0, charsPerLine);

		const int charactersLeft = message.length() - charsPerLine;

		secondLine = message.substr(charsPerLine, std::max(charactersLeft, charsPerLine));
	}

	ClearDisplay();


	for (auto& character : firstLine)
	{
		SendCharacter(character);
	}

	if (secondLine != "")
	{
		goToSecondLine();

		for (auto& character : secondLine)
		{
			SendCharacter(character);
		}
	}
}