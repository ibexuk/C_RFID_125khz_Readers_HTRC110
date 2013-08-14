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
//DRIVER C CODE FILE



#include "main.h"					//Global data type definitions (see https://github.com/ibexuk/C_Generic_Header_File )
#define RFID_C
#include "rfid.h"




//******************************
//******************************
//********** READ TAG **********
//******************************
//******************************
//Call this function to start the read tag process
void rfid_read_tag (void)
{
	rfid_read_tag_pending = 1;
}



//*****************************************
//*****************************************
//********** READ TAG GET RESULT **********
//*****************************************
//*****************************************
//Call this function to check for the end of the read tag process
//Returns 0 if still reading, 1 if completed.  Bit 7 will be set if the read failed.
//	After a sucessful read the ASCII tag ID will be in rfid_data_nibbles[0] | rfid_data_nibbles[9]
BYTE rfid_get_read_tag_result (void)
{
	//----- CHECK FOR STILL READING -----
	if ((rfid_read_tag_pending) || (rfid_state != RFID_IDLE))
		return(0x00);
	
	//----- READ IS COMPLETE -----
	if (rfid_read_tag_success)
		return(0x01);				//Success
	else
		return(0x81);				//Failed
}



//**************************************
//**************************************
//********** FORCE INITIALISE **********
//**************************************
//**************************************
//In typical use this function is not requried as the driver will automatically initialise the HTRC110 IC on powerup.  However if you are using
//this driver with multiple HTRC110 devices (for instance by switching the SCLK, DIN and DOUT lines by using one or more 74HC4053 IC's) then this
//function can be called during your applications initialisation after selecting each HTRC110.
void rfid_force_initialise (void)
{
	rfid_state = RFID_INITIALISE;
	
	while (rfid_state != RFID_IDLE)
		rfid_process();
}	




