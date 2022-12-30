/*
 * FRA421_Yugioh.c
 *
 *  Created on: Dec 30, 2022
 *      Author: AthimetA
 */

#include "FRA421_Yugioh.h"
#include <stdlib.h>
#include <string.h>

void YUGIOH_card_copy(YUGIOH_Card *src, YUGIOH_Card *dst)
{
	dst->cardData = src->cardData;
	dst->cardSignature = src->cardSignature;
	dst->cardState = src->cardState;
	dst->cardType = src->cardType;
}

void RFID_Clear_Card_Bufffer(RFID *rfid)
{
	YUGIOH_Card buffCard = {0};
	YUGIOH_Card *ptrYUGIOHCard = rfid->bufferCard;

	for (uint8_t i = 0; i < CARD_BUFF_LEN ; ++i)
	{
		YUGIOH_card_copy(&buffCard, ptrYUGIOHCard);
		ptrYUGIOHCard++;
	}
}

void YUGIOH_card_register(RFIDHandle *rfidmain)
{
	RFID *ptrRFID = rfidmain->RFID;
	Fra421_Card *ptrCard;
	YUGIOH_Card *ptrYUGIOHCard;

	ptrRFID = &rfidmain->RFID[rfidmain->slaveNum];
	ptrCard = &ptrRFID->detectedCard;
	ptrYUGIOHCard = &ptrRFID->bufferCard;

	// For now fix number
	ptrYUGIOHCard->cardData = ptrCard->data;
	ptrYUGIOHCard->cardSignature = 1 % 256;
	ptrYUGIOHCard->cardState = 2 % 3;
	ptrYUGIOHCard->cardType = 2 % 2;

}

void YUGIOH_card_Management(RFIDHandle *rfidmain)
{
	// Assign RFID
	RFID *ptrRFID = rfidmain->RFID;
	ptrRFID = &rfidmain->RFID[rfidmain->slaveNum];

	// Current Card
	YUGIOH_Card *ptrYugiohCard_Current;
	ptrYugiohCard_Current = &ptrRFID->bufferCard[0];

	// Main Card
	YUGIOH_Card *ptrYugiohCard_Main;
	ptrYugiohCard_Main = &ptrRFID->mainCard;

	/* Switch Action State
	 * 		- 0 = No Card
	 * 		- 1 = Card in Play
	 * 		- 2 = Card Played
	 *  */
	YUGIOH_card_copy(ptrYugiohCard_Current, ptrYugiohCard_Main);
	RFID_Clear_Card_Bufffer(ptrRFID);
	ptrRFID->action = 1;
//	switch (ptrRFID->action)
//	{
//		case 0:
//			// Main Card is not assign
//			YUGIOH_card_copy(ptrYugiohCard_Current, ptrYugiohCard_Main);
//			RFID_Clear_Card_Bufffer(ptrRFID);
//			ptrRFID->action = 1;
//			break;
//		case 1:
//			/* Switch Card Type
//			 * 		- 0 = Monster
//			 * 		- 1 = Spell
//			 * 		- 2 = Trap
//			 *  */
//			switch (ptrYugiohCard_Main->cardType)
//			{
//				case 0:
//					break;
//				case 1:
//					break;
//				case 2:
//					break;
//			}
//			break;
//		case 2:
//			break;
//	}
}
