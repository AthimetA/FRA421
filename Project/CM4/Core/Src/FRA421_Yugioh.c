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
	ptrYUGIOHCard = ptrRFID->bufferCard;

	// For now fix number
	ptrYUGIOHCard->cardData = ptrCard->data;
	ptrYUGIOHCard->cardSignature = 1 % 256;
	ptrYUGIOHCard->cardState = 0 % 3;
	ptrYUGIOHCard->cardType = 0 % 2;

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

	/*  Action State (Can not use Switch Case TT)
	 * https://stackoverflow.com/questions/2308323/why-no-switch-on-pointers
	 * 		- 0 = No Card
	 * 		- 1 = Card in Play
	 * 		- 2 = Card Played
	 * 		- 255 = Action error clear all buffer
	 *  */
	if (ptrRFID->action == 0) // Main Card is not assign
	{
		YUGIOH_card_copy(ptrYugiohCard_Current, ptrYugiohCard_Main);
		RFID_Clear_Card_Bufffer(ptrRFID);
		ptrRFID->action = 1;
	}
	else if (ptrRFID->action == 1)
	{
		/* Card Type
		 * 		- 0 = Monster
		 * 		- 1 = Spell
		 * 		- 2 = Trap
		 *  */
		if (ptrYugiohCard_Main->cardType == 0 || ptrYugiohCard_Main->cardType == 1)
		{
			if (ptrYugiohCard_Current->cardData == ptrYugiohCard_Main->cardData)
			{
				ptrYugiohCard_Main->cardState = 1;
				RFID_Clear_Card_Bufffer(ptrRFID);
				ptrRFID->action = 2;
			}
			else
			{
				ptrRFID->action = 255;
			}
		}
		else if (ptrYugiohCard_Main->cardType == 2)
		{
			ptrRFID->action = 2;
		}
	}
	else if (ptrRFID->action == 2)
	{
		// AFK
	}
	else if (ptrRFID->action == 255)
	{
		RFID_Clear_Card_Bufffer(ptrRFID);
		YUGIOH_Card buffCard = {0};
		YUGIOH_card_copy(&buffCard, ptrYugiohCard_Main);
		ptrRFID->action = 0;
	}
}
