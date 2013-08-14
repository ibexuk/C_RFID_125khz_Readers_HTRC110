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
//PIC18 SAMPLE PROJECT C CODE FILE




//----- INCLUDE FILES FOR THIS SOURCE CODE FILE -----
#include "main.h"					//Global data type definitions (see https://github.com/ibexuk/C_Generic_Header_File )
#define	MAIN_C						//(Define used for following header file to flag that it is the header file for this source file)
#include "ap-main.h"				//(Include header file for this source file)

//----- OTHER PROJECT FILES REQUIRED BY THIS SOURCE CODE FILE -----
#include "rfid.h"

//----- COMPILER LIBRARY FILES REQUIRED BY THIS SOURCE CODE FILE -----
//#include <stdio.h>					//(Needed for printf)
//#include <stdlib.h>
//#include <usart.h>





//******************************************
//******************************************
//********** DEVICE CONFIGURATION **********
//******************************************
//******************************************
//These configuration defines do not need to be included, but having them means that the configuration bits will be automatically set and will be included in the .hex
//file created for the project, so that they do not need to be manually set when programming devices at manufacture.
//(The config names are given in the '.inc' file for the device being used)

#ifdef __DEBUG				//Debug mode selected in MPLAB
//DEVELOPMENT MODE
	#pragma config OSC = HSPLL			//Ocsillator type
	#pragma config FCMEN = OFF			//Fail safe clock monitor (On = if osc fails the internal osc will be used)
	#pragma config IESO = OFF			//Ocsillator switchover mode
	#pragma config BOREN = SBORDIS		//Brown out reset (OFF = Disabled, ON = SBOREN bit enabled in software, NOSLP = enabled except SLEEP, SBOREN Disabled, SBORDIS enabled, SBOREN Disabled)
	#pragma config PWRT = OFF			//Powerup timer (neeeds to be off to enter debug mode for some reason)
	#pragma config BORV = 1				//Brown out reset voltage  (0 = 4.6V, 1 = 4.3V, 2 = 2.8V or 3 = 2.1V)
	#pragma config WDT = OFF			//Watchdog timer
	#pragma config WDTPS = 128			//Watchdog prescaller
	#pragma config MCLRE = ON			//MCLR pin enable
	#pragma config LPT1OSC = OFF		//Low power timer 1 oscillator
	#pragma config PBADEN = OFF			//PortB AtoD enable
	#pragma config CCP2MX = PORTC		//CCP2 pin
	#pragma config DEBUG = ON			//Debugger
	#pragma config LVP = OFF			//Low voltage program
	#pragma config STVREN = ON			//Stack overflow reset
	#pragma config XINST = OFF			//Extended instruction set enable
	#pragma config CP0 = OFF			//Code protect block
	#pragma config CP1 = OFF			//Code protect block
	#pragma config CPB = OFF			//Code protect boot block
	#pragma config CPD = OFF			//Protect data eeprom
	#pragma config WRT0 = OFF			//Table write protect block
	#pragma config WRT1 = OFF			//Table write protect block
	#pragma config WRTB = OFF			//Boot block write protect
	#pragma config WRTC = OFF			//Config registers write protect
	#pragma config WRTD = OFF			//Data eeprom write protect
	#pragma config EBTR0 = OFF			//Table read protect block
	#pragma config EBTR1 = OFF			//Table read protect block
	#pragma config EBTRB = OFF			//Table read protect boot block

