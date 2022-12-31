/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2022 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "string.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
//#include "card.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

#ifndef HSEM_ID_0
#define HSEM_ID_0 (0U) /* HW semaphore 0*/
#endif
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
#if defined ( __ICCARM__ ) /*!< IAR Compiler */
#pragma location=0x30000000
ETH_DMADescTypeDef  DMARxDscrTab[ETH_RX_DESC_CNT]; /* Ethernet Rx DMA Descriptors */
#pragma location=0x30000200
ETH_DMADescTypeDef  DMATxDscrTab[ETH_TX_DESC_CNT]; /* Ethernet Tx DMA Descriptors */

#elif defined ( __CC_ARM )  /* MDK ARM Compiler */

__attribute__((at(0x30000000))) ETH_DMADescTypeDef  DMARxDscrTab[ETH_RX_DESC_CNT]; /* Ethernet Rx DMA Descriptors */
__attribute__((at(0x30000200))) ETH_DMADescTypeDef  DMATxDscrTab[ETH_TX_DESC_CNT]; /* Ethernet Tx DMA Descriptors */

#elif defined ( __GNUC__ ) /* GNU Compiler */
ETH_DMADescTypeDef DMARxDscrTab[ETH_RX_DESC_CNT] __attribute__((section(".RxDecripSection"))); /* Ethernet Rx DMA Descriptors */
ETH_DMADescTypeDef DMATxDscrTab[ETH_TX_DESC_CNT] __attribute__((section(".TxDecripSection"))); /* Ethernet Tx DMA Descriptors */

#endif

ETH_TxPacketConfig TxConfig;

ETH_HandleTypeDef heth;

UART_HandleTypeDef huart3;

PCD_HandleTypeDef hpcd_USB_OTG_FS;

/* USER CODE BEGIN PV */
static enum {
	first_player, second_player
} player_state = first_player;
static enum {
	Drawn_Phase, Main_Phase, Battle_Phase, Chain_Phase
} STATE = Drawn_Phase;
static enum {
	selection, counter1, calculate, counter2
} BATTLE = selection;
static enum {
	player1_chain, player2_chain
} CHAIN = player1_chain;
static enum {
	check_card, set_monster, set_spell, set_trap
} MAIN = set_monster;

uint32_t stack_chain[10] = { 0 };
uint8_t end = 0;

uint8_t button_ATK = 0;
uint8_t button_DEF = 0;

typedef struct _Player {
	uint16_t life_point;
	uint8_t monster[3];
	uint8_t trapAndSpell[3];
	uint16_t GY[10];
	enum _button {
		button1, button2
	} button;
} Player;

Player player1 = { 4000, { 0 }, { 0 }, { 0 }, button1 };
Player player2 = { 4000, { 0 }, { 0 }, { 0 }, button2 };
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_ETH_Init(void);
static void MX_USART3_UART_Init(void);
static void MX_USB_OTG_FS_PCD_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
 * @brief  The application entry point.
 * @retval int
 */
int main(void) {
	/* USER CODE BEGIN 1 */

	/* USER CODE END 1 */
	/* USER CODE BEGIN Boot_Mode_Sequence_0 */
	int32_t timeout;
	/* USER CODE END Boot_Mode_Sequence_0 */

	/* USER CODE BEGIN Boot_Mode_Sequence_1 */
	/* Wait until CPU2 boots and enters in stop mode or timeout*/
	timeout = 0xFFFF;
	while ((__HAL_RCC_GET_FLAG(RCC_FLAG_D2CKRDY) != RESET) && (timeout-- > 0))
		;
	if (timeout < 0) {
		Error_Handler();
	}
	/* USER CODE END Boot_Mode_Sequence_1 */
	/* MCU Configuration--------------------------------------------------------*/

	/* Reset of all peripherals, Initializes the Flash interface and the Systick. */
	HAL_Init();

	/* USER CODE BEGIN Init */

	/* USER CODE END Init */

	/* Configure the system clock */
	SystemClock_Config();
	/* USER CODE BEGIN Boot_Mode_Sequence_2 */
	/* When system initialization is finished, Cortex-M7 will release Cortex-M4 by means of
	 HSEM notification */
	/*HW semaphore Clock enable*/
	__HAL_RCC_HSEM_CLK_ENABLE();
	/*Take HSEM */
	HAL_HSEM_FastTake(HSEM_ID_0);
	/*Release HSEM in order to notify the CPU2(CM4)*/
	HAL_HSEM_Release(HSEM_ID_0, 0);
	/* wait until CPU2 wakes up from stop mode */
	timeout = 0xFFFF;
	while ((__HAL_RCC_GET_FLAG(RCC_FLAG_D2CKRDY) == RESET) && (timeout-- > 0))
		;
	if (timeout < 0) {
		Error_Handler();
	}
	/* USER CODE END Boot_Mode_Sequence_2 */

	/* USER CODE BEGIN SysInit */

	/* USER CODE END SysInit */

	/* Initialize all configured peripherals */
	MX_GPIO_Init();
	MX_ETH_Init();
	MX_USART3_UART_Init();
	MX_USB_OTG_FS_PCD_Init();
	/* USER CODE BEGIN 2 */

	/* USER CODE END 2 */

	/* Infinite loop */
	/* USER CODE BEGIN WHILE */
	while (1) {
		switch (player_state) {
		case first_player:
			loop_game(&player1, &player2);
			break;
		case second_player:
			loop_game(&player2, &player1);
			break;
		}
		/* USER CODE END WHILE */

		/* USER CODE BEGIN 3 */
	}
	/* USER CODE END 3 */
}

