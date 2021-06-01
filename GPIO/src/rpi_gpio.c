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

#include "rpi_gpio.h"

#include<sys/types.h>
#include<sys/stat.h>
#include<sys/mman.h>
#include<stddef.h>
#include<fcntl.h>
#include<stdio.h>

static inline void assertAddress(const uint32_t* gpioBase)
{
	if (gpioBase == NULL) { printf("Start Address can't be NULL\n"); exit(NULLPTRERROR); }
	return;
}

static inline void assertPin(const uint8_t pin)
{
	if (pin >= PIN_COUNT) { printf("Pin out of bounds!\n"); exit(WRONG_PIN); }
	return;
}

int mapGpio(uint32_t** p_gpioBase)
{
	int _file = open("/dev/gpiomem", O_RDWR);
	if (_file < 0) { printf("Error in opening gpiomem/n"); return MAP_ERROR; }

	*p_gpioBase = (uint32_t*)mmap(0, 4096, PROT_READ | PROT_WRITE, MAP_SHARED, _file, 0);
	if (*p_gpioBase == NULL) { printf("Error in mapping /dev/gpiomem\n"); return NULLPTRERROR; }

	printf("Successfully mapped GPIO pins\n");
	return 0;
}

/*
*	Get Function Select Register (FSR) offset based on selected pin
*/
uint32_t getFSR_Offset(const uint8_t pin)
{
	assertPin(pin);

	return GPFSEL0 + pin / 10; // 10 pins per register starting at GPSEL0

}


int setPinMode(uint32_t* gpioBase, const uint8_t pin, const uint32_t mode)
{
	assertPin(pin);
	assertAddress(gpioBase);

	uint32_t FSR_Offset = getFSR_Offset(pin);
	if (FSR_Offset < 0) { printf("Error in getting set register offset\n"); return OFFSET_ERROR; }

	uint32_t mask = MIDDLE_MASK(pin);
	uint32_t currentState = *(gpioBase + FSR_Offset);

	currentState &= mask;
	currentState |= mode << pin % 10 * 3;

	*(gpioBase + FSR_Offset) = currentState;

	//==================================
	// printf("FSR Offset: %d\nshift: %d\nmask: %X\n", FSR_Offset, pin % 10 * 3, mask);
	//==================================
	return 0;
}


int setPinHigh(uint32_t* gpioBase, const uint8_t pin)
{
	assertPin(pin);
	assertAddress(gpioBase);

	uint32_t mask = BITMASK(pin);
	*(gpioBase + GPSET0) |= mask;

	return 0;


}


int setPinLow(uint32_t* gpioBase, const uint8_t pin)
{
	assertPin(pin);
	assertAddress(gpioBase);

	uint32_t mask = BITMASK(pin);
	*(gpioBase + GPCLR0) |= mask;

	return 0;
}


int pinWrite(uint32_t* gpioBase, const uint8_t pin, const uint8_t value)
{
	assertPin(pin);
	assertAddress(gpioBase);

	if (value == LOW)
		return setPinLow(gpioBase, pin);
	return setPinHigh(gpioBase, pin);
}


/*
*	Returns non-zero unsigned integer if given pin is HIGH, else returns 0
*/
uint32_t pinRead(uint32_t* gpioBase, const uint8_t pin)
{
	assertPin(pin);
	assertAddress(gpioBase);

	uint32_t mask = BITMASK(pin);

	return *(gpioBase + GPLEV0) & mask;
}

static inline void resetGPPUDCLK(uint32_t* gpioBase)
{
	*(gpioBase + GPPUDCLK0) &= 0;
	*(gpioBase + GPPUDCLK0 + 1) &= 0;
	return;
}

/*
*	From documentation:
*
*	The GPIO Pull-up/down Clock Registers control the actuation of internal pull-downs on
*		the respective GPIO pins. These registers must be used in conjunction with the GPPUD
*		register to effect GPIO Pull-up/down changes. The following sequence of events is
*		required:
*
*	1. Write to GPPUD to set the required control signal (i.e. Pull-up or Pull-Down or neither
*		to remove the current Pull-up/down)
*	2. Wait 150 cycles – this provides the required set-up time for the control signal
*	3. Write to GPPUDCLK0/1 to clock the control signal into the GPIO pads you wish to
*		modify – NOTE only the pads which receive a clock will be modified, all others will
*		retain their previous state.
*	4. Wait 150 cycles – this provides the required hold time for the control signal
*	5. Write to GPPUD to remove the control signal
*	6. Write to GPPUDCLK0/1 to remove the clock
*/
int setPUD(uint32_t* gpioBase, const uint8_t pin, const uint8_t PUDstate)
{
	assertPin(pin);
	assertAddress(gpioBase);
	resetGPPUDCLK(gpioBase);

	uint32_t clearMask = (uint32_t)(-1) << 2;
	*(gpioBase + GPPUD) &= clearMask;
	*(gpioBase + GPPUD) |= PUDstate;

	/*
	*	150 cycles with min CPU CLK speed of 600 MHz
	*	time = 150 * 1/600MHz = 0.25 us >>> usleep(1)
	*/
	usleep(1);

	*(gpioBase + GPPUDCLK0) |= BITMASK(pin);
	//printf("%X\t%X\n", *(gpioBase + GPPUD), *(gpioBase + GPPUDCLK0));

	usleep(1);
	resetGPPUDCLK(gpioBase);
	*(gpioBase + GPPUD) &= 0;


	return 0;

}