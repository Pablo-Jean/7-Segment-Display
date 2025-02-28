/*
 * disp7.c
 *
 *  Created on: Jan 28, 2022
 *      Author: pablo
 */


#include "disp7.h"

/*
 * Macros
 */

#define DISP_OFF		0x00

/*
 * Private Variables
 */

// Segments mapping for 0~9 and A~F
// with this map, the code is short, but at the cost of more
// memory
const uint8_t disp7_map[] = {
		DISP7_SEG_A | DISP7_SEG_B | DISP7_SEG_C | DISP7_SEG_D | DISP7_SEG_E | DISP7_SEG_F,	// 0
		DISP7_SEG_B | DISP7_SEG_C,															// 1
		DISP7_SEG_A | DISP7_SEG_B | DISP7_SEG_G | DISP7_SEG_E | DISP7_SEG_D, 				// 2
		DISP7_SEG_A | DISP7_SEG_B | DISP7_SEG_C | DISP7_SEG_D | DISP7_SEG_G, 				// 3
		DISP7_SEG_F | DISP7_SEG_G | DISP7_SEG_B | DISP7_SEG_C,				 				// 4
		DISP7_SEG_A | DISP7_SEG_F | DISP7_SEG_G | DISP7_SEG_C | DISP7_SEG_D, 				// 5
		DISP7_SEG_A | DISP7_SEG_C | DISP7_SEG_D | DISP7_SEG_E | DISP7_SEG_F | DISP7_SEG_G,	// 6
		DISP7_SEG_A | DISP7_SEG_B | DISP7_SEG_C,											// 7
		DISP7_SEG_ALL,																		// 8
		DISP7_SEG_A | DISP7_SEG_B | DISP7_SEG_C | DISP7_SEG_D | DISP7_SEG_F | DISP7_SEG_G,	// 9
		DISP7_SEG_A | DISP7_SEG_B | DISP7_SEG_C | DISP7_SEG_E | DISP7_SEG_F | DISP7_SEG_G,	// A
		DISP7_SEG_F | DISP7_SEG_E | DISP7_SEG_D | DISP7_SEG_C | DISP7_SEG_G,				// B
		DISP7_SEG_A | DISP7_SEG_F | DISP7_SEG_E | DISP7_SEG_D,								// C
		DISP7_SEG_B | DISP7_SEG_C | DISP7_SEG_D | DISP7_SEG_E | DISP7_SEG_G,				// D
		DISP7_SEG_A | DISP7_SEG_F | DISP7_SEG_E | DISP7_SEG_D | DISP7_SEG_G,				// E
		DISP7_SEG_A | DISP7_SEG_F | DISP7_SEG_E | DISP7_SEG_G								// G
};

/*
 * Privates
 */

// sets a gpio from the dispaly
// its automatically set the value if the display is Commom Anode or
// Commom Cathode
void __set_disp_pin_state(disp7_t *disp7, disp7_pins_e Pin, uint8_t set){
	// change GPIO state if type is COMMON_ANODE< the XOR (^) will revert
	// the value (remember : ANODE is on when gpio in 0
	disp7->WriteToPinFxn(Pin, set^disp7->eType);
}

// set on the display the desired segments
// each bit of segments is associated with a segment
// in order: 0 is assigned to segment A, 1 to segment B,
// until bit 8 that is assigned to the decimal point
void __set_segments(disp7_t *disp7, disp7_segments_e segments){
	disp7_pins_e i;

	// loop to define all gpio pins of the display
	for (i=DISP7_PIN_A ; i<DISP7_PINS_QTD ; i++){
		// this operation will check if the corresponding bit is set
		// if is set the LED is turned on
		if ((segments&(1<<i)) > 0){
			__set_disp_pin_state(disp7, i, DISP_SEG_ON);
			disp7->Segments[i] = DISP_SEG_ON;
		}
		// else, the led will turned off
		else{
			__set_disp_pin_state(disp7, i, DISP_SEG_OFF);
			disp7->Segments[i] = DISP_SEG_OFF;
		}
	}
}

/*
 * Publics
 */

// default parameters of display, defined as:
// 	segments : all OFF
//  gpio : all zero
//  type : DISP7_COMMON_CATHODE
void disp7_init (disp7_t *disp7, disp7_params_t *params){
	assert (disp7 != NULL);
	assert (params != NULL);
	assert (params->WriteToPinFxn != NULL);

	// reset all values to 0
	memset(disp7, 0, sizeof(disp7_t));
	disp7->WriteToPinFxn = params->WriteToPinFxn;
	disp7->eType = params->eType;

	disp7->bDecimalPoint = 0;
	disp7->u8Value = 0xFF;
	disp7->bInitialized = true;
}

// prints a number between 0~F, other numbers will turnoff the display
void disp7_print_number (disp7_t *disp7, uint8_t number, bool Dot){
	disp7_segments_e dp = 0;

	assert(disp7 != NULL);
	assert(disp7->bInitialized == true);

	if (Dot == true){
		dp = DISP7_SEG_DP;
	}
	disp7->u8Value = number;
	// check if number insn't out of the array
	if (number < sizeof(disp7_map)){
		__set_segments(disp7, disp7_map[number] | dp);
	}
	else{
		__set_segments(disp7, DISP_OFF | dp);
	}
}

// prints a number between 0~F with the decimal point on
// if numbers is above 0xF, the display will show only the decimal point
void disp7_print_number_from_char (disp7_t *disp7, char symbol){
	uint8_t number;
	bool IsDot = false;

	assert(disp7 != NULL);
	assert(disp7->bInitialized == true);

	if (symbol >= '0' && symbol <= '9'){
		number = symbol - '0';
	}
	else if (symbol >= 'A' && symbol <= 'F'){
		number = symbol - 'A';
	}
	else if (symbol == '.' || symbol == ','){
		number = disp7->u8Value;
		IsDot = true;
	}
	else{
		number = 0xFF;
		IsDot = disp7->bDecimalPoint;
	}

	disp7_print_number(disp7, number, IsDot);
}

// prints a raw value, with the bits assigned in order with segments
// example, 0 assigned to A, 1 to B, until, 7 to G and 8 to decimal point
void disp7_print_raw(disp7_t *disp7, disp7_segments_e raw){
	assert(disp7 != NULL);
	assert(disp7->bInitialized == true);

	__set_segments(disp7, raw);
}
