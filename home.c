/*
 * home.c
 *
 * Functions move XY and Z to Home positions
 * 
*/

#include <pic.h>
#include "globals.h";

// This function, X_HOME drives the stepper until X-H (X Home, RC0)
// goes low.  It will also reset X_location.  X_max_speed will be
// used as the stepper speed control.
void X_HOME(void)
{
	read_limit_status(); // Check port to see if Home
	
	if((limit_status & 0x01) != 1) // if X not home
	{
		RB0 = 0; 	// Counter clockwise
		X_START();	// Setup Timers and drive
		
		if(limit_status & 0x40 != 0x40)
		{
			while( (limit_status & 0x01) != 1) // while X not home
			{
				read_limit_status(); // Check port to see if Home
			}
			
			X_ABORT(); // Disable Drive and interrupts
			X_location = 0;  // set X axis location to X_Home
		}
	}			

}	// End of X_HOME function

// This function, Y_HOME drives the stepper until Y-H (Y Home, RC2)
// goes low.  It will also reset Y_location.  Y_max_speed will be
// used as the stepper speed control.
void Y_HOME(void)
{
	read_limit_status(); // Check port to see if Home
	
	if((limit_status & 0x04) != 4) // if Y not home
	{
		RA4 = 0; 	// Counter clockwise
		Y_START();	// Setup Timers and drive
		
		if(limit_status & 0x40 != 0x40)
		{
			while( (limit_status & 0x04) != 4) // while Y not home
			{
				read_limit_status(); // Check port to see if Home
			}
			
			Y_ABORT(); // Disable Drive and interrupts
			Y_location = 0;  // set Y axis location to Y_Home
		}
	}			

}	// End of Y_HOME function

// This function, Z_HOME drives the stepper until Z-H (Z Home, RC4)
// goes low.  It will also reset Z_location.  Z_max_speed will be
// used as the stepper speed control.
void Z_HOME(void)
{
	read_limit_status(); // Check port to see if Home
	
	if((limit_status & 0x04) != 4) // if Z not home
	{
		RB4 = 0; 	// Counter clockwise
		Z_START();	// Setup Timers and drive
		
		if(limit_status & 0x40 != 0x40)
		{
			while( (limit_status & 0x10) != 0x10) // while Z not home
			{
				read_limit_status(); // Check port to see if Home
			}
			
			Z_ABORT(); // Disable Drive and interrupts
			Z_location = 0;  // set Y axis location to Y_Home
		}
	}			

}	// End of Z_HOME function

