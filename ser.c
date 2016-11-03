/*
 * ser.c
 *
 * This contains the serial interface code for sending and executing on received data.  
 * 
*/

#include <pic.h>
#include "globals.h"

	volatile unsigned char rxfifo[SER_BUFFER_SIZE];			// Receive Buffer
	volatile bank1 unsigned char txfifo[SER_BUFFER_SIZE];	// Transmit Buffer

// This will write (TX) the txfifo buffer. It will handle
// the setup and transmission.
void SendData(void)
{

// The TXIF interrupt can be enabled by setting the TXIE
// interrupt enable bit of the PIE1 register. However, the
// TXIF flag bit will be set whenever the TXREG is empty,
// regardless of the state of TXIE enable bit.

	GIE = 0; // Disable general Interrupts while sending data

	// The first char sent back will be the message size
	// Send buffer based on the number of characters specified by 'TX_Size'
	for( unsigned char a = 0; a < SER_BUFFER_SIZE && a < txfifo[0]+1; a++)
	{
		TXREG = txfifo[a];  // The transmission of the Start bit, 
							// data bits and Stop bit sequence commences 
							// immediately following the transfer of the 
							// data to the TSR from the TXREG.
		
		// TXIF may not be valid for one instruction cycle. This will 
		// poll the flag looking for the TXREG to become empty indicating
		// the data has been shifted to TSR.  Polling 2000 times eliminates
		// a lockup condition that may occur if something 'bad' happens.		
		for( int poll = 0; poll < 1000 && TXIF != 1; poll++);
	}
	
	GIE = 1; // Re-enable general Interrupts
	
//	TXEN = 0;	// Disable the transmitter (not using interrupt)
	
}	// End SendData

