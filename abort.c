/*
 * abort.c
 *
 * Contains output drive disable.
 * 
*/

#include <pic.h>
#include "globals.h";

// This function, TRG_ABORT (trigger abort) doesn't bother checking to
// see if any function or cycle is executing.  It will simply force
// the output STEP ports to off, disabling all STEP lines and switch
// the reference voltage to the Hold/Rest current value.
void TRG_ABORT(void)
{
	X_ABORT();	// Disable Drive and Timer Interrupts
	Y_ABORT();  // Disable Drive and Timer Interrupts
	Z_ABORT();  // Disable Drive and Timer Interrupts

}	// End of TRG_ABORT function
  
// This function will disable the X drive and disable the 
// timer interrupt.  The timer will be cleared so it can be 
// used again.
void X_ABORT(void)
{
	TMR2IE = 0; 	// Clear TMR2 to PR2 Match Interrupt Enable bit
	TMR2ON = 0; 	// Turn Timer OFF 
	X_RESET();		// Set RESET low, disable ENABLE
	RB1 = 0;		// Turn Step Off	
	TMR2 = 0x00;	// Clear X drive Timer
	system_status &= 0b11101111;// X-Drive not running
}	// End of X_ABORT function

// This function will disable the Y drive and disable the 
// timer interrupt.  The timer will be cleared so it can be 
// used again.
void Y_ABORT(void)
{
	TMR4IE = 0; 	// Clear TMR4 to PR4 Match Interrupt Enable bit
	TMR4ON = 0; 	// Turn Timer OFF 
	Y_RESET();		// Set RESET low, disable ENABLE
	RA5 = 0;		// Turn Step Off	
	TMR4 = 0x00;	// Clear Y drive Timer
	system_status &= 0b11011111;// Y-Drive not running
}	 // End of Y_ABORT function

// This function will disable the Z drive and disable the 
// timer interrupt.  The timer will be cleared so it can be 
// used again.
void Z_ABORT(void)
{
	TMR6IE = 0; 	// Clear TMR6 to PR6 Match Interrupt Enable bit
	TMR6ON = 0; 	// Turn Timer OFF 
	Z_RESET();		// Set RESET low, disable ENABLE
	RB5 = 0;		// Turn Step Off	
	TMR6 = 0x00;	// Clear Z drive Timer
	system_status &= 0b10111111;// Z-Drive not running
}	// End of Z_ABORT function