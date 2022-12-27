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
	uint16_t defense:4;
	enum _effect{none,return_hand,no_damage} effect;

}Monster;

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

}Trap;




#endif /* SRC_SRAM4_H_ */