//**********************************
//**********************************
//********** PROCESS RFID **********
//**********************************
//**********************************
//This function should be called reguarly as part of your applications main loop
void rfid_process (void)
{
	BYTE tx_data;
	BYTE response;
	WORD t_ant;
	BYTE count;

	switch (rfid_state)
	{
	case RFID_INITIALISE:
		//----------------------
		//----------------------
		//----- INITIALISE -----
		//----------------------
		//----------------------
		//Pause for >= 10mS for HTRC110 to be ready (specified in section 11.1 of the Philips "Read/Write Devices based on the HITAG Read/Write IC HTRC110" application note AN97070 rev 1.1)
		rfid_1ms_timer = 11;
		rfid_state = RFID_INITIALISE_1;
	
		#ifdef RFID_SPI_SAMPLE_CS_PIN
			RFID_SPI_SAMPLE_CS_PIN(1);
		#endif

		break;
		
		
		
	case RFID_INITIALISE_1:
		//------------------------
		//------------------------
		//----- INITIALISE 1 -----
		//------------------------
		//------------------------
		if (rfid_1ms_timer)
			break;

		rfid_state = RFID_SET_IDLE;
		break;
		
		

	case RFID_SET_IDLE:
		//----------------------
		//----------------------
		//----- GO TO IDLE -----
		//----------------------
		//----------------------
		rfid_gain = 2;									//Default to a gain value of 2 (g=500 - this is default).  Permitted values are 0, 1, 2 or 3.

		//SET_CONFIG_PAGE page=0x3 data=0x0 (DISLP1=0, DISSMARTCOMP=0, FSEL=00[4MHz osc])
		rfid_tx_rx_byte(0x70, 0);
		
		//SET_CONFIG_PAGE page=0x0 data=0xB (GAIN=##, FILTERH=1, FILTERL=1)
		tx_data = 0x43 | (rfid_gain << 2);
		rfid_tx_rx_byte(tx_data, 0);
		
		//SET_CONFIG_PAGE page=0x2 data=0xF (THRESET=1, ACQAMP=1, FREEZE=11)
		rfid_tx_rx_byte(0x6f, 0);

		//SET_CONFIG_PAGE page=0x1 data=0x5 (PD_MODE=0[active], PD=1[power down], HYSTERESIS=0[off], TXDIS=1[coil driver off])
		rfid_tx_rx_byte(0x55, 0);
		
		rfid_state = RFID_IDLE;
		break;
		
		
		
	case RFID_IDLE:
		//----------------
		//----------------
		//----- IDLE -----
		//----------------
		//----------------
		//RFID is in inactive low power state
		
		//----- CHECK FOR READ TAG REQUEST -----
		if (rfid_read_tag_pending)
			rfid_state = RFID_READ_TAG_1;
		
		break;



	case RFID_READ_TAG_1:
		//----------------------
		//----------------------
		//----- READ TAG 1 -----
		//----------------------
		//----------------------
				
		//SET_CONFIG_PAGE page=0x3 data=0x0 (DISLP1=0, DISSMARTCOMP=0, FSEL=00[4MHz osc])
		rfid_tx_rx_byte(0x70, 0);
		
		//SET_CONFIG_PAGE page=0x0 data=0xB (GAIN=##, FILTERH=1, FILTERL=1)
		tx_data = 0x43 | (rfid_gain << 2);
		rfid_tx_rx_byte(tx_data, 0);
		
		//SET_CONFIG_PAGE page=0x1 data=0x0 (PD_MODE=0[active], PD=0[active], HYSTERESIS=0[off], TXDIS=0[coil driver on])
		rfid_tx_rx_byte(0x50, 0);
		
		//SET_CONFIG_PAGE page=0x2 data=0xB (THRESET=1, ACQAMP=0, FREEZE=11)
		rfid_tx_rx_byte(0x6b, 0);
		
		//Pause for >= 5mS for HTRC110 to be ready (specified in section 11.4 of the Philips "Read/Write Devices based on the HITAG Read/Write IC HTRC110" application note AN97070 rev 1.1)
		rfid_1ms_timer = 6;
		rfid_state = RFID_READ_TAG_2;
		break;



	case RFID_READ_TAG_2:
		//----------------------
		//----------------------
		//----- READ TAG 2 -----
		//----------------------
		//----------------------
		if (rfid_1ms_timer)
			break;

		//SET_CONFIG_PAGE page=0x2 data=0x8 (THRESET=1, ACQAMP=0, FREEZE=00[normal operation])
		rfid_tx_rx_byte(0x68, 0);

		//Pause for >= 1mS for HTRC110 to be ready (specified in section 11.4 of the Philips "Read/Write Devices based on the HITAG Read/Write IC HTRC110" application note AN97070 rev 1.1)
		rfid_1ms_timer = 2;
		rfid_state = RFID_READ_TAG_3;
		break;



	case RFID_READ_TAG_3:
		//----------------------
		//----------------------
		//----- READ TAG 3 -----
		//----------------------
		//----------------------
		if (rfid_1ms_timer)
			break;

		//SET_CONFIG_PAGE page=0x2 data=0x0 (THRESET=0, ACQAMP=0, FREEZE=00)
		rfid_tx_rx_byte(0x60, 0);
		
		//----- CHECK THE ANTENNA IS OK -----
		//GET_CONFIG_PAGE page=2
		response = rfid_tx_rx_byte(0x06, 1);
		if (response & 0x10)
		{
			//ANTFAIL IS SET - Antenna failure
			rfid_state = RFID_READ_FAIL;
			break;			
		}	

		//----- SET THE SAMPLING TIME -----
		//READ_PHASE
		t_ant = (WORD)rfid_tx_rx_byte(0x08, 1);
		t_ant <<= 1;							//Multiply by 2
		t_ant += RFID_T_OC;						//Add the Offset Compensation value
		t_ant &= 0x3f;
		
		//SET_SAMPLING_TIME d=t_ant
		rfid_tx_rx_byte((0x80 | (BYTE)t_ant), 0);
		
		//GET_SAMPLING_TIME
		response = rfid_tx_rx_byte(0x02, 1);
		if (response != t_ant)
		{
			//Error - the sampling time value was not accepted for some reason
			rfid_state = RFID_READ_FAIL;
			break;			
		}

		//HTRC110 is now outputting a 125kHz sine wave.  The tag (if present) will be powered by this and it will then start transmitting its 64 bits as a continuous
		//cycle (it transmits in a loop as long as its powered by the RF field).  It transmits its bits by interfearing with the RF sine wave signal which the HTRC110
		//detects for us.  When we now start reading the tag data we will be given the raw data output of the tag after being filtered into a digital signal by the HTRC110.

		//----- START READING THE TAG DATA -----
		rfid_start_read_tag();

		rfid_1ms_timer = 150;					//Set a timeout
		rfid_state = RFID_READ_TAG_READING;
		
		//----- ENABLE THE MISO RISING EDGE INTERRUPT -----
		rfid_capture_state = RFID_CAPTURE_INITIALISE;
		rfid_tries_count = 0;
		RFID_CLEAR_MISO_RISING_EDGE_IRQ();
		RFID_ENABLE_MISO_RISING_EDGE_IRQ();
		//The interrupt called function deals with reading and demodulating the tag data
		break;
	
	

	case RFID_READ_TAG_READING:
		//----------------------------------------
		//----------------------------------------
		//----- READING TAG DEMODULATED DATA -----
		//----------------------------------------
		//----------------------------------------
		//The tag data length is 64 bits with between 64 to 127 bits needing to be received before we have a complete sequence.
		//We also need to initially receive 22 bits to enable us to determin the bit rate.
		//Therefore read can take 149 bits x 512uS maximum bit rate = 76mS.
		//Tags will typically be set to use the slowest 512uS bit rate as slow transitions on the air interface are easier to
		//detect, which means you get additional range and more reliable communications.
		//Note that if no tag is present we expect to receive bad data (the data line will not be idle) and we are likely to see
		//valid header strings.  However the 14 seperate parity checks will only pass when a valid tag is present.
		
		if (rfid_capture_state == RFID_CAPTURE_SUCCESS)
		{
			//-----------------------------
			//----- TAG HAS BEEN READ -----
			//-----------------------------
			rfid_end_read_tag();
			
			//Convert ID value to ASCII
			for (count = 0; count < 10; count++)
			{	
				rfid_data_nibbles[count] += 0x30;
				if (rfid_data_nibbles[count] > 0x39)
					rfid_data_nibbles[count] += 0x07;
			}
			
			
			rfid_read_tag_success = 1;
			rfid_read_tag_pending = 0;
			rfid_state = RFID_SET_IDLE;
			break;
		}	
		
		if ((rfid_1ms_timer == 0) || (rfid_capture_state == RFID_CAPTURE_FAILED_GIVEN_UP))
		{		
			//------------------------------
			//----- TAG READING FAILED -----
			//------------------------------
			rfid_end_read_tag();
			
			rfid_read_tag_success = 0;
			rfid_read_tag_pending = 0;
			rfid_state = RFID_SET_IDLE;
			RFID_DISABLE_MISO_RISING_EDGE_IRQ();		//Ensure irq is disabled
		}
		
		break;



	case RFID_READ_FAIL:
		//---------------------------
		//---------------------------
		//----- READ TAG FAILED -----
		//---------------------------
		//---------------------------
		rfid_read_tag_success = 0;
		rfid_read_tag_pending = 0;
		
		rfid_state = RFID_SET_IDLE;
		break;

	
	} //switch (rfid_state)
	
}	






