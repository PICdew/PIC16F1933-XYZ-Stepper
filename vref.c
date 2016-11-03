/*
 * vref.c
 *
 * Sets reference voltage for the chopper circuit of the L297.
 * 
*/

#include <pic.h>
#include "globals.h";

volatile unsigned char X_vref = 0x11;	// X Drive Ref Limit (2.8 Amp Motor)
volatile unsigned char Y_vref = 0x11;	// Y Drive Ref Limit (2.8 Amp Motor)
volatile unsigned char Z_vref = 0x06;	// Z Drive Ref Limit (1 Amp Motor)

volatile unsigned char working_vref = 0x00;	// Current Ref executing
unsigned char vref_limit = 0x1C;			// L297 Ref Limit ( 4.7 Amps * 0.2 oHms = 0.94 Volts)

// This function, SET_VREF (set voltage reference) sets the
// reference voltage for chopper circuit. A voltage applied 
// to this pin determines the peak load current. DACOUT (RA2) 
// is used to provide this reference.  This pin has a 3 Volt 
// limit on the L297. The value used by this function is:
//
//    vref = (Stepper I Limit) * 0.2 Ohms 
// 
// The reference used to produce DACOUT is the fixed voltage 
// reference (FVR) of 1.024 volts which was setup on power-up 
// of the PIC.  This was used because ripples on VDD could be 
// transfered to DACOUT where the internal reference would not 
// be harmed. Also, DACOUT has a limited amount of resolution 
// (32 steps 0-31) so dividing it with respect to a reference 
// closer to the limit of design limit (4.7 amps, ~ 0.94 Volts)
// results in better step resolution.  This function uses a 
// fixed delay to allow for the reference to become stable.  This
// delay uses Timer0 and the interrupt routine so RX commands 
// can still be captured.
void SET_VREF(void)
{
	if( working_vref <= vref_limit)	// Verify <= ~0.94 Volts (0.9249 V)
	{
		DACCON1 = working_vref;	// VOLTAGE REFERENCE CONTROL REGISTER 1
								// — — — DACR<4:0>
	
		DACCON0 = 0b11101000;	//  VOLTAGE REFERENCE CONTROL REGISTER 0
								//  DACEN DACLPS DACOE — DACPSS<1:0> — DACNSS
								//  Turn on DACOUT and use FVR
								// Set to look for ( TMR0IF && ms_Timer_flag) interrupt
	
		msDelay(250);	// Delay for settle
						
		system_status &= 0b11111011;// Clear VREF value Error
	}
	else
		system_status |= 0b00000101;// Set VREF value Error

}	// End of SET_VREF function

//  This is the serial interface to set or write X_Vref.  The format is:
//	[0] [1] [2] 'WVX': Write Vref for X Drive
//  [3] vref update value ( 0 to 31 is the range but must be < vref_limit)
//  SET_VREF will handle the limit test.  
void write_X_VREF(void)
{
	X_vref = rxfifo[3];	// update
}	// End of write_X_VREF function	

//  This is the serial interface to set or write Y_Vref.  The format is:
//	[0] [1] [2] 'WVY': Write Vref for Y Drive
//  [3] vref update value ( 0 to 31 is the range but must be < vref_limit)
//  SET_VREF will handle the limit test.
void write_Y_VREF(void)
{
	Y_vref = rxfifo[3];	// update
}	// End of write_Y_VREF function

//  This is the serial interface to set or write Z_Vref.  The format is:
//	[0] [1] [2] 'WVZ': Write Vref for Z Drive
//  [3] vref update value ( 0 to 31 is the range but must be < vref_limit)
//  SET_VREF will handle the limit test.  
void write_Z_VREF(void)
{
	Z_vref = rxfifo[3];	// update
}	// End of write_Z_VREF function		

//  This reads working_vref.  The format is: 
//  [0] 1 				(transmit size)
//  [1] vref_working 	(current vref setting)
void read_VREF(void)
{
	txfifo[0] = 1;				// Returning 1 char
	txfifo[1] = working_vref; 	// reference voltage for chopper circuit
}	// End of read_VREF function		

//  This reads X_vref.  The format is: 
//  [0] 1 		(transmit size)
//  [1] X_vref 	(current vref setting)
void read_X_VREF(void)
{
	txfifo[0] = 1;			// Returning 1 char
	txfifo[1] = X_vref; 	// reference voltage for chopper circuit
}	// End of read_X_VREF function		

//  This reads Y_vref.  The format is: 
//  [0] 1 		(transmit size)
//  [1] X_vref 	(current vref setting)
void read_Y_VREF(void)
{
	txfifo[0] = 1;			// Returning 1 char
	txfifo[1] = Y_vref; 	// reference voltage for chopper circuit
}	// End of read_Y_VREF function

//  This reads Z_vref.  The format is: 
//  [0] 1 		(transmit size)
//  [1] Z_vref 	(current vref setting)
void read_Z_VREF(void)
{
	txfifo[0] = 1;			// Returning 1 char
	txfifo[1] = Z_vref; 	// reference voltage for chopper circuit
}	// End of read_Z_VREF function		