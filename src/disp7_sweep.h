/*
 * disp7_sweep.h
 *
 *  Created on: Mar 2, 2025
 *      Author: pablo
 */

#ifndef DISP7_SWEEP_H_
#define DISP7_SWEEP_H_

/*
 * Includes
 */

#include "disp7.h"

/*
 * Macros
 */


/*
 * Enumerates
 */


/*
 * Typedefs
 */

typedef void (*disp7_sweep_ctrl_fnx)(uint8_t TurnOn);

/*
 * Structs
 */

typedef struct {
	disp7_t *psDisplay;
	disp7_sweep_ctrl_fnx fxnControlDisplay;

	struct {
		void *next;
		uint8_t u8Value;
		bool bDot;
	}_intern;
}disp7_alone_t;

typedef struct{
	uint32_t u32RegDisplays;
	disp7_alone_t *psActDisp;

	disp7_alone_t *psFirstDisp;
	bool bInitialized;
}disp7_sweep_t;

/*
 * Publics
 */

void disp7Sweep_init(disp7_sweep_t *Disp7Sweep);

void disp7Sweep_add_display(disp7_sweep_t *Disp7Sweep, disp7_alone_t *Display);

void disp7Sweep_print_number(disp7_sweep_t *Disp7Sweep, uint32_t Number);

void Disp7Sweep_set_dot(disp7_sweep_t *Disp7Sweep, uint8_t index, bool DotEn);

void Disp7Sweep_print_string(disp7_sweep_t *Disp7Sweep, char *Str);

void disp7Sweep_timer_interrupt(disp7_sweep_t *Disp7Sweep);

#endif /* DISP7_SWEEP_H_ */
