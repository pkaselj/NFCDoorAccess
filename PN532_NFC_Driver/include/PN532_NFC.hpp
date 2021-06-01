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

#ifndef PN532_NFC_HPP
#define PN532_NFC_HPP

#include <nfc/nfc.h>
#include <string>

#include "NulLogger.hpp"

class PN532_NFC
{
public:
	PN532_NFC(ILogger* pLogger = NulLogger::getInstance());
	~PN532_NFC();

	// void setTimeoutMs(uint8_t timeout_ms) { m_timeout_ms = timeout_ms; };

	std::string readCardUUID();

	std::string getVersion() const { return m_version; }

private:
	ILogger* m_pLogger;

	nfc_context* m_pContext;
	nfc_device* m_pDevice;

	nfc_target m_target;
	// uint8_t m_timeout_ms;

	const nfc_modulation m_nmMifare = {
		.nmt = NMT_ISO14443A,
		.nbr = NBR_106
	};
	std::string m_version;

	void initiateContext();
	void logVersion();
	void openDevice();

};




#endif