//*****************************************************************
//*****************************************************************
//********** SAMPLING PHASE - DATA RISING EDGE INTERRUPT **********
//*****************************************************************
//*****************************************************************
//Call with:
//	us_since_last_rising_edge	The time in uS since the last edge interrupt.  You application interrupt hander must create this value from an available microcontroller timer.
//								The maximum time between interrupts will be approximately 1024uS, so your timer calculation only needs to deal with time values of up to say
//								1280uS (to give a reasonable margin of worst case tollerance).
//								Call with the timer value captured as close as possible to the rising edge occuring (reduce latency as much as possible)
//								This function will ignore the value for the first 2 calls allowing for initial timer error.
void rfid_sampling_rising_edge (WORD us_since_last_rising_edge)
{
	static WORD rfid_1us_bit_rate;		//(As this is an interrupt function we define all varaibles as static to avoid the compiler needing to waste time pushing values onto the stack etc before calling)
	static WORD w_temp;
	static WORD rfid_1us_125_value;
	static WORD rfid_1us_175_value;
	static BYTE rfid_bit_type;
	static BYTE manchester_header_bit_count;
	static BYTE biphase_header_bit_count;
	static BYTE rfid_modulation_is_manchester;
	static BYTE rfid_carry;
	static BYTE last_bit_state;
	static BYTE b_temp;
	

	//-------------------------------------
	//----- NEXT RISING EDGE DETECTED -----
	//-------------------------------------
		
	//Reset the irq if necessary
	RFID_CLEAR_MISO_RISING_EDGE_IRQ();
	

	//----------------------------------
	//----- DETERMINE THE BIT TYPE -----
	//----------------------------------
	if (rfid_capture_state >= RFID_CAPTURE_GET_HEADER)
	{
		if (us_since_last_rising_edge < rfid_1us_125_value)
			rfid_bit_type = 100;					//rfid_1us_bit_rate x 1
		else if (us_since_last_rising_edge < rfid_1us_175_value)
			rfid_bit_type = 150;					//rfid_1us_bit_rate x 1.5
		else
			rfid_bit_type = 200;					//rfid_1us_bit_rate x 2
	}


	switch (rfid_capture_state)
	{
	case RFID_CAPTURE_INITIALISE:
		//---------------------------------
		//---------------------------------
		//----- SETUP FOR NEW CAPTURE -----
		//---------------------------------
		//---------------------------------
		rfid_bit_count = 0;
		rfid_1us_bit_rate = 0xffff;

		rfid_capture_state = RFID_CAPTURE_GET_BIT_RATE;
		break;



	case RFID_CAPTURE_GET_BIT_RATE:
		//------------------------------
		//------------------------------
		//----- GET BIT RATE PHASE -----
		//------------------------------
		//------------------------------
		//We are looking for the minimum high edge to high edge time to determine the tags bit rate

		if (rfid_bit_count > 2)		//Ignore the first few transitions in case of initial timing error and switchover garbage from the HTRC110
		{
			if (us_since_last_rising_edge < rfid_1us_bit_rate)				//If this cycle was the shortest yet then store it
				rfid_1us_bit_rate = us_since_last_rising_edge;
		}

		
		if (rfid_bit_count >= 21)				//We need at least 14 bits to ensure that we will have seen a short cycle (worst case is BiPhase 111111111 header followed by 1111 data bits then the parity woudl be the first short bit cycle)
		{
			//----------------------------------
			//------- WE HAVE THE BIT RATE -----
			//----------------------------------
			//125kHz = 8uS per cycle.  Our timer value increments every 1uS.
			//For Manchester or BiPhase the tag will be responding with 1 bit every 64, 32 or 16 periods of the carrier frequency, and will change state once
			//or twice during this (the tag is allowed to use any of these three as its data rate).  However we only need to detect each rising edge to determine
			//the bit states, reducing the interrupt burden on the main application.  Also only detecting each rising edge removes issues of dealing with non perfect
			//high and low widths within each cycle - being radio they are often not perfectly 50/50, but the overall bit frequency will be relatively accurate.
			//So for either of these modulations our bit rate will be approximately 128uS, 256uS or 512uS.
			//If the tag is PSK it will be changing state every carrier frequency period (every 8uS)



			if (rfid_1us_bit_rate < 76)				//The fastest possible Manchester or BiPhase cycle bit rate is 128uS but give a good level of tollerance
			{
				//-----------------------------
				//----- MODULATION IS PSK -----
				//-----------------------------
				//PSK cycle rate is 16uS. 1 transition every 125kHz cycle
				//PSK is not used by typical tags as many readers don't support it due to the complexity burden of decoding it.
				rfid_capture_state = RFID_CAPTURE_FAILED;		//PSK is not supported
				break;					
			}
			//-----------------------------------------------
			//----- MODULATION IS MANCHESTER OR BIPHASE -----
			//-----------------------------------------------
			//rfid_1us_bit_rate will equal approximately 512, 256 or 128 caused by the shortest high edge to high edge complete cycle being 512uS, 256uS or 128uS long.
			//(the actual fastest signal edge transition period being every 256uS, 128uS or 64uS)

			//Check for bit rate not out of range
			if (rfid_1us_bit_rate > 720)
			{
				rfid_capture_state = RFID_CAPTURE_FAILED;
				break;					
			}
			//----- CALCULATE THE CYCLE TIMES -----
			//Now that we have the bit rate (the fastest high edge to high edge time) we can determine the 3 possible time values.
			//A high edge may take:
			//	rfid_1us_bit_rate x 1
			//	rfid_1us_bit_rate x 1.5
			//	rfid_1us_bit_rate x 2
			//In order to determine which has just occured we need a time value for (rfid_1us_bit_rate x 1.25) and (rfid_1us_bit_rate x 1.75)
			w_temp = rfid_1us_bit_rate >> 2;		//Get rfid_1us_bit_rate x 0.25
			rfid_1us_125_value = rfid_1us_bit_rate + w_temp;
			rfid_1us_175_value = rfid_1us_bit_rate + w_temp + w_temp + w_temp;

			rfid_capture_state = RFID_CAPTURE_GET_HEADER;
			rfid_bit_count = 0;
			manchester_header_bit_count = 0;
			biphase_header_bit_count = 0;
			break;
		}
		
		rfid_bit_count++;
		break;




	case RFID_CAPTURE_GET_HEADER:
		//-------------------------------------------
		//-------------------------------------------
		//----- WAIT FOR THE 9 HIGH BITS HEADER -----
		//-------------------------------------------
		//-------------------------------------------
		//The stop bit followed by header is the following sequence of bits : 0111111111
		//This cannot occur anywhere else in the tags 64 bit stream.
		//For Manchester encoding the header will be a sequence of rfid_bit_type=100 repeating 8 times for the 9 high bits.
		//For BiPhase encoding the header will be a sequence of rfid_bit_type=200 repeating 4 times for the 9 high bits.
		//The header is the only position in the bit stream where either of these sequences can occur.
		
		if (rfid_bit_type == 100)
			manchester_header_bit_count++;
		else
			manchester_header_bit_count = 0;
		
		if (rfid_bit_type == 200)
			biphase_header_bit_count++;
		else
			biphase_header_bit_count = 0;
		
		if (manchester_header_bit_count == 8)
		{
			//----- MANCHESTER ENCODING HEADER DETECTED -----
			rfid_modulation_is_manchester = 1;
			rfid_bit_count = 0;
			rfid_carry = 0;
			last_bit_state = 1;
			rfid_capture_state = RFID_CAPTURE_GET_DATA;
			break;
		}	
		
		if (biphase_header_bit_count == 8)
		{
			//----- BI-PHASE ENCODING HEADER DETECTED -----
			rfid_modulation_is_manchester = 0;
			rfid_bit_count = 0;
			rfid_carry = 0;
			last_bit_state = 1;
			rfid_capture_state = RFID_CAPTURE_GET_DATA;
			break;
		}	
		
		if (rfid_bit_count > 128)
		{
			//----- GIVE UP -----
			//We should have seen the header by now
			rfid_capture_state = RFID_CAPTURE_FAILED;
			break;
		}
		rfid_bit_count++;
		break;
		


	case RFID_CAPTURE_GET_DATA:
		//----------------------------------------------
		//----------------------------------------------
		//----- GET THE 54 BITS OF DATA AND PARITY -----
		//----------------------------------------------
		//----------------------------------------------

		//----- MODULATION DEFINITIONS FOR EM4102 BASED RFID TRANSPONDERS -----
		//Manchester Modulation:
		//	This is the default encoding method used by most tags.
		//	The tag will produce a level transition in the middle of the bit period.
		//	Low to high = logic 1, high to low = logic 0.
		//	There is always a transition in the middle of the bit period, but only a transition at the beginning of the bit period if requried for the next bit state.
		//
		//BiPhase Modulation:
		//	There is aways transition at the beginning of the bit period
		//	For logic 0 there is also a transition in the centre of the bit period
		//	For logic 1 there is no transition in the centre of the bit period
		
		//----- HOW THE DRIVER DETECTS THE BIT VALUES -----
		//This driver only detects the rising edge of the data stream.  This has been chosen as it reduces the demands on the microcontroller (half the number
		//of interrupts occur) and because from this each bit state can be determined.  The following is how it's done, and if you wish to understand the
		//operation it's easiest if you have example datastreams to hand.
		//Note that whilst Manchester and BiPhase modulation are standard approaches, their implementation differes depending on use.  EM4102 based tags use the
		//modulation in a specific way and th ebelow approach will not necessarily be true for other protocols using Manchester of BiPhase modulation.
		//We have the following variables:
		//	rfid_bit_type = 100		A 100% high edge to high edge cycle time has just occured
		//	rfid_bit_type = 150		A 150% high edge to high edge cycle time has just occured
		//	rfid_bit_type = 200		A 200% high edge to high edge cycle time has just occured
		//	rfid_carry				A flag we use to store carry when rfid_bit_type = 150
		//
		//For Manchester Modulation:
		//	When rfid_bit_type=100 the next bit is the same as the last bit
		//	When rfid_bit_type=150 and rfid_carry=0 the next bit is the inverse of the last bit, set rfid_carry
		//	When rfid_bit_type=150 and rfid_carry=1 the next bit is the same as the last bit, then the next bit is the inverse of the last bit, clear rfid_carry
		//	When rfid_bit_type=200 the next bit is the inverse of the last bit, and the next bit is the inverse again (i.e. 01 or 10)
		//(When rfid_carry=1 we are out of phase with the bitrate period)
		//
		//For BiPhase Modulation:
		//	If rfid_carry=0
		//		When rfid_bit_type=100 the next bit is 0
		//		When rfid_bit_type=150 the next bit is 1, set rfid_carry
		//		When rfid_bit_type=200 the next bits are 11
		//	If rfid_carry=1
		//		When rfid_bit_type=100 the next bit is 0, don't clear rfid_carry
		//		When rfid_bit_type=150 the next bits are 01, clear rfid_carry
		//		When rfid_bit_type=200 - not possible
		//(When rfid_carry=1 we are out of phase with the bitrate period)		
		
			
		if (rfid_modulation_is_manchester)
		{
			//---------------------------------
			//----- MANCHESTER MODULATION -----
			//---------------------------------
			if (rfid_bit_type == 100)
			{
				//The next bit is the same as the last bit
				b_temp = rfid_sampling_rising_edge_add_bit(last_bit_state);
			}
			else if (rfid_bit_type == 150)
			{
				if (!rfid_carry)
				{
					//The next bit is the inverse of the last bit, set rfid_carry
					last_bit_state ^= 1;
					b_temp = rfid_sampling_rising_edge_add_bit(last_bit_state);
					rfid_carry = 1;
				}	
				else
				{
					//The next bit is the same as the last bit, then the next bit is the inverse of the last bit, clear rfid_carry
					b_temp = rfid_sampling_rising_edge_add_bit(last_bit_state);
					last_bit_state ^= 1;
					b_temp = rfid_sampling_rising_edge_add_bit(last_bit_state);
					rfid_carry = 0;
					rfid_bit_count++;
				}	
			}
			else if (rfid_bit_type == 200)
			{
				//The next bit is the inverse of the last bit, and the next bit is the inverse again
				last_bit_state ^= 1;
				b_temp = rfid_sampling_rising_edge_add_bit(last_bit_state);
				last_bit_state ^= 1;
				b_temp = rfid_sampling_rising_edge_add_bit(last_bit_state);
				rfid_bit_count++;
			}
		}
		else
		{
			//-------------------------------
			//----- BI-PHASE MODULATION -----
			//-------------------------------
			if (!rfid_carry)
			{
				if (rfid_bit_type == 100)
				{
					//The next bit is 0
					b_temp = rfid_sampling_rising_edge_add_bit(0);
				}
				else if (rfid_bit_type == 150)
				{
					//The next bit is 1, set rfid_carry
					b_temp = rfid_sampling_rising_edge_add_bit(1);
					rfid_carry = 1;
				}
				else if (rfid_bit_type == 200)
				{
					//The next bits are 11
					b_temp = rfid_sampling_rising_edge_add_bit(1);
					b_temp = rfid_sampling_rising_edge_add_bit(1);
					rfid_bit_count++;
				}
			}
			else
			{
				if (rfid_bit_type == 100)
				{
					//The next bit is 0, don't clear rfid_carry
					b_temp = rfid_sampling_rising_edge_add_bit(0);
				}
				else if (rfid_bit_type == 150)
				{
					//The next bits are 01, clear rfid_carry
					b_temp = rfid_sampling_rising_edge_add_bit(0);
					b_temp = rfid_sampling_rising_edge_add_bit(1);
					rfid_carry = 0;
					rfid_bit_count++;
				}
				else if (rfid_bit_type == 200)
				{
					//not possible
					rfid_capture_state = RFID_CAPTURE_FAILED;
					break;
				}
			}
		}
		
		if (b_temp)
		{
			//-----------------------------
			//----- ALL BITS RECEIVED -----
			//-----------------------------
			if (b_temp & 0x02)
				rfid_capture_state = RFID_CAPTURE_FAILED;
			else
				rfid_capture_state = RFID_CAPTURE_SUCCESS;
			break;
		}	

		rfid_bit_count++;
		break;




	case RFID_CAPTURE_SUCCESS:
		//----------------------------
		//----------------------------
		//----- CAPTURE COMPELTE -----
		//----------------------------
		//----------------------------
		RFID_DISABLE_MISO_RISING_EDGE_IRQ();
		
		//The main state machine in rfid_process() will detect this state and end the read state.
		break;



	case RFID_CAPTURE_FAILED:
		//--------------------------
		//--------------------------
		//----- CAPTURE FAILED -----
		//--------------------------
		//--------------------------
		//There has either been an error or there is no tag present
		
		if (rfid_tries_count < 5)
		{
			//----- RETRY -----
			//When a tag is present and in range there is typcially no need for a retry - it will typically work first time.
			//However it doesn't hurt to try again a few times in case it helps where the signal is week.
			//As this driver checks for parity errors as nibbles are received (rather than at the end) failures can occur
			//early on in the bit sequence, so doing this will often mean we are ready to try again at the start of the very
			//next sequence from the tag.
			rfid_tries_count++;
			rfid_capture_state = RFID_CAPTURE_INITIALISE;
		}
		else
		{		
			rfid_capture_state = RFID_CAPTURE_FAILED_GIVEN_UP;
		}
		break;



	case RFID_CAPTURE_FAILED_GIVEN_UP:
		//-----------------------------------------------------------
		//-----------------------------------------------------------
		//----- CAPTURE FAILED MULTIPLE TIMES SO WE'VE GIVEN UP -----
		//-----------------------------------------------------------
		//-----------------------------------------------------------
		RFID_DISABLE_MISO_RISING_EDGE_IRQ();
		
		//The main state machine in rfid_process() will detect this state and end the read state.
		break;
		
	} //switch (rfid_state)

}




