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
//PIC18 SAMPLE PROJECT C CODE HEADER FILE






//*****************************
//*****************************
//********** DEFINES **********
//*****************************
//*****************************
#ifndef MAIN_C_INIT				//(Include this section only once for each source file that includes this header file)
#define	MAIN_C_INIT



#define	RED_LED_ON						LATBbits.LATB3
#define	GREEN_LED_ON					LATBbits.LATB4


#endif







//*******************************
//*******************************
//********** FUNCTIONS **********
//*******************************
//*******************************
#ifdef MAIN_C
//-----------------------------------
//----- INTERNAL ONLY FUNCTIONS -----
//-----------------------------------
void initialise (void);
void process_user_mode (void);
void interruptisrh (void);
void interruptisrl (void);


//-----------------------------------------
//----- INTERNAL & EXTERNAL FUNCTIONS -----
//-----------------------------------------
//(Also defined below as extern)


#else
//------------------------------
//----- EXTERNAL FUNCTIONS -----
//------------------------------


#endif






//****************************
//****************************
//********** MEMORY **********
//****************************
//****************************
#ifdef MAIN_C
//--------------------------------------------
//----- INTERNAL ONLY MEMORY DEFINITIONS -----
//--------------------------------------------


//Inline assembler access bank variables
#pragma udata access acssect 			//'acssect' is the section label
near BYTE asm_irq_temp;					//'near' qualifier specifies access RAM
near BYTE asm_irq_temp1;
#pragma udata							//(Return to normal)


//--------------------------------------------------
//----- INTERNAL & EXTERNAL MEMORY DEFINITIONS -----
//--------------------------------------------------
//(Also defined below as extern)
WORD general_use_10ms_timer = 0;
WORD general_use_100ms_timer;


#else
//---------------------------------------
//----- EXTERNAL MEMORY DEFINITIONS -----
//---------------------------------------
extern WORD general_use_10ms_timer;
extern WORD general_use_100ms_timer;


#endif










