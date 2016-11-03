/*
 * main.c
 *
 * This is the main entry point to the code.  It contains the main
 * function definition as well as the service interrupt routines.
 * 
*/

#include "globals.h";

	volatile unsigned gbl_ms_Timer = 0;	// Used when Pre-Scaler value of '0b100' equals 1:32.  With a FOCS of
										// 32 mHz, the Timer0 Flag will be set every 1.024 mSec when started
										// at 0x00.  Starting the timer at 0x06 resolves this offset issue
										// resulting in an interrupt every 1 mSec.
										// 1.024 = (1/(32M/4))* ( 256: TIMER0 is 8 bit) * (Pre-Scale: 32)

	volatile bit ms_Timer_flag = 0;		// mSec Timer flag used by the timer 0 interrupt to detirmine
										// which global timer gets updated.
									
	volatile int index = 0;			// Receiver 'rxfifo' index
	volatile int RX_Size = 0;		// Receiver number of chars in message (first char in every receive)

// This is configuration word 1 (defaults used for config 2).  They are defined in pic16lf1933.h and
// are used to configure the chip upon power up.

__CONFIG(FCMEN_OFF & MCLRE_OFF & BOREN_OFF & CPD_OFF & CP_OFF & CLKOUTEN_OFF & IESO_OFF & PWRTE_OFF & WDTE_OFF & FOSC_INTOSC & LVP_ON & PLLEN_ON);

// This is the interrupt service routine (isr). The transmitter receive
// interrupt will always be active but others may be enabled or disabled
// based on the functions using them.  An in-line or macro definition can
// be used to speed up the execution time but is not used do to the slow
// execution speeds required by this design.  Besides, this is easier to
// read if you don't actually have much programming and embedded controller
// experience.  This PIC only has one interrupt vector so priorities do
// not have to be set up.  When the interrupt occurs, a flag is set.  This
// interrupt forces the FW to jump to this interrupt vector.  The flags are
// reviewed to detirmine what needs to be done.
void interrupt isr(void)
{
	/**** This is the RX Interrupt flag ***/
	if(RCIF)
	{	
		// If an error occured, reset to clear it
		if(OERR) 
		{	
			CREN = 0;
			CREN = 1;
		}
		// The first value received will contain the number
		// of chars in the message
		if( index == 0)
			RX_Size = (int)RCREG;
	
		// Capture the second Char to the message size in the
		// rxfifo buffer
		if(index > 0 && index <= RX_Size)
			rxfifo[index-1] = RCREG;
		
		index++;
		// If we got all the chars, reset the index for the next
		// RX Interrupt and execute the command we received	
		if(index > RX_Size)
		{
			index = 0;
			Execute();  // (see ser.c)
		}	
	}
	/**** End of this is the RX Interrupt flag ***/
	
	if( TMR0IF && ms_Timer_flag) // Delay for Vref (see vref.c)
	{	
		TMR0 = 0x06;	// offset the timer to make the interrupt 1 mSec
		gbl_ms_Timer++; // everytime here equals 1 mSec so capture it
		TMR0IF = 0;		// reset this timer interrupt flag		
	}	

	if(TMR2IF)	// X Step Interrupt
	{	
		if(RB1)	// if drive High
		{
			if(RB0)	// X DIR Clockwise
				X_location++; // if moving away from home
			else
				X_location--; // if moving towards home
				
			RB1 = 0;	// Drive STEP low
		}
		else
			RB1 = 1;	// Drive STEP High
				
		TMR2 = 0x00;	// Clear X drive Timer
		TMR2IF = 0;		// Clear interrupt flag
	}
	
	if(TMR4IF)// Y Step Interrupt
	{	
		if(RA5)	// if drive High
		{
			if(RA4) // Y DIR Clockwise
				Y_location++; // if moving away from home
			else
				Y_location--; // if moving towards home
				
			RA5 = 0;	// Drive STEP Low
		}
		else
			RA5 = 1;	// Drive STEP High
			
		TMR4 = 0x00;	// Clear Y drive Timer
		TMR4IF = 0;		// Clear interrupt flag
	}				

	if(TMR6IF)// Z Step Interrupt
	{	
		if(RB5)	// if drive High
		{
			if(RB4)	// Z DIR Clockwise
				Z_location++; // if moving away from home
			else
				Z_location--; // if moving towards home
				
			RB5 = 0;	// Drive STEP low
		}
		else
			RB5 = 1;	// Drive STEP High
			
		TMR6 = 0x00;	// Clear Y drive Timer
		TMR6IF = 0;		// Clear interrupt flag
	}			
}	// End interrupt service routine (isr)

