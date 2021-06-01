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

#include<iostream>
#include<iomanip>
#include<string>
#include<sstream>
#include<cstring>
#include<fstream>

#include<nfc/nfc.h>
#include<freefare.h>


using namespace std;

const unsigned int UID_byteSize = 4;

#define FATAL_ERROR(COND, MSG) {	if (COND)\
									{\
									cout << MSG << endl;\
									exit(-1);\
									}\
								}

#define TRACE(MSG) cout << MSG << endl;

string formatRawUUID(char* szUID)
{
	FATAL_ERROR(strlen(szUID) != UID_byteSize * 2, "Invalid Card UID size!");

	stringstream UIDBuilder;

	UIDBuilder << (char)toupper(szUID[0]) << (char)toupper(szUID[1]) << "-"
		<< (char)toupper(szUID[2]) << (char)toupper(szUID[3]) << "-"
		<< (char)toupper(szUID[4]) << (char)toupper(szUID[5]) << "-"
		<< (char)toupper(szUID[6]) << (char)toupper(szUID[7]);


	return UIDBuilder.str();
}

MifareClassicKey default_keys[] = {
	{ 0xff, 0xff, 0xff, 0xff, 0xff, 0xff },
	{ 0xd3, 0xf7, 0xd3, 0xf7, 0xd3, 0xf7 },
	{ 0xa0, 0xa1, 0xa2, 0xa3, 0xa4, 0xa5 },
	{ 0xb0, 0xb1, 0xb2, 0xb3, 0xb4, 0xb5 },
	{ 0x4d, 0x3a, 0x99, 0xc3, 0x51, 0xdd },
	{ 0x1a, 0x98, 0x2c, 0x7e, 0x45, 0x9a },
	{ 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff },
	{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }
};

void hexdump(uint8_t* pData, size_t dataLen)
{
	// const std::string DUMP_FILE_NAME = "tag.dump";
	// ofstream binaryDump(DUMP_FILE_NAME, ios::binary);
	stringstream dumpBuilder;

	const unsigned int bytesPerLine = 16;
	const unsigned int byteCharRepresentationWidth = 4;
	const unsigned int spacesPerLine = bytesPerLine - 1;
	const unsigned int tabWidth = 8;
	const unsigned int lineCharWidth = bytesPerLine * byteCharRepresentationWidth + spacesPerLine + tabWidth;
	const std::string lineSeparator(lineCharWidth, '=');

	for (size_t i = 0; i < dataLen; i++)
	{
		// binaryDump << pData[i];
		dumpBuilder << "0x" << uppercase << setw(2) << setfill('0') << hex << (int)pData[i] << " ";

		if (i == 0 || i == dataLen - 1)
		{
			;
		}
		else if ((i+1) % 48 == 0)
		{
			dumpBuilder << "\n" << lineSeparator << "\n";
		}
		else if ((i+1) % 16 == 0)
		{
			dumpBuilder << "\n";
		}
		else if((i + 1) % 8 == 0)
		{
			dumpBuilder << "\t";
		}

	}

	cout << endl << dumpBuilder.str() << endl;

}

struct TLV
{
	TLV() {}
	~TLV() { delete[] m_pData; }

	uint8_t m_type = 0;
	uint16_t m_dataLen = 0;
	uint8_t* m_pData = nullptr;
};

void read();
void write();
void writeNDEF();
void dumpMemory();

const MifareClassicKey default_keyb = {
	0xd3, 0xf7, 0xd3, 0xf7, 0xd3, 0xf7
};

const MifareClassicBlock message = {
	0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F
};

const MifareClassicSectorNumber gSector = 4;
const MifareClassicBlockNumber gBlock = 0;

int
search_sector_key(FreefareTag tag, MifareClassicSectorNumber sector, MifareClassicKey* key, MifareClassicKeyType* key_type)
{
	MifareClassicBlockNumber block = mifare_classic_sector_last_block(sector);

	/*
	 * FIXME: We should not assume that if we have full access to trailer block
	 *        we also have a full access to data blocks.
	 */
	mifare_classic_disconnect(tag);
	for (size_t i = 0; i < (sizeof(default_keys) / sizeof(MifareClassicKey)); i++) {
		if ((0 == mifare_classic_connect(tag)) && (0 == mifare_classic_authenticate(tag, block, default_keys[i], MFC_KEY_A))) {
			if ((1 == mifare_classic_get_trailer_block_permission(tag, block, MCAB_WRITE_KEYA, MFC_KEY_A)) &&
				(1 == mifare_classic_get_trailer_block_permission(tag, block, MCAB_WRITE_ACCESS_BITS, MFC_KEY_A)) &&
				(1 == mifare_classic_get_trailer_block_permission(tag, block, MCAB_WRITE_KEYB, MFC_KEY_A))) {
				memcpy(key, &default_keys[i], sizeof(MifareClassicKey));
				*key_type = MFC_KEY_A;
				return 1;
			}
		}
		mifare_classic_disconnect(tag);

		if ((0 == mifare_classic_connect(tag)) && (0 == mifare_classic_authenticate(tag, block, default_keys[i], MFC_KEY_B))) {
			if ((1 == mifare_classic_get_trailer_block_permission(tag, block, MCAB_WRITE_KEYA, MFC_KEY_B)) &&
				(1 == mifare_classic_get_trailer_block_permission(tag, block, MCAB_WRITE_ACCESS_BITS, MFC_KEY_B)) &&
				(1 == mifare_classic_get_trailer_block_permission(tag, block, MCAB_WRITE_KEYB, MFC_KEY_B))) {
				memcpy(key, &default_keys[i], sizeof(MifareClassicKey));
				*key_type = MFC_KEY_B;
				return 1;
			}
		}
		mifare_classic_disconnect(tag);
	}

	FATAL_ERROR(true, "No known authentication key for sector!");
	return 0;
}

