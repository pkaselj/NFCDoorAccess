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