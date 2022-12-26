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
	uint16_t name:4;
	enum _position{ATK,DEF,ATK_Down,DEF_Up} position :1;
	uint16_t attack:4;
	uint16_t defense:4;
	enum _effect{none,return_hand,no_damage} effect :1;

}Monster;

typedef struct _Magic
{
	uint16_t name:4;
	enum _position{Down,Up} position:1;
	enum _effect_type{destroy_all,destroy_enemy,reborn,summon,change_position} effec_type:1;

}Magic;

typedef struct _Trap
{
	uint16_t name:4;
	enum _position{Down,Up} position:1;
	enum _effect_type{gain_point,negate_atk} effec_type:1;

}Trap;




#endif /* SRC_SRAM4_H_ */
