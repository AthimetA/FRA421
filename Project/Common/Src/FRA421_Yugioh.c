/*
 * FRA421_Yugioh.c
 *
 *  Created on: Dec 30, 2022
 *      Author: AthimetA
 */

#include "FRA421_Yugioh.h"
#include <stdlib.h>
#include <string.h>
//tuta include
#include <stdio.h>
#include "../../../CM7/Core/Inc/st7735.h"
#include "../../../CM7/Core/Inc/fonts.h"
#include "stm32h7xx_hal.h"
#include "../../../Drivers/STM32H7xx_HAL_Driver/Inc/stm32h7xx_hal_tim.h"
#include "../../../CM7/Core/Inc/stm32h7xx_it.h"

char C_LP1[4] = "";
char C_LP2[4] = "";
char C_LP_ATK[4] = "";
char C_LP_DEF[4] = "";
char c_turn[2] = "";
char t_c[3] = "";

uint16_t timeinit = 600;
uint16_t time = 0;
uint8_t turn = 0;

// RFID Handle Function
void RFID_Main_init(RFIDHandle *rfidmain) {
	RFID *ptrRFID = rfidmain->RFID;

	for (uint8_t i = 0; i < RFID_NUM_MAX; ++i) {
		ptrRFID = &rfidmain->RFID[i];
		ptrRFID->slaveAddr = i;
		ptrRFID->status = 0;
		ptrRFID->action = 0;
		RFID_Clear_Card_Bufffer(ptrRFID);
	}
}

void YUGIOH_card_copy(YUGIOH_Card *src, YUGIOH_Card *dst) {
	dst->cardData = src->cardData;
	dst->cardSignature = src->cardSignature;
	dst->cardState = src->cardState;
	dst->cardType = src->cardType;
	dst->cardLevel = src->cardLevel;
	dst->cardAtk = src->cardAtk;
	dst->cardDef = src->cardDef;
	dst->standPosition = src->standPosition;
	dst->targetPosition = src->targetPosition;
	dst->actionPoint_Atk = src->actionPoint_Atk;
	dst->actionPoint_Eff = src->actionPoint_Eff;
}

void YUGIOH_card_clear(YUGIOH_Card *card)
{
	YUGIOH_Card buff = {0};
	YUGIOH_card_copy(&buff, card);
}

void RFID_Clear_Card_Bufffer(RFID *rfid) {
	YUGIOH_Card *ptrYUGIOHCard = rfid->bufferCard;
	for (uint8_t i = 0; i < CARD_BUFF_LEN; ++i) {
		YUGIOH_card_clear(ptrYUGIOHCard);
		ptrYUGIOHCard++;
	}
}

void YUGIOH_card_register(RFIDHandle *rfidmain) {
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
	ptrRFID->action = 1;
	ptrYUGIOHCard->standPosition = ptrRFID->slaveAddr;

}
void YUGIOH_card_Buffer_Update(RFIDHandle *rfidmain) {
	// Assign RFID
	RFID *ptrRFID = rfidmain->RFID;
	ptrRFID = &rfidmain->RFID[rfidmain->slaveNum];
	// Buffer Card src
	YUGIOH_Card *ptrYugiohCard_Buffer_src = ptrRFID->bufferCard;
	ptrYugiohCard_Buffer_src = &ptrRFID->bufferCard[CARD_BUFF_LEN - 2];
	// Buffer Card dst
	YUGIOH_Card *ptrYugiohCard_Buffer_dst = ptrRFID->bufferCard;
	ptrYugiohCard_Buffer_dst = &ptrRFID->bufferCard[CARD_BUFF_LEN - 1];
	for (int i = CARD_BUFF_LEN; i >= 1; i--) {
		YUGIOH_card_copy(ptrYugiohCard_Buffer_src, ptrYugiohCard_Buffer_dst);
		ptrYugiohCard_Buffer_src--;
		ptrYugiohCard_Buffer_dst--;
	}
}
// Card Hash Function
const uint8_t archive_yugioh_card_sig[256] = { 0, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0,
		16, 0, 0, 0, 13, 17, 8, 0, 0, 0, 10, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 18,
		0, 0, 0, 0, 0, 0, 0, 1, 0, 15, 0, 0, 0, 0, 19, 0, 0, 0, 12, 0, 0, 0, 0,
		0, 20, 0, 0, 0, 0, 0, 0, 0, 0, 11, 0, 0, 0, 0, 0, 0, 12, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		9, 0, 0, 0, 3, 0, 0, 0, 0, 0, 6, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 0,
		9, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 13, 0, 20, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 17, 0, 0, 4, 0, 0, 0, 0, 0, 0, 0, 8, 0, 18, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 15, 0, 0, 0, 0, 0, 0, 0, 5, 0, 0, 16, 0, 14, 0, 4, 0, 0, 0, 0,
		5, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 6, 0, 0, 0, 0, 0, 10, 0, 0, 0, 0,
		0, 0, 7, 0, 19, 0, 11, 2, 0, 0, 0, };

const uint8_t archive_yugioh_card_type[256] = {
		0,  0,  0,  0,  0,  0,  0,  0,  1,  0,  0,  2,  0,  0,  0,
		2,  2,  1,  0,  0,  0,  1,  0,  0,  0,  0,  0,  0,  0,  0,
		0,  0,  2,  0,  0,  0,  0,  0,  0,  0,  1,  0,  3,  0,  0,
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
		0,  0,  0,  0,  0,  3,  0,  0,  0,  0,  0,  0,  0,  1,  0,
		0,  2,  0,  3,  0,  1,  0,  0,  0,  0,  1,  0,  0,  0,  0,
		0,  0,  0,  1,  0,  0,  0,  0,  1,  0,  0,  0,  0,  0,  1,
		0,  0,  0,  0,  0,  0,  1,  0,  2,  0,  2,  1,  0,  0,  0,
};

const uint8_t archive_yugioh_card_level[256] = { 0, 0, 0, 0, 0, 0, 0, 0, 4, 0,
		0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 0, 0,
		0, 1, 0, 0, 0, 0, 0, 8, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 4, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 4, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 4, 0, 0, 0, 0, 0, 0, 4, 0, 0, 0, 0, 4, 0, 0, 0, 0, 0,
		0, 0, 7, 0, 0, 0, 0, 8, 0, 0, 0, 0, 0, 4, 0, 0, 0, 0, 0, 0, 4, 0, 0, 0,
		0, 4, 0, 0, 0, };

const uint8_t archive_yugioh_card_atk[256] = { 0, 0, 0, 0, 0, 0, 0, 0, 14, 0, 0,
		0, 0, 0, 0, 0, 0, 6, 0, 0, 0, 12, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 25, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 12, 0, 0, 0,
		3, 0, 0, 0, 0, 0, 30, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 12, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 8, 0, 0, 0, 0, 0, 0, 0, 6, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 13, 0, 0, 0, 0, 0, 0, 8, 0, 0, 0, 0, 13, 0, 0, 0, 0,
		0, 0, 0, 25, 0, 0, 0, 0, 30, 0, 0, 0, 0, 0, 12, 0, 0, 0, 0, 0, 0, 10, 0,
		0, 0, 0, 14, 0, 0, 0, };

const uint8_t archive_yugioh_card_def[256] = { 0, 0, 0, 0, 0, 0, 0, 0, 12, 0, 0,
		0, 0, 0, 0, 0, 0, 15, 0, 0, 0, 20, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 21, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 10, 0, 0,
		0, 2, 0, 0, 0, 0, 0, 25, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 10, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 20, 0, 0, 0, 0, 0, 0, 0, 15, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 14, 0, 0, 0, 0, 0, 0, 20, 0, 0, 0, 0, 14, 0, 0,
		0, 0, 0, 0, 0, 21, 0, 0, 0, 0, 25, 0, 0, 0, 0, 0, 20, 0, 0, 0, 0, 0, 0,
		19, 0, 0, 0, 0, 12, 0, 0, 0, };

uint8_t CardHash_Encode(uint32_t key) {
	key = ((key >> 16) ^ key) * 0x45d9f3b;
	key = ((key >> 16) ^ key) * 0x45d9f3b;
	key = (key >> 16) ^ key;
	return (uint8_t) (key & 0xff);
}

void YUGIOH_card_Load_Data(YUGIOH_Card *card) {
	uint8_t idx = CardHash_Encode(card->cardData);
	card->cardSignature = archive_yugioh_card_sig[idx];
	card->cardType = archive_yugioh_card_type[idx];
	card->cardState = 0;
	card->cardLevel = archive_yugioh_card_level[idx];
	card->cardAtk = archive_yugioh_card_atk[idx];
	card->cardDef = archive_yugioh_card_def[idx];
	card->actionPoint_Atk = 0;
	card->actionPoint_Eff = 0;
	// Stand Position and target Position Will be assign outside this function
	card->standPosition = 255;
	card->targetPosition = 255;
}

void Player_Reading_Card(RFIDHandle *RFIDmain, State_game *state_game ,Player *player)
{
	// Assign RFID
	RFID *ptrRFID = RFIDmain->RFID;
	ptrRFID = &RFIDmain->RFID[RFIDmain->slaveNum];

	YUGIOH_Card *ptrYugiohCard_src;
	ptrYugiohCard_src = &ptrRFID->bufferCard[0];

	YUGIOH_Card *ptrYugiohCard_dst;
	ptrYugiohCard_dst = &player->ActtionBuffer[0];

	YUGIOH_Card *ptrYugiohCard_played;
	ptrYugiohCard_played = &player->cardOnBoard[0];

	uint8_t flag_played = 0;

	if (ptrRFID->action == 1) // Card Detected
	{

		// Check if in Board
		for (int i  = 0;  i < 6; ++i) {
			if (ptrYugiohCard_played->cardData == ptrYugiohCard_src->cardData && ptrYugiohCard_played->cardType == 1) {
				flag_played = 1;
				break;
			}
			ptrYugiohCard_played++;
		}

		// Check if in Played
		ptrYugiohCard_played = &player->CardInPlayed;
		if (ptrYugiohCard_played->cardData == ptrYugiohCard_src->cardData) {
			flag_played = 1;
		}

		// Check if in GY
		ptrYugiohCard_played = &player->GY[0];
		for (int i  = 0;  i < GY_BUFF_LEN; ++i) {
			if (ptrYugiohCard_played->cardData == ptrYugiohCard_src->cardData) {
				flag_played = 1;
				break;
			}
			ptrYugiohCard_played++;
		}

		// Check if in Buffer
		ptrYugiohCard_played = &player->ActtionBuffer[0];
		for (int i  = 0;  i < CHAIN_BUFF_LEN-1; ++i) {
			if (ptrYugiohCard_played->cardData == ptrYugiohCard_src->cardData) {
				flag_played = 1;
				break;
			}
			ptrYugiohCard_played++;
		}

		uint8_t inpos = ptrYugiohCard_src->standPosition %6;
		if(ptrYugiohCard_src->cardType == 1)
		{

			if(inpos <= 2)
			{
				flag_played = 1;
			}

		}
		else
		{
			if(inpos > 2)
			{
				flag_played = 1;
			}
		}


		if (player->turn == first && flag_played == 0)
		{


			// Update buffer
			YUGIOH_card_Buffer_Update_Player(player);
			YUGIOH_card_copy(ptrYugiohCard_src, ptrYugiohCard_dst);

			// Mod standPosition in case off 2 player
			ptrYugiohCard_dst->standPosition= ptrYugiohCard_dst->standPosition % 6;
			// Card Reading So it can't attack
			ptrYugiohCard_dst->actionPoint_Atk = 0;
			// Card Reading Then it can use it Effect
			ptrYugiohCard_dst->actionPoint_Eff = 1;
			RFID_Clear_Card_Bufffer(ptrRFID);
			ptrRFID->action = 0;
			state_game->action += 1;
		}
		else if (player->turn == second && flag_played == 0) {
			if (ptrYugiohCard_src->standPosition >= 6)
			{
				// Update buffer
				YUGIOH_card_Buffer_Update_Player(player);
				YUGIOH_card_copy(ptrYugiohCard_src, ptrYugiohCard_dst);

				// Mod standPosition in case off 2 player
				ptrYugiohCard_dst->standPosition= ptrYugiohCard_dst->standPosition % 6;
				// Card Reading So it can't attack
				ptrYugiohCard_dst->actionPoint_Atk = 0;
				// Card Reading Then it can use it Effect
				ptrYugiohCard_dst->actionPoint_Eff = 1;
				RFID_Clear_Card_Bufffer(ptrRFID);
				ptrRFID->action = 0;
				state_game->action += 1;
			}

		}
	}
}

