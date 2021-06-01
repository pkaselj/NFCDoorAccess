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

#ifndef GPIO_H
#define GPIO_H

#include"errorCodes.h"

#include <stdint.h>

// FUNCTION PROTOTYPES

/**
 * @brief Check if gpioBase pointer is null
 * 
 * @param gpioBase base GPIO address pointer
 */
static inline void assertAddress(const uint32_t* gpioBase);

/**
 * @brief Check if pin is valid
 * 
 * @param pin pin number *(use PIN# define for pin nr. #)* [PIN1..PIN27]
 */
static inline void assertPin(const uint8_t pin);

/**
 * @brief Get pointer to GPIO memory base address
 * 
 * @param p_gpioBase pointer to pointer in which address will be stored
 * @return int 0 on success, errorCodes.h error code on failure
 */
int mapGpio(uint32_t** p_gpioBase);

/**
 * @brief Get the offset to GPIO pin register for specified pin.
 * 
 * Since there are only 10 pins per register (3 bits per pin). \n
 * There are 3 registers which control GPIO pins. \n
 * 
 * @param pin pin number *(use PIN# define for pin nr. #)* [PIN1..PIN27]
 * @return uint32_t offset from *base GPIO pointer* to register which controls pin states
 */
uint32_t getFSR_Offset(const uint8_t pin);

/**
 * @brief Set the pin mode.
 * 
 *  There are 8 valid pin modes: \n
 * 	OUTPUT, INPUT, ALT0, ALT1, ALT2, ALT3, ALT4, ALT5
 * 
 * @param gpioBase pointer to base GPIO memory - mapGpio(&gpioBase)
 * @param pin pin number *(use PIN# define for pin nr. #)* [PIN1..PIN27]
 * @param mode pin mode *(use mode defines)* {OUTPUT, INPUT, ALT0..ALT5}
 * @return int 0 on success, errorCodes.h error code on failure
 */
int setPinMode(uint32_t* gpioBase, const uint8_t pin, const uint32_t mode);

/**
 * @brief Set specified pin to high state
 * 
 * @param gpioBase pointer to base GPIO memory - mapGpio(&gpioBase)
 * @param pin pin number *(use PIN# define for pin nr. #)* [PIN1..PIN27]
 * @return int int 0 on success, errorCodes.h error code on failure
 */
int setPinHigh(uint32_t* gpioBase, const uint8_t pin);

/**
 * @brief Set specified pin to low state
 * 
 * @param gpioBase pointer to base GPIO memory - mapGpio(&gpioBase)
 * @param pin pin number *(use PIN# define for pin nr. #)* [PIN1..PIN27]
 * @return int int 0 on success, errorCodes.h error code on failure
 */
int setPinLow(uint32_t* gpioBase, const uint8_t pin);

/**
 * @brief Set specified pin to specified state.
 * 
 * 	Wrapper for setPinHigh() and setPinLow() \n
 * 
 * 	*Pin mode must be set to OUTPUT* \n
 * 
 * @param gpioBase pointer to base GPIO memory - mapGpio(&gpioBase)
 * @param pin pin number *(use PIN# define for pin nr. #)* [PIN1..PIN27]
 * @param value pin state {HIGH, LOW}
 * @return int int 0 on success, errorCodes.h error code on failure
 */
int pinWrite(uint32_t* gpioBase, const uint8_t pin, const uint8_t value);

/**
 * @brief Read state from specified pin
 * 
 * *Pin mode must be set to INPUT*
 * 
 * @param gpioBase pointer to base GPIO memory - mapGpio(&gpioBase)
 * @param pin pin number *(use PIN# define for pin nr. #)* [PIN1..PIN27]
 * @return uint32_t if pin is in low state - returns 0, else if pin is in high state returns positive integer
 */
uint32_t pinRead(uint32_t* gpioBase, const uint8_t pin);

