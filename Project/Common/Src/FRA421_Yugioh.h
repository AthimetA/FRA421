/*
 * FRA421_Yugioh.h
 *
 *  Created on: Dec 29, 2022
 *      Author: AthimetA
 */

#ifndef INC_FRA421_YUGIOH_H_
#define INC_FRA421_YUGIOH_H_

#include <stdint.h>
#include "stm32h7xx_hal.h"

//Maximum length of the array
#define MAX_LEN 16

//Number of RFID
#define RFID_NUM_MAX 12
#define CARD_BUFF_LEN 4

//Number of ChainBuffer
#define CHAIN_BUFF_LEN 6

//Number of monster in area
#define MON_BUFF_LEN 3

//Number of GY
#define GY_BUFF_LEN 20

// user button
#define START_BUTTON_PORT GPIOA
#define START_BUTTON_PIN GPIO_PIN_3

#define TURN_BUTTON_PORT GPIOC
#define TURN_BUTTON_PIN GPIO_PIN_0

#define YES1_PORT GPIOC
#define YES1_PIN GPIO_PIN_3

#define NO1_PORT GPIOB
#define NO1_PIN GPIO_PIN_1

#define YES2_PORT GPIOC
#define YES2_PIN GPIO_PIN_2

#define NO2_PORT GPIOF
#define NO2_PIN GPIO_PIN_11

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
	uint8_t cardLevel;
	uint8_t cardAtk;
	uint8_t cardDef;
	uint8_t standPosition;
	uint8_t targetPosition;
	uint8_t actionPoint_Atk;
	uint8_t actionPoint_Eff;
	uint32_t cardData;
}YUGIOH_Card;

typedef struct _RFID
{
	uint16_t slaveAddr;
	uint8_t status;
	uint8_t action;
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

// This type def use for collect data
typedef struct _YUGIOH_CARD_HASH
{
	uint8_t cardNumber;
	uint8_t hashIdx;
	uint32_t hashData;
}YUGIOH_Hash;

typedef struct _Player
{
	uint8_t displayNSS;
    uint16_t life_point;
    //MAIN Buffer
    YUGIOH_Card cardOnBoard [6];
    YUGIOH_Card GY [20];
    //buffer
    YUGIOH_Card ChainBuffer [CHAIN_BUFF_LEN] ;
    YUGIOH_Card CardInPlayed;

//    enum _button{button1,button2} button;

    enum _turn{first,second} turn;
}Player;

typedef struct _state_game
{
	uint8_t action;
    uint8_t count_chain;
	Player player[2];
    enum _player_state {ready, first_player, second_player} player_state;
    enum _STATE {Drawn_Phase, Main_Phase, Battle_Phase, Winner} STATE;
    enum _BATTLE {
    	selection, counter_ATK,counter_DEF, chain_effect,calculate_damage, after_calculate
    } BATTLE;
    enum _MAIN {
    	await,select_position, check_card_type, advance_summon, activate_effect
    } MAIN;
    uint8_t test;
    uint8_t F_flag;
    uint8_t S_flag;

}State_game;

// RFID Handle Function
void RFID_Main_init(RFIDHandle *rfidmain);
void YUGIOH_card_copy(YUGIOH_Card *src, YUGIOH_Card *dst);
void YUGIOH_card_clear(YUGIOH_Card *card);
void RFID_Clear_Card_Bufffer(RFID *rfid);
void YUGIOH_card_register(RFIDHandle *rfidmain);
void YUGIOH_card_Buffer_Update(RFIDHandle *rfidmain);
// Card Hash Function
uint8_t CardHash_Encode(uint32_t key);
void YUGIOH_card_Load_Data(YUGIOH_Card *card);

//Game play hadler function
void YUGIOH_Clear_Card_Bufffer_Player(Player *player);
void YUGIOH_card_Buffer_Update_Player(Player *player);
void Player_Reading_Card(RFIDHandle *RFIDmain, State_game *state_game ,Player *player);
void GAME_PLAY_Management(RFIDHandle *RFIDmain,State_game *state_game);
void GAME_PLAY_CARD_PLAYED(RFIDHandle *RFIDmain,State_game *state_game);
void GAME_PLAY_Phase_Management(RFIDHandle *RFIDmain,State_game *state_game);
void GAME_PLAY_Main_Management(RFIDHandle *RFIDmain,State_game *state_game);
void YUGIOH_Clear_Card_Enemy_Player_Raigeki(Player *player);
void YUGIOH_Clear_Card_Enemy_Player_Dark_Hole(Player *player1,Player *player2);
void YUGIOH_To_GY(Player *player,YUGIOH_Card *card);
void YUGIOH_Clear_Card_All(Player *player);

//tuta display
void MainGUI();

#endif /* INC_FRA421_YUGIOH_H_ */