void Player_Reading_Card_Mainphase(RFIDHandle *RFIDmain, State_game *state_game ,Player *player)
{
	// Assign RFID
	RFID *ptrRFID = RFIDmain->RFID;
	ptrRFID = &RFIDmain->RFID[RFIDmain->slaveNum];

	YUGIOH_Card *ptrYugiohCard_src;
	ptrYugiohCard_src = &ptrRFID->bufferCard[0];

	YUGIOH_Card *ptrYugiohCard_dst;
	ptrYugiohCard_dst = &player->ActtionBuffer[0];

	YUGIOH_Card *ptrYugiohCard_played;
	ptrYugiohCard_played = &player->cardOnBoard[0];

	uint8_t flag_played = 0;

	if (ptrRFID->action == 1) // Card Detected
	{

		// Check if in Board
		for (int i  = 0;  i < 6; ++i) {
			if (ptrYugiohCard_played->cardData == ptrYugiohCard_src->cardData && ptrYugiohCard_played->cardType == 1) {
				flag_played = 1;
				break;
			}
			ptrYugiohCard_played++;
		}

		// Check if in Played
		ptrYugiohCard_played = &player->CardInPlayed;
		if (ptrYugiohCard_played->cardData == ptrYugiohCard_src->cardData) {
			flag_played = 1;
		}

		// Check if in GY
		ptrYugiohCard_played = &player->GY[0];
		for (int i  = 0;  i < GY_BUFF_LEN; ++i) {
			if (ptrYugiohCard_played->cardData == ptrYugiohCard_src->cardData) {
				flag_played = 1;
				break;
			}
			ptrYugiohCard_played++;
		}

		// Check if in Buffer
		ptrYugiohCard_played = &player->ActtionBuffer[0];
		for (int i  = 0;  i < CHAIN_BUFF_LEN-1; ++i) {
			if (ptrYugiohCard_played->cardData == ptrYugiohCard_src->cardData) {
				flag_played = 1;
				break;
			}
			ptrYugiohCard_played++;
		}

		if (player->turn == first && flag_played == 0) {

			if (ptrYugiohCard_src->standPosition < 6)
			{
				// Update buffer
				YUGIOH_card_Buffer_Update_Player(player);
				YUGIOH_card_copy(ptrYugiohCard_src, ptrYugiohCard_dst);

				// Mod standPosition in case off 2 player
				ptrYugiohCard_dst->standPosition= ptrYugiohCard_dst->standPosition % 6;
				// Card Reading So it can't attack
				ptrYugiohCard_dst->actionPoint_Atk = 0;
				// Card Reading Then it can use it Effect
				ptrYugiohCard_dst->actionPoint_Eff = 1;
				RFID_Clear_Card_Bufffer(ptrRFID);
				ptrRFID->action = 0;
				state_game->action += 1;
			}
		}
		else if (player->turn == second && flag_played == 0) {
			if (ptrYugiohCard_src->standPosition >= 6)
			{
				// Update buffer
				YUGIOH_card_Buffer_Update_Player(player);
				YUGIOH_card_copy(ptrYugiohCard_src, ptrYugiohCard_dst);

				// Mod standPosition in case off 2 player
				ptrYugiohCard_dst->standPosition= ptrYugiohCard_dst->standPosition % 6;
				// Card Reading So it can't attack
				ptrYugiohCard_dst->actionPoint_Atk = 0;
				// Card Reading Then it can use it Effect
				ptrYugiohCard_dst->actionPoint_Eff = 1;
				RFID_Clear_Card_Bufffer(ptrRFID);
				ptrRFID->action = 0;
				state_game->action += 1;
			}

		}

	}

}

void Player_Reading_Card_Reborn(RFIDHandle *RFIDmain, State_game *state_game ,Player *player)
{
	// Assign RFID
	RFID *ptrRFID = RFIDmain->RFID;
	ptrRFID = &RFIDmain->RFID[RFIDmain->slaveNum];

	YUGIOH_Card *ptrYugiohCard_src;
	ptrYugiohCard_src = &ptrRFID->bufferCard[0];

	YUGIOH_Card *ptrYugiohCard_dst;
	ptrYugiohCard_dst = &player->ActtionBuffer[0];

	YUGIOH_Card *ptrYugiohCard_played;
	ptrYugiohCard_played = &player->cardOnBoard[0];

	uint8_t flag_played = 1;

	if (ptrRFID->action == 1) // Card Detected
	{
		// Check if in GY
		ptrYugiohCard_played = &player->GY[0];
		for (int i  = 0;  i < GY_BUFF_LEN; ++i) {
			if (ptrYugiohCard_played->cardData == ptrYugiohCard_src->cardData) {
				flag_played = 0;
				break;
			}
			ptrYugiohCard_played++;
		}


		if (player->turn == first && flag_played == 0) {

			if (ptrYugiohCard_src->standPosition < 6)
			{
				// Update buffer
				YUGIOH_card_Buffer_Update_Player(player);
				YUGIOH_card_copy(ptrYugiohCard_src, ptrYugiohCard_dst);

				// Mod standPosition in case off 2 player
				ptrYugiohCard_dst->standPosition= ptrYugiohCard_dst->standPosition % 6;
				// Card Reading So it can't attack
				ptrYugiohCard_dst->actionPoint_Atk = 0;
				// Card Reading Then it can use it Effect
				ptrYugiohCard_dst->actionPoint_Eff = 1;
				RFID_Clear_Card_Bufffer(ptrRFID);
				ptrRFID->action = 0;
				state_game->action += 1;
			}
		}
		else if (player->turn == second && flag_played == 0) {
			if (ptrYugiohCard_src->standPosition >= 6)
			{
				// Update buffer
				YUGIOH_card_Buffer_Update_Player(player);
				YUGIOH_card_copy(ptrYugiohCard_src, ptrYugiohCard_dst);

				// Mod standPosition in case off 2 player
				ptrYugiohCard_dst->standPosition= ptrYugiohCard_dst->standPosition % 6;
				// Card Reading So it can't attack
				ptrYugiohCard_dst->actionPoint_Atk = 0;
				// Card Reading Then it can use it Effect
				ptrYugiohCard_dst->actionPoint_Eff = 1;
				RFID_Clear_Card_Bufffer(ptrRFID);
				ptrRFID->action = 0;
				state_game->action += 1;
			}

		}

	}
}


void Player_Reading_Card_Ancient_Rule(RFIDHandle *RFIDmain, State_game *state_game ,Player *player)
{
	// Assign RFID
	RFID *ptrRFID = RFIDmain->RFID;
	ptrRFID = &RFIDmain->RFID[RFIDmain->slaveNum];

	YUGIOH_Card *ptrYugiohCard_src;
	ptrYugiohCard_src = &ptrRFID->bufferCard[0];

	YUGIOH_Card *ptrYugiohCard_dst;
	ptrYugiohCard_dst = &player->ActtionBuffer[0];

	YUGIOH_Card *ptrYugiohCard_played;
	ptrYugiohCard_played = &player->cardOnBoard[0];

	uint8_t flag_played = 0;

	if (ptrRFID->action == 1) // Card Detected
	{

		// Check if in Board
		for (int i  = 0;  i < 6; ++i) {
			if (ptrYugiohCard_played->cardData == ptrYugiohCard_src->cardData && ptrYugiohCard_played->cardType == 1) {
				flag_played = 1;
				break;
			}
			ptrYugiohCard_played++;
		}

		// Check if in Played
		ptrYugiohCard_played = &player->CardInPlayed;
		if (ptrYugiohCard_played->cardData == ptrYugiohCard_src->cardData) {
			flag_played = 1;
		}

		// Check if in GY
		ptrYugiohCard_played = &player->GY[0];
		for (int i  = 0;  i < GY_BUFF_LEN; ++i) {
			if (ptrYugiohCard_played->cardData == ptrYugiohCard_src->cardData) {
				flag_played = 1;
				break;
			}
			ptrYugiohCard_played++;
		}

		// Check if in Buffer
		ptrYugiohCard_played = &player->ActtionBuffer[0];
		for (int i  = 0;  i < CHAIN_BUFF_LEN-1; ++i) {
			if (ptrYugiohCard_played->cardData == ptrYugiohCard_src->cardData) {
				flag_played = 1;
				break;
			}
			ptrYugiohCard_played++;
		}

		//

		if(ptrYugiohCard_src->cardLevel > 5){
			flag_played = 0;
		}

		if (player->turn == first && flag_played == 0) {

			if (ptrYugiohCard_src->standPosition < 6)
			{
				// Update buffer
				YUGIOH_card_Buffer_Update_Player(player);
				YUGIOH_card_copy(ptrYugiohCard_src, ptrYugiohCard_dst);

				// Mod standPosition in case off 2 player
				ptrYugiohCard_dst->standPosition= ptrYugiohCard_dst->standPosition % 6;
				// Card Reading So it can't attack
				ptrYugiohCard_dst->actionPoint_Atk = 0;
				// Card Reading Then it can use it Effect
				ptrYugiohCard_dst->actionPoint_Eff = 1;
				RFID_Clear_Card_Bufffer(ptrRFID);
				ptrRFID->action = 0;
				state_game->action += 1;
			}
		}
		else if (player->turn == second && flag_played == 0) {
			if (ptrYugiohCard_src->standPosition >= 6)
			{
				// Update buffer
				YUGIOH_card_Buffer_Update_Player(player);
				YUGIOH_card_copy(ptrYugiohCard_src, ptrYugiohCard_dst);

				// Mod standPosition in case off 2 player
				ptrYugiohCard_dst->standPosition= ptrYugiohCard_dst->standPosition % 6;
				// Card Reading So it can't attack
				ptrYugiohCard_dst->actionPoint_Atk = 0;
				// Card Reading Then it can use it Effect
				ptrYugiohCard_dst->actionPoint_Eff = 1;
				RFID_Clear_Card_Bufffer(ptrRFID);
				ptrRFID->action = 0;
				state_game->action += 1;
			}

		}

	}
}

void Player_Reading_Card_Monster_Effect(RFIDHandle *RFIDmain, State_game *state_game ,Player *player)
{
	// Assign RFID
	RFID *ptrRFID = RFIDmain->RFID;
	ptrRFID = &RFIDmain->RFID[RFIDmain->slaveNum];

	YUGIOH_Card *ptrYugiohCard_src;
	ptrYugiohCard_src = &ptrRFID->bufferCard[0];

	YUGIOH_Card *ptrYugiohCard_dst;
	ptrYugiohCard_dst = &player->ActtionBuffer[0];

	YUGIOH_Card *ptrYugiohCard_played;
	ptrYugiohCard_played = &player->cardOnBoard[0];

	uint8_t flag_played = 1;

	if (ptrRFID->action == 1) // Card Detected
	{

		// Check if not in Board
		for (int i  = 0;  i < 6; ++i) {

			if (ptrYugiohCard_played->cardData == ptrYugiohCard_src->cardData) {
				flag_played = 1;
				break;
			}
			ptrYugiohCard_played++;
		}

		if(ptrYugiohCard_src->cardSignature == 3){
			flag_played = 0;
		}

		// Check if in Played
		ptrYugiohCard_played = &player->CardInPlayed;
		if (ptrYugiohCard_played->cardData == ptrYugiohCard_src->cardData) {
			flag_played = 1;
		}

		// Check if in GY
		ptrYugiohCard_played = &player->GY[0];
		for (int i  = 0;  i < GY_BUFF_LEN; ++i) {
			if (ptrYugiohCard_played->cardData == ptrYugiohCard_src->cardData) {
				flag_played = 1;
				break;
			}
			ptrYugiohCard_played++;
		}

		// Check if in Buffer
		ptrYugiohCard_played = &player->ActtionBuffer[0];
		for (int i  = 0;  i < CHAIN_BUFF_LEN-1; ++i) {
			if (ptrYugiohCard_played->cardData == ptrYugiohCard_src->cardData) {
				flag_played = 1;
				break;
			}
			ptrYugiohCard_played++;
		}

		//


		if (player->turn == first && flag_played == 0) {

			if (ptrYugiohCard_src->standPosition < 6)
			{
				// Update buffer
				YUGIOH_card_Buffer_Update_Player(player);
				YUGIOH_card_copy(ptrYugiohCard_src, ptrYugiohCard_dst);

				// Mod standPosition in case off 2 player
				ptrYugiohCard_dst->standPosition= ptrYugiohCard_dst->standPosition % 6;
				// Card Reading So it can't attack
				ptrYugiohCard_dst->actionPoint_Atk = 0;
				// Card Reading Then it can use it Effect
				ptrYugiohCard_dst->actionPoint_Eff = 1;
				RFID_Clear_Card_Bufffer(ptrRFID);
				ptrRFID->action = 0;
				state_game->action += 1;
			}
		}
		else if (player->turn == second && flag_played == 0) {
			if (ptrYugiohCard_src->standPosition >= 6)
			{
				// Update buffer
				YUGIOH_card_Buffer_Update_Player(player);
				YUGIOH_card_copy(ptrYugiohCard_src, ptrYugiohCard_dst);

				// Mod standPosition in case off 2 player
				ptrYugiohCard_dst->standPosition= ptrYugiohCard_dst->standPosition % 6;
				// Card Reading So it can't attack
				ptrYugiohCard_dst->actionPoint_Atk = 0;
				// Card Reading Then it can use it Effect
				ptrYugiohCard_dst->actionPoint_Eff = 1;
				RFID_Clear_Card_Bufffer(ptrRFID);
				ptrRFID->action = 0;
				state_game->action += 1;
			}

		}

	}
}

void YUGIOH_Clear_Card_Bufffer_Player(Player *player) {
	YUGIOH_Card buffCard = { 0 };
	YUGIOH_Card *ptrYUGIOHCard = player->ActtionBuffer;
	for (uint8_t i = 0; i < CHAIN_BUFF_LEN; ++i) {
		YUGIOH_card_copy(&buffCard, ptrYUGIOHCard);
		ptrYUGIOHCard++;
	}
}