int main(int argc, char** argv)
{
	cout << "Start!" << endl;

	if (argc < 2)
	{
		TRACE("Read - implicit")
		read();
	}
	else if (0 == strcmp(argv[1], "-write"))
	{
		TRACE("Write")
		write();
	}
	else if (0 == strcmp(argv[1], "-read"))
	{
		TRACE("Read")
		read();
	}
	else if (0 == strcmp(argv[1], "-writeNDEF"))
	{
		TRACE("WriteNDEF")
		writeNDEF();
	}
	else if (0 == strcmp(argv[1], "-dump"))
	{
		TRACE("Dump");
		dumpMemory();
	}
	else
	{
		TRACE("INVALID PARAMETER! Usage: " + std::string(argv[0]) + " [-read|-write]")
	}

	return 0;
}




void writeNDEF()
{
	nfc_context* pContext = nullptr;
	nfc_init(&pContext);
	FATAL_ERROR(pContext == nullptr, "Context could not be initialized!")

		nfc_device* pDevice = nfc_open(pContext, nullptr);
	FATAL_ERROR(pDevice == nullptr, "Device could not be opened!")

		FreefareTag* arrTags = freefare_get_tags(pDevice);
	FATAL_ERROR(arrTags == nullptr, "Could not read freefare tags!")

		FreefareTag tag = arrTags[0];
	FATAL_ERROR(!tag, "No tag found!")

		switch (freefare_get_tag_type(tag))
		{
		case MIFARE_CLASSIC_1K:
			TRACE("Detected MIFARE_CLASSIC_1K CARD")
				break;

		case MIFARE_CLASSIC_4K:
			TRACE("Detected MIFARE_CLASSIC_4K CARD")
				break;

		default:
			FATAL_ERROR(true, "INVALID CARD TYPE");
		}


	char* szCardUID = freefare_get_tag_uid(tag);
	string sCardUID = formatRawUUID(szCardUID);
	delete[] szCardUID;

	TRACE("Tag UID: " + sCardUID);

	int connection_status = mifare_classic_connect(tag);
	FATAL_ERROR(connection_status != 0, "Could not connect to the tag!");
	TRACE("Connected to the tag!");

	Mad tagMad = mad_read(tag);
	FATAL_ERROR(!tagMad, "Error while reading tag MAD!");

	uint8_t rawData[] =
	{
		0x54, 0x65, 0x73, 0x74, 0x54, 0x65, 0x78, 0x74, 0x32, 0x21
	};

	TLV dataToBeWritten;
	dataToBeWritten.m_pData = rawData;
	dataToBeWritten.m_dataLen = sizeof(rawData) / sizeof(uint8_t);
	dataToBeWritten.m_type = 0x03;

	size_t encodedSize = 0;
	uint8_t* pEncodedData = tlv_encode(
		dataToBeWritten.m_type,
		dataToBeWritten.m_pData,
		dataToBeWritten.m_dataLen,
		&encodedSize);

	TRACE("ENCODED DATA SIZE: " + std::to_string((int)encodedSize));

	MifareClassicSectorNumber* pAllocatedSectors = mifare_application_alloc(tagMad, mad_nfcforum_aid, encodedSize);
	TRACE("ALLOCARED SECTORS ADDRESS: " + std::to_string((int)pAllocatedSectors));
	FATAL_ERROR(!pAllocatedSectors, "Error while allocating sectors for TLV record");

	int success = mad_write(tag, tagMad, default_keyb, 0);
	FATAL_ERROR(success < 0, "Error while trying to write MAD to the tag!");

	MifareClassicSectorNumber* pSectorIter = pAllocatedSectors;
	while (pSectorIter)
	{
		MifareClassicBlockNumber trailerBlockNumber = mifare_classic_sector_last_block(*pSectorIter);
		MifareClassicBlock defaultTrailerBlock;
		mifare_classic_trailer_block(&defaultTrailerBlock, default_keys[0], 0, 0, 0, 6, 40, default_keyb);

		MifareClassicKey key;
		MifareClassicKeyType keyType;
		int searchKeySuccess = search_sector_key(tag, *pSectorIter, &key, &keyType);
		FATAL_ERROR(!searchKeySuccess, "Could not find sector key!");

		int authSuccess = mifare_classic_authenticate(tag, trailerBlockNumber, key, keyType);
		FATAL_ERROR(authSuccess != 0, "Error while trying to authenticate sector!");

		int writeSuccess = mifare_classic_write(tag, trailerBlockNumber, defaultTrailerBlock);
		FATAL_ERROR(writeSuccess != 0, "Error while trying to write default trailer block data to the tag!");

		++pSectorIter;
	}

	size_t writtenData = mifare_application_write(tag, tagMad, mad_nfcforum_aid, pEncodedData, encodedSize, default_keys[0], MFC_KEY_A);
	FATAL_ERROR(writtenData < encodedSize, "Error while writing NDEF application data to the tag!");

	free(pEncodedData);
	free(pAllocatedSectors);
	free(tagMad);

	mifare_classic_disconnect(tag);
	freefare_free_tag(tag);
	nfc_close(pDevice);
	nfc_exit(pContext);
}



