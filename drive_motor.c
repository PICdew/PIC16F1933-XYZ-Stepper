/*
 * drive_mode.c
 *
 * Sets stepper drive.
 * 
*/

#include <pic.h>
#include "globals.h";

	volatile unsigned int X_new_location = 0;  	// this is the new X axis location from X_Home
	volatile unsigned int Y_new_location = 0;  	// this is the new Y axis location from Y_Home
	volatile unsigned int Z_new_location = 0;  	// this is the new Z axis location from Z_Home
	
// This function, X_DRIVE will drive the x-axis to a new location.
// This value will be compared to X_location to detirmine drive
// direction.
void X_DRIVE(void)
{
	TRG_ABORT();	// Abort all XYZ movement (see abort.c)
		
	if(X_new_location != X_location)	// Verify need to move
	{
		// Select drive direction
		if( X_new_location <  X_location)
		{
			// Can't be less then HOME
			RB0 = 0; // DIR = Counter clockwise
		}	
		else
		{
			// Can't drive past X-FFH
			RB0 = 1; // DIR = clockwise
		}	
			
		limit_status = ~(PORTC);	// Read Limit Status
		X_START();	// Setup Timers and drive
			
		while( (X_new_location != X_location) && ((system_status & 0x10) == 0x10))
		{
			if((RB0) && ((limit_status & 0x02) == 0x02))
				system_status &= 0b11101111; // Can't drive past X-FFH
			if((!RB0) && ((limit_status & 0x01) == 0x01))
				system_status &= 0b11101111; // Can't drive past X-H
			
			limit_status = ~(PORTC);	// Read Limit Status
		}
		
		X_ABORT(); // Disable Drive and interrupts		
	}

}	// End of X_DRIVE function

// This function, Y_DRIVE will drive the x-axis to a new location.
// This value will be compared to Y_location to detirmine drive
// direction.
void Y_DRIVE(void)
{
	TRG_ABORT();	// Abort all XYZ movement (see abort.c)
		
	if(Y_new_location != Y_location)	// Verify need to move
	{
		// Select drive direction
		if( Y_new_location <  Y_location)
		{
			// Can't be less then HOME
			RA4 = 0; // DIR = Counter clockwise
		}	
		else
		{
			// Can't drive past Y-FFH
			RA4 = 1; // DIR = clockwise
		}
			
		limit_status = ~(PORTC);	// Read Limit Status	
		Y_START();	// Setup Timers and drive
				
		while( (Y_new_location != Y_location) && ((system_status & 0x20) == 0x20))
		{
			if((RA4) && ((limit_status & 0x08) == 0x08))
				system_status &= 0b11011111; // Can't drive past Y-FFH
			if((!RA4) && ((limit_status & 0x04) == 0x04))
				system_status &= 0b11011111; // Can't drive past Y-H
				
			limit_status = ~(PORTC);	// Read Limit Status
		}
		
		Y_ABORT(); // Disable Drive and interrupts		
	}

}	// End of Y_DRIVE function

// This function, Z_DRIVE will drive the x-axis to a new location.
// This value will be compared to Z_location to detirmine drive
// direction.
void Z_DRIVE(void)
{
	TRG_ABORT();	// Abort all XYZ movement (see abort.c)

	if(Z_new_location != Z_location)	// Verify need to move
	{			
		// Select drive direction
		if( Z_new_location <  Z_location)
		{
			// Can't be less then HOME
			RB4 = 0; // DIR = Counter clockwise
		}	
		else
		{	
			// Can't drive past Z-FFH
			RB4 = 1; // DIR = clockwise
		}	
		
		limit_status = ~(PORTC);	// Read Limit Status	
		Z_START();	// Setup Timers and drive		
	
		while( ( Z_new_location != Z_location) && ((system_status & 0x40) == 0x40))
		{
			if((RB4) && ((limit_status & 0x20) == 0x20))
				system_status &= 0b10111111; // Can't drive past Z-FFH
			if((!RB4) && ((limit_status & 0x10) == 0x10))
				system_status &= 0b10111111; // Can't drive past Z-H
				
			limit_status = ~(PORTC);	// Read Limit Status
		}
	
		Z_ABORT(); // Disable Drive and interrupts			
	}

}	// End of Z_DRIVE function


//  This is the serial interface to drive X to a new location.  
//  The format is:
//	[0] [1] [2] 'SNX' - Send to New X location
//  [3] X_new_location MSB
//  [4] X_new_location LSB
//  The function to drive the channel will be called
void send_new_X(void)
{
	X_new_location = (unsigned int)((rxfifo[3] << 8) | rxfifo[4]); // Update RAM
	X_DRIVE();	// Call drive
	
}	// End of send_new_X function	

//  This is the serial interface to drive Y to a new location.  
//  The format is:
//	[0] [1] [2] 'SNY' - Send to New Y location
//  [3] Y_new_location MSB
//  [4] Y_new_location LSB
//  The function to drive the channel will be called
void send_new_Y(void)
{
	Y_new_location = (unsigned int)((rxfifo[3] << 8) | rxfifo[4]); // Update RAM
	Y_DRIVE();	// Call drive
	
}	// End of send_new_Y function	

//  This is the serial interface to drive Z to a new location.  
//  The format is:
//	[0] [1] [2] 'SNZ' - Send to New Z location
//  [3] Y_new_location MSB
//  [4] Y_new_location LSB
//  The function to drive the channel will be called
void send_new_Z(void)
{
	Z_new_location = (unsigned int)((rxfifo[3] << 8) | rxfifo[4]); // Update RAM
	Z_DRIVE();	// Call drive
	
}	// End of send_new_Z function	