// This function does the actual work.  Because functions are acted on based
// on serial communications, to speed up the proccessing of the request it is
// important to keep the received message as short as possible.  Here are the 
// available current settings as they should be sent to this serial interface.
//
// Note: the first byte sent is the message size in decimal.  The receive 
// interrupt routine will remove this message size byte prior to this call 
// being made:
//
//		'1I'  -  Initialize, set Drive Mode and send XYZ HOME (see initialize.c)
//		'2AA' -  Abort all (XYZ) drive (see abort.c)
//		'2AX' -  Abort X Drive (see abort.c)
//		'2AY' -  Abort Y Drive (see abort.c)
//		'2AZ' -  Abort Y Drive (see abort.c)
//		'5SNX**' - Send X to new '**' step location, where ** = 0 to 0xFFFF steps (see drive_motor.c)
//		'5SNY**' - Send Y to new '**' step location, where ** = 0 to 0xFFFF steps (see drive_motor.c)
//		'5SNZ**' - Send Z to new '**' step location, where ** = 0 to 0xFFFF steps (see drive_motor.c)
//		'3SHA' - Send All (XYZ) to HOME position (see drive_home.c)
//		'3SHX' - Send X to HOME position (see drive_home.c)
//		'3SHY' - Send Y to HOME position (see drive_home.c)
//		'3SHZ' - Send Z to HOME position (see drive_home.c)
//		'4WVX*' - Write X-Drive Vref, where X = 0 to 31 dec and is a ratio of 1.024 (see vref.c)
//		'4WVY*' - Write Y-Drive Vref, where X = 0 to 31 dec and is a ratio of 1.024 (see vref.c)
//		'4WVZ*' - Write Z-Drive Vref, where X = 0 to 31 dec and is a ratio of 1.024 (see vref.c)
//		'5WFX**' - Write X_max_speed to new '**' frequency, where ** = 0 to 0xFFFF Hz (see drive_timer.c)
//		'5WFY**' - Write Y_max_speed to new '**' frequency, where ** = 0 to 0xFFFF Hz (see drive_timer.c)
//		'5WFZ**' - Write Z_max_speed to new '**' frequency, where ** = 0 to 0xFFFF Hz (see drive_timer.c)
//		'5WSX**' - Write X_min_speed to new '**' frequency, where ** = 0 to 0xFFFF Hz (see drive_timer.c)
//		'5WSY**' - Write Y_min_speed to new '**' frequency, where ** = 0 to 0xFFFF Hz (see drive_timer.c)
//		'5WSZ**' - Write Z_min_speed to new '**' frequency, where ** = 0 to 0xFFFF Hz (see drive_timer.c)
//		'2RS'  - Read System Status (see system_status.c)
//		'2RL'  - Read Limit Status (see system_status.c)
//		'3RVX' - Read X-Drive Vref (see vref.c)
//		'3RVY' - Read Y-Drive Vref (see vref.c)
//		'3RVZ' - Read Z-Drive Vref (see vref.c)
//		'3RPX' - Read Position of X with respect to HOME (see drive_start.c)
//		'3RPY' - Read Position of Y with respect to HOME (see drive_start.c)
//		'3RPZ' - Read Position of Z with respect to HOME (see drive_start.c)
//		'2CS'  - Clear System Status (see system_status.c)
//		'2CL'  - Clear Limit Status (see system_status.c)
//
// These commands could be expanded if needed.
void Execute(void)
{
	//  This function was called from the ISR and may have disabled
	//  the GIE.  We want it restored so any function called will 
	//  execute correctly.
	GIE = 1;	// GIE PEIE TMR0IE INTE IOCIE TMR0IF INTF IOCIF
	
	// Check first byte for one of the above listed commands.
	if(  rxfifo[0] == 'A')	// Abort or Stop (See abort.c) 
	{
		if(rxfifo[1] == 'A')
			TRG_ABORT();	// XYZ abort All	
		else if(rxfifo[1] == 'X') 
			X_ABORT();		// X abort
		else if(rxfifo[1] == 'Y') 
			Y_ABORT();		// Y abort
		else if(rxfifo[1] == 'Z') 
			Z_ABORT();		// Z abort
		else
			system_status |= 0x03; 	// Invalid command
									// Error
	}
	else if(  rxfifo[0] == 'R')	// Read  
	{
		if(rxfifo[1] == 'S')	// System Status (see system_status.c) 
		{
			txfifo[1] = system_status; 	
			txfifo[0] = 1;			// Returning one char
		}
		else if(rxfifo[1] == 'L')	// Limit Status (see system_status.c)
		{
			txfifo[1] = ~(PORTC); 	
			txfifo[0] = 1;			// Returning one char
		}
		else if(rxfifo[1] == 'P')	// Position
		{ 
			if(rxfifo[2] == 'X')
				read_X_position();		//  Read Current X_location
			else if(rxfifo[2] == 'Y')
				read_Y_position();		//  Read Current Y_location
			else if(rxfifo[2] == 'Z')
				read_Z_position();		//  Read Current Z_location
			else
				system_status |= 0x03; 	// Invalid command
										// Error
		}
		else if(rxfifo[1] == 'V')	// VREF
		{ 
			if(rxfifo[2] == 'X')
				read_X_VREF();		//  Read Current X Vref Setting
			else if(rxfifo[2] == 'Y')
				read_Y_VREF();		//  Read Current Y Vref Setting
			else if(rxfifo[2] == 'Z')
				read_Z_VREF();		//  Read Current Z Vref Setting
			else
				system_status |= 0x03; 	// Invalid command
										// Error
		}
		else
			system_status |= 0x03; 	// Invalid command
									// Error
		
		SendData();	// return data								
	}
	else if(  rxfifo[0] == 'C')	// Clear (see system_status.c) 
	{
		if(rxfifo[1] == 'S')
			clear_system_status();	// System Status
		else if(rxfifo[1] == 'L')
			clear_limit_status();	// Limit Status
		else
			system_status |= 0x03; 	// Invalid command
									// Error
	}										
	else if(  rxfifo[0] == 'I')	// Intialize XYZ to HOME and return FW version
	{
		initialize();	// See (initialize.c)
		SendData();		// return data
	}
	else if(  rxfifo[0] == 'S')	// Send  
	{
		if(rxfifo[1] == 'N')	// New location
		{
			if(rxfifo[2] == 'X')
				send_new_X(); 	// Send to New X location (see drive_motor.c)	
			else if(rxfifo[2] == 'Y') 
				send_new_Y(); 	// Send to New Y location (see drive_motor.c)
			else if(rxfifo[2] == 'Z') 
				send_new_Z(); 	// Send to New Z location (see drive_motor.c)
			else
				system_status |= 0x03; 	// Invalid command
										// Error
		}
		else if(rxfifo[1] == 'H')	// HOME
		{	
			if(rxfifo[2] == 'A')	// Send All HOME
			{
				Z_HOME(); 		// Send to Z HOME (see drive_home.c)
				X_HOME(); 		// Send to X HOME (see drive_home.c)
				Y_HOME(); 		// Send to Y HOME (see drive_home.c)
			}	
			else if(rxfifo[2] == 'X')
				X_HOME(); 		// Send to X HOME (see drive_home.c)	
			else if(rxfifo[2] == 'Y') 
				Y_HOME(); 		// Send to Y HOME (see drive_home.c)
			else if(rxfifo[2] == 'Z') 
				Z_HOME(); 		// Send to Z HOME (see drive_home.c)
			else
				system_status |= 0x03; 	// Invalid command
										// Error
		}
	}	
	else if(  rxfifo[0] == 'W')	// Write  
	{
		if(rxfifo[1] == 'V')	// Voltage Reference
		{
			if(rxfifo[2] == 'X')
				write_X_VREF(); 	// Write X vref (see vref.c)	
			else if(rxfifo[2] == 'Y') 
				write_Y_VREF(); 	// Write Y vref (see vref.c)
			else if(rxfifo[2] == 'Z') 
				write_Z_VREF(); 	// Write Z vref (see vref.c)
			else
				system_status |= 0x03; 	// Invalid command
										// Error
		}
		else if(rxfifo[1] == 'F')	// Write Fastest Step Speed
		{
			if(rxfifo[2] == 'X')
				write_X_fast(); 	// Write X_max_speed (see drive_timer.c)	
			else if(rxfifo[2] == 'Y') 
				write_Y_fast(); 	// Write Y_max_speed (see drive_timer.c)
			else if(rxfifo[2] == 'Z') 
				write_Z_fast(); 	// Write Z_max_speed (see drive_timer.c)
			else
				system_status |= 0x03; 	// Invalid command
										// Error
		}
		else if(rxfifo[1] == 'S')	// Write Slowest Step Speed
		{
			if(rxfifo[2] == 'X')
				write_X_slow(); 	// Write X_min_speed (see drive_timer.c)	
			else if(rxfifo[2] == 'Y') 
				write_Y_slow(); 	// Write Y_min_speed (see drive_timer.c)
			else if(rxfifo[2] == 'Z') 
				write_Z_slow(); 	// Write Z_min_speed (see drive_timer.c)
			else
				system_status |= 0x03; 	// Invalid command
										// Error
		}
		else if(rxfifo[1] == 'P')	// Write Position
		{
			if(rxfifo[2] == 'X')
				write_X_position(); 	// Write X location
			else if(rxfifo[2] == 'Y') 
				write_Y_position(); 	// Write Y location
			else if(rxfifo[2] == 'Z') 
				write_Z_position(); 	// Write Z location
			else
				system_status |= 0x03; 	// Invalid command
										// Error
		}
		else
			system_status |= 0x03; 	// Invalid command
									// Error								
	}	
	else
		system_status |= 0x03; 	// Invalid command
								// Error
}	// End Execute
