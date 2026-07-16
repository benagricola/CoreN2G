/*
 * SharedSpiDevice.cpp
 *
 *  Created on: 16 Jun 2020
 *      Author: David
 */

#include "SharedSpiDevice.h"

#include <General/SafeVsnprintf.h>

// SharedSpiDevice members

SharedSpiDevice::SharedSpiDevice(const SpiParameters& params) noexcept : SpiDevice(params)
{
#if STM32 || RPXXXX
	// Mutex::Create keeps the name pointer rather than copying the string, so format the name into
	// a buffer that lives as long as this instance
	SafeSnprintf(mutexName, sizeof(mutexName), "SPI%u", (unsigned int)params.instanceNumber);
	mutex.Create(mutexName);
#else
	mutex.Create("SPI");
#endif
}

// End