/**
 * @brief reset GPPUDCLK registers (GPPUDCLK0 and GPPUDCLK1)
 * 
 * 	From documentation: \n
 *						\n
 *	The GPIO Pull-up/down Clock Registers control the actuation of internal pull-downs on \n
 *		the respective GPIO pins. These registers must be used in conjunction with the GPPUD \n
 *		register to effect GPIO Pull-up/down changes. The following sequence of events is \n
 *		required: \n
 * 				  \n
 *	1. Write to GPPUD to set the required control signal (i.e. Pull-up or Pull-Down or neither \n
 *		to remove the current Pull-up/down) \n
 *	2. Wait 150 cycles – this provides the required set-up time for the control signal \n
 *	3. Write to GPPUDCLK0/1 to clock the control signal into the GPIO pads you wish to \n
 *		modify – NOTE only the pads which receive a clock will be modified, all others will \n
 *		retain their previous state. \n
 *	4. Wait 150 cycles – this provides the required hold time for the control signal \n
 *	5. Write to GPPUD to remove the control signal \n
 *	6. Write to GPPUDCLK0/1 to remove the clock \n
 * 
 * @param gpioBase pointer to base GPIO memory - mapGpio(&gpioBase)
 */
static inline void resetGPPUDCLK(uint32_t* gpioBase);

/**
 * @brief Set pullup/pulldown on specified pin
 * 
 * @param gpioBase pointer to base GPIO memory - mapGpio(&gpioBase)
 * @param pin pin number *(use PIN# define for pin nr. #)* [PIN1..PIN27]
 * @param PUDstate pullup/pulldown state {OFF, PDOWN, PUP}
 * @return int 0 on success, errorCodes.h error code on failure
 */
int setPUD(uint32_t* gpioBase, const uint8_t pin, const uint8_t PUDstate);

int mapPeripheral(uint32_t* p_gpioBase);

#define PIN_COUNT 32

// ERRORS


// GPIO FUNCTIONS
#define OUTPUT 0b001
#define INPUT 0b000
#define ALT0 0b100
#define ALT1 0b101
#define ALT2 0b110
#define ALT3 0b111
#define ALT4 0b011
#define ALT5 0b010

// PULL UP AND PULL DOWN RESISTOR CONFIGURATIONS
#define OFF 0b00
#define PDOWN 0b01
#define PUP 0b10

// GPIO PIN STATES
#define HIGH 1
#define LOW 0


// REGISTERS
#define GPCLR0 10 // CLEAR GPIO REGISTER OFFSET
#define GPSET0 7 // GPIO FUNCTION SELECT REGISTER OFFSET
#define GPFSEL0 0 // GPIO MODE REGISTER OFFSET
#define GPLEV0 13 // GPIO STATE REGISTER OFFSET
#define GPPUD 37 // GPIO PULL UP/DOWN ENABLE REGISTER OFFSET
#define GPPUDCLK0 38 // GPIO PULL UP/DOWN ENABLE CLOCK REGISTER OFFSET


#define PIN2 2 
#define PIN3 3 
#define PIN4 4 
#define PIN5 5 
#define PIN6 6 
#define PIN7 7 
#define PIN8 8 
#define PIN9 9 
#define PIN10 10 
#define PIN11 11 
#define PIN12 12 
#define PIN13 13 
#define PIN14 14 
#define PIN15 15 
#define PIN16 16 
#define PIN17 17 
#define PIN18 18 
#define PIN19 19 
#define PIN20 20 
#define PIN21 21 
#define PIN22 22 
#define PIN23 23 
#define PIN24 24 
#define PIN25 25 
#define PIN26 26 
#define PIN27 27 



/*
 * @brief Create bitmask for GPIO mode register (GPFSEL) for specified pin
 * 
 *	Creates clear mask for X-th triplet of bits \n
 *	e.g. \n
 *	MIDDLE_MASK(0) = 1111 1111 1111 1111 1111 1111 1111 1000 \n
 *	MIDDLE_MASK(1) = 1111 1111 1111 1111 1111 1111 1100 0111 \n
 *	MIDDLE_MASK(2) = 1111 1111 1111 1111 1111 1110 0011 1111 \n
 *
 */
#define MIDDLE_MASK(X) (~(0b111 << (X) % 10 * 3))


/*
 * @brief Bit with weight X is set to 1, others to 0
 * 
 * 	e.g. \n
 * 	BITMASK(0) = 0000 0000 0000 0000 0000 0000 0000 0001 \n
 *  BITMASK(1) = 0000 0000 0000 0000 0000 0000 0000 0010 \n
 *  BITMASK(31)= 1000 0000 0000 0000 0000 0000 0000 0000 \n
 */
#define BITMASK(X) (1 << (X))


#endif




















