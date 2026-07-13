/*
 * SharedSpiDevice.cpp
 *
 *  Created on: 16 Jun 2020
 *      Author: David
 */

#include "SharedSpiDevice.h"

// SharedSpiDevice members

SharedSpiDevice::SharedSpiDevice(const SpiParameters& params) noexcept : SpiDevice(params)
{
#if STM32 || RPXXXX
	// Mutex::Create keeps the name POINTER, so each instance needs its own storage: with the old
	// single static buffer here, every SPI bus mutex reported the name of the last one constructed,
	// which makes mutex-holder diagnostics useless for telling the buses apart
	char *const name = new char[5];
	name[0] = 'S'; name[1] = 'P'; name[2] = 'I';
	name[3] = '0' + params.instanceNumber;
	name[4] = 0;
	mutex.Create(name);
#else
	mutex.Create("SPI");
#endif
}

// End
