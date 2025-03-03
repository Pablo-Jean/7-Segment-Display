/*
 * disp7.c
 *
 *  Created on: Jan 28, 2022
 *      Author: pablo
 */


#include "disp7.h"
#include "disp7_sweep.h"

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

/* Routiner for sweep */

void disp7Sweep_init(disp7_sweep_t *Disp7Sweep){
	assert (Disp7Sweep != NULL);
	assert (Disp7Sweep->bInitialized == false);

	Disp7Sweep->bInitialized = true;
	Disp7Sweep->psFirstDisp = NULL;
	Disp7Sweep->psActDisp = NULL;
	Disp7Sweep->u32RegDisplays = 0;
}

void disp7Sweep_add_display(disp7_sweep_t *Disp7Sweep, disp7_alone_t *Display){
	assert (Disp7Sweep != NULL);
	assert (Disp7Sweep->bInitialized == true);
	assert(Display != NULL);

	if (Disp7Sweep->psFirstDisp == NULL){
		Disp7Sweep->psFirstDisp = Display;
		Disp7Sweep->psActDisp = Display;
	}
	else{
		disp7_alone_t *temp;
		temp = Disp7Sweep->psFirstDisp;
		while(temp->_intern.next != NULL){
			temp = temp->_intern.next;
		}
		temp->_intern.next = Display;
	}
	Display->_intern.u8Value = 0xFF;
	Display->_intern.bDot = false;
	Display->_intern.next = NULL;
	Disp7Sweep->u32RegDisplays++;
}

void disp7Sweep_print_number(disp7_sweep_t *Disp7Sweep, uint32_t Number){
	disp7_alone_t *Display;

	assert (Disp7Sweep != NULL);
	assert (Disp7Sweep->bInitialized == true);

	Display = Disp7Sweep->psFirstDisp;
	// iterate for all displays, when reachs zero, fill display with 0xff
	// Starts on the last significant bit
	// TODO: Implement an option to print zero on left zeroes
	while (Display != NULL){
		if (Number > 0){
			Display->_intern.u8Value = Number % 10;
			// removes less significant digit
			Number = Number / 10;
		}
		else{
			// value 0xFF will turnoff display
			Display->_intern.u8Value = 0xFF;
		}
		// aways disable the dot
		Display->_intern.bDot = false;
		Display = (disp7_alone_t*)Display->_intern.next;
	}
}

void Disp7Sweep_set_dot(disp7_sweep_t *Disp7Sweep, uint8_t index, bool DotEn){
	uint8_t i;
	disp7_alone_t *Display;

	assert (Disp7Sweep != NULL);
	assert (Disp7Sweep->bInitialized == true);

	i = 0;
	Display = Disp7Sweep->psFirstDisp;
	while (Display != NULL && i != index ){
		Display = (disp7_alone_t*)Display->_intern.next;
		i++;
	}
	if (Display != NULL){
		Display->_intern.bDot = DotEn;
	}
}

void Disp7Sweep_print_string(disp7_sweep_t *Disp7Sweep, char *Str){
	disp7_alone_t *Display;
	size_t StrLen;
	int8_t i;

	StrLen = strlen(Str);

	assert (Disp7Sweep != NULL);
	assert (Disp7Sweep->bInitialized == true);
	assert(Str != NULL);
	assert(StrLen > 0);

	Display = Disp7Sweep->psFirstDisp;
	// iterate for all displays, when reachs zero, fill display with 0xff
	// TODO: Implement an option to print zero on left zeroes
	i = StrLen - 1;
	while (Display != NULL){
		// Starts for the less significant digit
		if (i > -1){
			if (Str[i] >= '0' && Str[i] <= '9'){
				Display->_intern.u8Value = (uint8_t)(Str[i] - '0');
			}
			else{
				Display->_intern.u8Value = 0xFF;
			}
		}
		else{
			// value 0xFF will turnoff display
			Display->_intern.u8Value = 0xFF;
		}
		// check if value as a dot
		if (Str[i] == '.' || Str[i] == ','){
			// when detect a dot, do not go to next display
			Display->_intern.bDot = true;
		}
		else{
			Display = (disp7_alone_t*)Display->_intern.next;
			if (Display != NULL){
				Display->_intern.bDot = false;
			}
		}
		i--;
	}
}

void disp7Sweep_timer_interrupt(disp7_sweep_t *Disp7Sweep){
	disp7_alone_t *Display;
	assert (Disp7Sweep != NULL);

	if (Disp7Sweep->bInitialized == false || Disp7Sweep->psActDisp == NULL){
		return;
	}

	Display = Disp7Sweep->psActDisp;
	Display->fxnControlDisplay(false);
	Display = (disp7_alone_t*)Display->_intern.next;
	if (Display == NULL){
		Display = Disp7Sweep->psFirstDisp;
	}
	disp7_print_number(Display->psDisplay, Display->_intern.u8Value, Display->_intern.bDot);
	Display->fxnControlDisplay(true);
	Disp7Sweep->psActDisp = Display;
}