/**
 * @brief System Clock Configuration
 * @retval None
 */
void SystemClock_Config(void) {
	RCC_OscInitTypeDef RCC_OscInitStruct = { 0 };
	RCC_ClkInitTypeDef RCC_ClkInitStruct = { 0 };

	/** Supply configuration update enable
	 */
	HAL_PWREx_ConfigSupply(PWR_DIRECT_SMPS_SUPPLY);

	/** Configure the main internal regulator output voltage
	 */
	__HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE0);

	while (!__HAL_PWR_GET_FLAG(PWR_FLAG_VOSRDY)) {
	}

	/** Macro to configure the PLL clock source
	 */
	__HAL_RCC_PLL_PLLSOURCE_CONFIG(RCC_PLLSOURCE_HSE);

	/** Initializes the RCC Oscillators according to the specified parameters
	 * in the RCC_OscInitTypeDef structure.
	 */
	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
	RCC_OscInitStruct.HSEState = RCC_HSE_BYPASS;
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
	RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
	RCC_OscInitStruct.PLL.PLLM = 1;
	RCC_OscInitStruct.PLL.PLLN = 120;
	RCC_OscInitStruct.PLL.PLLP = 2;
	RCC_OscInitStruct.PLL.PLLQ = 2;
	RCC_OscInitStruct.PLL.PLLR = 2;
	RCC_OscInitStruct.PLL.PLLRGE = RCC_PLL1VCIRANGE_3;
	RCC_OscInitStruct.PLL.PLLVCOSEL = RCC_PLL1VCOWIDE;
	RCC_OscInitStruct.PLL.PLLFRACN = 0;
	if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) {
		Error_Handler();
	}

	/** Initializes the CPU, AHB and APB buses clocks
	 */
	RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK
			| RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2 | RCC_CLOCKTYPE_D3PCLK1
			| RCC_CLOCKTYPE_D1PCLK1;
	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
	RCC_ClkInitStruct.SYSCLKDivider = RCC_SYSCLK_DIV1;
	RCC_ClkInitStruct.AHBCLKDivider = RCC_HCLK_DIV2;
	RCC_ClkInitStruct.APB3CLKDivider = RCC_APB3_DIV2;
	RCC_ClkInitStruct.APB1CLKDivider = RCC_APB1_DIV2;
	RCC_ClkInitStruct.APB2CLKDivider = RCC_APB2_DIV2;
	RCC_ClkInitStruct.APB4CLKDivider = RCC_APB4_DIV2;

	if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4) != HAL_OK) {
		Error_Handler();
	}
}

/**
 * @brief ETH Initialization Function
 * @param None
 * @retval None
 */
