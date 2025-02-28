/*
 * disp7.h
 *
 *  Created on: Jan 28, 2022
 *      Author: pablo
 */

#ifndef DISP7_H_
#define DISP7_H_

/*
 * Includes
 */

#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdbool.h>

#include "stm32g0xx.h"

/*
 * Enumerates
 */

typedef enum{
	DISP7_PIN_A	= 0x0,
	DISP7_PIN_B,
	DISP7_PIN_C,
	DISP7_PIN_D,
	DISP7_PIN_E,
	DISP7_PIN_F,
	DISP7_PIN_G,
	DISP7_PIN_dp,

	DISP7_PINS_QTD
}disp7_pins_e;

typedef enum{
	DISP7_SEG_A		= 1,
	DISP7_SEG_B		= (1<<1),
	DISP7_SEG_C		= (1<<2),
	DISP7_SEG_D		= (1<<3),
	DISP7_SEG_E		= (1<<4),
	DISP7_SEG_F		= (1<<5),
	DISP7_SEG_G		= (1<<6),
	DISP7_SEG_DP	= (1<<7),
	DISP7_SEG_ALL   = 0x7F
}disp7_segments_e;

typedef enum{
	DISP7_COMMON_CATHODE,
	DISP7_COMMON_ANODE
}disp7_type_e;

typedef enum{
	DISP_SEG_OFF,
	DISP_SEG_ON
}disp7_state_e;

/*
 * Function typedefs
 */
typedef void (*Disp7_WriteToPin_fxn)(disp7_pins_e DisplayPin, uint8_t ValueLevel);

/*
 * Structs and Unions
 */

typedef struct{
	Disp7_WriteToPin_fxn WriteToPinFxn;
	disp7_type_e eType;
	uint8_t u8Value;
	bool bDecimalPoint;
	bool bInitialized;
	disp7_state_e Segments[8];
}disp7_t;

typedef struct{
	Disp7_WriteToPin_fxn WriteToPinFxn;
	disp7_type_e eType;
}disp7_params_t;

/*
 * Functions Prototypes
 */

void disp7_init(disp7_t *disp7, disp7_params_t *params);

void disp7_print_number (disp7_t *disp7, uint8_t number, bool Dot);

void disp7_print_number_from_char (disp7_t *disp7, char number);

void disp7_print_raw(disp7_t *disp7, disp7_segments_e raw);

#endif /* DISP7_H_ */