void YUGIOH_Clear_Card_All(Player *player){
	YUGIOH_Card buffCard = { 0 };
	YUGIOH_Card *ptrYUGIOHCard = player->ActtionBuffer;

	for (uint8_t i = 0; i < CHAIN_BUFF_LEN; ++i) {
		YUGIOH_card_copy(&buffCard, ptrYUGIOHCard);
		ptrYUGIOHCard++;

	}
	ptrYUGIOHCard = player->GY;
	for(uint8_t i = 0; i < GY_BUFF_LEN; ++i){
		YUGIOH_card_copy(&buffCard, ptrYUGIOHCard);
		ptrYUGIOHCard++;
	}
	ptrYUGIOHCard = player->cardOnBoard;
	for(uint8_t i = 0; i < CHAIN_BUFF_LEN; ++i){
		YUGIOH_card_copy(&buffCard, ptrYUGIOHCard);
		ptrYUGIOHCard++;
	}

	ptrYUGIOHCard = &player->CardInPlayed;
	YUGIOH_card_copy(&buffCard, ptrYUGIOHCard);
}

void YUGIOH_card_Buffer_Update_Player(Player *player) {
	// Buffer Card src
	YUGIOH_Card *ptrYugiohCard_Buffer_src = player->ActtionBuffer;
	ptrYugiohCard_Buffer_src = &player->ActtionBuffer[CHAIN_BUFF_LEN - 2];
	// Buffer Card dst
	YUGIOH_Card *ptrYugiohCard_Buffer_dst = player->ActtionBuffer;
	ptrYugiohCard_Buffer_dst = &player->ActtionBuffer[CHAIN_BUFF_LEN - 1];
	for (int i = CHAIN_BUFF_LEN; i >= 1; i--) {
		YUGIOH_card_copy(ptrYugiohCard_Buffer_src, ptrYugiohCard_Buffer_dst);
		ptrYugiohCard_Buffer_src--;
		ptrYugiohCard_Buffer_dst--;
	}
}

void YUGIOH_card_Buffer_Update_Chain(State_game * state_game)
{

	// Buffer Card src
	YUGIOH_Card *ptrYugiohCard_Buffer_src = state_game->ChainBuffer;
	// Buffer Card dst
	YUGIOH_Card *ptrYugiohCard_Buffer_dst = state_game->ChainBuffer;

	ptrYugiohCard_Buffer_src = &state_game->ChainBuffer[CHAIN_BUFF_LEN - 2];
	ptrYugiohCard_Buffer_dst = &state_game->ChainBuffer[CHAIN_BUFF_LEN - 1];

	for (int i = CHAIN_BUFF_LEN; i >= 1; i--) {
		YUGIOH_card_copy(ptrYugiohCard_Buffer_src, ptrYugiohCard_Buffer_dst);
		state_game->ptrChainUser[i-1] = state_game->ptrChainUser[i-2];
		state_game->ptrChainOpponent[i-1] = state_game->ptrChainOpponent[i-2];
		ptrYugiohCard_Buffer_src--;
		ptrYugiohCard_Buffer_dst--;
	}
}

void YUGIOH_To_GY(Player *player,YUGIOH_Card *card){
	YUGIOH_Card buffCard = { 0 };


	// Buffer Card src
	YUGIOH_Card *ptrYugiohCard_Buffer_src = player->GY;
	ptrYugiohCard_Buffer_src = &player->GY[GY_BUFF_LEN - 2];
	// Buffer Card dst
	YUGIOH_Card *ptrYugiohCard_Buffer_dst = player->GY;
	ptrYugiohCard_Buffer_dst = &player->GY[GY_BUFF_LEN - 1];
	for (int i = GY_BUFF_LEN; i >= 1; i--) {
		//			if(){
		//
		//			}
		YUGIOH_card_copy(ptrYugiohCard_Buffer_src, ptrYugiohCard_Buffer_dst);
		ptrYugiohCard_Buffer_src--;
		ptrYugiohCard_Buffer_dst--;
	}

	ptrYugiohCard_Buffer_src = card;
	ptrYugiohCard_Buffer_dst = &player->GY[0];
	YUGIOH_card_copy(ptrYugiohCard_Buffer_src, ptrYugiohCard_Buffer_dst);
	ptrYugiohCard_Buffer_dst = &buffCard;
	YUGIOH_card_copy(ptrYugiohCard_Buffer_dst, ptrYugiohCard_Buffer_src);

}


void GAME_PLAY_Management(RFIDHandle *RFIDmain, State_game *state_game) {

	Player *ptrPlayer1 = state_game->player;
	Player *ptrPlayer2 = state_game->player;

	ptrPlayer1 = &state_game->player[0];
	ptrPlayer2 = &state_game->player[1];

	ptrPlayer1->turn = first;
	ptrPlayer2->turn = second;

	ptrPlayer1->displayNSS = 1;
	ptrPlayer2->displayNSS = 0;

	ptrPlayer1->yesBTN = HAL_GPIO_ReadPin(YES1_PORT, YES1_PIN);
	ptrPlayer1->noBTN = HAL_GPIO_ReadPin(NO1_PORT, NO1_PIN);

	ptrPlayer2->yesBTN = HAL_GPIO_ReadPin(YES2_PORT, YES2_PIN);
	ptrPlayer2->noBTN = HAL_GPIO_ReadPin(NO2_PORT, NO2_PIN);

	enum _player_state {
		Game_not_start,Game_init, first_player, second_player,Game_Ended
	} MS;
	MS = state_game->MainGame_State;


	// Player ATK and DEF
	Player *ptrPlayerAtk = state_game->player;
	Player *ptrPlayerDef = state_game->player;

	switch (MS) {
	case Game_not_start:
		// START display for ready state
		//player 2
		ST7735_WriteString(5, 5, "Player 2: ", Font_7x10, ST7735_WHITE, ST7735_BLACK);
		ST7735_WriteString(0, 15, "__________________", Font_7x10, ST7735_WHITE, ST7735_BLACK);
		ST7735_WriteString(8, 35, "Hit button", Font_11x18, ST7735_MAGENTA, ST7735_BLACK);
		ST7735_WriteString(25, 60, "to DUEL", Font_11x18, ST7735_CYAN, ST7735_BLACK);
		ST7735_WriteString(0, 75, "__________________", Font_7x10, ST7735_WHITE, ST7735_BLACK);
		ST7735_WriteString(0, 90, "Initial LP: ", Font_7x10, ST7735_WHITE, ST7735_BLACK);
		ST7735_WriteString(93, 90, "4000", Font_7x10, ST7735_GREEN, ST7735_BLACK);
		ST7735_WriteString(0, 105, "Initial Timer: ", Font_7x10, ST7735_WHITE, ST7735_BLACK);
		ST7735_WriteString(100, 105, "180", Font_7x10, ST7735_GREEN, ST7735_BLACK);
		//player 1
		ST7735_WriteString1(5, 5, "Player 1: ", Font_7x10, ST7735_WHITE, ST7735_BLACK);
		ST7735_WriteString1(0, 15, "__________________", Font_7x10, ST7735_WHITE, ST7735_BLACK);
		ST7735_WriteString1(8, 35, "Hit button", Font_11x18, ST7735_MAGENTA, ST7735_BLACK);
		ST7735_WriteString1(20, 60, "to DUEL", Font_11x18, ST7735_CYAN, ST7735_BLACK);
		ST7735_WriteString1(0, 75, "__________________", Font_7x10, ST7735_WHITE, ST7735_BLACK);
		ST7735_WriteString1(0, 90, "Initial LP: ", Font_7x10, ST7735_WHITE, ST7735_BLACK);
		ST7735_WriteString1(93, 90, "4000", Font_7x10, ST7735_GREEN, ST7735_BLACK);
		ST7735_WriteString1(0,105, "Initial Timer: ", Font_7x10, ST7735_WHITE, ST7735_BLACK);
		ST7735_WriteString1(100, 105, "180", Font_7x10, ST7735_GREEN, ST7735_BLACK);
		//END display for ready state
		if (HAL_GPIO_ReadPin(START_BUTTON_PORT, START_BUTTON_PIN)
				== GPIO_PIN_RESET) {
			ST7735_FillScreen(ST7735_BLACK);
			ST7735_FillScreen1(ST7735_BLACK);
			state_game->MainGame_State = Game_init;
		}
		else
		{
			state_game->PlyerAction_State = PS_AFK;
			state_game->PlyerAction_Main_Substate = PMS_AFK;
			state_game->PlyerAction_Battle_Substate = PBS_AFK;
			state_game->PlyerAction_Chain_Substate = PCS_AFK;
		}
		break;
	case Game_init:
		ST7735_WriteString(5, 5, "Player 2: ", Font_7x10, ST7735_WHITE, ST7735_BLACK);
		ST7735_WriteString(0, 15, "__________________", Font_7x10, ST7735_WHITE, ST7735_BLACK);
		ST7735_WriteString(25, 35, "You are", Font_11x18, ST7735_MAGENTA, ST7735_BLACK);
		ST7735_WriteString(33, 60, "SECOND", Font_11x18, ST7735_CYAN, ST7735_BLACK);
		ST7735_WriteString(0, 75, "__________________", Font_7x10, ST7735_WHITE, ST7735_BLACK);
		ST7735_WriteString1(5, 5, "Player 1: ", Font_7x10, ST7735_WHITE, ST7735_BLACK);
		ST7735_WriteString1(0, 15, "__________________", Font_7x10, ST7735_WHITE, ST7735_BLACK);
		ST7735_WriteString1(25, 35, "You are", Font_11x18, ST7735_MAGENTA, ST7735_BLACK);
		ST7735_WriteString1(37, 60, "FIRST", Font_11x18, ST7735_CYAN, ST7735_BLACK);
		ST7735_WriteString1(0, 75, "__________________", Font_7x10, ST7735_WHITE, ST7735_BLACK);
		HAL_Delay(3000);
		ST7735_FillScreen(ST7735_BLACK);
		ST7735_FillScreen1(ST7735_BLACK);
		MainGUI();

		// Reset Player
		ptrPlayer1->life_point = 4000;
		ptrPlayer2->life_point = 4000;
		YUGIOH_Clear_Card_All(ptrPlayer1);
		YUGIOH_Clear_Card_All(ptrPlayer2);

		state_game->MainGame_State = first_player;
		state_game->PlyerAction_State = Drawn_Phase;
		break;
	case first_player:
		if (state_game -> F_flag == 0){
			ST7735_WriteString1(15, 90, "Your TURN", Font_11x18, ST7735_YELLOW, ST7735_BLACK);
			HAL_Delay(2000);
			ST7735_FillRectangle1(0, 90, 128,128-90,ST7735_BLACK);
			state_game->F_flag += 1;
			state_game->S_flag = 0;
			turn += 1;
		}
		ptrPlayerAtk = &state_game->player[0];
		ptrPlayerDef = &state_game->player[1];
		GAME_PLAY_Phase_Management(RFIDmain,state_game,ptrPlayerAtk,ptrPlayerDef);
		break;
	case second_player:
		if (state_game -> S_flag == 0){
			ST7735_WriteString(15, 90, "Your TURN", Font_11x18, ST7735_YELLOW, ST7735_BLACK);
			HAL_Delay(2000);
			ST7735_FillRectangle(0, 90, 128,128-90,ST7735_BLACK);
			state_game->S_flag += 1;
			state_game->F_flag = 0;
			turn += 1;
		}
		ptrPlayerAtk = &state_game->player[1];
		ptrPlayerDef = &state_game->player[0];
		GAME_PLAY_Phase_Management(RFIDmain,state_game,ptrPlayerAtk,ptrPlayerDef);
		break;
	case Game_Ended:
		ST7735_FillRectangleNSS(0, 90, 128, 128 - 90, ST7735_BLACK,1);
		ST7735_FillRectangleNSS(0, 90, 128, 128 - 90, ST7735_BLACK,0);
		HAL_Delay(200);
		ST7735_WriteStringNSS(20, 90, "YOU WIN", Font_11x18, ST7735_MAGENTA, ST7735_BLACK,1);
		ST7735_WriteStringNSS(15, 90, "YOU LOSE", Font_11x18, ST7735_RED, ST7735_BLACK,0);
		HAL_Delay(200);
		break;
	}
}