#else
//NOT IN DEVELOPMENT MODE
	#pragma config OSC = HSPLL			//Ocsillator type
	#pragma config FCMEN = OFF			//Fail safe clock monitor (On = if osc fails the internal osc will be used)
	#pragma config IESO = OFF			//Ocsillator switchover mode
	#pragma config BOREN = SBORDIS		//Brown out reset (OFF = Disabled, ON = SBOREN bit enabled in software, NOSLP = enabled except SLEEP, SBOREN Disabled, SBORDIS enabled, SBOREN Disabled)
	#pragma config PWRT = ON			//Powerup timer
	#pragma config BORV = 1				//Brown out reset voltage  (0 = 4.6V, 1 = 4.3V, 2 = 2.8V or 3 = 2.1V)
	#pragma config WDT = ON				//Watchdog timer
	#pragma config WDTPS = 128			//Watchdog prescaller
	#pragma config MCLRE = ON			//MCLR pin enable
	#pragma config LPT1OSC = OFF		//Low power timer 1 oscillator
	#pragma config PBADEN = OFF			//PortB AtoD enable
	#pragma config CCP2MX = PORTC		//CCP2 pin
	#pragma config DEBUG = OFF			//Debugger
	#pragma config LVP = OFF			//Low voltage program
	#pragma config STVREN = ON			//Stack overflow reset
	#pragma config XINST = OFF			//Extended instruction set enable
	#pragma config CP0 = ON				//Code protect block
	#pragma config CP1 = ON				//Code protect block
	#pragma config CPB = ON				//Code protect boot block
	#pragma config CPD = ON				//Protect data eeprom
	#pragma config WRT0 = OFF			//Table write protect block
	#pragma config WRT1 = OFF			//Table write protect block
	#pragma config WRTB = OFF			//Boot block write protect
	#pragma config WRTC = OFF			//Config registers write protect
	#pragma config WRTD = OFF			//Data eeprom write protect
	#pragma config EBTR0 = OFF			//Table read protect block
	#pragma config EBTR1 = OFF			//Table read protect block
	#pragma config EBTRB = OFF			//Table read protect boot block
#endif




//*************************************************************************************************************************************
//*************************************************************************************************************************************
//*************************************************************************************************************************************
//*************************************************************************************************************************************
//*************************************************************************************************************************************
//*************************************************************************************************************************************
//*************************************************************************************************************************************

