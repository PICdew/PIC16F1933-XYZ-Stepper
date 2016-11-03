/*
 * system_status.c
 *
 * Contains System Status and Error information functions.
 * 
*/

#include <pic.h>
#include "globals.h"

// Bits of the following register will contain information of the Limit
// status.  This is 'cheap and easy' but is also not that hard to maintain
// and use. It should usually be ored with it's self when updated such as:
//		 limit_status = limit_status | 0bXXXXXXXX;
// The part that is tricky about this is keeping track of what bit means what.
//	Status:
//	0bXXXX XXX1 	X Home Status (RC0, X-H)
//	0bXXXX XX1X 	X Far From Home (RC1, X-FFH)
//	0bXXXX X1XX 	Y Home Status (RC2, Y-H)
//	0bXXXX 1XXX 	Y Far From Home (RC3, Y-FFH)
//	0bXXX1 XXXX 	Z Home Status (RC4, Z-H)
// 	0bXX1X XXXX 	Z Far From Home (RC5, Z-FFH)
// 	0bXXXX XXXX 	Not Used
// 	0bXXXX XXXX 	Not Used
	volatile unsigned char limit_status = 0x00;		// Contains Limit Switch Status Info
	
// The following will read the status of XYZ limits
void read_limit_status(void)
{
	limit_status = ~(PORTC);
}// End read_limit_status function	

// The following will read the status of XYZ limits for transmit
void tx_read_limit_status(void)
{
	txfifo[1] = ~(PORTC); 	
	txfifo[0] = 1;			// Returning one char
}// End read_limit_status function	

// The Following will clear the limit status
void clear_limit_status(void)
{
	limit_status = 0b00000000; // Clear Limit Status	
}// End clear_limit_status function

// Bits of the following register will contain System Status Information
// as follows:
// 
//	Errors:
//	0bXXXX XXX1 	System Error
//	0bXXXX XX1Y 	Invalid Serial Command
//	0bXXXX X1XY 	VREF value Error
//	0bXXXX 1XXY 	Period value Error
//	Status:
//	0bXXX1 XXXX 	X-Drive Running
// 	0bXX1X XXXX 	Y-Drive Running
// 	0bX1XX XXXX 	Z-Drive Running
// 	0bYXXX XXXX 	Not Used
	volatile unsigned char system_status = 0x00; 	// Contains System Status Info
	
// The following will read the system status
void read_system_status(void)
{
	txfifo[1] = system_status; 	
	txfifo[0] = 1;			// Returning one char
}// End read_system_status function	

// The Following will clear the limit status
void clear_system_status(void)
{
	system_status = 0b00000000; // Clear Limit Status	
}// End read_system_status function