void GAME_PLAY_Phase_Management(RFIDHandle *RFIDmain,State_game *state_game,Player *playerAtk,Player *playerDef)
{
	enum _STATE {
		PS_AFK,Drawn_Phase, Main_Phase, Battle_Phase
	} PAS;
	PAS = state_game->PlyerAction_State;

	enum _MAIN {
		PMS_AFK, PMS_ActionAwait, select_position, check_card_type, advance_summon, activate_effect,chaining_main_ATK,chaining_main_DEF
	} PMS;
	PMS = state_game->PlyerAction_Main_Substate;

	enum _BATTLE {
		PBS_AFK, PBS_ActionAwait, counter_ATK,counter_DEF, chain_effect,calculate_damage, after_calculate
	} PBS;
	PBS = state_game->PlyerAction_Battle_Substate;

	// Card PTR
	YUGIOH_Card *ptrYugiohCard_src;
	YUGIOH_Card *ptrYugiohCard_dst;

	Player **ptrUser;
	Player **ptrOpponent;

	switch (PAS) {
	case PS_AFK:
		break;
	case Drawn_Phase:
		HAL_TIM_Base_Start_IT(&TIM7_PORT);
		LCDvalue(playerAtk,playerDef);
		ST7735_WriteString(110, 35, "DP", Font_7x10, ST7735_CYAN, ST7735_BLACK);
		ST7735_WriteString1(110, 35, "DP", Font_7x10, ST7735_CYAN, ST7735_BLACK);
		ST7735_WriteStringNSS(5, 90, "Wait a minute", Font_7x10, ST7735_WHITE, ST7735_BLACK,playerDef->displayNSS);
		ST7735_WriteStringNSS(5, 90, "Draw a card", Font_7x10, ST7735_WHITE, ST7735_BLACK,playerAtk->displayNSS);
		// END display for draw phase state
		if (HAL_GPIO_ReadPin(TURN_BUTTON_PORT, TURN_BUTTON_PIN)
				== GPIO_PIN_RESET) {
			ST7735_FillScreen(ST7735_BLACK);
			ST7735_FillScreen1(ST7735_BLACK);
			MainGUI();
			state_game->PlyerAction_State = Main_Phase;

			// Wait for card to be read State = 0 Mean AFK
			state_game->PlyerAction_Main_Substate = PMS_ActionAwait;
			state_game->action = 0;
			HAL_Delay(1000);
			ST7735_WriteStringNSS(5, 90, "Wait a minute", Font_7x10, ST7735_WHITE, ST7735_BLACK,playerDef->displayNSS);
			ST7735_WriteStringNSS(5, 90, "Time to PLAY", Font_7x10, ST7735_WHITE, ST7735_BLACK,playerAtk->displayNSS);
		}
		break;
	case Main_Phase:
		LCDvalue(playerAtk,playerDef);

		switch (PMS)
		{
		case PMS_AFK:
			break;
		case PMS_ActionAwait:

			if (state_game->action == 0)
			{
				ST7735_WriteString(110, 35, "MP", Font_7x10, ST7735_GREEN, ST7735_BLACK);
				ST7735_WriteString1(110, 35, "MP", Font_7x10, ST7735_GREEN, ST7735_BLACK);
				//    ST7735_WriteStringNSS(5, 90, "Wait a minute", Font_7x10, ST7735_WHITE, ST7735_BLACK,playerDef->displayNSS);
				//    ST7735_WriteStringNSS(5, 90, "Place a card", Font_7x10, ST7735_WHITE, ST7735_BLACK,playerAtk->displayNSS);
				// Reading Until RFID action = 1 Mean Card Detected
				if(HAL_GPIO_ReadPin(TURN_BUTTON_PORT, TURN_BUTTON_PIN)
						== GPIO_PIN_RESET){
					ST7735_FillScreen(ST7735_BLACK);
					ST7735_FillScreen1(ST7735_BLACK);
					MainGUI();
					state_game->action = 50;
					state_game->PlyerAction_Main_Substate = PMS_AFK;
					YUGIOH_Monster_Activated(playerAtk);
					state_game->PlyerAction_State = Battle_Phase;
					state_game->PlyerAction_Battle_Substate = PBS_ActionAwait;
					HAL_Delay(1000);
					ST7735_WriteStringNSS(5, 90, "Time to defend", Font_7x10, ST7735_WHITE, ST7735_BLACK, playerDef->displayNSS);
					ST7735_WriteStringNSS(5, 90, "Time to battle", Font_7x10, ST7735_WHITE, ST7735_BLACK, playerAtk->displayNSS);
				}
				Player_Reading_Card(RFIDmain,state_game,playerAtk);
			}
			else if (state_game->action == 1)
			{
				// Reset Chain count
				ST7735_FillRectangleNSS(0, 90, 128, 128 - 90, ST7735_BLACK,playerAtk->displayNSS);
				state_game->ChainCount = 0;
				state_game->PlyerAction_Main_Substate = select_position;
			}
			break;
		case select_position:

			ST7735_WriteStringNSS(5, 90, "select position", Font_7x10, ST7735_WHITE, ST7735_BLACK,playerAtk->displayNSS);
			// Current state_game->action = 1
			ptrYugiohCard_src = &playerAtk->ActtionBuffer[0];
			ptrYugiohCard_dst = &playerAtk->CardInPlayed;

			if (state_game->action == 1)
			{
				if (playerAtk->noBTN == GPIO_PIN_RESET) {
					ptrYugiohCard_src->cardState = 0;
					state_game->action = 2;
				}
				else if(playerAtk->yesBTN == GPIO_PIN_RESET) {
					///
					uint8_t idxC = YUGIOH_Check_Spell_On_board(playerAtk, ptrYugiohCard_src);
					///
					if (idxC != 255) {
						YUGIOH_card_clear(&playerAtk->cardOnBoard[idxC]);
					}
					ptrYugiohCard_src->cardState = 1;
					state_game->action = 2;
				}

			}
			else if (state_game->action == 2)
			{
				ST7735_FillRectangleNSS(0, 90, 128, 128 - 90, ST7735_BLACK,playerAtk->displayNSS);
				ST7735_FillRectangleNSS(0, 90, 128, 128 - 90, ST7735_BLACK,playerDef->displayNSS);
				YUGIOH_card_copy(ptrYugiohCard_src, ptrYugiohCard_dst);
				state_game->action = 3;
				state_game->PlyerAction_Main_Substate = check_card_type;
			}
			break;
		case check_card_type:
			// Current state_game->action = 3

			ptrYugiohCard_src = &playerAtk->CardInPlayed;

			if (state_game->action == 3)
			{
				if (ptrYugiohCard_src->cardType == 3)
				{
					ST7735_WriteStringNSS(5, 90, "You set a card", Font_7x10, ST7735_WHITE, ST7735_BLACK,playerAtk->displayNSS);
					ST7735_WriteStringNSS(5, 90, "Opponent set", Font_7x10, ST7735_WHITE, ST7735_BLACK,playerDef->displayNSS);
					ST7735_WriteStringNSS(5, 105, "a card", Font_7x10, ST7735_WHITE, ST7735_BLACK,playerDef->displayNSS);

					ptrYugiohCard_src->cardState = 0;

					// Add card to board
					uint8_t idx = ptrYugiohCard_src->standPosition % 6;
					ptrYugiohCard_dst = &playerAtk->cardOnBoard[idx];

					YUGIOH_card_copy(ptrYugiohCard_src, ptrYugiohCard_dst);

					YUGIOH_Clear_Card_Bufffer_Player(playerAtk);
					YUGIOH_card_copy(&playerAtk->ActtionBuffer[0], ptrYugiohCard_src);

					state_game->action = 0;
					state_game->PlyerAction_Main_Substate = PMS_ActionAwait;

				}
				else if (ptrYugiohCard_src->cardType == 2)
				{
					//check if activate now or just set
					state_game->count_chain = 1;
					if(ptrYugiohCard_src->cardState == 1){
						ST7735_WriteStringNSS(5, 90, "Opponent chain a card", Font_7x10, ST7735_WHITE, ST7735_BLACK,playerAtk->displayNSS);
						ST7735_WriteStringNSS(5, 90, "You chain a card", Font_7x10, ST7735_WHITE, ST7735_BLACK,playerDef->displayNSS);
						//ST7735_FillRectangleNSS(0, 90, 128, 128 - 90, ST7735_BLACK,playerDef->displayNSS);

						state_game->action = 4;

						// Add card + Player to chain
						YUGIOH_card_Buffer_Update_Chain(state_game);
						YUGIOH_card_copy(ptrYugiohCard_src, &state_game->ChainBuffer[0]);
						state_game->ptrChainUser[0] = playerAtk;
						state_game->ptrChainOpponent[0] = playerDef;
						state_game->ChainCount++;
						ST7735_FillRectangleNSS(0, 90, 128, 128 - 90, ST7735_BLACK,playerAtk->displayNSS);
						ST7735_FillRectangleNSS(0, 90, 128, 128 - 90, ST7735_BLACK,playerDef->displayNSS);
						YUGIOH_Clear_Card_Bufffer_Player(playerAtk);
						YUGIOH_card_copy(&playerAtk->ActtionBuffer[0], ptrYugiohCard_src);

						state_game->PlyerAction_Main_Substate = chaining_main_DEF;
					}
					else{

						ST7735_WriteStringNSS(5, 90, "You set a card", Font_7x10, ST7735_WHITE, ST7735_BLACK,playerAtk->displayNSS);
						ST7735_WriteStringNSS(5, 90, "Opponent set", Font_7x10, ST7735_WHITE, ST7735_BLACK,playerDef->displayNSS);
						ST7735_WriteStringNSS(5, 105, "a card", Font_7x10, ST7735_WHITE, ST7735_BLACK,playerDef->displayNSS);
						// Add card to board
						uint8_t idx = ptrYugiohCard_src->standPosition % 6;
						ptrYugiohCard_dst = &playerAtk->cardOnBoard[idx];

						YUGIOH_card_copy(ptrYugiohCard_src, ptrYugiohCard_dst);

						YUGIOH_Clear_Card_Bufffer_Player(playerAtk);
						YUGIOH_card_copy(&playerAtk->ActtionBuffer[0], ptrYugiohCard_src);

						state_game->action = 0;
						state_game->PlyerAction_Main_Substate = PMS_ActionAwait;
					}
				}
				else if (ptrYugiohCard_src->cardType == 1)
				{
					state_game->test = 2;
					if (ptrYugiohCard_src->cardLevel < 7)
					{
						ST7735_WriteStringNSS(5, 90, "You summon", Font_7x10, ST7735_WHITE, ST7735_BLACK,playerAtk->displayNSS);
						ST7735_WriteStringNSS(5, 105, "a MONSTER", Font_7x10, ST7735_YELLOW, ST7735_BLACK,playerAtk->displayNSS);

						ST7735_WriteStringNSS(5, 90, "Opponent summon", Font_7x10, ST7735_WHITE, ST7735_BLACK,playerDef->displayNSS);
						ST7735_WriteStringNSS(5, 105, "a MONSTER", Font_7x10, ST7735_YELLOW, ST7735_BLACK,playerDef->displayNSS);

						state_game->test = 1;
						// Add card to board
						uint8_t idx = ptrYugiohCard_src->standPosition % 6;
						ptrYugiohCard_dst = &playerAtk->cardOnBoard[idx];

						YUGIOH_card_copy(ptrYugiohCard_src, ptrYugiohCard_dst);

						YUGIOH_Clear_Card_Bufffer_Player(playerAtk);
						YUGIOH_card_copy(&playerAtk->ActtionBuffer[0], ptrYugiohCard_src);

						state_game->action = 0;
						state_game->PlyerAction_Main_Substate = PMS_ActionAwait;
					}
					else
					{


						YUGIOH_Clear_Card_Bufffer_Player(playerAtk);
						state_game->action = 4;
						state_game->PlyerAction_Main_Substate = advance_summon;
					}
				}
			}
			break;
		case advance_summon:
			// Current state_game->action = 4

			ptrYugiohCard_src = &playerAtk->CardInPlayed;

			if (state_game->action == 4)
			{
				// Reading Until RFID action += 1 Mean Card Detected
				Player_Reading_Card(RFIDmain,state_game,playerAtk);
			}
			else if (state_game->action == 5)
			{
				// Reading Until RFID action += 1 Mean Card Detected
				Player_Reading_Card(RFIDmain,state_game,playerAtk);
			}
			else if (state_game->action == 6)
			{
				uint8_t monsterflag = 0 ;
				ptrYugiohCard_dst = &playerAtk->ActtionBuffer[0];
				if (ptrYugiohCard_dst->cardLevel <= 4)
				{
					monsterflag += 1;
				}
				ptrYugiohCard_dst++;
				if (ptrYugiohCard_dst->cardLevel <= 4)
				{
					monsterflag += 1;
				}

				if (monsterflag == 2)
				{
					// Add card to board
					uint8_t idx = ptrYugiohCard_src->standPosition % 6;
					ptrYugiohCard_dst = &playerAtk->cardOnBoard[idx];

					YUGIOH_card_copy(ptrYugiohCard_src, ptrYugiohCard_dst);

					YUGIOH_To_GY(playerAtk,&playerAtk->ActtionBuffer[0]);
					YUGIOH_To_GY(playerAtk,&playerAtk->ActtionBuffer[1]);

					YUGIOH_Clear_Card_Bufffer_Player(playerAtk);
					YUGIOH_card_copy(&playerAtk->ActtionBuffer[0], ptrYugiohCard_src);

					state_game->action = 0;
					state_game->PlyerAction_Main_Substate = PMS_ActionAwait;
				}
				else
				{
					YUGIOH_Clear_Card_Bufffer_Player(playerAtk);
					YUGIOH_card_copy(&playerAtk->ActtionBuffer[0], &playerAtk->CardInPlayed);
				}
			}

			ST7735_WriteStringNSS(5, 90, "You summon", Font_7x10, ST7735_WHITE, ST7735_BLACK,playerAtk->displayNSS);
			ST7735_WriteStringNSS(5, 105, "a MONSTER", Font_7x10, ST7735_YELLOW, ST7735_BLACK,playerAtk->displayNSS);

			ST7735_WriteStringNSS(5, 90, "Opponent summon", Font_7x10, ST7735_WHITE, ST7735_BLACK,playerDef->displayNSS);
			ST7735_WriteStringNSS(5, 105, "a MONSTER", Font_7x10, ST7735_YELLOW, ST7735_BLACK,playerDef->displayNSS);
			break;
		case chaining_main_DEF:

			// Current state_game->action = 4

			ST7735_WriteStringNSS(5, 90, "Waiting Opponent Action", Font_7x10, ST7735_WHITE, ST7735_BLACK,playerAtk->displayNSS);
			ST7735_WriteStringNSS(5, 90, "Do you want to chain card ?", Font_7x10, ST7735_WHITE, ST7735_BLACK,playerDef->displayNSS);
			if(state_game->action == 4)
			{

				if (playerDef->noBTN == GPIO_PIN_RESET){
					ST7735_FillRectangleNSS(0, 90, 128, 128 - 90, ST7735_BLACK,playerDef->displayNSS);
					state_game->PlyerAction_Main_Substate = activate_effect;
					state_game->count_chain = 0;
				}
				Player_Reading_Card(RFIDmain,state_game,playerDef);
			}
			else if ((state_game->action == 5 )){
				ST7735_FillRectangleNSS(0, 90, 128, 128 - 90, ST7735_BLACK,playerAtk->displayNSS);
				ptrYugiohCard_src = &playerDef->ActtionBuffer[0];

				uint8_t idx = YUGIOH_Check_Trap_On_board(playerDef, ptrYugiohCard_src);

				if (idx != 255)
				{
					YUGIOH_card_Buffer_Update_Chain(state_game);
					ptrYugiohCard_dst = &playerDef->cardOnBoard[idx];
					ptrYugiohCard_dst->actionPoint_Eff = 0; //Trap is now use
					YUGIOH_card_copy(ptrYugiohCard_dst, &state_game->ChainBuffer[0]);
					state_game->ptrChainUser[0] = playerDef;
					state_game->ptrChainOpponent[0] = playerAtk;
					state_game->ChainCount++;

					state_game->PlyerAction_Main_Substate = chaining_main_ATK;
					state_game->action = 4;
				}
				else
				{
					//display this is not trap card
					state_game->action = 4;
				}
			}
			break;
		case chaining_main_ATK:
			ST7735_WriteStringNSS(5, 90, "Do you want to chain card ?", Font_7x10, ST7735_WHITE, ST7735_BLACK,playerAtk->displayNSS);
			ST7735_WriteStringNSS(5, 90, "Waiting Opponent Action", Font_7x10, ST7735_WHITE, ST7735_BLACK,playerDef->displayNSS);
			if(state_game->action == 4)
			{
				Player_Reading_Card(RFIDmain,state_game,playerAtk);
				if (playerAtk->noBTN == GPIO_PIN_RESET){
					ST7735_FillRectangleNSS(0, 90, 128, 128 - 90, ST7735_BLACK,playerAtk->displayNSS);
					ST7735_FillRectangleNSS(0, 90, 128, 128 - 90, ST7735_BLACK,playerDef->displayNSS);
					state_game->PlyerAction_Main_Substate = activate_effect;
					state_game->count_chain = 0;
				}
			}
			else if ((state_game->action == 5 )){
				ptrYugiohCard_src = &playerAtk->ActtionBuffer[0];

				uint8_t idx = YUGIOH_Check_Trap_On_board(playerAtk, ptrYugiohCard_src);

				if (idx != 255)
				{
					YUGIOH_card_Buffer_Update_Chain(state_game);
					ptrYugiohCard_dst = &playerAtk->cardOnBoard[idx];
					ptrYugiohCard_dst->actionPoint_Eff = 0; //Trap is now use
					YUGIOH_card_copy(ptrYugiohCard_dst, &state_game->ChainBuffer[0]);
					state_game->ptrChainUser[0] = playerAtk;
					state_game->ptrChainOpponent[0] = playerDef;
					state_game->ChainCount++;

					state_game->PlyerAction_Main_Substate = chaining_main_ATK;
					state_game->action = 4;
				}
				else
				{
					//display this is not trap card
					state_game->action = 4;
				}
			}
			break;
		case activate_effect:
			ST7735_WriteStringNSS(5, 90, "You activate", Font_7x10, ST7735_WHITE, ST7735_BLACK,playerAtk->displayNSS);
			ST7735_WriteStringNSS(5, 105, "a spell card", Font_7x10, ST7735_GREEN, ST7735_BLACK,playerAtk->displayNSS);
			ST7735_WriteStringNSS(5, 90, "Opponent activate", Font_7x10, ST7735_WHITE, ST7735_BLACK,playerDef->displayNSS);
			ST7735_WriteStringNSS(5, 105, "a spell card", Font_7x10, ST7735_GREEN, ST7735_BLACK,playerDef->displayNSS);
			// Current Action = 4
			ptrUser = &state_game->ptrChainUser[state_game->count_chain];
			ptrOpponent = &state_game->ptrChainOpponent[state_game->count_chain];

			if(state_game->action == 4)
			{
				// Base use to check Card Eff
				if (state_game->count_chain < state_game->ChainCount)
				{
					state_game->test = 165;

					ptrYugiohCard_src = &state_game->ChainBuffer[state_game->count_chain];

					if (ptrYugiohCard_src->cardSignature == 11)
					{
						YUGIOH_Clear_Card_Enemy_Player_Raigeki(*ptrOpponent);
						YUGIOH_To_GY(*ptrUser, ptrYugiohCard_src);
						state_game->count_chain++;
						ST7735_WriteStringNSS(5, 90, "Activated spell", Font_7x10, ST7735_WHITE, ST7735_BLACK,(*ptrUser)->displayNSS);
						ST7735_WriteStringNSS(5, 105, "Raigeki", Font_7x10, ST7735_GREEN, ST7735_BLACK,(*ptrUser)->displayNSS);
						ST7735_WriteStringNSS(5, 90, "Opponent chain", Font_7x10, ST7735_WHITE, ST7735_BLACK,(*ptrOpponent)->displayNSS);
						ST7735_WriteStringNSS(5, 105, "a spell card", Font_7x10, ST7735_GREEN, ST7735_BLACK,(*ptrOpponent)->displayNSS);
						HAL_Delay(1000);
						ST7735_FillRectangleNSS(0, 90, 128, 128 - 90, ST7735_BLACK,(*ptrUser)->displayNSS);
						ST7735_FillRectangleNSS(0, 90, 128, 128 - 90, ST7735_BLACK,(*ptrOpponent)->displayNSS);
					}
					else if (ptrYugiohCard_src->cardSignature == 12)
					{
						YUGIOH_Clear_Card_Enemy_Player_Dark_Hole(*ptrUser,*ptrOpponent);
						YUGIOH_To_GY(*ptrUser, ptrYugiohCard_src);
						state_game->count_chain++;
						ST7735_WriteStringNSS(5, 90, "Activated spell", Font_7x10, ST7735_WHITE, ST7735_BLACK,(*ptrUser)->displayNSS);
						ST7735_WriteStringNSS(5, 105, "Dark Hole", Font_7x10, ST7735_GREEN, ST7735_BLACK,(*ptrUser)->displayNSS);
						ST7735_WriteStringNSS(5, 90, "Opponent chain", Font_7x10, ST7735_WHITE, ST7735_BLACK,(*ptrOpponent)->displayNSS);
						ST7735_WriteStringNSS(5, 105, "a spell card", Font_7x10, ST7735_GREEN, ST7735_BLACK,(*ptrOpponent)->displayNSS);
						HAL_Delay(1000);
						ST7735_FillRectangleNSS(0, 90, 128, 128 - 90, ST7735_BLACK,(*ptrUser)->displayNSS);
						ST7735_FillRectangleNSS(0, 90, 128, 128 - 90, ST7735_BLACK,(*ptrOpponent)->displayNSS);
					}
					else if (ptrYugiohCard_src->cardSignature == 13)
					{
						state_game->test = 166;
						state_game->action = 5;

					}
					else if(ptrYugiohCard_src->cardSignature == 14 || ptrYugiohCard_src->cardSignature == 15)
					{
						state_game->test = 133;
						YUGIOH_Gift_of_the_Mystical_Elf(*ptrUser,*ptrOpponent);
						YUGIOH_To_GY(*ptrUser, ptrYugiohCard_src);
						state_game->count_chain++;
						ST7735_WriteStringNSS(5, 90, "Act:Gift of the", Font_7x10, ST7735_WHITE, ST7735_BLACK,(*ptrUser)->displayNSS);
						ST7735_WriteStringNSS(5, 105, "Mystical Elf", Font_7x10, ST7735_MAGENTA, ST7735_BLACK,(*ptrUser)->displayNSS);
						ST7735_WriteStringNSS(5, 90, "Opponent chain", Font_7x10, ST7735_WHITE, ST7735_BLACK,(*ptrOpponent)->displayNSS);
						ST7735_WriteStringNSS(5, 105, "a trap card", Font_7x10, ST7735_MAGENTA, ST7735_BLACK,(*ptrOpponent)->displayNSS);
						HAL_Delay(1000);
						ST7735_FillRectangleNSS(0, 90, 128, 128 - 90, ST7735_BLACK,(*ptrUser)->displayNSS);
						ST7735_FillRectangleNSS(0, 90, 128, 128 - 90, ST7735_BLACK,(*ptrOpponent)->displayNSS);

					}
					else if(ptrYugiohCard_src->cardSignature == 16)
					{
						state_game->test = 124;
						state_game->action = 5;
					}
					else if(ptrYugiohCard_src->cardSignature == 17)
					{
						state_game->action = 5;
					}
					else if((ptrYugiohCard_src->cardSignature == 18)||(ptrYugiohCard_src->cardSignature == 19))
					{
						if ((*ptrUser)->yesBTN == GPIO_PIN_RESET){
							YUGIOH_To_GY(*ptrUser, ptrYugiohCard_src);
							state_game->count_chain++;
						}
					}
				}
				else
				{
					ST7735_FillRectangleNSS(0, 90, 128, 128 - 90, ST7735_BLACK,playerAtk->displayNSS);
					ST7735_FillRectangleNSS(0, 90, 128, 128 - 90, ST7735_BLACK,playerDef->displayNSS);
					// All Chain Clear
					state_game->action = 0;
					state_game->PlyerAction_Main_Substate = PMS_ActionAwait;
				}

			}
			else if (state_game->action == 5)
			{
				state_game->test = 167;
				ptrYugiohCard_src = &state_game->ChainBuffer[state_game->count_chain];

				if((*ptrUser)->noBTN == GPIO_PIN_RESET){
					YUGIOH_Clear_Card_Bufffer_Player(*ptrUser);
					YUGIOH_card_copy(&(*ptrUser)->ActtionBuffer[0],&(*ptrUser)->CardInPlayed);
					state_game->action = 4;
					state_game->count_chain++;
				}
				if (ptrYugiohCard_src->cardSignature == 13)
				{
					Player_Reading_Card_Reborn(RFIDmain, state_game, *ptrUser);
				}
				else if(ptrYugiohCard_src->cardSignature == 16){
					Player_Reading_Card_Ancient_Rule(RFIDmain, state_game, *ptrUser);
				}
				else if (ptrYugiohCard_src->cardSignature == 17)
				{
					Player_Reading_Card_Mainphase(RFIDmain, state_game, *ptrOpponent);
				}
				else
				{	Player_Reading_Card(RFIDmain, state_game, *ptrUser);}
			}
			else if (state_game->action == 6)
			{
				state_game->test = 169;

				ptrYugiohCard_src = &state_game->ChainBuffer[state_game->count_chain];

				if (ptrYugiohCard_src->cardSignature == 13){

					// Reborn
					ST7735_WriteStringNSS(5, 90, "select position", Font_7x10, ST7735_WHITE, ST7735_BLACK,(*ptrUser)->displayNSS);

					if ((*ptrUser)->noBTN == GPIO_PIN_RESET){
						ST7735_FillRectangleNSS(0, 90, 128, 128 - 90, ST7735_BLACK,(*ptrUser)->displayNSS);
						state_game->test = 170;
						ptrYugiohCard_dst = &(*ptrUser)->ActtionBuffer[0];
						ptrYugiohCard_dst->cardState = 0;
						YUGIOH_card_copy(ptrYugiohCard_dst, &(*ptrUser)->CardInPlayed);
						YUGIOH_Reborn(*ptrUser);
						state_game->action = 4;
						YUGIOH_To_GY(*ptrUser, ptrYugiohCard_src);
						state_game->count_chain++;
						ST7735_WriteStringNSS(5, 90, "Activated spell", Font_7x10, ST7735_WHITE, ST7735_BLACK,(*ptrUser)->displayNSS);
						ST7735_WriteStringNSS(5, 105, "Monster reborn", Font_7x10, ST7735_GREEN, ST7735_BLACK,(*ptrUser)->displayNSS);
						ST7735_WriteStringNSS(5, 90, "Opponent chain", Font_7x10, ST7735_WHITE, ST7735_BLACK,(*ptrOpponent)->displayNSS);
						ST7735_WriteStringNSS(5, 105, "a spell card", Font_7x10, ST7735_GREEN, ST7735_BLACK,(*ptrOpponent)->displayNSS);
						HAL_Delay(1000);
						ST7735_FillRectangleNSS(0, 90, 128, 128 - 90, ST7735_BLACK,(*ptrUser)->displayNSS);
						ST7735_FillRectangleNSS(0, 90, 128, 128 - 90, ST7735_BLACK,(*ptrOpponent)->displayNSS);
					}
					else if ((*ptrUser)->yesBTN == GPIO_PIN_RESET)
					{
						ST7735_FillRectangleNSS(0, 90, 128, 128 - 90, ST7735_BLACK,(*ptrUser)->displayNSS);
						state_game->test = 171;
						ptrYugiohCard_dst = &(*ptrUser)->ActtionBuffer[0];
						ptrYugiohCard_dst->cardState = 1;
						YUGIOH_card_copy(ptrYugiohCard_dst, &(*ptrUser)->CardInPlayed);
						YUGIOH_Reborn(*ptrUser);
						state_game->action = 4;
						YUGIOH_To_GY(*ptrUser, ptrYugiohCard_src);
						state_game->count_chain++;
						ST7735_WriteStringNSS(5, 90, "Activated spell", Font_7x10, ST7735_WHITE, ST7735_BLACK,(*ptrUser)->displayNSS);
						ST7735_WriteStringNSS(5, 105, "Monster reborn", Font_7x10, ST7735_GREEN, ST7735_BLACK,(*ptrUser)->displayNSS);
						ST7735_WriteStringNSS(5, 90, "Opponent chain", Font_7x10, ST7735_WHITE, ST7735_BLACK,(*ptrOpponent)->displayNSS);
						ST7735_WriteStringNSS(5, 105, "a spell card", Font_7x10, ST7735_GREEN, ST7735_BLACK,(*ptrOpponent)->displayNSS);
						HAL_Delay(1000);
						ST7735_FillRectangleNSS(0, 90, 128, 128 - 90, ST7735_BLACK,(*ptrUser)->displayNSS);
						ST7735_FillRectangleNSS(0, 90, 128, 128 - 90, ST7735_BLACK,(*ptrOpponent)->displayNSS);
					}
				}
				else if(ptrYugiohCard_src->cardSignature == 16){

					//					YUGIOH_Ancient_Rules(*ptrUser);
					//					state_game->action = 4;
					//					YUGIOH_To_GY(*ptrUser, ptrYugiohCard_src);
					//					state_game->count_chain++;
					//					ST7735_WriteStringNSS(5, 90, "Activated spell", Font_7x10, ST7735_WHITE, ST7735_BLACK,(*ptrUser)->displayNSS);
					//					ST7735_WriteStringNSS(5, 105, "Ancient Rules", Font_7x10, ST7735_GREEN, ST7735_BLACK,(*ptrUser)->displayNSS);
					//					ST7735_WriteStringNSS(5, 90, "Opponent chain", Font_7x10, ST7735_WHITE, ST7735_BLACK,(*ptrOpponent)->displayNSS);
					//					ST7735_WriteStringNSS(5, 105, "a spell card", Font_7x10, ST7735_GREEN, ST7735_BLACK,(*ptrOpponent)->displayNSS);
					//					HAL_Delay(1000);
					//					ST7735_FillRectangleNSS(0, 90, 128, 128 - 90, ST7735_BLACK,(*ptrUser)->displayNSS);
					//					ST7735_FillRectangleNSS(0, 90, 128, 128 - 90, ST7735_BLACK,(*ptrOpponent)->displayNSS);
					ST7735_WriteStringNSS(5, 90, "select position", Font_7x10, ST7735_WHITE, ST7735_BLACK,(*ptrUser)->displayNSS);

					if ((*ptrUser)->noBTN == GPIO_PIN_RESET){
						ST7735_FillRectangleNSS(0, 90, 128, 128 - 90, ST7735_BLACK,(*ptrUser)->displayNSS);
						state_game->test = 170;
						ptrYugiohCard_dst = &(*ptrUser)->ActtionBuffer[0];
						ptrYugiohCard_dst->cardState = 0;
						YUGIOH_card_copy(ptrYugiohCard_dst, &(*ptrUser)->CardInPlayed);
						YUGIOH_Ancient_Rules(*ptrUser);
						state_game->action = 4;
						YUGIOH_To_GY(*ptrUser, ptrYugiohCard_src);
						state_game->count_chain++;
						ST7735_WriteStringNSS(5, 90, "Activated spell", Font_7x10, ST7735_WHITE, ST7735_BLACK,(*ptrUser)->displayNSS);
						ST7735_WriteStringNSS(5, 105, "Monster reborn", Font_7x10, ST7735_GREEN, ST7735_BLACK,(*ptrUser)->displayNSS);
						ST7735_WriteStringNSS(5, 90, "Opponent chain", Font_7x10, ST7735_WHITE, ST7735_BLACK,(*ptrOpponent)->displayNSS);
						ST7735_WriteStringNSS(5, 105, "a spell card", Font_7x10, ST7735_GREEN, ST7735_BLACK,(*ptrOpponent)->displayNSS);
						HAL_Delay(1000);
						ST7735_FillRectangleNSS(0, 90, 128, 128 - 90, ST7735_BLACK,(*ptrUser)->displayNSS);
						ST7735_FillRectangleNSS(0, 90, 128, 128 - 90, ST7735_BLACK,(*ptrOpponent)->displayNSS);
					}
					else if ((*ptrUser)->yesBTN == GPIO_PIN_RESET)
					{
						ST7735_FillRectangleNSS(0, 90, 128, 128 - 90, ST7735_BLACK,(*ptrUser)->displayNSS);
						state_game->test = 171;
						ptrYugiohCard_dst = &(*ptrUser)->ActtionBuffer[0];
						ptrYugiohCard_dst->cardState = 1;
						YUGIOH_card_copy(ptrYugiohCard_dst, &(*ptrUser)->CardInPlayed);
						YUGIOH_Ancient_Rules(*ptrUser);
						// Do Something EFF
						Test_EFF(*ptrUser,*ptrOpponent);
						state_game->action = 4;
						YUGIOH_To_GY(*ptrUser, ptrYugiohCard_src);
						state_game->count_chain++;
						ST7735_WriteStringNSS(5, 90, "Activated spell", Font_7x10, ST7735_WHITE, ST7735_BLACK,(*ptrUser)->displayNSS);
						ST7735_WriteStringNSS(5, 105, "Monster reborn", Font_7x10, ST7735_GREEN, ST7735_BLACK,(*ptrUser)->displayNSS);
						ST7735_WriteStringNSS(5, 90, "Opponent chain", Font_7x10, ST7735_WHITE, ST7735_BLACK,(*ptrOpponent)->displayNSS);
						ST7735_WriteStringNSS(5, 105, "a spell card", Font_7x10, ST7735_GREEN, ST7735_BLACK,(*ptrOpponent)->displayNSS);
						HAL_Delay(1000);
						ST7735_FillRectangleNSS(0, 90, 128, 128 - 90, ST7735_BLACK,(*ptrUser)->displayNSS);
						ST7735_FillRectangleNSS(0, 90, 128, 128 - 90, ST7735_BLACK,(*ptrOpponent)->displayNSS);
					}
				}
				else if(ptrYugiohCard_src->cardSignature == 17){

					ptrYugiohCard_dst = &(*ptrOpponent)->ActtionBuffer[0];
					YUGIOH_card_copy(ptrYugiohCard_dst,  &(*ptrUser)->ActtionBuffer[0]);
					YUGIOH_Stop_Defense(*ptrUser,*ptrOpponent);
					state_game->action = 4;
					YUGIOH_To_GY(*ptrUser, ptrYugiohCard_src);
					state_game->count_chain++;
					ST7735_WriteStringNSS(5, 90, "Activated spell", Font_7x10, ST7735_WHITE, ST7735_BLACK,(*ptrUser)->displayNSS);
					ST7735_WriteStringNSS(5, 105, "Stop Defense", Font_7x10, ST7735_GREEN, ST7735_BLACK,(*ptrUser)->displayNSS);
					ST7735_WriteStringNSS(5, 90, "Opponent chain", Font_7x10, ST7735_WHITE, ST7735_BLACK,(*ptrOpponent)->displayNSS);
					ST7735_WriteStringNSS(5, 105, "a spell card", Font_7x10, ST7735_GREEN, ST7735_BLACK,(*ptrOpponent)->displayNSS);
					HAL_Delay(1000);
					ST7735_FillRectangleNSS(0, 90, 128, 128 - 90, ST7735_BLACK,(*ptrUser)->displayNSS);
					ST7735_FillRectangleNSS(0, 90, 128, 128 - 90, ST7735_BLACK,(*ptrOpponent)->displayNSS);
				}
			}
			break;
		}
		break;
		case Battle_Phase:
			LCDvalue(playerAtk,playerDef);
			switch(PBS){
			case PBS_AFK:
				break;
			case PBS_ActionAwait:
				//ATK action 50
				ST7735_WriteString(110, 35, "BP", Font_7x10, ST7735_YELLOW, ST7735_BLACK);
				ST7735_WriteString1(110, 35, "BP", Font_7x10, ST7735_YELLOW, ST7735_BLACK);
				if(state_game->action == 50){
					Player_Reading_Card(RFIDmain, state_game, playerDef);
					if(HAL_GPIO_ReadPin(TURN_BUTTON_PORT, TURN_BUTTON_PIN)
							== GPIO_PIN_RESET){
						HAL_TIM_Base_Stop_IT(&TIM7_PORT);
						_micro = 0;
						ST7735_FillRectangleNSS(0, 90, 128, 128 - 90, ST7735_BLACK,playerAtk->displayNSS);
						ST7735_WriteStringNSS(15, 90, "END TURN", Font_11x18, ST7735_YELLOW, ST7735_BLACK,playerAtk->displayNSS);
						HAL_Delay(2000);
						ST7735_FillRectangleNSS(0, 90, 128, 128 - 90, ST7735_BLACK,playerAtk->displayNSS);
						ST7735_FillRectangleNSS(0, 90, 128, 128 - 90, ST7735_BLACK,playerDef->displayNSS);
						state_game->action = 0;
						YUGIOH_Trap_Can_Activated(playerAtk);
						if(state_game->MainGame_State == first_player){
							state_game->MainGame_State = second_player;
						}
						else{
							state_game->MainGame_State = first_player;
						}
						state_game->PlyerAction_State = Drawn_Phase;
					}
				}
				else if(state_game->action == 51){
					ST7735_FillRectangleNSS(0, 90, 128, 128 - 90, ST7735_BLACK,playerAtk->displayNSS);
					ST7735_FillRectangleNSS(0, 90, 128, 128 - 90, ST7735_BLACK,playerDef->displayNSS);
					// RFID Detect Card on DEF side

					/*
					 *  First Check Card if def has monster
					 */

					uint8_t check_def_mon = 0;

					ptrYugiohCard_src = &playerDef->ActtionBuffer[0];

					uint8_t targetpos = ptrYugiohCard_src->standPosition;

					ptrYugiohCard_dst = &playerDef->cardOnBoard[3];

					// Check Target to Atk
					for(uint8_t i = 0;i < 3; ++i){
						if(ptrYugiohCard_dst->cardData == 0){
							check_def_mon++;
						}
						ptrYugiohCard_dst++;
					}

					/*
					 *  First Check Atk Condition
					 */

					uint8_t flag_can_atk = 0;
					ptrYugiohCard_src = &playerDef->ActtionBuffer[0];
					ptrYugiohCard_dst = &playerAtk->cardOnBoard[3];

					for (uint8_t i = 0;i < 3; ++i) {
						if(ptrYugiohCard_src->cardData == ptrYugiohCard_dst->cardData){
							if(ptrYugiohCard_dst->cardState == 1){
								flag_can_atk = 2;
								break;
							}
							else{
								// Card is in Board but in DEF
								flag_can_atk = 1;
							}
						}
						ptrYugiohCard_dst++;
					}

					if (check_def_mon < 3)
					{
						if (playerDef->cardOnBoard[targetpos].cardData == 0)
						{
							flag_can_atk = 1;
						}
					}

					if (flag_can_atk == 2) {


						// If in this condition ptrYugiohCard_src should point to Atk player Card
						if (ptrYugiohCard_dst->actionPoint_Atk > 0 && ptrYugiohCard_dst->cardState == 1)
						{
							// Copy Monster from ATK Board To in play
							ptrYugiohCard_dst->actionPoint_Atk -= 1; // Action commit ATK point reduce
							YUGIOH_card_copy(ptrYugiohCard_dst, &playerAtk->CardInPlayed);

							ptrYugiohCard_dst = &playerAtk->CardInPlayed;

							if(check_def_mon == 3){
								ptrYugiohCard_dst->targetPosition = 99;
							}
							else
							{
								ptrYugiohCard_dst->targetPosition = targetpos;
							}

							// Clear Def Buffer
							YUGIOH_Clear_Card_Bufffer_Player(playerDef);
							state_game->action = 52;
							state_game->PlyerAction_Battle_Substate = counter_DEF;

						}
						else
						{
							// This else = Have card but already atk , or in def
							state_game->test = 222;
							state_game->action = 50;
							state_game->PlyerAction_Battle_Substate = PBS_ActionAwait;
						}
					}
					else{
						// This else = Dont Have card
						state_game->test = 223;
						state_game->action = 50;
						state_game->PlyerAction_Battle_Substate = PBS_ActionAwait;
					}
				}
				break;
			case counter_DEF:
				//action 52
				ST7735_WriteStringNSS(5, 90, "You declared", Font_7x10, ST7735_WHITE, ST7735_BLACK,playerAtk->displayNSS);
				ST7735_WriteStringNSS(5, 105, "an ATTACK", Font_7x10, ST7735_WHITE, ST7735_BLACK,playerAtk->displayNSS);
				ST7735_WriteStringNSS(5, 90, "Do you chain", Font_7x10, ST7735_WHITE, ST7735_BLACK,playerDef->displayNSS);
				ST7735_WriteStringNSS(5, 105, "a CARD ?", Font_7x10, ST7735_WHITE, ST7735_BLACK,playerDef->displayNSS);
				if(state_game->action == 52)
				{
					if (playerDef->noBTN == GPIO_PIN_RESET){
						state_game->PlyerAction_Battle_Substate = chain_effect;
						state_game->action = 54;
						state_game->count_chain = 0;
					}
					Player_Reading_Card(RFIDmain,state_game,playerDef);
					Player_Reading_Card_Monster_Effect(RFIDmain,state_game,playerDef);
				}
				else if ((state_game->action == 53 ))
				{
					ST7735_FillRectangleNSS(0, 90, 128, 128 - 90, ST7735_BLACK,playerAtk->displayNSS);
					ST7735_FillRectangleNSS(0, 90, 128, 128 - 90, ST7735_BLACK,playerDef->displayNSS);
					ptrYugiohCard_src = &playerDef->ActtionBuffer[0];

					uint8_t idx = YUGIOH_Check_Trap_On_board(playerDef, ptrYugiohCard_src);

					if(ptrYugiohCard_src->cardSignature == 3){
						YUGIOH_card_Buffer_Update_Chain(state_game);
						ptrYugiohCard_src->actionPoint_Eff = 0;
						YUGIOH_card_copy(ptrYugiohCard_src, &state_game->ChainBuffer[0]);
						state_game->ptrChainUser[0] = playerDef;
						state_game->ptrChainOpponent[0] = playerAtk;
						state_game->ChainCount++;

						state_game->PlyerAction_Battle_Substate = counter_ATK;
						state_game->action = 52;
					}
					else{


						if (idx != 255)
						{
							YUGIOH_card_Buffer_Update_Chain(state_game);
							ptrYugiohCard_dst = &playerDef->cardOnBoard[idx];
							ptrYugiohCard_dst->actionPoint_Eff = 0; //Trap is now use
							YUGIOH_card_copy(ptrYugiohCard_dst, &state_game->ChainBuffer[0]);
							state_game->ptrChainUser[0] = playerDef;
							state_game->ptrChainOpponent[0] = playerAtk;
							state_game->ChainCount++;

							state_game->PlyerAction_Battle_Substate = counter_ATK;
							state_game->action = 52;
						}
					}
				}





				break;
			case counter_ATK:
				//action 54
				ST7735_WriteStringNSS(5, 90, "Do you chain", Font_7x10, ST7735_WHITE, ST7735_BLACK,playerAtk->displayNSS);
				ST7735_WriteStringNSS(5, 105, "a CARD", Font_7x10, ST7735_WHITE, ST7735_BLACK,playerAtk->displayNSS);
				ST7735_WriteStringNSS(5, 90, "Waiting player", Font_7x10, ST7735_WHITE, ST7735_BLACK,playerDef->displayNSS);
				ST7735_WriteStringNSS(5, 105, "for CHAIN card", Font_7x10, ST7735_WHITE, ST7735_BLACK,playerDef->displayNSS);
				if(state_game->action == 52)
				{

					if (playerAtk->noBTN == GPIO_PIN_RESET){
						//affect
						state_game->action = 54;
						state_game->PlyerAction_Battle_Substate = chain_effect;
						state_game->count_chain = 0;
					}

					Player_Reading_Card(RFIDmain,state_game,playerDef);
					//					Player_Reading_Card_Monster_Effect(RFIDmain,state_game,playerDef);
				}
				else if ((state_game->action == 53 )){
					ST7735_FillRectangleNSS(0, 90, 128, 128 - 90, ST7735_BLACK,playerAtk->displayNSS);
					ST7735_FillRectangleNSS(0, 90, 128, 128 - 90, ST7735_BLACK,playerDef->displayNSS);

					ptrYugiohCard_src = &playerAtk->ActtionBuffer[0];

					uint8_t idx = YUGIOH_Check_Trap_On_board(playerAtk, ptrYugiohCard_src);

					if(ptrYugiohCard_src->cardSignature == 3){
						YUGIOH_card_Buffer_Update_Chain(state_game);
						ptrYugiohCard_src->actionPoint_Eff = 0;
						YUGIOH_card_copy(ptrYugiohCard_src, &state_game->ChainBuffer[0]);
						state_game->ptrChainUser[0] = playerAtk;
						state_game->ptrChainOpponent[0] = playerDef;
						state_game->ChainCount++;

						state_game->PlyerAction_Battle_Substate = counter_DEF;
						state_game->action = 52;
					}
					else{
						if (idx != 255)
						{
							YUGIOH_card_Buffer_Update_Chain(state_game);
							ptrYugiohCard_dst = &playerDef->cardOnBoard[idx];
							ptrYugiohCard_dst->actionPoint_Eff = 0; //Trap is now use
							YUGIOH_card_copy(ptrYugiohCard_dst, &state_game->ChainBuffer[0]);
							state_game->ptrChainUser[0] = playerDef;
							state_game->ptrChainOpponent[0] = playerAtk;
							state_game->ChainCount++;

							state_game->PlyerAction_Battle_Substate = counter_ATK;
							state_game->action = 52;
						}
					}
				}

				break;
			case chain_effect:
				ST7735_WriteStringNSS(5, 90, "Resolve EFFECT", Font_7x10, ST7735_WHITE, ST7735_BLACK,playerAtk->displayNSS);
				ST7735_WriteStringNSS(5, 90, "Resolve EFFECT", Font_7x10, ST7735_WHITE, ST7735_BLACK,playerDef->displayNSS);

				ptrUser = &state_game->ptrChainUser[state_game->count_chain];
				ptrOpponent = &state_game->ptrChainOpponent[state_game->count_chain];

				if(state_game->action == 54)
				{
					// Base use to check Card Eff
					if (state_game->count_chain < state_game->ChainCount)
					{
						state_game->test = 165;

						ptrYugiohCard_src = &state_game->ChainBuffer[state_game->count_chain];

						if (ptrYugiohCard_src->cardSignature == 3)
						{
							(*ptrOpponent)->CardInPlayed.targetPosition = 98;
							YUGIOH_To_GY(*ptrUser, ptrYugiohCard_src);
							state_game->count_chain++;
						}
						else if(ptrYugiohCard_src->cardSignature == 14 || ptrYugiohCard_src->cardSignature == 15)
						{
							state_game->test = 133;
							YUGIOH_Gift_of_the_Mystical_Elf(*ptrUser,*ptrOpponent);
							YUGIOH_To_GY(*ptrUser, ptrYugiohCard_src);
							state_game->count_chain++;

						}
						else if(ptrYugiohCard_src->cardSignature == 20){

							YUGIOH_To_GY(*ptrUser, ptrYugiohCard_src);
							if(state_game->MainGame_State == first_player){
								state_game->MainGame_State = second_player;
							}
							else{
								state_game->MainGame_State = first_player;
							}
							state_game->PlyerAction_State = Drawn_Phase;
						}

					}
					else
					{
						// All Chain Clear
						ST7735_FillRectangleNSS(0, 90, 128, 128 - 90, ST7735_BLACK,playerAtk->displayNSS);
						ST7735_FillRectangleNSS(0, 90, 128, 128 - 90, ST7735_BLACK,playerDef->displayNSS);
						state_game->action = 50;
						state_game->PlyerAction_Battle_Substate = calculate_damage;
					}

				}
				break;
			case  calculate_damage:
				ptrYugiohCard_src = &playerAtk->CardInPlayed;

				uint8_t atk = ptrYugiohCard_src->cardAtk;

				if(ptrYugiohCard_src->targetPosition == 99)
				{
					playerDef->life_point -= atk*100;
					state_game->PlyerAction_Battle_Substate = after_calculate;
				}
				else if(ptrYugiohCard_src->targetPosition == 98)
				{
					state_game->PlyerAction_Battle_Substate = after_calculate;
				}
				else
				{
					ptrYugiohCard_dst = playerDef->cardOnBoard;

					ptrYugiohCard_dst = &playerDef->cardOnBoard[ptrYugiohCard_src->targetPosition];

					state_game->test = 54;
					if(ptrYugiohCard_dst->cardState == 0){
						ST7735_WriteStringNSS(5, 90, "You attacked", Font_7x10, ST7735_WHITE, ST7735_BLACK,playerAtk->displayNSS);
						ST7735_WriteStringNSS(5, 105, "a DEF MONSTER", Font_7x10, ST7735_YELLOW, ST7735_BLACK,playerAtk->displayNSS);
						ST7735_WriteStringNSS(5, 90, "Opp. attacked", Font_7x10, ST7735_WHITE, ST7735_BLACK,playerDef->displayNSS);
						ST7735_WriteStringNSS(5, 105, "your DEF MONSTER", Font_7x10, ST7735_YELLOW, ST7735_BLACK,playerDef->displayNSS);
						//                    uint8_t atk = ptrYugiohCard_src->cardAtk;
						uint8_t def = ptrYugiohCard_dst->cardDef;
						if(atk < def){
							ST7735_FillRectangleNSS(0, 90, 128, 128 - 90, ST7735_BLACK,playerAtk->displayNSS);
							ST7735_FillRectangleNSS(0, 90, 128, 128 - 90, ST7735_BLACK,playerDef->displayNSS);
							playerAtk->life_point -= (def-atk)*100;
							YUGIOH_To_GY(playerAtk, &playerAtk->cardOnBoard[ptrYugiohCard_src->standPosition]);
							YUGIOH_Clear_Card_Bufffer_Player(playerAtk);

							state_game->PlyerAction_Battle_Substate = after_calculate;
						}
						else if(atk > def){
							ST7735_FillRectangleNSS(0, 90, 128, 128 - 90, ST7735_BLACK,playerAtk->displayNSS);
							ST7735_FillRectangleNSS(0, 90, 128, 128 - 90, ST7735_BLACK,playerDef->displayNSS);
							YUGIOH_To_GY(playerDef, ptrYugiohCard_dst);
							state_game->PlyerAction_Battle_Substate = after_calculate;
						}
					}
					else if(ptrYugiohCard_dst->cardState == 1){
						ST7735_WriteStringNSS(5, 90, "You attacked", Font_7x10, ST7735_WHITE, ST7735_BLACK,playerAtk->displayNSS);
						ST7735_WriteStringNSS(5, 105, "a ATK MONSTER", Font_7x10, ST7735_YELLOW, ST7735_BLACK,playerAtk->displayNSS);
						ST7735_WriteStringNSS(5, 90, "Opp. attacked", Font_7x10, ST7735_WHITE, ST7735_BLACK,playerDef->displayNSS);
						ST7735_WriteStringNSS(5, 105, "your ATK MONSTER", Font_7x10, ST7735_YELLOW, ST7735_BLACK,playerDef->displayNSS);
						uint8_t atk2 = ptrYugiohCard_dst->cardAtk;
						state_game->test = 60;
						if(atk < atk2){
							ST7735_FillRectangleNSS(0, 90, 128, 128 - 90, ST7735_BLACK,playerAtk->displayNSS);
							ST7735_FillRectangleNSS(0, 90, 128, 128 - 90, ST7735_BLACK,playerDef->displayNSS);
							state_game->test = 61;
							playerAtk->life_point -= (atk2-atk)*100;
							YUGIOH_To_GY(playerAtk, &playerAtk->cardOnBoard[ptrYugiohCard_src->standPosition]);
							YUGIOH_Clear_Card_Bufffer_Player(playerAtk);
							state_game->PlyerAction_Battle_Substate = after_calculate;
						}
						else if(atk > atk2){
							ST7735_FillRectangleNSS(0, 90, 128, 128 - 90, ST7735_BLACK,playerAtk->displayNSS);
							ST7735_FillRectangleNSS(0, 90, 128, 128 - 90, ST7735_BLACK,playerDef->displayNSS);
							state_game->test = 70;
							playerDef->life_point -= (atk-atk2)*100;
							YUGIOH_To_GY(playerDef, ptrYugiohCard_dst);
							state_game->PlyerAction_Battle_Substate = after_calculate;
						}

					}
				}


				break;
			case after_calculate:
				ST7735_WriteStringNSS(5, 90, "Finish attack", Font_7x10, ST7735_WHITE, ST7735_BLACK,playerAtk->displayNSS);
				ST7735_WriteStringNSS(5, 90, "Finish defense", Font_7x10, ST7735_WHITE, ST7735_BLACK,playerDef->displayNSS);

				// Clear Card in Played (action ended)
				ptrYugiohCard_src = &playerAtk->ActtionBuffer[0];
				YUGIOH_card_clear(ptrYugiohCard_src);

				if(playerDef->life_point == 0 || playerDef->life_point >= 60000){
					state_game->MainGame_State = Game_Ended;
				}
				else{

					state_game->action = 50;
					ST7735_FillRectangleNSS(0, 90, 128, 128 - 90, ST7735_BLACK,playerAtk->displayNSS);
					ST7735_FillRectangleNSS(0, 90, 128, 128 - 90, ST7735_BLACK,playerDef->displayNSS);

					ST7735_WriteStringNSS(5, 90, "Finish attack", Font_7x10, ST7735_WHITE, ST7735_BLACK,playerAtk->displayNSS);
					ST7735_WriteStringNSS(5, 90, "Finish defense", Font_7x10, ST7735_WHITE, ST7735_BLACK,playerDef->displayNSS);
					state_game->PlyerAction_Battle_Substate = PBS_ActionAwait;
				}
				break;
			}
			break;

	}
}

