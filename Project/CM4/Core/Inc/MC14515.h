/*
 * MC14515.h
 *
 *  Created on: Dec 27, 2022
 *      Author: AthimetA
 */

#ifndef INC_MC14515_H_
#define INC_MC14515_H_

#include "main.h"
#include "stm32h7xx_hal.h"

typedef struct
{
	GPIO_TypeDef *STPort,*INHPort,*D1Port,*D2Port,*D3Port,*D4Port;
	uint16_t STPin;
	uint16_t INHPin;
	uint16_t D1Pin;
	uint16_t D2Pin;
	uint16_t D3Pin;
	uint16_t D4Pin;
}MC14515Handle;

void MC14515_Latch(MC14515Handle *,uint16_t );
void MC14515_Set_Output_All_High(MC14515Handle *);
void MC14515_Test_Function(MC14515Handle *);

#endif /* INC_MC14515_H_ */