static void MX_ETH_Init(void) {

	/* USER CODE BEGIN ETH_Init 0 */

	/* USER CODE END ETH_Init 0 */

	static uint8_t MACAddr[6];

	/* USER CODE BEGIN ETH_Init 1 */

	/* USER CODE END ETH_Init 1 */
	heth.Instance = ETH;
	MACAddr[0] = 0x00;
	MACAddr[1] = 0x80;
	MACAddr[2] = 0xE1;
	MACAddr[3] = 0x00;
	MACAddr[4] = 0x00;
	MACAddr[5] = 0x00;
	heth.Init.MACAddr = &MACAddr[0];
	heth.Init.MediaInterface = HAL_ETH_RMII_MODE;
	heth.Init.TxDesc = DMATxDscrTab;
	heth.Init.RxDesc = DMARxDscrTab;
	heth.Init.RxBuffLen = 1524;

	/* USER CODE BEGIN MACADDRESS */

	/* USER CODE END MACADDRESS */

	if (HAL_ETH_Init(&heth) != HAL_OK) {
		Error_Handler();
	}

	memset(&TxConfig, 0, sizeof(ETH_TxPacketConfig));
	TxConfig.Attributes = ETH_TX_PACKETS_FEATURES_CSUM
			| ETH_TX_PACKETS_FEATURES_CRCPAD;
	TxConfig.ChecksumCtrl = ETH_CHECKSUM_IPHDR_PAYLOAD_INSERT_PHDR_CALC;
	TxConfig.CRCPadCtrl = ETH_CRC_PAD_INSERT;
	/* USER CODE BEGIN ETH_Init 2 */

	/* USER CODE END ETH_Init 2 */

}

/**
 * @brief USART3 Initialization Function
 * @param None
 * @retval None
 */
static void MX_USART3_UART_Init(void) {

	/* USER CODE BEGIN USART3_Init 0 */

	/* USER CODE END USART3_Init 0 */

	/* USER CODE BEGIN USART3_Init 1 */

	/* USER CODE END USART3_Init 1 */
	huart3.Instance = USART3;
	huart3.Init.BaudRate = 115200;
	huart3.Init.WordLength = UART_WORDLENGTH_8B;
	huart3.Init.StopBits = UART_STOPBITS_1;
	huart3.Init.Parity = UART_PARITY_NONE;
	huart3.Init.Mode = UART_MODE_TX_RX;
	huart3.Init.HwFlowCtl = UART_HWCONTROL_NONE;
	huart3.Init.OverSampling = UART_OVERSAMPLING_16;
	huart3.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
	huart3.Init.ClockPrescaler = UART_PRESCALER_DIV1;
	huart3.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
	if (HAL_UART_Init(&huart3) != HAL_OK) {
		Error_Handler();
	}
	if (HAL_UARTEx_SetTxFifoThreshold(&huart3, UART_TXFIFO_THRESHOLD_1_8)
			!= HAL_OK) {
		Error_Handler();
	}
	if (HAL_UARTEx_SetRxFifoThreshold(&huart3, UART_RXFIFO_THRESHOLD_1_8)
			!= HAL_OK) {
		Error_Handler();
	}
	if (HAL_UARTEx_DisableFifoMode(&huart3) != HAL_OK) {
		Error_Handler();
	}
	/* USER CODE BEGIN USART3_Init 2 */

	/* USER CODE END USART3_Init 2 */

}

/**
 * @brief USB_OTG_FS Initialization Function
 * @param None
 * @retval None
 */
static void MX_USB_OTG_FS_PCD_Init(void) {

	/* USER CODE BEGIN USB_OTG_FS_Init 0 */

	/* USER CODE END USB_OTG_FS_Init 0 */

	/* USER CODE BEGIN USB_OTG_FS_Init 1 */

	/* USER CODE END USB_OTG_FS_Init 1 */
	hpcd_USB_OTG_FS.Instance = USB_OTG_FS;
	hpcd_USB_OTG_FS.Init.dev_endpoints = 9;
	hpcd_USB_OTG_FS.Init.speed = PCD_SPEED_FULL;
	hpcd_USB_OTG_FS.Init.dma_enable = DISABLE;
	hpcd_USB_OTG_FS.Init.phy_itface = PCD_PHY_EMBEDDED;
	hpcd_USB_OTG_FS.Init.Sof_enable = DISABLE;
	hpcd_USB_OTG_FS.Init.low_power_enable = DISABLE;
	hpcd_USB_OTG_FS.Init.lpm_enable = DISABLE;
	hpcd_USB_OTG_FS.Init.battery_charging_enable = ENABLE;
	hpcd_USB_OTG_FS.Init.vbus_sensing_enable = ENABLE;
	hpcd_USB_OTG_FS.Init.use_dedicated_ep1 = DISABLE;
	if (HAL_PCD_Init(&hpcd_USB_OTG_FS) != HAL_OK) {
		Error_Handler();
	}
	/* USER CODE BEGIN USB_OTG_FS_Init 2 */

	/* USER CODE END USB_OTG_FS_Init 2 */

}

