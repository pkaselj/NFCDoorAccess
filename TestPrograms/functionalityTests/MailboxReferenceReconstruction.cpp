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

#include <unistd.h>
#include <string.h>

#include "DataMailbox.hpp"

int main()
{
	uint8_t arrRawData[] =
	{ 
		0xe8, 0xfd, 0xe5, 0x76,
		0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00,
		0xc0, 0xde, 0xb3, 0x76,
		0x7c, 0x80, 0xac, 0x76,
		0x62, 0x02, 0x00, 0x00,
		0xcc, 0xae, 0xb9, 0x7e,
		0x0e, 0x00, 0x00, 0x00,
		0x4e, 0x4f, 0x5f, 0x44,
		0x45, 0x53, 0x54, 0x49,
		0x4e, 0x41, 0x54, 0x49,
		0x4f, 0x4e, 0x00, 0x33
	};


	//memcpy(&ref, arrRawData, sizeof(arrRawData));
	
	sleep(100);
	
}