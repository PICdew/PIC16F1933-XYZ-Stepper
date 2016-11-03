/*
 * global.h
 * This file contains the definitions of items needing to be used by
 * multiple functions.
*/
#include <pic.h>
#include <htc.h>

// Using Internal Clock of 32 Mhz
	#define FOSC 32000000L
	
	extern volatile unsigned int gbl_ms_Timer;	// Used when Pre-Scaler value of '0b100' equals 1:32.  With a FOCS of
												// 32 mHz, the Timer0 Flag will be set every 1.024 mSec.
												// 1.024 = (1/(32M/4))* ( 256: TIMER0 is 8 bit) * (Pre-Scale: 32)
	
	extern volatile bit ms_Timer_flag;	// mSec Timer flag used by the timer 0 interrupt to detirmine
										// which global timer gets updated.
										
// abort.c
	extern void TRG_ABORT(void); 	// Disables XYZ drives and Timers
	extern void X_ABORT(void);		// Disables X drive and Timer
	extern void Y_ABORT(void);		// Disables Y drive and Timer
	extern void Z_ABORT(void);		// Disables Z drive and Timer
	
// drive_home.c
	extern void X_HOME(void);		// Stepper Drive to HOME (X-H RC0 = Low)
	extern void Y_HOME(void);		// Stepper Drive to HOME (Y-H RC2 = Low)
	extern void Z_HOME(void);		// Stepper Drive to HOME (Z-H RC4 = Low)
	
// drive_mode.c
	extern void X_HALF_STEP(void);	// X HALF STEP MODE (default drive mode)
	extern void Y_HALF_STEP(void);	// Y HALF STEP MODE (default drive mode)
	extern void Z_HALF_STEP(void);	// Z HALF STEP MODE (default drive mode)
	
	extern void X_RESET(void); // X translator to HOME (state 1, ABCD = 0101)
	extern void Y_RESET(void); // Y translator to HOME (state 1, ABCD = 0101)
	extern void Z_RESET(void); // Z translator to HOME (state 1, ABCD = 0101)

// drive_motor.c
	extern volatile unsigned int X_new_location;  	// this is the new X axis location from X_Home
	extern volatile unsigned int Y_new_location;  	// this is the new Y axis location from Y_Home
	extern volatile unsigned int Z_new_location;  	// this is the new Z axis location from Z_Home
	
	extern void X_DRIVE(void);	// Drive X axis to X_new_location
	extern void Y_DRIVE(void);	// Drive Y axis to Y_new_location
	extern void Z_DRIVE(void);	// Drive Z axis to Z_new_location
	
	// serial com access
		extern void send_new_X(void); // Write X_new_location & call X_DRIVE
		extern void send_new_Y(void); // Write Y_new_location & call Y_DRIVE 
		extern void send_new_Z(void); // Write Z_new_location & call Z_DRIVE
		
// drive_start.c
	extern volatile unsigned int X_location;  	// this is the X axis current location from X_Home
	extern volatile unsigned int Y_location;  	// this is the Y axis current location from Y_Home
	extern volatile unsigned int Z_location;  	// this is the Z axis cureent location from Z_Home
	
	extern void X_START(void);	// Setup Timers and Drive for X stepper
	extern void Y_START(void);	// Setup Timers and Drive for Y stepper
	extern void Z_START(void);	// Setup Timers and Drive for Z stepper
	
	// serial com access
		extern void read_X_position(void);	// This is the current X location with respect to HOME
		extern void read_Y_position(void);	// This is the current Y location with respect to HOME
		extern void read_Z_position(void);	// This is the current Z location with respect to HOME
		
		extern void write_X_position(void);
		extern void write_Y_position(void);
		extern void write_Z_position(void);
	
// drive_timer.c
	extern volatile unsigned int X_max_speed;  	// this will X axis max step frequency
	extern volatile unsigned int Y_max_speed;  	// this will Y axis max step frequency
	extern volatile unsigned int Z_max_speed;  	// this will Z axis max step frequency
	
	volatile unsigned int X_min_speed;  	// this will X axis min step frequency
	volatile unsigned int Y_min_speed;  	// this will Y axis min step frequency
	volatile unsigned int Z_min_speed;  	// this will Z axis min step frequency
	
	extern void SET_TIMERS(void);	// Set drive timers with 8 uSec resolution
	extern unsigned char get_period(unsigned int speed); // Get Compare Period Value
	extern void msDelay(unsigned int msTime);	// Timer0 mSec Delay with interrupts active
	
	// serial com access
		extern void write_X_fast(void);	// Set X_max_speed
		extern void write_Y_fast(void);	// Set Y_max_speed
		extern void write_Z_fast(void);	// Set Y_max_speed
		extern void write_X_slow(void);	// Set X_min_speed
		extern void write_Y_slow(void);	// Set Y_min_speed
		extern void write_Z_slow(void);	// Set Y_min_speed

// Initialize_PIC.c
	extern void initialize(void);	// Drive XYZ to Home and return FW version on Serial Interface
	
// Serial Interface defs (ser.c)
	#define SER_BUFFER_SIZE		16	// Transmit and Receive Buffer Size
	extern volatile unsigned char rxfifo[SER_BUFFER_SIZE];		// Receive Buffer
	extern volatile bank1 unsigned char txfifo[SER_BUFFER_SIZE];// Transmit Buffer
	extern volatile int RX_Size;	// Receiver number of chars in message
	
	extern void SendData(void);		// Transmit data function (see ser.c)
	extern void Execute(void);		// Execute Received request (see ser.c)

// system_status.c
	extern volatile unsigned char system_status; 	// Contains System Status Info
	extern volatile unsigned char limit_status; 	// Contains XYZ limit Switch Info
	
	extern void read_limit_status(void);	// Read limit_status
	extern void tx_read_limit_status(void); // Read limit_status and transmit value
	extern void read_system_status(void);	// Read system_status
												
	extern void clear_limit_status(void);	// Clear limit_status
	extern void clear_system_status(void);	// Clear system_status

// vref.c
	extern volatile unsigned char X_vref;		// X Drive Ref Limit
	extern volatile unsigned char Y_vref;		// Y Drive Ref Limit
	extern volatile unsigned char Z_vref;		// Z Drive Ref Limit
	extern volatile unsigned char working_vref;	// Current Ref executing
	
	// To Set HW DACOUT
	extern void SET_VREF(void); // Sets DACOUT to working_vref value
	
	// serial com access
		extern void write_X_VREF(void);	// Write X_ref & call SET_VREF via Serial Interface
		extern void write_Y_VREF(void);	// Write X_ref & call SET_VREF via Serial Interface
		extern void write_Z_VREF(void);	// Write X_ref & call SET_VREF via Serial Interface

		extern void read_X_VREF(void);	// Read X_ref via Serial Interface
		extern void read_Y_VREF(void);	// Read Y_ref via Serial Interface
		extern void read_Z_VREF(void);	// Read Z_ref via Serial Interface
		extern void read_VREF(void);	// Read working_vref via Serial Interface
		