void Test_EFF(Player *playerUser,Player *playerOpponent)
{
	playerUser->life_point += 1000;
	playerOpponent->life_point -=100;
}


void YUGIOH_Clear_Card_Enemy_Player_Raigeki(Player *player) {
	YUGIOH_Card *ptrYUGIOHCard;
	ptrYUGIOHCard = &player->cardOnBoard[3];
	for (uint8_t i = 0; i < MON_BUFF_LEN; ++i) {
		if(ptrYUGIOHCard->cardData != 0){
			YUGIOH_To_GY(player,ptrYUGIOHCard);
		}
		ptrYUGIOHCard++;
	}
}

void YUGIOH_Clear_Card_Enemy_Player_Dark_Hole(Player *player1,Player *player2) {

	//player1
	YUGIOH_Card *ptrYUGIOHCard_player1 = player1->cardOnBoard;
	ptrYUGIOHCard_player1 = &player1->cardOnBoard[3];

	//player2
	YUGIOH_Card *ptrYUGIOHCard_player2 = player2->cardOnBoard;
	ptrYUGIOHCard_player2 = &player2->cardOnBoard[3];
	for (uint8_t i = 0; i < MON_BUFF_LEN; ++i) {
		if(ptrYUGIOHCard_player1->cardData != 0){
			YUGIOH_To_GY(player1,ptrYUGIOHCard_player1);
		}
		ptrYUGIOHCard_player1++;
	}
	for (uint8_t i = 0; i < MON_BUFF_LEN; ++i) {
		if(ptrYUGIOHCard_player2->cardData != 0){
			YUGIOH_To_GY(player2,ptrYUGIOHCard_player2);
		}
		ptrYUGIOHCard_player2++;
	}
}

