/*
IBEX UK LTD http://www.ibexuk.com
Electronic Product Design Specialists
RELEASED SOFTWARE

The MIT License (MIT)

Copyright (c) 2013, IBEX UK Ltd, http://ibexuk.com

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
the Software, and to permit persons to whom the Software is furnished to do so,
subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/
//Project Name:		HTRC110 EM4102 125KHZ RFID READER DRIVER
//DRIVER C CODE HEADER FILE







//##################################
//##################################
//########## USING DRIVER ##########
//##################################
//##################################

//###################################################
//##### APPLICATION INITIALISE SETUP INTERRUPTS #####		<<< Adjust as required for your microcontroller and compiler (example shown here is for the example project PIC182420 using the Microchip C18 compiler)
//###################################################
/*
	//----- SETUP INT0  -----
	//Used for: RFID modulated data rising edge detect
	INTCON2bits.INTEDG0 = 1;			//Interrupt on rising edge
	//Interrupt will be enabled as requried by the RFID driver
*/

//###############################################
//##### APPLICATION INITIALISE SETUP TIMERS #####		<<< Adjust as required for your microcontroller and compiler (example shown here is for the example project PIC182420 using the Microchip C18 compiler)
//###############################################
/*
	//----- SETUP TIMER 1 -----
	//Used for: RFID demodulation
	T1CON = 0b10000001;					//Timer 1 on, internal, 1:1 prescale, R/W 16bits in 1 operation
*/



//###################################################
//##### RFID MISO RISING EDGE INTERRUPT HANDLER #####	<<< Adjust as required for your microcontroller and compiler (example shown here is for the example project PIC182420 using the Microchip C18 compiler)
//###################################################
/*
	static WORD w_temp;
	static WORD rfid_timer_value;
	static WORD rfid_timer_value_last;

	if (INTCONbits.INT0IE && INTCONbits.INT0IF)
	{
		//----- RFID MODULATED DATA RISING EDGE DETECT INTERRUPT -----
		//Get the rfid timer value (we need this with as little latency after the rising edge as possible)
		rfid_timer_value = (WORD)TMR1L;						//(read low byte first which loads high byte for this PIC18)
		rfid_timer_value |= ((WORD)TMR1H) << 8;
		
		//We need to pass the time since the last interrupt to the rfid fucntion x 1uS.
		//For this application and microcontroller our 16bit timer1 value has been configured to increment every 100nS.
		//We only need to allow for up to 1280uS so we have plenty of headroom to divide this down to the required 1uS resolution.
		
		w_temp = rfid_timer_value - rfid_timer_value_last;		//Get difference from last time (roll overs are inherantly dealt with)
		rfid_timer_value_last = rfid_timer_value;
		w_temp = w_temp / 10;									//Divide to give 1uS resolution
		
		//Call the RFID function to process the event
		rfid_sampling_rising_edge(w_temp);
	}
*/

//############################
//##### ADD TO HEARTBEAT #####
//############################
/*
	//-----------------------------
	//----- HERE EVERY 1 mSec -----
	//-----------------------------
	//----- RFID READER TIMER -----
	if (rfid_1ms_timer)
		rfid_1ms_timer--;
*/

//############################
//##### ADD TO MAIN LOOP #####
//############################
/*
	//----- PROCESS RFID DRIVER -----
	rfid_process();
*/

//For further information please see the project technical manual


//*****************************
//*****************************
//********** DEFINES **********
//*****************************
//*****************************
#ifndef RFID_C_INIT		//Do only once the first time this file is used
#define	RFID_C_INIT


//---------------------------------
//----- RFID SERIAL PORT PINS -----		<<< Adjust as required for your microcontroller and compiler (example shown here is for the example project PIC182420 using the Microchip C18 compiler)
//---------------------------------
#define	RFID_SCLK(state)				LATCbits.LATC1 = state		//Output
#define	RFID_MOSI(state)				LATCbits.LATC0 = state		//Output
#define	RFID_MISO						PORTBbits.RB0				//Input

//#define	RFID_SPI_SAMPLE_CS_PIN(state)	LATCbits.LATC2 = state	//An optional output that will be driven low to mimic a SPI bus chip select pin at the start of each byte transfer.  Comment
																	//out if not requried.  Useful if you want to capture the data transfers using a logic analyser.  Setup as 8bit, MSB first,
																	//tx valid on rising edge, rx valid on rising edge, cs active = low.

