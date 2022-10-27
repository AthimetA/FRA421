/*
 * SRAM4.h
 *
 *  Created on: Oct 20, 2022
 *      Author: AlphaP
 */

#ifndef SRC_SRAM4_H_
#define SRC_SRAM4_H_
#define SHARED_MEMORY_ADDRESS 0x38000000
//#define TASK2_NO_HSEM
//#define TASK2_WITH_HSEM
typedef struct
{
	uint32_t State1;
	uint32_t DATA[500];
}SharedMemory;

SharedMemory *const SRAM4 = (SharedMemory*)(SHARED_MEMORY_ADDRESS);


#endif /* SRC_SRAM4_H_ */
