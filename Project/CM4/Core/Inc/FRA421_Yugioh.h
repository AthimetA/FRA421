/*
 * FRA421_Yugioh.h
 *
 *  Created on: Dec 29, 2022
 *      Author: AthimetA
 */

#ifndef INC_FRA421_YUGIOH_H_
#define INC_FRA421_YUGIOH_H_

#include <stdint.h>
//Maximum length of the array
#define MAX_LEN 16

//Number of RFID
#define RFID_NUM_MAX 12
#define CARD_BUFF_LEN 4

typedef union _FRA421_CARD
{
	struct _CardBit
	{
		uint8_t bit0;
		uint8_t bit1;
		uint8_t bit2;
		uint8_t bit3;
	}Cardbit;
	uint32_t data;
}Fra421_Card;

typedef struct _YUGIOH_CARD
{
	uint8_t cardSignature;
	uint8_t cardType;
	uint8_t cardState;
	uint8_t Reserve;
	uint32_t cardData;
}YUGIOH_Card;

typedef struct _RFID
{
	uint16_t slaveAddr;
	uint8_t status;
	Fra421_Card detectedCard;
	YUGIOH_Card mainCard; // This is the card that stand on the field
	YUGIOH_Card bufferCard[CARD_BUFF_LEN]; // [0,1,2,3] Memory
}RFID;

typedef struct _RFIDHANDLE
{
	uint16_t slaveNum;
	uint8_t status;
	uint8_t cardStr[MAX_LEN+1];
	uint8_t cardData[MAX_LEN+1];
	RFID RFID[RFID_NUM_MAX];

}RFIDHandle;

void YUGIOH_card_register(RFIDHandle *rfidmain);

#endif /* INC_FRA421_YUGIOH_H_ */
