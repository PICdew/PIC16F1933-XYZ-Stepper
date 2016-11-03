/*
 * drive_mode.c
 *
 * Sets stepper drive modes.
 * 
*/

#include <pic.h>
#include "globals.h";

// This is the default setting for the motor drive.  It is done
// by selected a high level on the HALF/FULL input.  RESET/Enable
// will be left low so the L297 drives are off (ABCD = 0000).
void X_HALF_STEP(void)
{
	X_RESET();	// Reset translator to HOME position
	RB3 = 0; 	// CONTROL: chopper acts on INH1 and INH2
	RB2 = 1; 	// HALF/FULL set to HALF
}	//End X_HALF_STEP Function

// This is the default setting for the motor drive.  It is done
// by selected a high level on the HALF/FULL input.  RESET/Enable
// will be left low so the L297 drives are off (ABCD = 0000).
void Y_HALF_STEP(void)
{
	Y_RESET();	// Reset translator to HOME position
	RA7 = 0; 	// CONTROL: chopper acts on INH1 and INH2
	RA6 = 1; 	// HALF/FULL set to HALF
}	//End Y_HALF_STEP Function

// This is the default setting for the motor drive.  It is done
// by selected a high level on the HALF/FULL input.  RESET/Enable
// will be left low so the L297 drives are off (ABCD = 0000).
void Z_HALF_STEP(void)
{
	Z_RESET();	// Reset translator to HOME position
	RB7 = 0; 	// CONTROL: chopper acts on INH1 and INH2
	RB6 = 1; 	// HALF/FULL set to HALF
}	//End Z_HALF_STEP Function
	
// This function, X_RESET will set the X drive to its HOME
// position upon re-enable (ABCD = 0101).  This position needs 
// to be known when the motor is run in NORMAL DRIVE MODE 
// (also called "two-phase-on" drive) or when WAVE DRIVE MODE 
// (also called "one-phase-on" drive)is used.  This function 
// will disable the output drives (ABCD = 0000) while the L297
// is in RESET.
void X_RESET(void)
{
	RA0 = 0;	// Enable and RESET
	msDelay(1); // Delay for settle
}	// 	End X_RESET Function

// This function, Y_RESET will set the Y drive to its HOME
// position upon re-enable (ABCD = 0101).  This position needs 
// to be known when the motor is run in NORMAL DRIVE MODE 
// (also called "two-phase-on" drive) or when WAVE DRIVE MODE 
// (also called "one-phase-on" drive)is used.  This function 
// will disable the output drives (ABCD = 0000) while the L297
// is in RESET.
void Y_RESET(void)
{
	RA1 = 0;	// Enable and RESET
	msDelay(1); // Delay for settle
}	// 	End Y_RESET Function

// This function, Z_RESET will set the Z drive to its HOME
// position upon re-enable (ABCD = 0101).  This position needs 
// to be known when the motor is run in NORMAL DRIVE MODE 
// (also called "two-phase-on" drive) or when WAVE DRIVE MODE 
// (also called "one-phase-on" drive)is used.  This function 
// will disable the output drives (ABCD = 0000) while the L297
// is in RESET.
void Z_RESET(void)
{
	RA3 = 0;	// Enable and RESET
	msDelay(1); // Delay for settle
}	// 	End Z_RESET Function
	
