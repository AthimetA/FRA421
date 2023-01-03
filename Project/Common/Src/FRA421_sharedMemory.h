/*
 * FRA421_sharedMemory.h
 *
 *  Created on: Jan 3, 2023
 *      Author: AthimetA
 */

#ifndef INC_FRA421_SHAREDMEMORY_H_
#define INC_FRA421_SHAREDMEMORY_H_

#include "FRA421_Yugioh.h"

#define SHARED_MEMORY_ADDRESS 0x38000000

RFIDHandle *const RFIDMain = (RFIDHandle*)(SHARED_MEMORY_ADDRESS);

#endif /* INC_FRA421_SHAREDMEMORY_H_ */
