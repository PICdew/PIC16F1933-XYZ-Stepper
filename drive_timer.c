/*
 * drive_timer.c
 *
 * Sets stepper drive timers.
 * 
*/

#include <pic.h>
#include "globals.h";

	volatile unsigned int X_max_speed = 2000;  	// this will X axis max step frequency (for ramp)
	volatile unsigned int Y_max_speed = 2000;  	// this will Y axis max step frequency (for ramp)
	volatile unsigned int Z_max_speed = 1000;  	// this will Z axis max step frequency (for ramp)
	
	volatile unsigned int X_min_speed = 1300;  	// this will X axis min step frequency
	volatile unsigned int Y_min_speed = 750;  	// this will Y axis min step frequency
	volatile unsigned int Z_min_speed = 500;  	// this will Z axis min step frequency

// This function will set the stepper timers up.  The resolution
// of these timers will be 8 uSec. (32M FOSC/4) / 64(post scale)
// This allows for a range from 125000 to about 490 Hz without 
// having to adjust the timers.  The drive pulse will be half the
// width of the stepper frequency.  The interrupt routines will
// toggle the drive (High/Low) based on the timer matching the
// period.
void SET_TIMERS(void)
{
	TMR2 = 0x00;	// Clear X drive Timer
	TMR4 = 0x00;	// Clear Y drive Timer
	TMR6 = 0x00;	// Clear Z drive Timer
	
	T2CON = 0b00000011; // X drive, 1:1 pre-scale, 1:64 post, Off
	T4CON = 0b00000011; // Y drive, 1:1 pre-scale, 1:64 post, Off
	T6CON = 0b00000011; // Z drive, 1:1 pre-scale, 1:64 post, Off
	
}	// End of SET_TIMERS function

// This function will calculate the timer period for the interrupt 
// match (PR2 - X, PR4 - Y, PR6 - Z).  The timer will count to this
// value.  When a match occurs an interrupt is issued.  The period
// setting will be 1/2 the frequency width so the signal will be on
// half the cycle and off the other half.  One cycle equals two period
// interrupts.  The max speed is limited by the unsigned speed value
// of 0xFFF.  The min is rounded up from ~245.09 which is 0xFF.
unsigned char get_period(unsigned int speed)
{
	if((speed <= 65535.0) && (speed >= 246.0))// This could be faster then 65kHz
	{
		system_status &= 0b11110111;	// clear Period error status
		return (unsigned char)((125000/(speed * 2.0)));
	}
	else
	{
		system_status |= 0b00001001;	// set Period error status
		return 0xFF;
	}
	
}	// End of get_period function

// The following is the serial interface access to sets the max frequency 
// to run the X-Drive
void write_X_fast(void)
{
	X_max_speed	= (unsigned int)((rxfifo[3] << 8) | rxfifo[4]);
}	// End of write_X_fast function		

// The following is the serial interface access to sets the max frequency 
// to run the Y-Drive
void write_Y_fast(void)
{
	Y_max_speed	= (unsigned int)((rxfifo[3] << 8) | rxfifo[4]);
}	// End of write_Y_fast function

// The following is the serial interface access to sets the max frequency 
// to run the Z-Drive
void write_Z_fast(void)
{
	Z_max_speed	= (unsigned int)((rxfifo[3] << 8) | rxfifo[4]);
}	// End of write_Z_fast function	

// The following is the serial interface access to sets the lowest frequency 
// to run the X-Drive
void write_X_slow(void)
{
	X_min_speed	= (unsigned int)((rxfifo[3] << 8) | rxfifo[4]);
}	// End of write_X_slow function	

// The following is the serial interface access to sets the lowest frequency 
// to run the Y-Drive
void write_Y_slow(void)
{
	Y_min_speed	= (unsigned int)((rxfifo[3] << 8) | rxfifo[4]);
}	// End of write_Y_slow function	

// The following is the serial interface access to sets the lowest frequency 
// to run the Z-Drive
void write_Z_slow(void)
{
	Z_min_speed	= (unsigned int)((rxfifo[3] << 8) | rxfifo[4]);
}	// End of write_X_slow function		


// Set up delay for settle.  Use interrupt so RX commands can still be captured
void msDelay(unsigned int msTime)
{
	TMR0 = 0x06;  		// This is the gbl_ms_Timer (Timer 0)
	TMR0IF = 0;			// Clear Timer Flag
	TMR0IE = 1;			// Timer Interrupt Enable
	gbl_ms_Timer = 0; 	// Reset this timer
	ms_Timer_flag = 1;	// Enable FW Flag
		
	// Poll Timer
	while((gbl_ms_Timer < msTime) && ms_Timer_flag)
	{
		/* do nothing but wait. Allow for interrupts. */
	}	

	ms_Timer_flag = 0;	// Disable FW Flag
	TMR0IE = 0;			// Disable Interrupt		
}	// End of msDelay
		
