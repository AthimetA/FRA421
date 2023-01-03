/*
 * FRA421_Yugioh.c
 *
 *  Created on: Dec 30, 2022
 *      Author: AthimetA
 */

#include "FRA421_Yugioh.h"
#include <stdlib.h>
#include <string.h>

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
	dst->actionPositon = src->actionPositon;
}
void RFID_Clear_Card_Bufffer(RFID *rfid) {
	YUGIOH_Card buffCard = { 0 };
	YUGIOH_Card *ptrYUGIOHCard = rfid->bufferCard;
	for (uint8_t i = 0; i < CARD_BUFF_LEN; ++i) {
		YUGIOH_card_copy(&buffCard, ptrYUGIOHCard);
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
	ptrYUGIOHCard->actionPositon = ptrRFID->slaveAddr;

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

const uint8_t archive_yugioh_card_type[256] = { 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0,
		2, 0, 0, 0, 2, 2, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0,
		0, 0, 0, 0, 0, 1, 0, 2, 0, 0, 0, 0, 2, 0, 0, 0, 2, 0, 0, 0, 0, 0, 3, 0,
		0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0,
		1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 0, 3, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 2, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0,
		0, 0, 0, 0, 0, 1, 0, 0, 2, 0, 2, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0,
		0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 2, 0, 2,
		1, 0, 0, 0, };

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
}

void YUGIOH_Clear_Card_Bufffer_Player(Player *player) {
	YUGIOH_Card buffCard = { 0 };
	YUGIOH_Card *ptrYUGIOHCard = player->ChainBuffer;
	for (uint8_t i = 0; i < CHAIN_BUFF_LEN; ++i) {
		YUGIOH_card_copy(&buffCard, ptrYUGIOHCard);
		ptrYUGIOHCard++;
	}
}

void YUGIOH_card_Buffer_Update_Player(Player *player) {
	// Buffer Card src
	YUGIOH_Card *ptrYugiohCard_Buffer_src = player->ChainBuffer;
	ptrYugiohCard_Buffer_src = &player->ChainBuffer[CHAIN_BUFF_LEN - 2];
	// Buffer Card dst
	YUGIOH_Card *ptrYugiohCard_Buffer_dst = player->ChainBuffer;
	ptrYugiohCard_Buffer_dst = &player->ChainBuffer[CHAIN_BUFF_LEN - 1];
	for (int i = CHAIN_BUFF_LEN; i >= 1; i--) {
		YUGIOH_card_copy(ptrYugiohCard_Buffer_src, ptrYugiohCard_Buffer_dst);
		ptrYugiohCard_Buffer_src--;
		ptrYugiohCard_Buffer_dst--;
	}
}


void GAME_PLAY_Management(RFIDHandle *RFIDmain, State_game *state_game) {

	Player *ptrPlayer1 = state_game->player;
	Player *ptrPlayer2 = state_game->player;
	ptrPlayer1 = &state_game->player[0];
	ptrPlayer2 = &state_game->player[1];

	enum _player_state {
		ready, first_player, second_player
	} player_state;
	player_state = state_game->player_state;

	switch (player_state) {
	case ready:
		if (HAL_GPIO_ReadPin(START_BUTTON_PORT, START_BUTTON_PIN)
				== GPIO_PIN_RESET) {
			ptrPlayer1->life_point = 4000;
			ptrPlayer2->life_point = 4000;
			state_game->player_state = first_player;
		} else {
			state_game->test = 98;
		}
		break;
	case first_player:
		GAME_PLAY_Phase_Management(RFIDmain,state_game);
	case second_player:
		GAME_PLAY_Phase_Management(RFIDmain,state_game);
		break;
	}
}

void GAME_PLAY_Phase_Management(RFIDHandle *RFIDmain, State_game *state_game)
{
	enum _STATE {
		Drawn_Phase, Main_Phase, Battle_Phase, Chain_Phase
	} STATE;
	STATE = state_game->STATE;

	enum _MAIN {
		await,select_position, check_card_type, advance_summon, activate_effect
	} MAIN;
	MAIN = state_game->MAIN;

	// Player
	Player *ptrPlayerAtk = state_game->player;
	Player *ptrPlayerDef = state_game->player;

	if (state_game->player_state == first_player) {
		ptrPlayerAtk = &state_game->player[0];
		ptrPlayerDef = &state_game->player[1];
	} else if (state_game->player_state == first_player) {
		ptrPlayerAtk = &state_game->player[1];
		ptrPlayerDef = &state_game->player[0];
	}

	// Card PTR
	YUGIOH_Card *ptrYugiohCard_src;
	YUGIOH_Card *ptrYugiohCard_dst;

	switch (STATE) {
	case Drawn_Phase:
		if (HAL_GPIO_ReadPin(TURN_BUTTON_PORT, TURN_BUTTON_PIN)
				== GPIO_PIN_RESET) {
			state_game->STATE = Main_Phase;
			state_game->MAIN = await;
			// Wait for card to be read State = 0 Mean AFK
			state_game->action = 0;
		}
		break;
	case Main_Phase:

		switch (MAIN)
		{
		case await:
			if (state_game->action == 0)
			{
				// Reading Until RFID action = 1 Mean Card Detected
				Player_Reading_Card(RFIDmain,state_game,ptrPlayerAtk);
			}
			else if (state_game->action == 1)
			{
				state_game->MAIN = select_position;
			}
			break;
		case select_position:

			// Current state_game->action = 1
			ptrYugiohCard_src = &ptrPlayerAtk->ChainBuffer[0];
			ptrYugiohCard_dst = &ptrPlayerAtk->CardInPlayed;

			if (state_game->action == 1)
			{
				if (state_game->player_state == first_player)
				{
					if(HAL_GPIO_ReadPin(YES1_PORT, YES1_PIN) == GPIO_PIN_RESET)
					{
						ptrYugiohCard_src->cardState = 1;
						state_game->action = 2;
					}
					else if (HAL_GPIO_ReadPin(NO1_PORT, NO1_PIN) == GPIO_PIN_RESET)
					{
						ptrYugiohCard_src->cardState = 0;
						state_game->action = 2;
					}

				}
				else if (state_game->player_state == second_player)
				{
					if(HAL_GPIO_ReadPin(YES2_PORT, YES2_PIN) == GPIO_PIN_RESET)
					{
						ptrYugiohCard_src->cardState = 1;
						state_game->action = 2;
					}
					else if (HAL_GPIO_ReadPin(NO2_PORT, NO2_PIN) == GPIO_PIN_RESET)
					{
						ptrYugiohCard_src->cardState = 0;
						state_game->action = 2;
					}
				}


			}
			else if (state_game->action == 2)
			{
				YUGIOH_card_copy(ptrYugiohCard_src, ptrYugiohCard_dst);
				YUGIOH_Clear_Card_Bufffer_Player(ptrPlayerAtk);
				state_game->action = 3;
				state_game->MAIN = check_card_type;
			}
			break;
		case check_card_type:
			// Current state_game->action = 3

			ptrYugiohCard_src = &ptrPlayerAtk->CardInPlayed;

			if (state_game->action == 3)
			{
				if (ptrYugiohCard_src->cardType == 3)
				{
					// Add card to board
					uint8_t idx = ptrYugiohCard_src->actionPositon % 6;
					ptrYugiohCard_dst = &ptrPlayerAtk->cardOnBoard[idx];

					YUGIOH_card_copy(ptrYugiohCard_src, ptrYugiohCard_dst);

					YUGIOH_Clear_Card_Bufffer_Player(ptrPlayerAtk);
					YUGIOH_card_copy(&ptrPlayerAtk->ChainBuffer[0], ptrYugiohCard_src);

					state_game->action = 0;
					state_game->MAIN = await;

				}
				else if (ptrYugiohCard_src->cardType == 2)
				{
					//Pass
				}
				else if (ptrYugiohCard_src->cardType == 1)
				{
					if (ptrYugiohCard_src->cardLevel < 7)
					{
						// Add card to board
						uint8_t idx = ptrYugiohCard_src->actionPositon % 6;
						ptrYugiohCard_dst = &ptrPlayerAtk->cardOnBoard[idx];

						YUGIOH_card_copy(ptrYugiohCard_src, ptrYugiohCard_dst);

						YUGIOH_Clear_Card_Bufffer_Player(ptrPlayerAtk);
						YUGIOH_card_copy(&ptrPlayerAtk->ChainBuffer[0], ptrYugiohCard_src);

						state_game->action = 0;
						state_game->MAIN = await;
					}
					else
					{
						YUGIOH_Clear_Card_Bufffer_Player(ptrPlayerAtk);
						state_game->action = 4;
						state_game->MAIN = advance_summon;
					}
				}
			}
			break;
		case advance_summon:
			// Current state_game->action = 4

			ptrYugiohCard_src = &ptrPlayerAtk->CardInPlayed;

			if (state_game->action == 4)
			{
				// Reading Until RFID action += 1 Mean Card Detected
				Player_Reading_Card(RFIDmain,state_game,ptrPlayerAtk);
			}
			else if (state_game->action == 5)
			{
				// Reading Until RFID action += 1 Mean Card Detected
				Player_Reading_Card(RFIDmain,state_game,ptrPlayerAtk);
			}
			else if (state_game->action == 6)
			{
				uint8_t monsterflag = 0 ;
				ptrYugiohCard_dst = &ptrPlayerAtk->ChainBuffer[0];
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
					uint8_t idx = ptrYugiohCard_src->actionPositon % 6;
					ptrYugiohCard_dst = &ptrPlayerAtk->cardOnBoard[idx];

					YUGIOH_card_copy(ptrYugiohCard_src, ptrYugiohCard_dst);

					YUGIOH_Clear_Card_Bufffer_Player(ptrPlayerAtk);
					YUGIOH_card_copy(&ptrPlayerAtk->ChainBuffer[0], ptrYugiohCard_src);

					state_game->action = 0;
					state_game->MAIN = await;
				}
				else
				{
					YUGIOH_Clear_Card_Bufffer_Player(ptrPlayerAtk);
					YUGIOH_card_copy(&ptrPlayerAtk->ChainBuffer[0], &ptrPlayerAtk->CardInPlayed);
				}
			}



			else

			break;
		case activate_effect:
			break;
		}
		break;
		case Battle_Phase:
			break;
		case Chain_Phase:
			break;

	}
}