//********************************
//********************************
//********** INITIALISE **********
//********************************
//********************************
void initialise (void)
{
	ClrWdt();

	//-------------------------------------
	//----- SETUP INTERNAL OSCILLATOR -----
	//-------------------------------------
	//(Comment out for external oscillator)
	//	Use this config setting:	#pragma config OSC = INTIO67		//Ocsillator type Internal, RA6 & 7 are IO
	//OSCCON = 0b01110010;				//8MHz internal oscillator selected
	//OSCTUNE = 0b11000000;				//Turn on the PLL.  Osc = 32MHz

	//-------------------------
	//----- SETUP IO PINS -----
	//-------------------------
	//(Device will powerup with all IO pins as inputs)

	//Setup the state of any pins that are outputs (inputs are don't care)
	LATA = 0b00000000;
	LATB = 0b00000000;
	LATC = 0b00000000;

	//Setup the IO pin type (0 = output, 1 = input)
	TRISA = 0b00000000;
	TRISB = 0b00000001;
	TRISC = 0b00000000;

	//Setup internal pull ups
	INTCON2bits.RBPU = 0;				//Port B pull ups (0 = on)

	//----- SETUP THE A TO D PINS -----
	ADCON0 = 0b00000000;
	ADCON1 = 0b00001111;				//All AtoD pins are digital


	//----------------------------
	//----- SETUP INTERRUPTS -----
	//----------------------------
	
	//----- SETUP INT0  -----
	//Used for: RFID modulated data rising edge detect
	INTCON2bits.INTEDG0 = 1;			//Interrupt on rising edge
	//Interrupt will be enabled as requried by the RFID driver


	//------------------------
	//----- SETUP TIMERS -----
	//------------------------
	//(INCLUDE THE USAGE OF ALL TIMERS HERE EVEN IF NOT SETUP HERE SO THIS IS THE ONE POINT OF
	//REFERENCE TO KNOW WHICH TIMERS ARE IN USE AND FOR WHAT).

	//----- SETUP TIMER 0 -----
	//Used for: Heartbeat
	T0CONbits.T08BIT = 0;				//Configured as a 16bit timer
	T0CONbits.T0CS = 0;					//Use internal instruction clock
	T0CONbits.T0SE = 0;
	T0CONbits.PSA = 0;					//Prescaller is in use (0 = in use)
	T0CONbits.T0PS2 = 0;				//Prescaler 1:2
	T0CONbits.T0PS1 = 0;
	T0CONbits.T0PS0 = 0;
	T0CONbits.TMR0ON = 1;				//Timer 0 on

	INTCONbits.TMR0IE = 1;				//Enable the Timer 0 irq

	//----- SETUP TIMER 1 -----
	//Used for: RFID demodulation
	T1CONbits.RD16 = 1;					//R/W 16bits in 1 operation
	T1CONbits.T1RUN = 0;				//Don't use timer 1 dedicated oscillator
	T1CONbits.T1OSCEN = 0;				//Timer 1 dedicated oscillator disabled
	T1CONbits.T1CKPS1 = 0;				//1:1 prescale
	T1CONbits.T1CKPS0 = 0;
	T1CONbits.T1SYNC = 0;
	T1CONbits.TMR1CS = 0;				//Use internal instruction  clock
	T1CONbits.TMR1ON = 1;				//Enable timer 1
	//PIE1bits.TMR1IE = 1;				//Enable TMR1 irq


	//----- SETUP TIMER 2 -----
	//Used for: Available
	/*
	T2CONbits.T2OUTPS3 = 0;				//1:1 postscale
	T2CONbits.T2OUTPS2 = 0;
	T2CONbits.T2OUTPS1 = 0;
	T2CONbits.T2OUTPS0 = 0;
	T2CONbits.T2CKPS1 = 1;				//1:16 prescale
	T2CONbits.T2CKPS0 = 0;
	T2CONbits.TMR2ON = 1;				//Enable timer 2
	*/

	//----- SETUP TIMER 3 -----
	//Used for: Available
	/*
	T3CONbits.RD16 = 1;					// R/W 16bits in 1 operation
	T3CONbits.T3CCP2 = 0;				// R/W 16bits in 1 operation
	T3CONbits.T3CCP1 = 0;
	T3CONbits.T3CKPS1 = 0;				//1:1 prescale
	T3CONbits.T3CKPS0 = 0;
	T3CONbits.T3SYNC = 0;
	T3CONbits.TMR3CS = 0;				//Use internal instruction  clock
	T3CONbits.TMR3ON = 1;				//Enable timer 3
	*/



	//-----------------------------------------------------
	//----- SETUP SYNCHRONOUS SERIAL PORT (I2C / SSP) -----
	//-----------------------------------------------------
	//----- SETUP SSP AS I2C -----
	//Used for: Available
	/*
	//I2C requirements:- 100kHz clock high time 4.0uS, clock low time 4.7uS, 400kHz clock high time 0.6uS, clock low time 1.3uS
	//The PIC18 does not fully conform to the 400kHz I2C spec (which applies to all rates >100kHz) but may be used with care
	//where higher rates are required.
	//Lower 7 bits of SSPADD are the baud rate.  ((Tcycle / 2) * SSPADD * 2) = time per bit.  1 / time per bit = frequency.
	//400kHz @ 40MHz OSC = SSPADD 25
	//100kHz @ 40MHz OSC = SSPADD 100
	//100kHz @ 32MHz OSC = SSPADD 80
	//100kHz @ 4MHz OSC = SSPADD 10
	SSPADD = 100;					//setup i2c clk speed
	OpenI2C(MASTER, SLEW_ON);		//Slew rate control should be on for 100KHz, off for 400KHz
	*/

	//----- SETUP SSP AS SPI BUS -----
	//Used for: Available
	/*
	SSPSTATbits.SMP = 0;
	SSPSTATbits.CKE = 1;		//Data is valid on idle to active edge of the clock
	SSPCON1bits.CKP = 0;		//Clock low in idle bus state
	SSPCON1bits.SSPM3 = 0;		//Clock frequency = TMR2 / 2 (tilt sensors are max 500KHz clock)
	SSPCON1bits.SSPM2 = 0;
	SSPCON1bits.SSPM1 = 1;
	SSPCON1bits.SSPM0 = 1;
	SSPCON1bits.SSPEN = 1;		//Enable SSP Port
	*/


	//-----------------------
	//----- SETUP USART -----
	//-----------------------
	//Setup to use for debug 'printf' output
	/*
	//SETUP BAUD RATE
	TXSTAbits.BRGH = 0;					//Low speed mode
	BAUDCONbits.BRG16 = 1;				//16 bit baud rate generator
	SPBRGH = (((((DWORD)INSTRUCTION_CLOCK_FREQUENCY << 2) / 9600) / 16) - 1) >> 8;			//(((OSCILLATOR_FREQUENCY / DESIRED_BAUD) / DIVIDE_VALUE) - 1)
	SPBRG = (((((DWORD)INSTRUCTION_CLOCK_FREQUENCY << 2) / 9600) / 16) - 1) & 0x00ff;		//Note that different BRGH & BRG16 settings use different divide values - see device datasheet

	//SETUP TX
	TXSTAbits.TX9 = 0;					//8 bit tx
	TXSTAbits.TXEN = 1;					//Transmit enabled
	TXSTAbits.SYNC = 0;					//Asynchronous mode
	
	//SETUP RX
	RCSTAbits.RX9 = 0;					//8 bit rx
	RCSTAbits.ADDEN = 0;				//Address detect disabled
	RCSTAbits.CREN = 1;					//Enable receiver
	RCSTAbits.SPEN = 1;					//Serial port enabled
	*/

	//------------------------
	//----- USING PRINTF -----
	//------------------------
	//If using C18 and the compiler small memory model (devices <= 64K of program memory) you can use '(rom far char*)' to stop a build warning
	//being generated, otherwise omit the (rom far char*)

	//Example displaying just text:-
	//printf ((rom far char*)"\n----- POWER-UP ----- \n\r");

	//Example displaying a value:-
	//temp_value = 199;
	//itoa((int) temp_value, temp_string);
	//printf ((rom far char*)"%s\n\r", temp_string);

	//Example of getting terminal input:-
	//printf ((rom far char*)"\nEnter 5 digit value\n\r");
	//getsUSART((char*)temp_string,5);
	//temp_string[5] = 0x00;
	//printf ((rom far char*)"You typed: %s\n\r", temp_string);


	//-----------------------------
	//----- ENABLE INTERRUPTS -----
	//-----------------------------
   	ENABLE_INT;






}








