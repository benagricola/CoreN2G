/*
 * SharedSpiDevice.h
 *
 *  Created on: 16 Jun 2020
 *      Author: David
 */

#ifndef SRC_HARDWARE_SPI_SHAREDSPIDEVICE_H_
#define SRC_HARDWARE_SPI_SHAREDSPIDEVICE_H_

#include "SpiDevice.h"
#include <RTOSIface/RTOSIface.h>

class SharedSpiDevice : public SpiDevice
{
public:
	explicit SharedSpiDevice(const SpiParameters& params) noexcept;

	// Get ownership of this SPI, return true if successful
	bool Take(uint32_t timeout) noexcept { return mutex.Take(timeout); }

	// Release ownership of this SPI
	void Release() noexcept { mutex.Release(); }

private:
	Mutex mutex;
#if STM32 || RPXXXX
	char mutexName[8];							// Mutex::Create keeps the name pointer, so the storage must live as long as the mutex
#endif
};

#endif /* SRC_HARDWARE_SPI_SHAREDSPIDEVICE_H_ */