void YUGIOH_Reborn(Player *player){
	uint8_t flag = 0;
	uint8_t index_GY = 0;
	// Buffer Card src
	YUGIOH_Card *ptrYugiohCard_Buffer_src = &player->CardInPlayed;
	// Buffer Card dst
	YUGIOH_Card *ptrYugiohCard_Buffer_dst = player->GY;
	ptrYugiohCard_Buffer_dst = &player->GY[0];

	for (uint8_t i = 0;i < GY_BUFF_LEN ; ++i) {
		if(ptrYugiohCard_Buffer_src->cardData == ptrYugiohCard_Buffer_dst->cardData){
			flag = 1;
			index_GY = i;
			break;
		}
		ptrYugiohCard_Buffer_dst++;
	}

	ptrYugiohCard_Buffer_dst = &player->cardOnBoard[3];

	if(flag == 1){
		uint8_t idx = ptrYugiohCard_Buffer_src->standPosition % 6;
		YUGIOH_card_copy(ptrYugiohCard_Buffer_src, &player->cardOnBoard[idx]);

		ptrYugiohCard_Buffer_src = &player->GY[index_GY+1];
		ptrYugiohCard_Buffer_dst = &player->GY[index_GY];

		for (int i = index_GY; i < GY_BUFF_LEN ; ++i) {
			YUGIOH_card_copy(ptrYugiohCard_Buffer_src, ptrYugiohCard_Buffer_dst);
			ptrYugiohCard_Buffer_src++;
			ptrYugiohCard_Buffer_dst++;
		}


	}
}