/**
 * @brief GPIO Initialization Function
 * @param None
 * @retval None
 */
static void MX_GPIO_Init(void) {
	GPIO_InitTypeDef GPIO_InitStruct = { 0 };

	/* GPIO Ports Clock Enable */
	__HAL_RCC_GPIOC_CLK_ENABLE();
	__HAL_RCC_GPIOH_CLK_ENABLE();
	__HAL_RCC_GPIOA_CLK_ENABLE();
	__HAL_RCC_GPIOB_CLK_ENABLE();
	__HAL_RCC_GPIOD_CLK_ENABLE();
	__HAL_RCC_GPIOG_CLK_ENABLE();

	/*Configure GPIO pins : NO2_Pin YES1_Pin */
	GPIO_InitStruct.Pin = NO2_Pin | YES1_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

	/*Configure GPIO pin : YES2_Pin */
	GPIO_InitStruct.Pin = YES2_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(YES2_GPIO_Port, &GPIO_InitStruct);

	/*Configure GPIO pin : NO1_Pin */
	GPIO_InitStruct.Pin = NO1_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(NO1_GPIO_Port, &GPIO_InitStruct);

}

/* USER CODE BEGIN 4 */
void loop_game(Player *player_ATK, Player *player_DEF) {
//	Player* ptr_atk = player_ATK;
	switch (STATE) {
	case Drawn_Phase:
		/*press botton*/
		if ((HAL_GPIO_ReadPin(YES1_GPIO_Port, YES1_Pin) == GPIO_PIN_RESET)
				& (player_ATK->button == button1)) {
			STATE = Main_Phase;
		} else if ((HAL_GPIO_ReadPin(YES2_GPIO_Port, YES2_Pin) == GPIO_PIN_RESET)
				& (player_ATK->button == button2)) {
			STATE = Main_Phase;
		}
//		STATE = Main_Phase;
		break;
	case Main_Phase:
		if (HAL_GPIO_ReadPin(YES2_GPIO_Port, YES1_Pin) == GPIO_PIN_RESET) {
			player_ATK->life_point = 2000;
			STATE = Drawn_Phase;
		}
		/*check card type and what is card*/
		switch (MAIN) {
		case check_card:
			if (monster card) {
				if (player_ATK.monster >= 3) {
					print("can't add more monster");
				} else {
					/*chain*/
					MAIN = set_monster;

				}
			} else if (spell_card) {
				if (player_ATK.trapAndSpell >= 3) {
					print("can't add more spell");
				} else {
					/*chain*/
					MAIN = set_spell;
				}
			} else if (trap_card) {
				if (player_ATK.trapAndSpell >= 3) {
					print("can't add more spell");
				} else {
					/*chain*/
					MAIN = set_trap;
				}
			} else if (button_ATK == GPIO_PIN_RESET) {
				STATE = Battle_Phase;
			}
			break;
		case set_monster:
			player_ATK.monster[card.position] = monster;
			MAIN = check_card;
			break;
		case set_spell:
			player_ATK.trapAndSpell[position] = spell;
			if (spell duble) {
				if (magic.effect_type == destroy_all) {
					/*clear monster data*/
					/*for i*/
					player_ATK.GY = 0;
					player_DEF.monster = 0;
					/*add monster to GY1 and GY2*/
				} else if (magic.effect_type == destroy_enemy) {
					/*clear monster enemy data*/
					player_DEF.monster = 0;
				} else if (magic.effect_type == reborn) {
					/*reborn from GY*/

				} else if (magic.effect_type == summon) {
					/*place > level 5 in some RFID by touch the card*/
				} else if (magic.effect_type == change_position) {
					/*change enemy monster def to atk*/
					if (magic.monster == defense) {
						/*change*/
					}
				}
			}
			MAIN = check_card;
			break;
		case set_trap:
			player_ATK.trapAndSpell[card.position] = trap;
			break;
		}
		break;
	case Battle_Phase:
		/*After card touch check monster than*/
		if (RFID_Read) {
			switch (BATTLE) {
			case selection:
				if (RFID_Read) {
					Monster_ATK, Monster_DEF = read;
					if (Monster_ATK.position == ATK) {
						if (Monster_DEF.position == DEF) {
							/*flip*/
							Monster_DEF.positon == DEF_Up;
						}
						BATTLE = counter1;
					} else {
						print("this monster is not in ATK position");
					}
				} else if (button) {
					STATE = Drawn_Phase;
					if (player_state == first_player) {
						player_state = second_player;
					} else {
						player_state = first_player;
					}
				}
				break;
			case counter1:
				/*check RFID*/
				if (monster.effect = no_damage) {
					monster.attacker = 1;
					monster.status = GY;
					BATTLE = selection;
				} else if (trap.position == Up) {
					if (trap.turn == 1) {
						if (trap.type = gain_point) {
							player.life_point += player.monster * 300;
							BATTLE = selection;
						} else if (trap.type = negate_atk) {
							STATE = End_Phase;
						}
					}
				}
				break;
			case calculate:
				if (Monster_DEF.position == DEF_Up) {
					uint16_t result = Monster_ATK.attack - Monster_DEF.defense;
					if (result > 0) {
						Monster_DEF.status = GY2;
					} else if (result < 0) {
						Monster_ATK.status = GY1;
					} else {
						Monster_ATK.status = GY1;
						Monster_DEF.status = GY2;
					}
				} else if (Monster_DEF.position == ATK) {
					uint16_t result = Monster_ATK.attack - Monster_DEF.attack;
					if (result > 0) {
						player_DEF->life_point -= result;
						Monster_DEF.status = GY2;
					} else if (result < 0) {
						Monster_ATK.status = GY1;
					} else {
						Monster_ATK.status = GY1;
						Monster_DEF.status = GY2;
					}

				}

			}
		}
		break;
	case Chain_Phase:
		/**/
		if (button_DEF) {

		}
	}

}

