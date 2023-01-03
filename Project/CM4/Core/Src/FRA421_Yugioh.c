/*
 * FRA421_Yugioh.c
 *
 *  Created on: Dec 30, 2022
 *      Author: AthimetA
 */

#include "../../../Common/Src/FRA421_Yugioh.h"
#include <stdlib.h>
#include <string.h>

// RFID Handle Function
void RFID_Main_init(RFIDHandle *rfidmain)
{
	RFID *ptrRFID = rfidmain->RFID;

	for (uint8_t i = 0;  i < RFID_NUM_MAX; ++i)
	{
		ptrRFID = &rfidmain->RFID[i];
		ptrRFID->slaveAddr = i;
		ptrRFID->status = 0;
		ptrRFID->action = 0;
		RFID_Clear_Card_Bufffer(ptrRFID);
	}
}

void YUGIOH_card_copy(YUGIOH_Card *src, YUGIOH_Card *dst)
{
	dst->cardData = src->cardData;
	dst->cardSignature = src->cardSignature;
	dst->cardState = src->cardState;
	dst->cardType = src->cardType;
	dst->cardLevel = src->cardLevel;
	dst->cardAtk = src->cardAtk;
	dst->cardDef = src->cardDef;
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
	// Load Data from Hash
	YUGIOH_card_Load_Data(ptrYUGIOHCard);
}
void YUGIOH_card_Buffer_Update(RFIDHandle *rfidmain)
{
	// Assign RFID
	RFID *ptrRFID = rfidmain->RFID;
	ptrRFID = &rfidmain->RFID[rfidmain->slaveNum];
	// Buffer Card src
	YUGIOH_Card *ptrYugiohCard_Buffer_src = ptrRFID->bufferCard;
	ptrYugiohCard_Buffer_src = &ptrRFID->bufferCard[CARD_BUFF_LEN-2];
	// Buffer Card dst
	YUGIOH_Card *ptrYugiohCard_Buffer_dst = ptrRFID->bufferCard;
	ptrYugiohCard_Buffer_dst = &ptrRFID->bufferCard[CARD_BUFF_LEN-1];
	for (int i = CARD_BUFF_LEN; i >= 1 ; i--)
	{
		YUGIOH_card_copy(ptrYugiohCard_Buffer_src, ptrYugiohCard_Buffer_dst);
		ptrYugiohCard_Buffer_src--;
		ptrYugiohCard_Buffer_dst--;
	}
}
// Card Hash Function
const uint8_t archive_yugioh_card_sig[256] = {
		 0,  0,  0,  0,  0,  0,  0,  0,  2,  0,  0, 16,  0,  0,  0,
		13, 17,  8,  0,  0,  0, 10,  0,  0,  0,  0,  0,  0,  0,  0,
		 0,  0, 18,  0,  0,  0,  0,  0,  0,  0,  1,  0, 15,  0,  0,
		 0,  0, 19,  0,  0,  0, 12,  0,  0,  0,  0,  0, 20,  0,  0,
		 0,  0,  0,  0,  0,  0, 11,  0,  0,  0,  0,  0,  0, 12,  0,
		 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
		 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  9,  0,
		 0,  0,  3,  0,  0,  0,  0,  0,  6,  0,  0,  0,  0,  0,  0,
		 0,  3,  0,  0,  0,  0,  0,  9,  0,  0,  0,  0,  0,  0,  0,
		 0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 13,  0, 20,  0,  0,
		 0,  0,  0,  0,  0,  0,  0, 17,  0,  0,  4,  0,  0,  0,  0,
		 0,  0,  0,  8,  0, 18,  0,  0,  0,  0,  0,  0,  0,  0,  0,
		 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
		 0,  0,  0,  0,  0, 15,  0,  0,  0,  0,  0,  0,  0,  5,  0,
		 0, 16,  0, 14,  0,  4,  0,  0,  0,  0,  5,  0,  0,  0,  0,
		 0,  0,  0,  1,  0,  0,  0,  0,  6,  0,  0,  0,  0,  0, 10,
		 0,  0,  0,  0,  0,  0,  7,  0, 19,  0, 11,  2,  0,  0,  0,
		};

const uint8_t archive_yugioh_card_type[256] = {
		 0,  0,  0,  0,  0,  0,  0,  0,  1,  0,  0,  2,  0,  0,  0,
		 2,  2,  1,  0,  0,  0,  1,  0,  0,  0,  0,  0,  0,  0,  0,
		 0,  0,  2,  0,  0,  0,  0,  0,  0,  0,  1,  0,  2,  0,  0,
		 0,  0,  2,  0,  0,  0,  2,  0,  0,  0,  0,  0,  3,  0,  0,
		 0,  0,  0,  0,  0,  0,  2,  0,  0,  0,  0,  0,  0,  2,  0,
		 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
		 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  1,  0,
		 0,  0,  1,  0,  0,  0,  0,  0,  1,  0,  0,  0,  0,  0,  0,
		 0,  1,  0,  0,  0,  0,  0,  1,  0,  0,  0,  0,  0,  0,  0,
		 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  2,  0,  3,  0,  0,
		 0,  0,  0,  0,  0,  0,  0,  2,  0,  0,  1,  0,  0,  0,  0,
		 0,  0,  0,  1,  0,  2,  0,  0,  0,  0,  0,  0,  0,  0,  0,
		 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
		 0,  0,  0,  0,  0,  2,  0,  0,  0,  0,  0,  0,  0,  1,  0,
		 0,  2,  0,  2,  0,  1,  0,  0,  0,  0,  1,  0,  0,  0,  0,
		 0,  0,  0,  1,  0,  0,  0,  0,  1,  0,  0,  0,  0,  0,  1,
		 0,  0,  0,  0,  0,  0,  1,  0,  2,  0,  2,  1,  0,  0,  0,
		};

