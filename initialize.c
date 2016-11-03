/*
 * initialize.c
 *
 * Resets inputs and Outputs (not the serial interface).  This
 * will drive all stepper motors to their HOME positions and
 * returns the current FW version to show TX and RX are working.
 *
*/

#include <pic.h>
#include "globals.h"

// This function doesn't bother checking to see if any functions are executing.  
// It will initialize the drives by sending them to their HOME position and
// return the PIC FW version.
void initialize(void)
{	
	TRG_ABORT();			// Abort all XYZ movement (see abort.c)
	clear_system_status();	// Clear system status (see system_status.c)
	clear_limit_status();	// Clear limit status (see system_status.c)
	
	// Set up Stepper Drive Mode
	X_HALF_STEP();	// X HALF STEP MODE (default drive mode: see drive.mode.c)
	Y_HALF_STEP();	// Y HALF STEP MODE (default drive mode: see drive.mode.c)
	Z_HALF_STEP();	// Z HALF STEP MODE (default drive mode: see drive.mode.c)
	
	// Drive Motors HOME
	X_HOME();	// This will drive to X-H for a starting location (see home.c)
	Y_HOME();	// This will drive to Y-H for a starting location (see home.c)
	Z_HOME();	// This will drive to Z-H for a starting location (see home.c)

	// Return version of all this
	txfifo[0] = 7; 	// Return 7 Chars
	txfifo[1] = 'X';
	txfifo[2] = 'Y';
	txfifo[3] = 'Z';
	txfifo[4] = '_';
	txfifo[5] = '1';// Is this lame?  It's in ASCII.
	txfifo[6] = '.';// It may be...
	txfifo[7] = '0';// Okay, now it is for sure.
	
}	// End of Initialize function
   