void write()
{
		nfc_context* pContext = nullptr;
		nfc_init(&pContext);
		FATAL_ERROR(pContext == nullptr, "Context could not be initialized!")

			nfc_device* pDevice = nfc_open(pContext, nullptr);
		FATAL_ERROR(pDevice == nullptr, "Device could not be opened!")

			FreefareTag* arrTags = freefare_get_tags(pDevice);
		FATAL_ERROR(arrTags == nullptr, "Could not read freefare tags!")

			FreefareTag tag = arrTags[0];
		FATAL_ERROR(!tag, "No tag found!")

			switch (freefare_get_tag_type(tag))
			{
			case MIFARE_CLASSIC_1K:
				TRACE("Detected MIFARE_CLASSIC_1K CARD")
					break;

			case MIFARE_CLASSIC_4K:
				TRACE("Detected MIFARE_CLASSIC_4K CARD")
					break;

			default:
				FATAL_ERROR(true, "INVALID CARD TYPE");
			}


		char* szCardUID = freefare_get_tag_uid(tag);
		string sCardUID = formatRawUUID(szCardUID);
		delete[] szCardUID;

		TRACE("Tag UID: " + sCardUID);

		int connection_status = mifare_classic_connect(tag);
		FATAL_ERROR(connection_status != 0, "Could not connect to the tag!");
		TRACE("Connected to the tag!")

		MifareClassicBlockNumber block = mifare_classic_sector_first_block(gSector);
		TRACE("Sector: " + std::to_string((int)gSector) + " :: Block: " + std::to_string((int)block))

		MifareClassicKey validKey = { 0 };
		MifareClassicKeyType validKeyType = MFC_KEY_A;
		int keyFound = search_sector_key(tag, gSector, &validKey, &validKeyType);
		FATAL_ERROR(keyFound == 0, "No valid key known!")

		int writeStatus = mifare_classic_write(tag, block, message);
		FATAL_ERROR(writeStatus != 0, "Error while writing to the card!")
		TRACE("Successfully written data to the card!")

		mifare_classic_disconnect(tag);
		freefare_free_tag(tag);
		nfc_close(pDevice);
		nfc_exit(pContext);
}








