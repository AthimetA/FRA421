/*
 * card.h
 *
 *  Created on: Oct 20, 2022
 *      Author: AlphaP
 */

#ifndef CARD_H
#define CARD_H

//#define TASK2_NO_HSEM
//#define TASK2_WITH_HSEM

typedef struct _Monster
{
	uint16_t name;
	uint8_t star;
	enum _position{ATK,DEF,ATK_Down,DEF_Up} position;
	uint16_t attack;
	uint16_t defense;
	enum _effect{none,return_hand,no_damage} effect;
	uint8_t attacker;
	enum _position{ATK,DEF,ATK_Down,DEF_Up} position;
	enum _status{live1,live2,GY1,GY2} status;

}Monster;

typedef struct _Player
{
	uint16_t life_point;
	uint8_t monster;
	uint8_t trapAndSpell;

}Player;

typedef struct _Magic
{
	uint16_t name;
	enum _position{Down,Up} position;
	enum _effect_type{destroy_all,destroy_enemy,reborn,summon,change_position} effec_type;

}Magic;

typedef struct _Trap
{
	uint16_t name;
	enum _position{Down,Up} position;
	enum _effect_type{gain_point,negate_atk} effec_type;
	uint8_t turn;

}Trap;




#endif /* SRC_SRAM4_H_ */