//void battle( Monster_ATK, Monster_DEF) {
//	switch (BATTLE) {
//	case selection:
//		if (Monster_ATK.position == ATK) {
//			if (Monster_DEF.position == DEF) {
//				/*flip*/
//				Monster_DEF.positon == DEF_Up;
//			}
//			BATTLE = counter1;
//		}
//		break;
//	case counter1:
//		/*check RFID*/
//		if (monster.effect = no_damage) {
//			monster.attacker = 1;
//			monster.status = GY;
//			BATTLE = selection;
//		} else if (trap.position == Up) {
//			if (trap.turn == 1) {
//				if (trap.type = gain_point) {
//					player.life_point += player.monster * 300;
//					BATTLE = selection;
//				} else if (trap.type = negate_atk) {
//					STATE = End_Phase;
//				}
//			}
//		}
//		break;
//	case calculate:
//		if (Monster_DEF.position == DEF_Up) {
//			uint16_t result = Monster_ATK.attack - Monster_DEF.defense;
//			if (result > 0) {
//				Monster_DEF.status = GY2;
//			} else if (result < 0) {
//				Monster_ATK.status = GY1;
//			} else {
//				Monster_ATK.status = GY1;
//				Monster_DEF.status = GY2;
//			}
//		} else if (Monster_DEF.position == ATK) {
//			uint16_t result = Monster_ATK.attack - Monster_DEF.attack;
//			if (result > 0) {
//				Monster_DEF.status = GY2;
//			} else if (result < 0) {
//				Monster_ATK.status = GY1;
//			} else {
//				Monster_ATK.status = GY1;
//				Monster_DEF.status = GY2;
//			}
//
//		}
//
//	}
//	break;

//}

//void chain() {
//	uint8_t i = 0;
//	switch (CHAIN) {
//	case player1_chain:
//		if (button_DEF == yes) {
//			/*read RFID ifRFID is trap and turn == 1 stack += 1*/
//			stack_chain[i] = name;
//			i += 1;
//		} else {
//			i = 0;
//		}
//		break;
//	case player2_chain:
//		if(button)
//	}
//}
/* USER CODE END 4 */

/**
 * @brief  This function is executed in case of error occurrence.
 * @retval None
 */
void Error_Handler(void) {
	/* USER CODE BEGIN Error_Handler_Debug */
	/* User can add his own implementation to report the HAL error return state */
	__disable_irq();
	while (1) {
	}
	/* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
