/*
 * FRA421_Yugioh.c
 *
 *  Created on: Dec 30, 2022
 *      Author: AthimetA
 */

#include "FRA421_Yugioh.h"

void YUGIOH_card_register(YUGIOH_Card *YUGIOHCard, Fra421_Card *Card)
{
	// Assign Card Data
	YUGIOHCard->cardData = Card->data;

	// For now just random
	YUGIOHCard->cardSignature = rand() % 256;
	YUGIOHCard->cardState = rand() % 3;
	YUGIOHCard->cardType = rand() % 2;
}