//**********************************************************************************************************************************************
//**********************************************************************************************************************************************
//**********************************************************************************************************************************************
//**********************************************************************************************************************************************
//**********************************************************************************************************************************************
//**********************************************************************************************************************************************
//**********************************************************************************************************************************************

//***********************************
//***********************************
//********** MAIN FUNCTION **********
//***********************************
//***********************************
void main ( void )
{


	//**********************
	//**********************
	//***** INITIALISE *****
	//**********************
	//**********************
	initialise();





	//*********************
	//*********************
	//***** MAIN LOOP *****
	//*********************
	//*********************
	while(1)						//(Do forever)
	{
		//----- RESET THE WATCHDOG TIMEOUT TIMER -----
		ClrWdt();



		//----- PROCESS RFID DRIVER -----
		rfid_process();


		//----- PROCESS OUR USER MODE -----
		process_user_mode();

		
	}
}


//*******************************************
//*******************************************
//********** PROCESS OUR USER MODE **********
//*******************************************
//*******************************************
void process_user_mode (void)
{
	static BYTE doing_read = 0;
	BYTE result;
	
	
	if (!doing_read)
	{
		//-----------------------------------
		//----- WAITING TO DO NEXT READ -----
		//-----------------------------------
		if (general_use_10ms_timer == 0)
		{
			//----- START THE NEXT READ -----
			rfid_read_tag();
			
			doing_read = 1;
			general_use_10ms_timer = 25;
		}
	}
	else
	{
		//----------------------------------------
		//----- WAITING FOR READ TO COMPLETE -----
		//----------------------------------------
		result = rfid_get_read_tag_result();
		if (result == 0)
			return;			//Not complete
		
		if (result == 1)
		{
			//----- VALID RFID TAG DETECTED -----
			RED_LED_ON = 0;
			GREEN_LED_ON = 1;
		}	
		else
		{
			//----- READ ERROR - NO VALID TAG DETECTED -----
			RED_LED_ON = 1;
			GREEN_LED_ON = 0;
		}	
		doing_read = 0;
	}	
	
}	



