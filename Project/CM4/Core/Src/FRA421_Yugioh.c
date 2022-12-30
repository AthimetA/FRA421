/*
 * FRA421_Yugioh.c
 *
 *  Created on: Dec 30, 2022
 *      Author: AthimetA
 */

#include "FRA421_Yugioh.h"
#include <stdlib.h>
#include <string.h>

void YUGIOH_card_register(RFIDHandle *rfidmain)
{
	RFID *ptrRFID = rfidmain->RFID;
	Fra421_Card *ptrCard;
	YUGIOH_Card *ptrYUGIOHCard;

	//	// PTR RFID pick slave
	//	ptrRFID += rfidmain->slaveNum;
	//	// PTR Card
	//	ptrCard = &ptrRFID->detectedCard;
	//	ptrYUGIOHCard = ptrRFID->bufferCard;

	ptrRFID = &rfidmain->RFID[rfidmain->slaveNum];
	ptrCard = &ptrRFID->detectedCard;
	ptrYUGIOHCard = &ptrRFID->bufferCard;

	ptrYUGIOHCard->cardData = ptrCard->data;
	ptrYUGIOHCard->cardSignature = 1 % 256;
	ptrYUGIOHCard->cardState = 2 % 3;
	ptrYUGIOHCard->cardType = 2 % 2;

}