void Player_Reading_Card(RFIDHandle *RFIDmain, State_game *state_game ,Player *player)
{
	// Assign RFID
	RFID *ptrRFID = RFIDmain->RFID;
	ptrRFID = &RFIDmain->RFID[RFIDmain->slaveNum];

	YUGIOH_Card *ptrYugiohCard_src;
	ptrYugiohCard_src = &ptrRFID->bufferCard[0];

	YUGIOH_Card *ptrYugiohCard_dst;
	ptrYugiohCard_dst = &player->ChainBuffer[0];

	if (ptrRFID->action == 1) // Card Detected
	{
		// Update buffer
		YUGIOH_card_Buffer_Update_Player(player);
		YUGIOH_card_copy(ptrYugiohCard_src, ptrYugiohCard_dst);

		RFID_Clear_Card_Bufffer(ptrRFID);
		ptrRFID->action = 0;

		state_game->action += 1;
	}
}

//void GAME_PLAY_CARD_PLAYED(RFIDHandle *RFIDmain, State_game *state_game) {
//	// Assign RFID
//	RFID *ptrRFID = RFIDmain->RFID;
//	ptrRFID = &RFIDmain->RFID[RFIDmain->slaveNum];
//
//	// Card Play Src
//	YUGIOH_Card *ptrYugiohCard_src;
//	ptrYugiohCard_src = &ptrRFID->bufferCard[0];
//
//	// Player
//	Player *ptrPlayer = state_game->player;
//	if (state_game->player_state == first_player) {
//		ptrPlayer = &state_game->player[0];
//	} else if (state_game->player_state == first_player) {
//		ptrPlayer = &state_game->player[1];
//	}
//
//	// Card Play Dst
//	YUGIOH_Card *ptrYugiohCard_dst = ptrPlayer->ChainBuffer;
//
//	if (ptrRFID->action == 1) {
//		state_game->actionPos = ptrRFID->slaveAddr;
//		YUGIOH_card_Buffer_Update2(ptrPlayer);
//		YUGIOH_card_copy(ptrYugiohCard_src, ptrYugiohCard_dst);
//		RFID_Clear_Card_Bufffer(ptrRFID);
//		ptrRFID->action = 0;
//		state_game->action = 1; // Action is on
//	}
//}
//
//void GAME_PLAY_Main_Management(RFIDHandle *RFIDmain, State_game *state_game) {
//
//	enum _MAIN {
//		select_position, check_card_type, advance_summon, activate_effect
//	} MAIN;
//	MAIN = state_game->MAIN;
//
//	// Assign RFID
//	RFID *ptrRFID = RFIDmain->RFID;
//	ptrRFID = &RFIDmain->RFID[RFIDmain->slaveNum];
//
//	// Assign Player
//	Player *ptrPlayer = state_game->player;
//	if (state_game->player_state == first_player) {
//		ptrPlayer = &state_game->player[0];
//	} else if (state_game->player_state == first_player) {
//		ptrPlayer = &state_game->player[1];
//	}
//
//
//	// In main phase from draw phase
//
//	/*
//	 *  First await action from RFID current RFID action = 0 mean no card detected
//	 */
//
//	if (ptrRFID->action == 0)
//	{
//		// Pass
//	}
//	else if (ptrRFID->action == 1) // Card detected from RFID
//	{
//
//	}
//
//
//	Player *ptrPlayer1 = state_game->player;
//	Player *ptrPlayer2 = state_game->player;
//	ptrPlayer1 = &state_game->player[0];
//	ptrPlayer2 = &state_game->player[1];
//
//	YUGIOH_Card *ptrYUGIOHCard_src = ptrPlayer1->ChainBuffer;
//	ptrYUGIOHCard_src = &ptrPlayer1->ChainBuffer[0];
//
//	YUGIOH_Card *ptrYUGIOHCard_inplay = ptrPlayer1->CardInPlayed;
//	ptrYUGIOHCard_inplay = &ptrPlayer1->CardInPlayed;
//
//	YUGIOH_Card *ptrYUGIOHCard_dst = ptrPlayer1->cardOnBoard;
//	uint8_t idx = (state_game->actionPos) % 6;
//	ptrYUGIOHCard_dst = &ptrPlayer1->cardOnBoard[idx];
//
//	enum _MAIN {
//		select_position, check_card_type, advance_summon, activate_effect
//	} MAIN;
//	MAIN = state_game->MAIN;
//
//	switch (MAIN) {
//	case select_position:
//		if (state_game->action == 1) {
//			if ((HAL_GPIO_ReadPin(YES1_PORT, YES1_PIN) == GPIO_PIN_RESET)
//					&& (state_game->player_state == first_player)) {
//				ptrYUGIOHCard_src->cardState = 1;
//				YUGIOH_card_copy(ptrYUGIOHCard_src, ptrYUGIOHCard_inplay);
//				YUGIOH_Clear_Card_Bufffer(ptrPlayer1);
//				state_game->MAIN = check_card_type;
//			} else if ((HAL_GPIO_ReadPin(YES2_PORT, YES2_PIN) == GPIO_PIN_RESET)
//					&& (state_game->player_state == first_player)) {
//				ptrYUGIOHCard_src->cardState = 1;
//				YUGIOH_card_copy(ptrYUGIOHCard_src, ptrYUGIOHCard_inplay);
//				YUGIOH_Clear_Card_Bufffer(ptrPlayer1);
//				state_game->MAIN = check_card_type;
//			} else if ((HAL_GPIO_ReadPin(NO1_PORT, NO1_PIN) == GPIO_PIN_RESET)
//					&& (state_game->player_state == first_player)) {
//				ptrYUGIOHCard_src->cardState = 0;
//				YUGIOH_card_copy(ptrYUGIOHCard_src, ptrYUGIOHCard_inplay);
//				YUGIOH_Clear_Card_Bufffer(ptrPlayer1);
//				state_game->MAIN = check_card_type;
//			} else if ((HAL_GPIO_ReadPin(NO2_PORT, NO2_PIN) == GPIO_PIN_RESET)
//					&& (state_game->player_state == first_player)) {
//				ptrYUGIOHCard_src->cardState = 0;
//				YUGIOH_card_copy(ptrYUGIOHCard_src, ptrYUGIOHCard_inplay);
//				YUGIOH_Clear_Card_Bufffer2(ptrPlayer1);
//				state_game->MAIN = check_card_type;
//			}
//		}
//		break;
//	case check_card_type:
//		state_game->action = 0;
//		if (ptrYUGIOHCard_src->cardType == 3) {
//			state_game->MAIN = select_position;
//		} else if (ptrYUGIOHCard_src->cardType == 2) {
//			if (ptrYUGIOHCard_src->cardState == 1) {
//				state_game->MAIN = activate_effect;
//			} else {
//				YUGIOH_card_copy(ptrYUGIOHCard_src, ptrYUGIOHCard_dst);
//				YUGIOH_Clear_Card_Bufffer(ptrPlayer1);
//				state_game->MAIN = select_position;
//			}
//		} else if (ptrYUGIOHCard_src->cardType == 1) {
//			if (ptrYUGIOHCard_src->cardLevel > 4) {
//
////				YUGIOH_card_Buffer_Update2(ptrPlayer1);
//				state_game->MAIN = advance_summon;
//			} else {
//				state_game->MAIN = select_position;
//			}
//		}
//		break;
//	case advance_summon:
//		if(state_game->action == 1){
//			YUGIOH_card_Buffer_Update2(ptrPlayer1);
//			state_game->action = 0;
//			if(state_game->action == 1){
//				YUGIOH_card_Buffer_Update2(ptrPlayer1);
//				if(ptrYUGIOHCard_src->cardLevel >= 7){
//
//						}
//			}
//		}
//
//
//	}
//}
