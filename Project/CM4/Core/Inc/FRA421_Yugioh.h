/*
 * FRA421_Yugioh.h
 *
 *  Created on: Dec 29, 2022
 *      Author: AthimetA
 */

#ifndef INC_FRA421_YUGIOH_H_
#define INC_FRA421_YUGIOH_H_

//Maximum length of the array
#define MAX_LEN 16

//Number of RFID
#define RFID_NUM_MAX 12

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

typedef struct _RFID
{
	uint16_t slaveAddr;
	uint8_t RFIDstatus;
	Fra421_Card cardMem;
	Fra421_Card card;
}RFID;

typedef struct _RFIDHANDLE
{
	uint16_t slaveNum;
	uint8_t status;
	uint8_t cardStr[MAX_LEN+1];
	uint8_t cardData[MAX_LEN+1];
	RFID RFID[RFID_NUM_MAX];

}RFIDHandle;

#endif /* INC_FRA421_YUGIOH_H_ */
