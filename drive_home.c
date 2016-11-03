/*
 * drive_motor.c
 *
 * Functions move XY and Z to Home positions
 * 
*/

#include <pic.h>
#include "globals.h";

// This function, X_HOME drives the stepper until X-H (X Home, RC0)
// goes low.  It will also reset X_location.  X_min_speed will be
// used as the stepper speed control.
void X_HOME(void)
{
	read_limit_status(); // Check port to see if Home
	
	if((limit_status & 0x01) != 0x01) // if X not home
	{
		TRG_ABORT();	// Abort all XYZ movement (see abort.c)

		RB0 = 0; 	// Counter clockwise
		X_START();	// Start Drive HOME
		
		if((system_status & 0x08) != 0x08)// Check for period error
		{
			// while X not home and not Aborted
			while( ((limit_status & 0x01) != 0x01) && ((system_status & 0x10) == 0x10)) 
			{
				limit_status = ~(PORTC);	 // Check port to see if Home
			}
			
			X_ABORT(); // Disable Drive and interrupts
		
			if((limit_status & 0x01) == 0x01)	// If it made it HOME
				X_new_location = X_location = 0;// set X axis location to X_Home
		}
	
	}			

}	// End of X_HOME function

// This function, Y_HOME drives the stepper until Y-H (Y Home, RC2)
// goes low.  It will also reset Y_location.  Y_min_speed will be
// used as the stepper speed control.
void Y_HOME(void)
{
	read_limit_status(); // Check port to see if Home
	
	if((limit_status & 0x04) != 0x04) // if Y not home
	{
		TRG_ABORT();	// Abort all XYZ movement (see abort.c)

		RA4 = 0; 	// Counter clockwise
		Y_START();	// Start Drive HOME
		
		if((system_status & 0x08) != 0x08)// Check for period error
		{
			 // while Y not home and not Aborted
			while( ((limit_status & 0x04) != 0x04) && ((system_status & 0x20) == 0x20))
			{
				limit_status = ~(PORTC);	 // Check port to see if Home
			}
			
			Y_ABORT(); // Disable Drive and interrupts
			
			if((limit_status & 0x04) == 0x04)	// If it made it HOME
				Y_new_location = Y_location = 0;// set Y axis location to Y_Home
		}
	
	}			

}	// End of Y_HOME function

// This function, Z_HOME drives the stepper until Z-H (Z Home, RC4)
// goes low.  It will also reset Z_location.  Z_min_speed will be
// used as the stepper speed control.
void Z_HOME(void)
{
	read_limit_status(); // Check port to see if Home
	
	if((limit_status & 0x10) != 0x10) // if Z not home
	{
		TRG_ABORT();	// Abort all XYZ movement (see abort.c)

		RB4 = 0; 	// Counter clockwise
		Z_START();	// Start Drive HOME
		
		if((system_status & 0x08) != 0x08)// Check for period error
		{
			// while Z not home and not Aborted
			while( ((limit_status & 0x10) != 0x10) && ((system_status & 0x40) == 0x40)) 
			{
				limit_status = ~(PORTC);	 // Check port to see if Home
			}
			
			Z_ABORT(); // Disable Drive and interrupts
			
			if((limit_status & 0x10) == 0x10)	// If it made it HOME
				Z_new_location = Z_location = 0;// set Y axis location to Y_Home
		}
	}			

}	// End of Z_HOME function

