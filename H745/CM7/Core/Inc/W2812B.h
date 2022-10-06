/*
 * W2812B.h
 *
 *  Created on: Oct 6, 2022
 *      Author: AthimetA
 */

#ifndef INC_W2812B_H_
#define INC_W2812B_H_

#include "stm32h7xx_hal.h"

typedef struct
{
	uint8_t R,G,B;
}W2812BStructure;
void W2812B_Init(SPI_HandleTypeDef *hspi);

void W2812B_UpdateData(W2812BStructure *input);
void HToRGB(int h,W2812BStructure *input);

#endif /* INC_W2812B_H_ */