void YUGIOH_Ancient_Rules(Player *player){
	YUGIOH_Card *ptrYugiohCard_src = player->ActtionBuffer;
	ptrYugiohCard_src = &player->ActtionBuffer[0];

	uint8_t idx = ptrYugiohCard_src->standPosition % 6;

	YUGIOH_Card *ptrYugiohCard_dst = player->cardOnBoard;
	ptrYugiohCard_dst = &player->cardOnBoard[idx];

	YUGIOH_card_copy(ptrYugiohCard_src, ptrYugiohCard_dst);

}

void YUGIOH_Gift_of_the_Mystical_Elf(Player *player1,Player *player2){
	YUGIOH_Card *ptrYUGIOHCard_player1 = player1->cardOnBoard;
	ptrYUGIOHCard_player1 = &player1->cardOnBoard[3];

	//player2
	YUGIOH_Card *ptrYUGIOHCard_player2 = player2->cardOnBoard;
	ptrYUGIOHCard_player2 = &player2->cardOnBoard[3];

	uint8_t count_monster = 0;
	for (uint8_t i = 0; i < MON_BUFF_LEN; ++i) {
		if(ptrYUGIOHCard_player1->cardType == 1){
			count_monster += 1;
		}
		ptrYUGIOHCard_player1++;
	}
	for (uint8_t i = 0; i < MON_BUFF_LEN; ++i) {
		if(ptrYUGIOHCard_player2->cardType == 1){
			count_monster += 1;
		}
		ptrYUGIOHCard_player2++;
	}

	player1->life_point += 300*count_monster;

}