//----------------------
//----- INTERRUPTS -----		<<< Adjust as required for your microcontroller and compiler (example shown here is for the example project PIC182420 using the Microchip C18 compiler)
//----------------------
//This IRQ needs to call the rfid_sampling_rising_edge() function eaqch time a rising edge is detected on the MISO pin.
//Latency is reasonably important.  We will potentially get a rising edge approximately every 128uS (its radio so although 125kHz is the ideal it may not be perfect) and we
//need to detect times with a resolution of significantly better than 64uS.
//If you don't want to use an interrupt or this and instead wan't to stall you applicaiton while RFID read is active, you could create a loop waiting for the rising edge and call
//the rfid_sampling_rising_edge() function in the same way that an irq would.
//This interrupt should be disabled on powerup.
#define	RFID_ENABLE_MISO_RISING_EDGE_IRQ()			INTCONbits.INT0IE = 1
#define	RFID_DISABLE_MISO_RISING_EDGE_IRQ()			INTCONbits.INT0IE = 0
#define	RFID_CLEAR_MISO_RISING_EDGE_IRQ()			INTCONbits.INT0IF = 0



//------------------
//----- DELAYS -----		<<< Adjust as required for your microcontroller and compiler (example shown here is for the example project PIC182420 using the Microchip C18 compiler)
//------------------
#define	RFID_SETUP_DELAY()				Nop()						//Optional delay between signal changes - may be useful for very fast microcontrollers (Nop(); is a single cycle null
																	//instruction for the Microchip C19 compiler)
#define	RFID_BIT_DELAY()				Nop(); Nop(); Nop()			//Delay to create serial interface bit time.  Requirement is not specified in the HTRC110 datasheet but >= 1uS is a
																	//reliable value to use (Nop(); is a single cycle null instruction for the Microchip C18 compiler)



//----- RFID PARAMETERS -----
#define	RFID_T_OC						0x3F		//Offset Compensation value.  0x3F is typical (if no capacitor is connected to RX).  See section 9.2.4 of the Philips "Read/Write Devices
													//based on the HITAG Read/Write IC HTRC110" application note AN97070 (rev 1.1)


typedef enum _SM_RFID
{
	RFID_INITIALISE,
	RFID_INITIALISE_1,
    RFID_SET_IDLE,
    RFID_IDLE,
    RFID_READ_TAG_1,
    RFID_READ_TAG_2,
    RFID_READ_TAG_3,
    RFID_READ_TAG_READING,
    RFID_READ_FAIL
} SM_RFID;

typedef enum _SM_RFID_CAPTURE
{
	RFID_CAPTURE_INITIALISE,
	RFID_CAPTURE_GET_BIT_RATE,
	RFID_CAPTURE_GET_HEADER,
	RFID_CAPTURE_GET_DATA,
	RFID_CAPTURE_SUCCESS,
	RFID_CAPTURE_FAILED,
	RFID_CAPTURE_FAILED_GIVEN_UP
} SM_RFID_CAPTURE;


#endif


//*******************************
//*******************************
//********** FUNCTIONS **********
//*******************************
//*******************************
#ifdef RFID_C
//-----------------------------------
//----- INTERNAL ONLY FUNCTIONS -----
//-----------------------------------
BYTE rfid_tx_rx_byte (BYTE tx_data, BYTE get_response);
void rfid_start_read_tag (void);
void rfid_end_read_tag (void);
BYTE rfid_sampling_rising_edge_add_bit (BYTE bit_state);


//-----------------------------------------
//----- INTERNAL & EXTERNAL FUNCTIONS -----
//-----------------------------------------
//(Also defined below as extern)
void rfid_read_tag (void);
BYTE rfid_get_read_tag_result (void);
void rfid_force_initialise (void);
void rfid_process (void);
void rfid_sampling_rising_edge (WORD us_since_last_rising_edge);


#else
//------------------------------
//----- EXTERNAL FUNCTIONS -----
//------------------------------
extern void rfid_read_tag (void);
extern BYTE rfid_get_read_tag_result (void);
extern void rfid_force_initialise (void);
extern void rfid_process (void);
extern void rfid_sampling_rising_edge (WORD us_since_last_rising_edge);


#endif




//****************************
//****************************
//********** MEMORY **********
//****************************
//****************************
#ifdef RFID_C
//--------------------------------------------
//----- INTERNAL ONLY MEMORY DEFINITIONS -----
//--------------------------------------------
BYTE rfid_gain;
BYTE rfid_read_tag_pending = 0;
BYTE rfid_read_tag_success;
BYTE rfid_capture_state;
BYTE rfid_bit_count;
BYTE rfid_tries_count;


//--------------------------------------------------
//----- INTERNAL & EXTERNAL MEMORY DEFINITIONS -----
//--------------------------------------------------
//(Also defined below as extern)
BYTE rfid_data_nibbles[11];
BYTE rfid_state = RFID_INITIALISE;
volatile BYTE rfid_1ms_timer;


#else
//---------------------------------------
//----- EXTERNAL MEMORY DEFINITIONS -----
//---------------------------------------
extern BYTE rfid_data_nibbles[11];
extern BYTE rfid_state;
extern volatile BYTE rfid_1ms_timer;


#endif



