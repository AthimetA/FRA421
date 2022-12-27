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

// PIN
#define MC14515_ST_PORT GPIOC
#define MC14515_ST_PIN GPIO_PIN_6

#define MC14515_INH_PORT GPIOB
#define MC14515_INH_PIN GPIO_PIN_8

#define MC14515_D1_PORT GPIOB
#define MC14515_D1_PIN GPIO_PIN_15

#define MC14515_D2_PORT GPIOB
#define MC14515_D2_PIN GPIO_PIN_12

#define MC14515_D3_PORT GPIOB
#define MC14515_D3_PIN GPIO_PIN_9

#define MC14515_D4_PORT GPIOC
#define MC14515_D4_PIN GPIO_PIN_7

void MC14515_Latch(uint16_t);
void MC14515_Set_Output_All_High();
void MC14515_Test_Function();

#endif /* INC_MC14515_H_ */
