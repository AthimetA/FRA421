/*
 * MC14515.c
 *
 *  Created on: Dec 27, 2022
 *      Author: AthimetA
 */


#include "MC14515.h"

void MC14515_Latch(MC14515Handle *mc14515,uint16_t selected_output)
{

	switch (selected_output)
	{
		case 0:
			// Set Data 0000
			HAL_GPIO_WritePin(mc14515->D1Port, mc14515->D1Pin, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(mc14515->D2Port, mc14515->D2Pin, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(mc14515->D3Port, mc14515->D3Pin, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(mc14515->D4Port, mc14515->D4Pin, GPIO_PIN_RESET);
			break;
		case 1:
			// Set Data 0001
			HAL_GPIO_WritePin(mc14515->D1Port, mc14515->D1Pin, GPIO_PIN_SET);
			HAL_GPIO_WritePin(mc14515->D2Port, mc14515->D2Pin, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(mc14515->D3Port, mc14515->D3Pin, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(mc14515->D4Port, mc14515->D4Pin, GPIO_PIN_RESET);
			break;
		case 2:
			// Set Data 0010
			HAL_GPIO_WritePin(mc14515->D1Port, mc14515->D1Pin, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(mc14515->D2Port, mc14515->D2Pin, GPIO_PIN_SET);
			HAL_GPIO_WritePin(mc14515->D3Port, mc14515->D3Pin, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(mc14515->D4Port, mc14515->D4Pin, GPIO_PIN_RESET);
			break;
		case 3:
			// Set Data 0011
			HAL_GPIO_WritePin(mc14515->D1Port, mc14515->D1Pin, GPIO_PIN_SET);
			HAL_GPIO_WritePin(mc14515->D2Port, mc14515->D2Pin, GPIO_PIN_SET);
			HAL_GPIO_WritePin(mc14515->D3Port, mc14515->D3Pin, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(mc14515->D4Port, mc14515->D4Pin, GPIO_PIN_RESET);
			break;
		case 4:
			// Set Data 0100
			HAL_GPIO_WritePin(mc14515->D1Port, mc14515->D1Pin, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(mc14515->D2Port, mc14515->D2Pin, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(mc14515->D3Port, mc14515->D3Pin, GPIO_PIN_SET);
			HAL_GPIO_WritePin(mc14515->D4Port, mc14515->D4Pin, GPIO_PIN_RESET);
			break;
		case 5:
			// Set Data 0101
			HAL_GPIO_WritePin(mc14515->D1Port, mc14515->D1Pin, GPIO_PIN_SET);
			HAL_GPIO_WritePin(mc14515->D2Port, mc14515->D2Pin, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(mc14515->D3Port, mc14515->D3Pin, GPIO_PIN_SET);
			HAL_GPIO_WritePin(mc14515->D4Port, mc14515->D4Pin, GPIO_PIN_RESET);
			break;
		case 6:
			// Set Data 0110
			HAL_GPIO_WritePin(mc14515->D1Port, mc14515->D1Pin, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(mc14515->D2Port, mc14515->D2Pin, GPIO_PIN_SET);
			HAL_GPIO_WritePin(mc14515->D3Port, mc14515->D3Pin, GPIO_PIN_SET);
			HAL_GPIO_WritePin(mc14515->D4Port, mc14515->D4Pin, GPIO_PIN_RESET);
			break;
		case 7:
			// Set Data 0111
			HAL_GPIO_WritePin(mc14515->D1Port, mc14515->D1Pin, GPIO_PIN_SET);
			HAL_GPIO_WritePin(mc14515->D2Port, mc14515->D2Pin, GPIO_PIN_SET);
			HAL_GPIO_WritePin(mc14515->D3Port, mc14515->D3Pin, GPIO_PIN_SET);
			HAL_GPIO_WritePin(mc14515->D4Port, mc14515->D4Pin, GPIO_PIN_RESET);
			break;
		case 8:
			// Set Data 1000
			HAL_GPIO_WritePin(mc14515->D1Port, mc14515->D1Pin, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(mc14515->D2Port, mc14515->D2Pin, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(mc14515->D3Port, mc14515->D3Pin, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(mc14515->D4Port, mc14515->D4Pin, GPIO_PIN_SET);
			break;
		case 9:
			// Set Data 1001
			HAL_GPIO_WritePin(mc14515->D1Port, mc14515->D1Pin, GPIO_PIN_SET);
			HAL_GPIO_WritePin(mc14515->D2Port, mc14515->D2Pin, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(mc14515->D3Port, mc14515->D3Pin, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(mc14515->D4Port, mc14515->D4Pin, GPIO_PIN_SET);
			break;
		case 10:
			// Set Data 1010
			HAL_GPIO_WritePin(mc14515->D1Port, mc14515->D1Pin, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(mc14515->D2Port, mc14515->D2Pin, GPIO_PIN_SET);
			HAL_GPIO_WritePin(mc14515->D3Port, mc14515->D3Pin, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(mc14515->D4Port, mc14515->D4Pin, GPIO_PIN_SET);
			break;
		case 11:
			// Set Data 1011
			HAL_GPIO_WritePin(mc14515->D1Port, mc14515->D1Pin, GPIO_PIN_SET);
			HAL_GPIO_WritePin(mc14515->D2Port, mc14515->D2Pin, GPIO_PIN_SET);
			HAL_GPIO_WritePin(mc14515->D3Port, mc14515->D3Pin, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(mc14515->D4Port, mc14515->D4Pin, GPIO_PIN_SET);
			break;
		case 12:
			// Set Data 1100
			HAL_GPIO_WritePin(mc14515->D1Port, mc14515->D1Pin, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(mc14515->D2Port, mc14515->D2Pin, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(mc14515->D3Port, mc14515->D3Pin, GPIO_PIN_SET);
			HAL_GPIO_WritePin(mc14515->D4Port, mc14515->D4Pin, GPIO_PIN_SET);
			break;
		case 13:
			// Set Data 1101
			HAL_GPIO_WritePin(mc14515->D1Port, mc14515->D1Pin, GPIO_PIN_SET);
			HAL_GPIO_WritePin(mc14515->D2Port, mc14515->D2Pin, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(mc14515->D3Port, mc14515->D3Pin, GPIO_PIN_SET);
			HAL_GPIO_WritePin(mc14515->D4Port, mc14515->D4Pin, GPIO_PIN_SET);
			break;
		case 14:
			// Set Data 1110
			HAL_GPIO_WritePin(mc14515->D1Port, mc14515->D1Pin, GPIO_PIN_SET);
			HAL_GPIO_WritePin(mc14515->D2Port, mc14515->D2Pin, GPIO_PIN_SET);
			HAL_GPIO_WritePin(mc14515->D3Port, mc14515->D3Pin, GPIO_PIN_SET);
			HAL_GPIO_WritePin(mc14515->D4Port, mc14515->D4Pin, GPIO_PIN_SET);
			break;
		case 15:
			// Set Data 1111
			HAL_GPIO_WritePin(mc14515->D1Port, mc14515->D1Pin, GPIO_PIN_SET);
			HAL_GPIO_WritePin(mc14515->D2Port, mc14515->D2Pin, GPIO_PIN_SET);
			HAL_GPIO_WritePin(mc14515->D3Port, mc14515->D3Pin, GPIO_PIN_SET);
			HAL_GPIO_WritePin(mc14515->D4Port, mc14515->D4Pin, GPIO_PIN_SET);
			break;
	}

	// Set ST to High ready to store data
	HAL_GPIO_WritePin(mc14515->STPort, mc14515->STPin, GPIO_PIN_SET);

	// Set INH to Low
	HAL_GPIO_WritePin(mc14515->INHPort, mc14515->INHPin, GPIO_PIN_RESET);

	// Latch Data
	HAL_GPIO_WritePin(mc14515->STPort, mc14515->STPin, GPIO_PIN_RESET);

}

void MC14515_Set_Output_All_High(MC14515Handle *mc14515)
{
	// Set INH to High
	HAL_GPIO_WritePin(mc14515->INHPort, mc14515->INHPin, GPIO_PIN_SET);

	// Set ST to High ready to store data
	HAL_GPIO_WritePin(mc14515->STPort, mc14515->STPin, GPIO_PIN_SET);

	// Latch Data
	HAL_GPIO_WritePin(mc14515->STPort, mc14515->STPin, GPIO_PIN_RESET);
}

void MC14515_Test_Function(MC14515Handle *mc14515)
{
	HAL_GPIO_TogglePin(mc14515->STPort, mc14515->STPin);
	HAL_GPIO_TogglePin(mc14515->INHPort, mc14515->INHPin);

	HAL_GPIO_TogglePin(mc14515->D1Port, mc14515->D1Pin);
	HAL_GPIO_TogglePin(mc14515->D2Port, mc14515->D2Pin);
	HAL_GPIO_TogglePin(mc14515->D3Port, mc14515->D3Pin);
	HAL_GPIO_TogglePin(mc14515->D4Port, mc14515->D4Pin);
}
