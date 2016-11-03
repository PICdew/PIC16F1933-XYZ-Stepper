/*
 * drive_start.c
 *
 * Sets up Timer and drive for Stepper Channels.
 * 
*/

#include <pic.h>
#include "globals.h";

	volatile unsigned int X_location = 0;  	// this is the X axis current location from X_Home
	volatile unsigned int Y_location = 0;  	// this is the Y axis current location from Y_Home
	volatile unsigned int Z_location = 0;  	// this is the Z axis current location from Z_Home

// This function, X_START sets up the timers and enables the
// drive for the X stepper channel.
void X_START(void)
{
	X_HALF_STEP();	// X HALF STEP MODE (default drive mode: see drive_mode.c)
	PR2 = get_period(X_min_speed); // Set Timer2 Period
	
	// Set L297 vref if needed
	if( working_vref != X_vref)
	{
		working_vref = X_vref;	// update working_vref for X drive (see vref.c)
		SET_VREF();				// write X vref HW (see vref.c)
	}	
			
	if((system_status & 0x08) != 0x08)
	{
		system_status |= 0x10;  // X-Drive running
		RA0 = 1;   	// Enable RESET Line
		msDelay(10);// Delay for RESET/Enable
		TMR2IF = 0; // Clear Interrupt Flag
		TMR2IE = 1; // TMR2 to PR2 Match Interrupt Enable bit
		TMR2ON = 1; // Turn Timer On
		RB1 = 1;	// Turn Step On
	}	

}	// End of X_START function

// This function, Y_START sets up the timers and enables the
// drive for the Y stepper channel.
void Y_START(void)
{
	Y_HALF_STEP();	// Y HALF STEP MODE (default drive mode: see drive_mode.c)
	PR4 = get_period(Y_min_speed); // Set Timer4 Period

	// Set L297 vref if needed
	if( working_vref != Y_vref)
	{
		working_vref = Y_vref;	// update working_vref for Y drive (see vref.c)
		SET_VREF();				// write Y vref HW (see vref.c)
	}	
			
	if((system_status & 0x08) != 0x08)
	{
		system_status |= 0x20;  // Y-Drive running
		RA1 = 1;	// Enable RESET Line
		msDelay(10);// Delay for RESET/Enable
		TMR4IF = 0; // Clear Interrupt Flag
		TMR4IE = 1; // TMR4 to PR4 Match Interrupt Enable bit
		TMR4ON = 1; // Turn Timer On
		RA5 = 1;	// Turn Step On
	}	

}	// End of Y_START function

// This function, Z_START sets up the timers and enables the
// drive for the Z stepper channel.
void Z_START(void)
{
	Z_HALF_STEP();	// Z HALF STEP MODE (default drive mode: see drive_mode.c)
	PR6 = get_period(Z_min_speed); // Set Timer6 Period
	
	// Set L297 vref if needed
	if( working_vref != Z_vref)
	{
		working_vref = Z_vref;	// update working_vref for Z drive (see vref.c)
		SET_VREF();				// write Z vref HW (see vref.c)
	}	
			
	if((system_status & 0x08) != 0x08)
	{
		system_status |= 0x40;  // Z-Drive running
		RA3 = 1;	// Enable RESET Line
		msDelay(10);// Delay for RESET/Enable
		TMR6IF = 0; // Clear Interrupt Flag
		TMR6IE = 1; // TMR6 to PR6 Match Interrupt Enable bit
		TMR6ON = 1; // Turn Timer On
		RB5 = 1;	// Turn Step On
	}	

}	// End of Z_START function

//  This is the serial interface access to read the current X location
//  with respect to HOME
void read_X_position(void)
{
	txfifo[0] = 2;				// Returning 2 chars
	txfifo[1] = (unsigned char)(X_location >> 8 & 0xff);// X_location MSB
	txfifo[2] = (unsigned char)(X_location & 0xff); 	// X_location LSB
}	// End of read_X_location function

//  This is the serial interface access to read the current Y location
//  with respect to HOME
void read_Y_position(void)
{
	txfifo[0] = 2;				// Returning 2 chars
	txfifo[1] = (unsigned char)(Y_location >> 8 & 0xff);// Y_location MSB
	txfifo[2] = (unsigned char)(Y_location & 0xff); 	// Y_location LSB
}	// End of read_Y_location function

//  This is the serial interface access to read the current Z location
//  with respect to HOME
void read_Z_position(void)
{
	txfifo[0] = 2;				// Returning 2 chars
	txfifo[1] = (unsigned char)(Z_location >> 8 & 0xff);// Z_location MSB
	txfifo[2] = (unsigned char)(Z_location & 0xff); 	// Z_location LSB
}	// End of read_Z_location function

void write_X_position(void)
{
	X_location	= (unsigned int)((rxfifo[3] << 8) | rxfifo[4]);
}	// End of write_X_position function	

void write_Y_position(void)
{
	Y_location	= (unsigned int)((rxfifo[3] << 8) | rxfifo[4]);
}	// End of write_Y_position function	

void write_Z_position(void)
{
	Z_location	= (unsigned int)((rxfifo[3] << 8) | rxfifo[4]);
}	// End of write_Z_position function		