//************************************************************************************************************************************************************
//************************************************************************************************************************************************************
//************************************************************************************************************************************************************
//************************************************************************************************************************************************************
//************************************************************************************************************************************************************
//************************************************************************************************************************************************************
//************************************************************************************************************************************************************
//INTERNAL DRIVER FUNCTIONS BELOW HERE


//***************************************
//***************************************
//********** ADD NEXT DATA BIT **********
//***************************************
//***************************************
//Returns:
//	0	More bits to get
//	1	All bits received and value is valid
//	3	All bits received and value is bad
BYTE rfid_sampling_rising_edge_add_bit (BYTE bit_state)
{
	static BYTE nibble_index;
	static BYTE bit_index;
	static BYTE return_value;
	static BYTE no_of_ones;
	static WORD parity_check_data;
	
	
	if (rfid_bit_count == 0)
	{
		//----- FIRST BIT - INITIALISE THE REGISTERS -----
		return_value = 0;
		nibble_index = 0;
		bit_index = 0;
		rfid_data_nibbles[0] = 0;
		rfid_data_nibbles[1] = 0;
		rfid_data_nibbles[2] = 0;
		rfid_data_nibbles[3] = 0;
		rfid_data_nibbles[4] = 0;
		rfid_data_nibbles[5] = 0;
		rfid_data_nibbles[6] = 0;
		rfid_data_nibbles[7] = 0;
		rfid_data_nibbles[8] = 0;
		rfid_data_nibbles[9] = 0;
		rfid_data_nibbles[10] = 0;
	}
	
	if (nibble_index > 10)			//Check just in case
		return(return_value);

	
	switch (bit_index)
	{
	case 0:
		//----- BIT 3 -----
		if (bit_state)
			rfid_data_nibbles[nibble_index] |= 0x08;
		bit_index++;
		break;
		
	case 1:
		//----- BIT 2 -----
		if (bit_state)
			rfid_data_nibbles[nibble_index] |= 0x04;
		bit_index++;
		break;
		
	case 2:
		//----- BIT 1 -----
		if (bit_state)
			rfid_data_nibbles[nibble_index] |= 0x02;
		bit_index++;
		break;
		
	case 3:
		//----- BIT 0 -----
		if (bit_state)
			rfid_data_nibbles[nibble_index] |= 0x01;
		bit_index++;
		break;
		
	case 4:
		//----- PARITY -----
		if (nibble_index < 10)
		{
			//----- JUST COMPLETED NEXT 5 BIT NIBBLE -----
			
			//Check the parity is even (total number of high bits should be even)
			parity_check_data = (WORD)rfid_data_nibbles[nibble_index];
			no_of_ones = 0;
			while (parity_check_data)
			{
				no_of_ones++;
				parity_check_data &= (parity_check_data - 1);
			}
			//no_of_ones = the number of high bits
			if (
			((no_of_ones & 0x01) && (bit_state == 0)) ||
			(((no_of_ones & 0x01) == 0) && (bit_state))
			)
			{
				//----- PARITY ERROR -----
				return_value = 3;
			}
		}
		else
		{
			//----- JUST GOT THE FINAL STOP BIT -----
			if (bit_state)
			{
				//----- ERROR - STOP BIT SHOULD BE LOW -----
				return_value = 3;
			}
			else
			{
				//----- CHECK THE COLUMN PARITY -----
				return_value = 1;		//Default to parity good
				for (bit_index = 0x08; bit_index > 0; bit_index >>= 1)
				{
					parity_check_data = 0;
					if (rfid_data_nibbles[0] & bit_index)
						parity_check_data |= 0x0200;
					if (rfid_data_nibbles[1] & bit_index)
						parity_check_data |= 0x0100;
					if (rfid_data_nibbles[2] & bit_index)
						parity_check_data |= 0x0080;
					if (rfid_data_nibbles[3] & bit_index)
						parity_check_data |= 0x0040;
					if (rfid_data_nibbles[4] & bit_index)
						parity_check_data |= 0x0020;
					if (rfid_data_nibbles[5] & bit_index)
						parity_check_data |= 0x0010;
					if (rfid_data_nibbles[6] & bit_index)
						parity_check_data |= 0x0008;
					if (rfid_data_nibbles[7] & bit_index)
						parity_check_data |= 0x0004;
					if (rfid_data_nibbles[8] & bit_index)
						parity_check_data |= 0x0002;
					if (rfid_data_nibbles[9] & bit_index)
						parity_check_data |= 0x0001;
					
					no_of_ones = 0;
					while (parity_check_data)
					{
						no_of_ones++;
						parity_check_data &= (parity_check_data - 1);
					}
					//no_of_ones = the number of high bits
					if (
					((no_of_ones & 0x01) && ((rfid_data_nibbles[10] & bit_index) == 0)) ||
					(((no_of_ones & 0x01) == 0) && ((rfid_data_nibbles[10] & bit_index)))
					)
					{
						//----- PARITY ERROR -----
						return_value = 3;
					}
				} //for (bit_index = 0x08; bit_index > 0; bit_index >>= 1)
			}	
		}	
			
		bit_index = 0;
		nibble_index++;
		break;
		
	}
		
	
	return(return_value);
}