//*************************************************************************************************************************************
//*************************************************************************************************************************************
//*************************************************************************************************************************************
//*************************************************************************************************************************************
//*************************************************************************************************************************************
//*************************************************************************************************************************************
//*************************************************************************************************************************************

//************************************************
//************************************************
//********** INTERRUPT VECTOR LOCATIONS **********
//************************************************
//************************************************

//***********************************
//***** HIGH PRIORITY INTERRUPT *****
//***********************************
#pragma code highVector=0x08			//This is the vector address for a PIC18 high priority interrupt 
void athighvector (void)
{
	_asm 	GOTO	interruptisrh	_endasm
}
#pragma code 		//(Return to normal)

//**********************************
//***** LOW PRIORITY INTERRUPT *****
//**********************************
#pragma code lowVector=0x18			//This is the vector address for a PIC18 low priority interrupt 
void atlowvector (void)
{
	_asm	goto	interruptisrl	_endasm
}
#pragma code 		//(Return to normal)



//*************************************************************
//*************************************************************
//********** HIGH PRIORITY INTERRUPT SERVICE ROUTINE **********
//*************************************************************
//*************************************************************
//#pragma interrupt interruptisrh			//<<< This is the normal C18 compiler definition to indicate the interrupt high priority function.
#pragma interruptlow interruptisrh			//<<<For PIC18 devices with the silicon bug that means the RETFIE FAST instruction cannot be used use this C18
											//compiler definition so that the low priority context saving aproach is used (if not sure just use this)
