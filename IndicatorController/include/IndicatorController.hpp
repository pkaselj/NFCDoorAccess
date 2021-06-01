/*
*	 Copyright (C) Petar Kaselj 2021
*
*	 This file is part of NFCDoorAccess.
*
*	 NFCDoorAccess is written by Petar Kaselj as an employee of
*	 Emovis tehnologije d.o.o. which allowed its release under
*	 this license.
*
*    NFCDoorAccess is free software: you can redistribute it and/or modify
*    it under the terms of the GNU General Public License as published by
*    the Free Software Foundation, either version 3 of the License, or
*    (at your option) any later version.
*
*    NFCDoorAccess is distributed in the hope that it will be useful,
*    but WITHOUT ANY WARRANTY; without even the implied warranty of
*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*    GNU General Public License for more details.
*
*    You should have received a copy of the GNU General Public License
*    along with NFCDoorAccess.  If not, see <https://www.gnu.org/licenses/>.
*
*/

#ifndef INDICATOR_CONTROLLER_HPP
#define INDICATOR_CONTROLLER_HPP

#include "DataMailbox.hpp"

#include "BuzzerController.hpp"
#include "DoorController.hpp"
#include "I2C_LCD_Controller.hpp"

/// Abstract base class used to control peripherals/indicators like Doors, Buzzers and LCD_Screens
class I_IndicatorController
{
public:
	I_IndicatorController() {};
	virtual ~I_IndicatorController() {};

	virtual void BuzzerPing() = 0;
	virtual void BuzzerSuccess() = 0;
	virtual void BuzzerFailure() = 0;

	virtual void LCD_Clear() = 0;
	virtual void LCD_Clear_wDefaultMsg() = 0;
	virtual void LCD_Put_Permanently(const std::string& message) = 0;
	virtual void LCD_Put_wTimeout(const std::string& message) = 0;

	virtual void OpenDoor_wBuzzerSuccess() = 0;

private:

};

class IndicatorController_Client : private I_IndicatorController
{
public:
	IndicatorController_Client(
		const std::string& identifier,
		ILogger* pLogger = NulLogger::getInstance()
	);

	~IndicatorController_Client() {};

	void ProcessRequest(unsigned int timeout_ms);

	virtual void BuzzerPing();
	virtual void BuzzerSuccess();
	virtual void BuzzerFailure();

	virtual void LCD_Clear();
	virtual void LCD_Clear_wDefaultMsg();
	virtual void LCD_Put_Permanently(const std::string& message);
	virtual void LCD_Put_wTimeout(const std::string& message);

	virtual void OpenDoor_wBuzzerSuccess();

private:

	DataMailbox m_mailbox;
	MailboxReference m_refServer;
	ILogger* m_pLogger;

	void checkParameters();

	void sendRequest(const InputParameter& command);
	void sendConnectionlessRequest(const InputParameter& command);

};

/// Struct which defines indicator pins
struct Pinout
{
	Pinout(unsigned int buzzerPin_BCM,
		unsigned int doorPin_BCM,
		unsigned int doorOpenTime_ms,
		unsigned int lcd_i2c_bus)

	:	m_buzzerPin_BCM(buzzerPin_BCM),
		m_doorPin_BCM(doorPin_BCM),
		m_doorOpenTime_ms(doorOpenTime_ms),
		m_lcd_i2c_bus(lcd_i2c_bus)
	{}

	unsigned int m_buzzerPin_BCM;
	unsigned int m_doorPin_BCM;
	unsigned int m_doorOpenTime_ms;
	unsigned int m_lcd_i2c_bus;
};

/// Requires gpioInitialise() (from pigpio library) called before constructing
class IndicatorController_Server : private I_IndicatorController
{
public:
	IndicatorController_Server(
		const std::string& identifier,
		const Pinout& pinout,
		ILogger* pLogger = NulLogger::getInstance()
	);

	~IndicatorController_Server() {};

	void ListenAndParseRequest(unsigned int timeout_ms);

	virtual void BuzzerPing();
	virtual void BuzzerSuccess();
	virtual void BuzzerFailure();

	virtual void LCD_Clear();
	virtual void LCD_Clear_wDefaultMsg();
	virtual void LCD_Put_Permanently(const std::string& message);
	virtual void LCD_Put_wTimeout(const std::string& message);

	virtual void OpenDoor_wBuzzerSuccess();

private:
	ILogger* m_pLogger;
	DataMailbox m_mailbox;
	Buzzer m_buzzer;
	Door m_door;
	I2C_LCD m_lcd;

	void ParseRequest(const InputParameter& request);

	CommandMessage* castToAppropriateType(DataMailboxMessage* pMessage);
	const InputParameter getParameter(CommandMessage* pMessage);

};


#endif