const uint8_t archive_yugioh_card_level[256] = {
		 0,  0,  0,  0,  0,  0,  0,  0,  4,  0,  0,  0,  0,  0,  0,
		 0,  0,  3,  0,  0,  0,  4,  0,  0,  0,  0,  0,  0,  0,  0,
		 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  7,  0,  0,  0,  0,
		 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
		 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
		 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
		 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  4,  0,
		 0,  0,  1,  0,  0,  0,  0,  0,  8,  0,  0,  0,  0,  0,  0,
		 0,  1,  0,  0,  0,  0,  0,  4,  0,  0,  0,  0,  0,  0,  0,
		 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
		 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  4,  0,  0,  0,  0,
		 0,  0,  0,  3,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
		 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
		 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  4,  0,
		 0,  0,  0,  0,  0,  4,  0,  0,  0,  0,  4,  0,  0,  0,  0,
		 0,  0,  0,  7,  0,  0,  0,  0,  8,  0,  0,  0,  0,  0,  4,
		 0,  0,  0,  0,  0,  0,  4,  0,  0,  0,  0,  4,  0,  0,  0,
		};

const uint8_t archive_yugioh_card_atk[256] = {
		 0,  0,  0,  0,  0,  0,  0,  0, 14,  0,  0,  0,  0,  0,  0,
		 0,  0,  6,  0,  0,  0, 12,  0,  0,  0,  0,  0,  0,  0,  0,
		 0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 25,  0,  0,  0,  0,
		 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
		 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
		 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
		 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 12,  0,
		 0,  0,  3,  0,  0,  0,  0,  0, 30,  0,  0,  0,  0,  0,  0,
		 0,  3,  0,  0,  0,  0,  0, 12,  0,  0,  0,  0,  0,  0,  0,
		 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
		 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  8,  0,  0,  0,  0,
		 0,  0,  0,  6,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
		 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
		 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 13,  0,
		 0,  0,  0,  0,  0,  8,  0,  0,  0,  0, 13,  0,  0,  0,  0,
		 0,  0,  0, 25,  0,  0,  0,  0, 30,  0,  0,  0,  0,  0, 12,
		 0,  0,  0,  0,  0,  0, 10,  0,  0,  0,  0, 14,  0,  0,  0,
		};

const uint8_t archive_yugioh_card_def[256] = {
		 0,  0,  0,  0,  0,  0,  0,  0, 12,  0,  0,  0,  0,  0,  0,
		 0,  0, 15,  0,  0,  0, 20,  0,  0,  0,  0,  0,  0,  0,  0,
		 0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 21,  0,  0,  0,  0,
		 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
		 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
		 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
		 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 10,  0,
		 0,  0,  2,  0,  0,  0,  0,  0, 25,  0,  0,  0,  0,  0,  0,
		 0,  2,  0,  0,  0,  0,  0, 10,  0,  0,  0,  0,  0,  0,  0,
		 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
		 0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 20,  0,  0,  0,  0,
		 0,  0,  0, 15,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
		 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
		 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 14,  0,
		 0,  0,  0,  0,  0, 20,  0,  0,  0,  0, 14,  0,  0,  0,  0,
		 0,  0,  0, 21,  0,  0,  0,  0, 25,  0,  0,  0,  0,  0, 20,
		 0,  0,  0,  0,  0,  0, 19,  0,  0,  0,  0, 12,  0,  0,  0,
		};

uint8_t CardHash_Encode(uint32_t key)
{
	key = ((key >> 16) ^ key) * 0x45d9f3b;
	key = ((key >> 16) ^ key) * 0x45d9f3b;
	key = (key >> 16) ^ key;
  return (uint8_t)(key & 0xff);
}

void YUGIOH_card_Load_Data(YUGIOH_Card *card)
{
	uint8_t idx = CardHash_Encode(card->cardData);
	card->cardSignature = archive_yugioh_card_sig[idx];
	card->cardType = archive_yugioh_card_type[idx];
	card->cardState = 0;
	card->cardLevel = archive_yugioh_card_level[idx];
	card->cardAtk = archive_yugioh_card_atk[idx];
	card->cardDef = archive_yugioh_card_def[idx];
}