//**********************************
//**********************************
//********** TX / RX BYTE **********
//**********************************
//**********************************
BYTE rfid_tx_rx_byte (BYTE tx_data, BYTE get_response)
{
	BYTE rx_data = 0;


	RFID_SCLK(0);
	RFID_BIT_DELAY();			//Ensure the low state is long enough

	//INITIALISE THE SERIAL INTERFACE
	//(Low to high transition of DIN while SCLK is high marks the start of a transfer)
	RFID_MOSI(0);
	RFID_SETUP_DELAY();
	RFID_SCLK(1);
	RFID_SETUP_DELAY();
	RFID_MOSI(1);
	RFID_BIT_DELAY();


	//BIT 7
	RFID_SCLK(0);
	RFID_BIT_DELAY();
	
	#ifdef RFID_SPI_SAMPLE_CS_PIN
		RFID_SPI_SAMPLE_CS_PIN(0);
	#endif
	
	if (tx_data & 0x80)
		RFID_MOSI(1);
	else
		RFID_MOSI(0);
	RFID_SCLK(1);		//Data in valid in high state
	RFID_BIT_DELAY();

	//BIT 6
	RFID_SCLK(0);
	RFID_BIT_DELAY();
	if (tx_data & 0x40)
		RFID_MOSI(1);
	else
		RFID_MOSI(0);
	RFID_SCLK(1);
	RFID_BIT_DELAY();

	//BIT 5
	RFID_SCLK(0);
	RFID_BIT_DELAY();
	if (tx_data & 0x20)
		RFID_MOSI(1);
	else
		RFID_MOSI(0);
	RFID_SCLK(1);
	RFID_BIT_DELAY();

	//BIT 4
	RFID_SCLK(0);
	RFID_BIT_DELAY();
	if (tx_data & 0x10)
		RFID_MOSI(1);
	else
		RFID_MOSI(0);
	RFID_SCLK(1);
	RFID_BIT_DELAY();

	//BIT 3
	RFID_SCLK(0);
	RFID_BIT_DELAY();
	if (tx_data & 0x08)
		RFID_MOSI(1);
	else
		RFID_MOSI(0);
	RFID_SCLK(1);
	RFID_BIT_DELAY();

	//BIT 2
	RFID_SCLK(0);
	RFID_BIT_DELAY();
	if (tx_data & 0x04)
		RFID_MOSI(1);
	else
		RFID_MOSI(0);
	RFID_SCLK(1);
	RFID_BIT_DELAY();

	//BIT 1
	RFID_SCLK(0);
	RFID_BIT_DELAY();
	if (tx_data & 0x02)
		RFID_MOSI(1);
	else
		RFID_MOSI(0);
	RFID_SCLK(1);
	RFID_BIT_DELAY();

	//BIT 0
	RFID_SCLK(0);
	RFID_BIT_DELAY();
	if (tx_data & 0x01)
		RFID_MOSI(1);
	else
		RFID_MOSI(0);
	RFID_SCLK(1);
	RFID_BIT_DELAY();


	if (get_response)
	{
		//BIT 7
		RFID_SCLK(0);
		RFID_BIT_DELAY();
		RFID_SCLK(1);		//Data out valid in high state
		RFID_BIT_DELAY();
		if (RFID_MISO)
			rx_data |= 0x80;
	
		//BIT 6
		RFID_SCLK(0);
		RFID_BIT_DELAY();
		RFID_SCLK(1);
		RFID_BIT_DELAY();
		if (RFID_MISO)
			rx_data |= 0x40;
	
		//BIT 5
		RFID_SCLK(0);
		RFID_BIT_DELAY();
		RFID_SCLK(1);
		RFID_BIT_DELAY();
		if (RFID_MISO)
			rx_data |= 0x20;
	
		//BIT 4
		RFID_SCLK(0);
		RFID_BIT_DELAY();
		RFID_SCLK(1);
		RFID_BIT_DELAY();
		if (RFID_MISO)
			rx_data |= 0x10;
	
		//BIT 3
		RFID_SCLK(0);
		RFID_BIT_DELAY();
		RFID_SCLK(1);
		RFID_BIT_DELAY();
		if (RFID_MISO)
			rx_data |= 0x08;
	
		//BIT 2
		RFID_SCLK(0);
		RFID_BIT_DELAY();
		RFID_SCLK(1);
		RFID_BIT_DELAY();
		if (RFID_MISO)
			rx_data |= 0x04;
	
		//BIT 1
		RFID_SCLK(0);
		RFID_BIT_DELAY();
		RFID_SCLK(1);
		RFID_BIT_DELAY();
		if (RFID_MISO)
			rx_data |= 0x02;
	
		//BIT 0
		RFID_SCLK(0);
		RFID_BIT_DELAY();
		RFID_SCLK(1);
		RFID_BIT_DELAY();
		if (RFID_MISO)
			rx_data |= 0x01;
	}
		
	RFID_SCLK(0);
	
	#ifdef RFID_SPI_SAMPLE_CS_PIN
		RFID_SPI_SAMPLE_CS_PIN(1);
	#endif

	return(rx_data);
}