void read()
{
		nfc_context* pContext = nullptr;
		nfc_init(&pContext);
		FATAL_ERROR(pContext == nullptr, "Context could not be initialized!")

			nfc_device* pDevice = nfc_open(pContext, nullptr);
		FATAL_ERROR(pDevice == nullptr, "Device could not be opened!")

			FreefareTag* arrTags = freefare_get_tags(pDevice);
		FATAL_ERROR(arrTags == nullptr, "Could not read freefare tags!")

			FreefareTag tag = arrTags[0];
		FATAL_ERROR(!tag, "No tag found!")

			switch (freefare_get_tag_type(tag))
			{
			case MIFARE_CLASSIC_1K:
				TRACE("Detected MIFARE_CLASSIC_1K CARD")
					break;

			case MIFARE_CLASSIC_4K:
				TRACE("Detected MIFARE_CLASSIC_4K CARD")
					break;

			default:
				FATAL_ERROR(true, "INVALID CARD TYPE");
			}


		char* szCardUID = freefare_get_tag_uid(tag);
		string sCardUID = formatRawUUID(szCardUID);
		delete[] szCardUID;

		TRACE("Tag UID: " + sCardUID);

		int connection_status = mifare_classic_connect(tag);
		FATAL_ERROR(connection_status != 0, "Could not connect to the tag!");
		TRACE("Connected to the tag!")


			Mad tagMad = mad_read(tag);
		FATAL_ERROR(!tagMad, "Could not read tag MAD!");
		TRACE("MAD read successfully!")

			uint8_t arrBuffer[4096]{ 0 };
		uint8_t* pBuffer = arrBuffer;
		ssize_t bufferSize = -1;

		bufferSize = mifare_application_read(tag, tagMad, mad_nfcforum_aid, arrBuffer, sizeof(arrBuffer), mifare_classic_nfcforum_public_key_a, MFC_KEY_A);
		FATAL_ERROR(bufferSize == -1, "Mifare - could not read tag!")

			hexdump(arrBuffer, bufferSize);

		bool bTerminatorFlag = false;
		while (!bTerminatorFlag)
		{
			TRACE("Record")
				TLV tlvRecord;
			tlvRecord.m_pData = tlv_decode(pBuffer, &tlvRecord.m_type, &tlvRecord.m_dataLen);
			FATAL_ERROR(tlvRecord.m_pData == nullptr, "Error while decoding TLV data!")

				TRACE("TLV data decoded")

				switch (tlvRecord.m_type)
				{
				case 0x00:
					TRACE("Null TLV!")
						break;

				case 0x03:
					TRACE("Valid TLV");
					hexdump(tlvRecord.m_pData, tlvRecord.m_dataLen);
					break;

				case 0xFD:
					TRACE("Proprietary TLV")
						break;

				case 0xFE:
					TRACE("Terminator TLV")
						bTerminatorFlag = true;
					break;

				default:
					TRACE("UNKNOWN TLV")
						break;
				}

			if (!bTerminatorFlag)
			{
				pBuffer += tlv_record_length(pBuffer, nullptr, nullptr);
			}
		}

		freefare_free_tag(tag);
		free(tagMad);
		nfc_close(pDevice);
		nfc_exit(pContext);
}

void dumpMemory()
{
	nfc_context* pContext = nullptr;
	nfc_init(&pContext);
	FATAL_ERROR(pContext == nullptr, "Context could not be initialized!");

	nfc_device* pDevice = nfc_open(pContext, 0);
	FATAL_ERROR(pDevice == nullptr, "Device could not be opened!");


	FreefareTag* arrTags = freefare_get_tags(pDevice);
	FATAL_ERROR(arrTags == nullptr, "Could not read freefare tags!");

	FreefareTag tag = arrTags[0];
	FATAL_ERROR(!tag, "No tag found!");

	switch (freefare_get_tag_type(tag))
	{
	case MIFARE_CLASSIC_1K:
		TRACE("Detected MIFARE_CLASSIC_1K CARD")
			break;

	default:
		FATAL_ERROR(true, "INVALID CARD TYPE");
	}

	char* szCardUID = freefare_get_tag_uid(tag);
	string sCardUID = formatRawUUID(szCardUID);
	delete[] szCardUID;

	TRACE("Found card with UID: " + sCardUID);

	const unsigned int SECTOR_COUNT = 16;
	const unsigned int BLOCKS_PER_SECTOR = 4;

	const unsigned int BLOCKS_COUNT = SECTOR_COUNT * BLOCKS_PER_SECTOR;

	int connection_status = mifare_classic_connect(tag);
	FATAL_ERROR(connection_status != 0, "Could not connect to the tag!");
	TRACE("Connected to the tag!");

	unsigned int i = 0;
	while (i < BLOCKS_COUNT)
	{
		unsigned int currentSector = i / 4;

		MifareClassicKey key = "";
		MifareClassicKeyType keyType = MFC_KEY_A;
		int success = search_sector_key(tag, i % 4, &key, &keyType);
		if (success == 0)
		{
			TRACE("Could not find key for sector: " + std::to_string(i % 4));

			unsigned int nextSector = currentSector + 1;
			unsigned int nextSectorBlockStart = mifare_classic_sector_first_block(nextSector);

			i = nextSectorBlockStart;

			continue;
		}

		TRACE("Sector: " + std::to_string(currentSector) + " Block: " + std::to_string(i - currentSector * 4));

		success = mifare_classic_authenticate(tag, i, key, keyType);
		if (success != 0)
		{
			TRACE("Auth error!");
			++i;
			continue;
		}

		MifareClassicBlock pData = "";
		success = mifare_classic_read(tag, i, &pData);
		if (success != 0)
		{
			TRACE("Read error!");
			++i;
			continue;
		}

		hexdump(pData, 16);
		++i;
	}

	mifare_classic_disconnect(tag);
	nfc_close(pDevice);
	nfc_exit(pContext);

}