/*
 * W2812BSPI.h
 *
 *  Created on: Aug 26, 2021
 *      Author: AlphaP
 */

#ifndef INC_W2812BSPI_H_
#define INC_W2812BSPI_H_

#include "stm32h7xx_hal.h"

typedef struct
{
	uint8_t R,G,B;
}W2812BStructure;
void W2812B_Init(SPI_HandleTypeDef *hspi);

void W2812B_UpdateData(W2812BStructure *input);
void HToRGB(int h,W2812BStructure *input);

#endif /* INC_W2812BSPI_H_ */