//************************************
//************************************
//********** START READ_TAG **********
//************************************
//************************************
void rfid_start_read_tag (void)
{
	RFID_SCLK(0);
	RFID_BIT_DELAY();			//Ensure the low state is long enough

	//INITIALISE THE SERIAL INTERFACE
	//(Low to high transition of DIN while SCLK is high marks the start of a transfer)
	RFID_MOSI(0);
	RFID_SETUP_DELAY();
	RFID_SCLK(1);
	RFID_SETUP_DELAY();
	RFID_MOSI(1);
	RFID_BIT_DELAY();
	 

	//BIT 7
	RFID_SCLK(0);
	RFID_BIT_DELAY();
	
	#ifdef RFID_SPI_SAMPLE_CS_PIN
		RFID_SPI_SAMPLE_CS_PIN(0);
	#endif
	
	RFID_MOSI(1);
	RFID_SCLK(1);		//Data in valid in high state
	RFID_BIT_DELAY();

	//BIT 6
	RFID_SCLK(0);
	RFID_BIT_DELAY();
	RFID_MOSI(1);
	RFID_SCLK(1);
	RFID_BIT_DELAY();

	//BIT 5
	RFID_SCLK(0);
	RFID_BIT_DELAY();
	RFID_MOSI(1);
	RFID_SCLK(1);
	RFID_BIT_DELAY();

	RFID_SCLK(0);
	RFID_MOSI(0);
	
	//THIS HTRC110 NOW IMMEDIATELY SWITCHES TO TRANSPARENT MODE.  THE TRASNPONDER DATA IS DIRECTLY PRESENTED AT DOUT.
	//Delay from RX to DOUT is dependant on demodulator settings.
	
}	



//**********************************
//**********************************
//********** END READ_TAG **********
//**********************************
//**********************************
void rfid_end_read_tag (void)
{

	//----- TERMIATE READ_TAG MODE -----
	//Low to high transition on SCLK
	RFID_SCLK(1);
	
	#ifdef RFID_SPI_SAMPLE_CS_PIN
		RFID_SPI_SAMPLE_CS_PIN(0);
	#endif
}