// This is the starting point of this code upon power-up.  When the device
// is turned on, the registers will be configured or setup for serial 
// communication and the device inputs and outputs will be configured. After 
// the device has been initialized this function will simply execute doing nothing 
// but wait for an interrupt from the receiver.  This code does nothing unless it 
// is told to by way of a receiver interrupt.
void main(void)
{
// This following will setup everything needed for the serial interface.
// and the inputs and outputs.  If parameters are changed from these starting 
// values by functions executing within this code structure they should be reset 
// by that function before exiting the call.

	// This sets the internal clock frequency.  Much of this was done using
	// configuration word 1&2 (see __CONFIG in above)
	OSCCON = 0xF0;		// 32mHz internal clock
						// The FOSC bits in Configuration Word 1 must be
						// set to use the INTOSC source as the device
						// system clock (FOSC<2:0> = 100).
						// The IRCF bits in the OSCCON register must be
						// set to the 8 MHz HFINTOSC selection
						// (IRCF<3:0> = 1110).
						// The SPLLEN bit in the OSCCON register must be
						// set to enable the 4xPLL, or the PLLEN bit of the
						// Configuration Word 2 must be programmed to a
						// ‘1'

	// The following will set up the communications (TX and RX) rate.  This
	// is setting the prescale factors.  The rate is relative to the FOSC.
	SPBRG = 0x19;		// baud rate = ~19200 (19231)
						// SYNC = 0, BRGH = 0, BRG16 = 0
						// calc baud rate err ~-0.16% for 19200
						// If an error occurs it will show up as
						// timeout (usually). 	
	
	RCSTA = 0b10110000;	// SPEN RX9 SREN CREN ADDEN FERR OERR RX9D
						// SPEN will enable the EUSART and the TX/CK pin is automatically made an 
						// output. Since the PIN maybe used for some analog function, it must be 
						// disabled by clearing the corresponding bit of the ANSEL register
						
	TXSTA = 0b00100000;	//	TRANSMIT STATUS AND CONTROL REGISTER
						//	CSRC TX9 TXEN SYNC SENDB BRGH TRMT TX9D
	
	BAUDCON = 0b0000000;// ABDOVF RCIDL — SCKP BRG16 — WUE ABDEN
	
	// This sets the fixed voltage reference for the DACOUT (see vref.c)
	FVRCON = 0b10000100;// FVRCON: FIXED VOLTAGE REFERENCE CONTROL REGISTER
						// FVREN FVRRDY Reserved Reserved CDAFVR1 CDAFVR0 ADFVR1 ADFVR0
						// FVRRDY is a status flag but will always read as set
						// ADFVR1 ADFVR0 set the ref to 1x or 1.024 volts

	// These configure ports that will be used for limit switches & communications...
	//  RC0 = X-H		(digital input)
	//  RC1 = X-FFH		(digital input)
	//  RC2 = Y-H		(digital input)
	//  RC3 = Y-FFH		(digital input)
	//  RC4 = Y-H		(digital input)
	//  RC5 = Y-FFH		(digital input)
	//	RC6 = Tx		(digital output)
	//	RC7 = Rx		(digital input)
	
	PORTC = 0b00000000;	// Power up values are okay.  Init Port: RC0 to RC7
	
	TRISC = 0b10111111; 	// TRISA: PORTA TRI-STATE REGISTER
							// TRISA7 TRISA6 TRISA5 TRISA4 TRISA3 TRISA2 TRISA1 TRISA0
							// bit set = input bit cleared = output

	// These configure ports that will be digital outputs for X & Z Drive...
	//	RB0 = X Drive DIR 			(digital output)
	//	RB1 = X Drive STEP 			(digital output)
	//	RB2 = X Drive HALF/FULL 	(digital output)
	//	RB3 = X Drive PHASE/INH1,2 	(digital output)
	//
	//	RB4 = Z Drive DIR 			(digital output)
	//	RB5 = Z Drive STEP 			(digital output)
	//	RB6 = Z Drive HALF/FULL 	(digital output)
	//	RB7 = Z Drive PHASE/INH1,2 	(digital output)
	
	PORTB = 0b00000000;	// Power up values are okay.  Init Port: RB0, RB1, RB2, RB3...
	
	TRISB = 0b00000000; 	// TRISB: PORTB TRI-STATE REGISTER
							// TRISB7 TRISB6 TRISB5 TRISB4 TRISB3 TRISB2 TRISB1 TRISB0
							// bit set = input bit cleared = output

	ANSELB = 0b00000000;	// ANSELB: PORTB ANALOG SELECT REGISTER
							// — — ANSB5 ANSB4 ANSB3 ANSB2 ANSB1 ANSB0
							// bit set = Analog bit cleared = digital

	// These configure ports that will be used for XYZ Drive Mode & control...
	//	RA0 = X Drive RESET			(digital output)
	//	RA1 = Y Drive RESET 		(digital output)
	//	RA2 = DAQOUT VREF	 		(analog output)
	//	RA3 = Z Drive RESET 		(digital output)
	//	RA4 = Z Drive DIR 			(digital output)
	//	RA5 = Z Drive STEP 			(digital output)
	//	RA6 = Z Drive HALF/FULL 	(digital output)
	//	RA7 = Z Drive PHASE/INH1,2 	(digital output)
	
	PORTA = 0b00000000;		// Power up values are okay.  Init Port: RA0, RA1, RA2...
	
	TRISA = 0b00000000; 	// TRISA: PORTA TRI-STATE REGISTER
							// TRISA7(1) TRISA6(1) TRISA5 TRISA4 TRISA3 TRISA2 TRISA1 TRISA0
							// bit set = input bit cleared = output

	ANSELA = 0b00000100;	// ANSELA: PORTA ANALOG SELECT REGISTER
							// — — ANSA5 ANSA4 ANSA3 ANSA2 ANSA1 ANSA0
							// bit set = Analog bit cleared = digital

	OPTION_REG = 0b10000100;		// WPUEN INTEDG TMR0CS TMR0SE PSA PS<2:0>
									// PSA cleared to assign a prescaler
									// PS<2:0> of 0x100 sets the prescaler to 1:32
									// And the Timer mode is selected by clearing the
									// TMR0CS bit of the OPTION register (FOSC/4).

	// This enables the interrupts (also contains some flags)
	INTCON = 0b11000000;// GIE PEIE TMR0IE INTE IOCIE TMR0IF INTF IOCIF
	PIE1 = 0b00100000;	// TMR1GIE ADIE RCIE TXIE SSP1IE CCP1IE TMR2IE TMR1IE
	
	SET_TIMERS();	// Set drive Timer scale (see drive_timer.c)

	while(1)	
	{
	  /* Do nothing but wait for a receiver interrupt */
	}

}
