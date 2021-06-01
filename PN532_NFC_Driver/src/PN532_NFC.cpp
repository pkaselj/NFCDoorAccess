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

#include "PN532_NFC.hpp"

#include "Kernel.hpp"

#include <sstream>
#include <iomanip>

std::string parseHexUUID(uint8_t* pData, size_t dataLen);

PN532_NFC::PN532_NFC(ILogger* pLogger)
    :   m_pLogger(pLogger)
{

    if (m_pLogger == nullptr)
    {
        m_pLogger = NulLogger::getInstance();
    }


    initiateContext();
    logVersion();
    openDevice();

    // setTimeoutMs(0);
}

PN532_NFC::~PN532_NFC()
{
    nfc_close(m_pDevice);
    nfc_exit(m_pContext);
}


void PN532_NFC::initiateContext()
{
    nfc_init(&m_pContext);
    if (m_pContext == nullptr)
    {
        *m_pLogger << "PN532 - Could not initialize NFC context!";
        Kernel::Fatal_Error("PN532 - Could not initialize NFC context!");
    }
}

void PN532_NFC::logVersion()
{
    m_version = std::string(nfc_version());
    *m_pLogger << "Current libnfc version: " + m_version;
}

void PN532_NFC::openDevice()
{
    m_pDevice = nfc_open(m_pContext, nullptr);
    if (m_pDevice == nullptr)
    {
        *m_pLogger << "PN532 - Could not open NFC device!";
        Kernel::Fatal_Error("PN532 - Could not open NFC device!");
    }
}
    

std::string PN532_NFC::readCardUUID()
{
    uint8_t* pData = nullptr;
    size_t dataLen = 0;


    // TODO Check for leaks

    if (nfc_initiator_list_passive_targets(m_pDevice, m_nmMifare, &m_target, 1) > 0)
    {
        pData = m_target.nti.nai.abtUid;
        dataLen = m_target.nti.nai.szUidLen;
    }

    return parseHexUUID(pData, dataLen);
}

std::string parseHexUUID(uint8_t* pData, size_t dataLen)
{
    if (pData == nullptr)
    {
        return "";
    }

    std::stringstream stringBuilder;
    stringBuilder.str("");

    for (size_t i = 0; i < dataLen - 1; i++)
    {
        stringBuilder << std::uppercase << std::setw(2) << std::setfill('0') << std::hex << (int)pData[i] << "-";
    }
    stringBuilder << std::uppercase << std::setw(2) << std::setfill('0') << std::hex << (int)pData[dataLen - 1];

    return stringBuilder.str();
}