void interruptisrh (void)
{
	static BYTE hb_10ms_timer = 0;
	static BYTE hb_100ms_timer = 0;
	static BYTE hb_1sec_timer = 0;
	static WORD w_temp;
	static WORD rfid_timer_value;
	static WORD rfid_timer_value_last;
	

	if (INTCONbits.INT0IE && INTCONbits.INT0IF)
	{
		//------------------------------------------------------------
		//----- RFID MODULATED DATA RISING EDGE DETECT INTERRUPT -----
		//------------------------------------------------------------
		//Get the rfid timer value (we need this with as little latency after the rising edge as possible)
		rfid_timer_value = (WORD)TMR1L;						//(read low byte first which loads high byte for this PIC18)
		rfid_timer_value |= ((WORD)TMR1H) << 8;
		
		//We need to pass the time since the last interrupt to the rfid fucntion x 1uS.
		//For this application and microcontroller our 16bit timer1 value has been configured to increment every 125nS.
		//We only need to allow for up to 1280uS so we have plenty of headroom to divide this down to the required 1uS resolution.
		
		w_temp = rfid_timer_value - rfid_timer_value_last;		//Get difference from last time (roll overs are inherantly dealt with)
		rfid_timer_value_last = rfid_timer_value;
		w_temp = w_temp >> 3;									//Divide to give 1uS resolution
		
		//Call the RFID function to process the event
		rfid_sampling_rising_edge(w_temp);
	}




	//------------------------
	//----- USART TX IRQ -----
	//------------------------
	//if(PIE1bits.TXIE && PIR1bits.TXIF)
	//	usart_transmit_irq();

	//------------------------
	//----- USART RX IRQ -----
	//------------------------
	//if(PIE1bits.RCIE && PIR1bits.RCIF)
	//	usart_receive_irq();


	//-----------------------------
	//-----------------------------
	//----- TIMER 0 HEARTBEAT -----
	//-----------------------------
	//-----------------------------
	if(INTCONbits.TMR0IF && INTCONbits.TMR0IE)
	{
		INTCONbits.TMR0IF = 0;					//Reset the timer 0 irq flag

		_asm
		//Reset timer for the next rollover (in assembler for accuracy)
		movff	TMR0L,asm_irq_temp				//read the current timer value (read low byte loads the high byte)
		movff	TMR0H,asm_irq_temp1
		movlw	((0x10000 - (((INSTRUCTION_CLOCK_FREQUENCY / 1000) / 2) - 14)) & 0x00ff)		//<<< /100 for 10mS irq, /1000 for 1mS irq
		addwf	asm_irq_temp,1,0				//(1 = file register, 0 = access ram)
		movlw	((0x10000 - (((INSTRUCTION_CLOCK_FREQUENCY / 1000) / 2) - 14)) >> 8)			//<<< /100 for 10mS irq, /1000 for 1mS irq
		addwfc	asm_irq_temp1,1,0				//(1 = file register, 0 = access ram)
		movff	asm_irq_temp1,TMR0H				//Store new value (high byte first)
		movff	asm_irq_temp,TMR0L
		_endasm

		//The above code takes 12 instruction cycles
		//The timer is inhibited for 2 instruction cycles after the low byte is written
		//Therefore subtract 14 from the value to be written
		//For 24MHz oscillator 1mS irq = 6000 cycles.  Minus 14 = 5986.  /2 as timer uses 1:2 prescaller = 2993.  0x10000 - 2993 = 0xF44F
		//For 24MHz oscillator 10mS irq = 60000 cycles.  Minus 14 = 59986.  /2 as timer uses 1:2 prescaller = 29993.  0x10000 - 29993 = 0x8AD7
		//For 40MHz oscillator 1mS irq = 10000 cycles.  Minus 14 = 9986.  /2 as timer uses 1:2 prescaller = 4993.  0x10000 - 4993 = 0xec7f
		//For 40MHz oscillator 10mS irq = 100000 cycles.  Minus 14 = 99986.  /2 as timer uses 1:2 prescaller = 49993.  0x10000 - 49993 = 0x3cb7

		//-----------------------------
		//-----------------------------
		//----- HERE EVERY 1 mSec -----
		//-----------------------------
		//-----------------------------
	
		//----- RFID READER TIMER -----
		if (rfid_1ms_timer)
			rfid_1ms_timer--;

		hb_10ms_timer++;
		if (hb_10ms_timer == 10)
		{
			//-------------------------------
			//-------------------------------
			//----- HERE EVERY 10 mSec -----
			//-------------------------------
			//-------------------------------
			hb_10ms_timer = 0;
		


			//----- GENERAL USE 10mS TIMER -----
			if (general_use_10ms_timer)
				general_use_10ms_timer--;



		} //if (hb_10ms_timer == 10)

		hb_100ms_timer++;
		if (hb_100ms_timer == 100)
		{
			//-------------------------------
			//-------------------------------
			//----- HERE EVERY 100 mSec -----
			//-------------------------------
			//-------------------------------
			hb_100ms_timer = 0;

			//----- GENERAL USE 100mS TIMER -----
			if (general_use_100ms_timer)
				general_use_100ms_timer--;





			hb_1sec_timer++;
			if (hb_1sec_timer == 10)
			{
				//----------------------------
				//----------------------------
				//----- HERE EVERY 1 Sec -----
				//----------------------------
				//----------------------------
				hb_1sec_timer = 0;


			}
		}
	}



	//<<<<< ADD OTHER HIGH PRIORITY INTERRUPTS HERE <<<<<
	
	
}	
	

//************************************************************
//************************************************************
//********** LOW PRIORITY INTERRUPT SERVICE ROUTINE **********
//************************************************************
//************************************************************
#pragma interruptlow interruptisrl			//<<< C18 compiler definition to indicate the interrupt low priority function.
void interruptisrl (void)
{


	//<<<<< ADD OTHER LOW PRIORITY INTERRUPTS HERE <<<<<
	
	
}
#pragma code 		//(Return to normal)












