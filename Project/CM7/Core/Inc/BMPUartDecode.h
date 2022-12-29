/*
 * BMPUartDecode.h
 *
 *  Created on: Aug 21, 2021
 *      Author: AlphaP
 */

#ifndef INC_BMPUARTDECODE_H_
#define INC_BMPUARTDECODE_H_

#include "stm32h7xx_hal.h"

typedef enum {
	BMP_idle,
	BMP_Header_2,
	BMP_Size_4,
	BMP_Reserved0_4,
	BMP_Imagestartpoint_4,
	BMP_SizeHeader_4,
	BMP_PicWidth_4,
	BMP_PicHeight_4,
	BMP_ColorPlanes_2,
	BMP_BitPerPixel_2,
	BMP_Notused1_n,
	BMP_Pixeldata_n,
	BMP_Notused2_n

} stateBMP;

typedef union U8ToU32Convert {
	uint8_t U8[4];
	uint32_t U32;
} Convert8_32;

#define IMG_W 128
#define IMG_H 128

void BMPDecoder(uint8_t dataIn, uint8_t *array);

#endif /* INC_BMPUARTDECODE_H_ */