void YUGIOH_Stop_Defense(Player *player1,Player *player2){
	YUGIOH_Card *ptrYugiohCard_src = player1->ActtionBuffer;
	ptrYugiohCard_src = &player1->ActtionBuffer[0];

	uint8_t idx = ptrYugiohCard_src->standPosition % 6;

	YUGIOH_Card *ptrYUGIOHCard_dst = player2->cardOnBoard;
	ptrYUGIOHCard_dst = &player2->cardOnBoard[idx];

	if((ptrYUGIOHCard_dst->cardState == 0) && (ptrYUGIOHCard_dst->cardData != 0)){
		ptrYUGIOHCard_dst->cardState = 1;
	}

}

//void YUGIOH_Ancient_Rules(Player *player){
//
//}
//
//void YUGIOH_Stop_Defense(Player *player,){
//
//}


//void YUGIOH_Pot_Of_Greed(Player *player){
//
//}


uint8_t YUGIOH_Check_Trap_On_board(Player *player,YUGIOH_Card *card)
{
	YUGIOH_Card *ptrCardCheck;
	ptrCardCheck = &player->cardOnBoard[0];

	for (int i = 0; i < 3; ++i)
	{
		if(card->cardData == ptrCardCheck->cardData)
		{
			if(ptrCardCheck->actionPoint_Eff > 0 && ptrCardCheck->cardType == 3)
			{
				return i;
			}
		}
		ptrCardCheck++;
	}
	return 255;
}

uint8_t YUGIOH_Check_Spell_On_board(Player *player,YUGIOH_Card *card)
{
	YUGIOH_Card *ptrCardCheck;
	ptrCardCheck = &player->cardOnBoard[0];

	for (int i = 0; i < 3; ++i)
	{
		if(card->cardData == ptrCardCheck->cardData)
		{
			if(ptrCardCheck->actionPoint_Eff > 0 && ptrCardCheck->cardType == 2)
			{
				return i;
			}
		}
		ptrCardCheck++;
	}
	return 255;
}

void YUGIOH_Trap_Can_Activated(Player *player)
{
	YUGIOH_Card *ptrCard;
	ptrCard = &player->cardOnBoard[0];
	for (int i = 0; i < 3; ++i)
	{
		if (ptrCard->cardData != 0)
		{
			ptrCard->actionPoint_Eff = 1;
		}
		ptrCard++;
	}
}

void YUGIOH_Monster_Activated(Player *player)
{
	YUGIOH_Card *ptrCard;
	ptrCard = &player->cardOnBoard[3];
	for (int i = 3; i < 6; ++i)
	{
		if (ptrCard->cardData != 0)
		{
			ptrCard->actionPoint_Atk = 1;
		}
		ptrCard++;
	}
}
void MainGUI()
{
	ST7735_WriteString1(5, 5, "Player 1: ", Font_7x10, ST7735_MAGENTA, ST7735_BLACK);
	ST7735_WriteString1(5, 20, "Life points: ", Font_7x10, ST7735_WHITE, ST7735_BLACK);
	ST7735_WriteString1(5, 35, "Turns:", Font_7x10, ST7735_WHITE, ST7735_BLACK);
	ST7735_WriteString(60, 35, "|Phase:", Font_7x10, ST7735_WHITE, ST7735_BLACK);
	ST7735_WriteString1(0, 50, "Remaining time: ", Font_7x10, ST7735_WHITE, ST7735_BLACK);
	ST7735_WriteString1(0, 60, "__________________", Font_7x10, ST7735_WHITE, ST7735_BLACK);
	ST7735_WriteString(5, 5, "Player 2: ", Font_7x10, ST7735_MAGENTA, ST7735_BLACK);
	ST7735_WriteString(5, 20, "Life points: ", Font_7x10, ST7735_WHITE, ST7735_BLACK);
	ST7735_WriteString(5, 35, "Turns: ", Font_7x10, ST7735_WHITE, ST7735_BLACK);
	ST7735_WriteString1(60, 35, "|Phase:", Font_7x10, ST7735_WHITE, ST7735_BLACK);
	ST7735_WriteString(0, 50, "Remaining time: ", Font_7x10, ST7735_WHITE, ST7735_BLACK);
	ST7735_WriteString(0, 60, "__________________", Font_7x10, ST7735_WHITE, ST7735_BLACK);
}

void LCDvalue(Player *playerAtk, Player *playerDef){
	uint16_t a = 0 ;
	time = timeinit;
	a = _micro / 1000000;
	time -= a;
	sprintf(t_c, "%d",time);
	for (int i = 0 ; i < 3 ; i++){
		if(t_c[i] == 0){
			t_c[i] = 32;
			t_c[i+1] = 32;
		}
	}
	if(time == 0){
		HAL_TIM_Base_Stop_IT(&TIM7_PORT);
		time = 0;
		ST7735_FillScreen(ST7735_BLACK);
		ST7735_FillScreen1(ST7735_BLACK);
		while(time == 0){
			uint8_t credit = 1;
			if(credit == 0){
				ST7735_WriteStringNSS(15, 35, "YOU LOSE", Font_11x18, ST7735_RED, ST7735_BLACK,playerAtk->displayNSS);
				ST7735_WriteStringNSS(5, 60, "player timed out", Font_7x10, ST7735_WHITE, ST7735_BLACK,playerAtk->displayNSS);
				ST7735_WriteStringNSS(20, 35, "YOU WIN", Font_11x18, ST7735_YELLOW, ST7735_BLACK,playerDef->displayNSS);
				ST7735_WriteStringNSS(0, 60, "opponent timed out", Font_7x10, ST7735_WHITE, ST7735_BLACK,playerDef->displayNSS);
			}
			// Author CREDITTTTTT
			else if(credit == 1){
				ST7735_WriteStringNSS(15, 25, "YOU LOSE", Font_11x18, ST7735_RED, ST7735_BLACK,playerAtk->displayNSS);
				ST7735_WriteStringNSS(5, 50, "player timed out", Font_7x10, ST7735_WHITE, ST7735_BLACK,playerAtk->displayNSS);
				ST7735_WriteStringNSS(20, 25, "YOU WIN", Font_11x18, ST7735_YELLOW, ST7735_BLACK,playerDef->displayNSS);
				ST7735_WriteStringNSS(0, 50, "opponent timed out", Font_7x10, ST7735_WHITE, ST7735_BLACK,playerDef->displayNSS);
				ST7735_WriteStringNSS(0, 65, "__________________", Font_7x10, ST7735_WHITE, ST7735_BLACK,0);
				ST7735_WriteStringNSS(0, 65, "__________________", Font_7x10, ST7735_WHITE, ST7735_BLACK,1);
				ST7735_WriteStringNSS(0, 100, "Dev1:TinnZx", Font_7x10, ST7735_CYAN, ST7735_BLACK,0);
				ST7735_WriteStringNSS(0, 115, "Dev2:Azthorax", Font_7x10, ST7735_GREEN, ST7735_BLACK,0);
				ST7735_WriteStringNSS(0, 115, "Dev3:FANNUT", Font_7x10, ST7735_YELLOW, ST7735_BLACK,1);
				ST7735_WriteStringNSS(0, 90, "FRA421 Project", Font_7x10, ST7735_MAGENTA, ST7735_BLACK,1);
			}
		}
	}

	sprintf(C_LP_ATK, "%d",playerAtk->life_point);
	if(C_LP_ATK[3] == 0){
		C_LP_ATK[3] = C_LP_ATK[2];
		C_LP_ATK[2] = C_LP_ATK[1];
		C_LP_ATK[1] = C_LP_ATK[0];
		C_LP_ATK[0] = 32;
	}
	ST7735_WriteStringNSS( 90, 20, C_LP_ATK, Font_7x10, ST7735_GREEN, ST7735_BLACK,playerAtk->displayNSS);
	ST7735_WriteStringNSS( 105, 50, t_c, Font_7x10, ST7735_GREEN, ST7735_BLACK,playerAtk->displayNSS);
	sprintf(C_LP_DEF, "%d",playerDef->life_point);
	if(C_LP_DEF[3] == 0){
		C_LP_DEF[3] = C_LP_DEF[2];
		C_LP_DEF[2] = C_LP_DEF[1];
		C_LP_DEF[1] = C_LP_DEF[0];
		C_LP_DEF[0] = 32;
	}
	ST7735_WriteStringNSS( 90, 20, C_LP_DEF, Font_7x10, ST7735_GREEN, ST7735_BLACK,playerDef->displayNSS);
	ST7735_WriteStringNSS( 105, 50, "180", Font_7x10, ST7735_GREEN, ST7735_BLACK,playerDef->displayNSS);
	sprintf(c_turn, "%d",turn);
	ST7735_WriteStringNSS(50, 35, c_turn, Font_7x10, ST7735_GREEN, ST7735_BLACK,0);
	ST7735_WriteStringNSS(50, 35, c_turn, Font_7x10, ST7735_GREEN, ST7735_BLACK,1);

}
