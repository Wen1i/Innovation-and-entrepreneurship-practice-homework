/********************************************************************
* FileName:		AccessPoint.c
* Dependencies: none   
* Processor:	PIC24 (tested)
* Complier:     Microchip C30 v3.23 or higher (tested)
*
* Processor:	PIC18, PIC32, dsPIC30, dsPIC33 (untested)
* Complier:     Microchip C18 v3.04 or higher (untested)
*               Microchip C32 v1.02 or higher	
*
* Company:		Microchip Technology, Inc.
*
* Copyright and Disclaimer Notice for P2P Software:
*
* Copyright © 2007-2010 Microchip Technology Inc.  All rights reserved.
*
* Microchip licenses to you the right to use, modify, copy and distribute 
* Software only when embedded on a Microchip microcontroller or digital 
* signal controller and used with a Microchip radio frequency transceiver, 
* which are integrated into your product or third party product (pursuant 
* to the terms in the accompanying license agreement).  
*
* You should refer to the license agreement accompanying this Software for 
* additional information regarding your rights and obligations.
*
* SOFTWARE AND DOCUMENTATION ARE PROVIDED “AS IS” WITHOUT WARRANTY OF ANY 
* KIND, EITHER EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION, ANY 
* WARRANTY OF MERCHANTABILITY, TITLE, NON-INFRINGEMENT AND FITNESS FOR A 
* PARTICULAR PURPOSE. IN NO EVENT SHALL MICROCHIP OR ITS LICENSORS BE 
* LIABLE OR OBLIGATED UNDER CONTRACT, NEGLIGENCE, STRICT LIABILITY, 
* CONTRIBUTION, BREACH OF WARRANTY, OR OTHER LEGAL EQUITABLE THEORY ANY 
* DIRECT OR INDIRECT DAMAGES OR EXPENSES INCLUDING BUT NOT LIMITED TO 
* ANY INCIDENTAL, SPECIAL, INDIRECT, PUNITIVE OR CONSEQUENTIAL DAMAGES, 
* LOST PROFITS OR LOST DATA, COST OF PROCUREMENT OF SUBSTITUTE GOODS, 
* TECHNOLOGY, SERVICES, OR ANY CLAIMS BY THIRD PARTIES (INCLUDING BUT 
* NOT LIMITED TO ANY DEFENSE THEREOF), OR OTHER SIMILAR COSTS.
*
*********************************************************************
* File Description:
*
* This program is based off of Microchip's MiWi Development Environment
* and demonstrates the capabilities of Powercast's P2110 Energy Harvester
* product.  This program is for receiving a MiWi packet from the End
* Point node and displaying the sensor information received on a 
* computer terminal screen.
*   
* Change History (Microchip Code):
*  Rev   Date         Author    Description
*  0.1   1/03/2008    yfy       Initial revision
*  2.0   4/15/2009    yfy       MiMAC and MiApp revision
*  3.1   5/28/2010    yfy       MiWi DE 3.1
*
* Change History (Powercast Code):
*  Ver   Date         Author    Description
*  1.0   10/14/2010   DWH       Initial revision
*  1.1   04/29/2011   DWH       Updated version for endpoint code revision
********************************************************************/

/************************ HEADERS **********************************/
#include "Console.h"
#include "ConfigApp.h"
#include "HardwareProfile.h"
#include "WirelessProtocols\MCHP_API.h"
#include <math.h>


/************************ VARIABLES ********************************/
unsigned int version = 11;		// current code version (10 = 1.0, 11 = 1.1,...)

unsigned long Packet = 0;		// packet counter

// Variables to get the packet data from node
int boardMode = 0;				// Sets mode of current receiver
unsigned char nodeID = 0;
unsigned char TXID = 0;
unsigned char thermValH = 0;	// sensor data from node
unsigned char thermValL = 0;
unsigned char vddValH = 0;
unsigned char vddValL = 0;
unsigned char lightValH = 0;
unsigned char lightValL = 0;
unsigned char humValH = 0;
unsigned char humValL = 0;
unsigned char extValH = 0;
unsigned char extValL = 0;
unsigned char rssiValH = 0; 
unsigned char rssiValL = 0;

// Variables used to convert packet data to actual data and print
float tem;					// used for converting temp data
float res;					// used for converting temp data
float curr;					// used for converting light data
unsigned int temp_int;		// Scratchpad integer

// ADC and sensor constants
#define VBG_VAL         1228800UL   // VBG = 1.2V, VBG_VAL = 1200 mV * 1024 counts
#define T_BIAS			10000.0		// Bias res for temp sensor
#define L_BIAS			1620.0		// Bias res for light sensor
#define B_CONST			3380.0		// B Constant of thermistor
#define R_0				10000.0		// Thermistor resistance at 25C
#define T_0				298.15		// Temp in kelvin at 25C

// Variables used for getting total time and time between packets
long Total_sec_cnt = 0;
unsigned int Sec_between_packet[8] = {0,0,0,0,0,0,0,0};
unsigned int Hundred_millisecond_cnt = 0;

// Variables used for printing to the screen             
unsigned int thermVal = 0;		// thermister value from node
unsigned int tempVal = 0;       // temperature in F
unsigned int vddVal = 0;        // board VDD level
unsigned int lightVal = 0;      // light sensor data
unsigned int humVal = 0;        // humidity sensor data
unsigned int extVal = 0;        // external sensor data
unsigned int rssiVal = 0;       // RSSI value

unsigned int total_time_sec = 0;	// total time display of seconds
unsigned int total_time_min = 0;	// total time display of minutes 
unsigned int total_time_hr = 0;		// total time display of hours
unsigned int delta_time_sec = 0;	// time between packets display of seconds
unsigned int delta_time_min = 0; 	// time between packets display of minutes

BYTE PressedButton;				// checks for button presses

/************************ FUNCTIONS ********************************/
void PrintScreen(void);				// Prints data to console
void GetRSSI(void);					// Get received power level from ADC value
void _ISR _DefaultInterrupt(void);	// interrupt for Timer1, used for keeping time


/*******************************************************************/
// AdditionalConnectionPayload variable array defines the additional 
// information to identify a device on a P2P connection. This array
// will be transmitted with the P2P_CONNECTION_REQUEST command to 
// initiate the connection between the two devices. Along with the 
// long address of this device, this  variable array will be stored 
// in the P2P Connection Entry structure of the partner device. The 
// size of this array is ADDITIONAL_CONNECTION_PAYLOAD, defined in 
// P2PDefs.h.
// In this demo, this variable array is set to be empty.
/******************************************************************/
#if ADDITIONAL_CONNECTION_PAYLOAD > 0
    BYTE AdditionalConnectionPayload[ADDITIONAL_CONNECTION_PAYLOAD];
#endif
/*******************************************************************/
// AdditionalNodeID variable array defines the additional 
// information to identify a device on a PAN. This array
// will be transmitted when initiate the connection between 
// the two devices. This  variable array will be stored in 
// the Connection Entry structure of the partner device. The 
// size of this array is ADDITIONAL_NODE_ID_SIZE, defined in 
// ConfigApp.h.
// In this demo, this variable array is set to be empty.
/******************************************************************/
#define LIGHT   0x01
#define SWITCH  0x02

#if ADDITIONAL_NODE_ID_SIZE > 0
    BYTE AdditionalNodeID[ADDITIONAL_NODE_ID_SIZE] = {LIGHT};
#endif

/*******************************************************************/
// The variable myChannel defines the channel that the P2P connection
// is operate on. This variable will be only effective if energy scan
// (ENABLE_ED_SCAN) is not turned on. Once the energy scan is turned
// on, the operating channel will be one of the channels available with
// least amount of energy (or noise).
/*******************************************************************/
BYTE myChannel = 25;

/*********************************************************************
* Function:         void main(void)
*
* PreCondition:     none
*
* Input:		    none
*
* Output:		    none
*
* Side Effects:	    none
*
* Overview:		    This is the main function.  It initializes the 
*					chip, then runs a while loop that waits for 
*					received packets.  Once a packet is received
*					the data is displayed on the console.
*
* Note:			    
**********************************************************************/
#if defined(__18CXX)
void main(void)
#else
int main(void)
#endif
{   
    /*******************************************************************/
    // Initialize the system
    /*******************************************************************/
    BoardInit();         
    ConsoleInit();  
    
    // Need to change some Console parameters for XLP16 board
    #if defined(XLP16)
    	U2MODEbits.RXINV = 1;
    	U2STAbits.UTXINV = 1;
    #endif
  
	// LEDs are ACTIVE LOW, turn them both OFF
    LED_1 = 1;
    LED_2 = 1;
 
	// Initial Startup Display
    ConsolePutROMString((ROM char*)"\r\nLifetime Power(R) Energy Harvesting Development Kit for Wireless Sensors");
    ConsolePutROMString((ROM char*)"\r\nPowercast Corporation");
	ConsolePutROMString((ROM char*)"\r\nVersion ");
	ConsolePut((version / 10) % 10 + '0');
	ConsolePut('.');
	ConsolePut(version % 10 + '0');
	ConsolePutROMString((ROM char*)"\r\n");

    //ConsolePutROMString((ROM char*)"Test in first");
     /*******************************************************************/
    // Initialize Microchip proprietary protocol. Which protocol to use
    // depends on the configuration in ConfigApp.h
    /*******************************************************************/
    /*******************************************************************/
    // Function MiApp_ProtocolInit initialize the protocol stack. The
    // only input parameter indicates if previous network configuration
    // should be restored. In this simple example, we assume that the 
    // network starts from scratch.
    /*******************************************************************/
    MiApp_ProtocolInit(FALSE);   

    // Set default channel
    if ( MiApp_SetChannel(myChannel) == FALSE )
    {
        Printf("\r\nSelection of channel ");
        PrintDec(myChannel);
        Printf(" is not supported in current condition.\r\n");
        #if defined(__18CXX)
            return;
        #else
            return 0;
        #endif
    }
    
    /*******************************************************************/
    // Function MiApp_ConnectionMode defines the connection mode. The
    // possible connection modes are:
    //  ENABLE_ALL_CONN:    Enable all kinds of connection
    //  ENABLE_PREV_CONN:   Only allow connection already exists in 
    //                      connection table
    //  ENABL_ACTIVE_SCAN_RSP:  Allow response to Active scan
    //  DISABLE_ALL_CONN:   Disable all connections. 
    /*******************************************************************/
    MiApp_ConnectionMode(ENABLE_ALL_CONN);
    
     
    /*******************************************************************/
    // Function MiApp_EstablishConnection try to establish a new 
    // connection with peer device. 
    // The first parameter is the index to the active scan result, which 
    //      is acquired by discovery process (active scan). If the value 
    //      of the index is 0xFF, try to establish a connection with any 
    //      peer.
    // The second parameter is the mode to establish connection, either 
    //      direct or indirect. Direct mode means connection within the 
    //      radio range; Indirect mode means connection may or may not 
    //      in the radio range. 
    /*******************************************************************/
//    while( (i = MiApp_EstablishConnection(0xFF, CONN_MODE_DIRECT)) == 0xFF );
    
    /*******************************************************************/
    // Function DumpConnection is used to print out the content of the
    // Connection Entry on the hyperterminal. It may be useful in 
    // the debugging phase.
    // The only parameter of this function is the index of the  
    // Connection Entry. The value of 0xFF means to print out all
    // valid Connection Entry; otherwise, the Connection Entry
    // of the input index will be printed out.
    /*******************************************************************/
    #ifdef ENABLE_DUMP
//        DumpConnection(0xFF);
    #endif

    // Turn on LED 1
    LED_2 = 0;

	// Set up Timer1
  	T1CON = 0x0000; 		// stops the timer1 and reset control flag
  	TMR1 = 0x3CAF;       	// 0xFFFF - 0xC350 = 50,000 pulses = 100 msec count at Fosc=8MHz with Timer prescalar of 1:8
  	IPC0bits.T1IP =0x3; 	// setup Timer1 interrupt for desired priority level
	IEC0bits.T1IE = 1; 		// enable Timer1 interrupts	
	T1CON = 0x8010;		 	// enable timer1 with prescalar of 1:8 

    while(1)
    {
        /*******************************************************************/
        // Function MiApp_MessageAvailable returns a boolean to indicate if 
        //  a packet has been received by the transceiver. If a packet has
        //  been received, all information will be stored in the rxFrame, 
        //  structure of RECEIVED_MESSAGE.
        /*******************************************************************/
        if ( 1 )
        {
            //ConsolePutROMString((ROM char*)"Test in MiApp_Messss");
            BYTE SM4KEY[] = { 0x01,0x23,0x45,0x67,0x89,0xab,0xcd,0xef,0xfe,0xdc,0xba,0x98,0x76,0x54,0x32,0x10 };
            SM4Dec(rxMessage.Payload,16,SM4KEY);
            
            
            
	        // Increment Packet counter
        	Packet++;
        	if (Packet == 100000)		// only displaying 5 digits, roll over
        		Packet = 0;

	        // Mode of the board that sent the packet
	        boardMode = rxMessage.Payload[0];
	        
	        // ID of the node that sent the packet
           	nodeID = rxMessage.Payload[1];

			// Calculate total time
       		total_time_hr = Total_sec_cnt / 3600;
   			total_time_min = (Total_sec_cnt % 3600) / 60;
       		total_time_sec = Total_sec_cnt % 60;
        
       		// Calulate time since last packet
//			delta_time_min = Sec_between_packet[nodeID] / 60;
//       		delta_time_sec = Sec_between_packet[nodeID] % 60;

	        if (boardMode == 0)
	        {
		        // ************* NORMAL MODE ******************
	        
            	// Get TXID
//            	TXID = rxMessage.Payload[2];

				// Get VDD, used to convert ADC readings to mV
            	vddValH = rxMessage.Payload[2];
            	vddValL = rxMessage.Payload[3];
            	vddVal = ((unsigned int)vddValH << 8) + vddValL;
            	vddVal = VBG_VAL / vddVal; 						// calculate VDD from VBG


				// Convert RSSI ADC reading to mV
//            	rssiValH = rxMessage.Payload[5];
//            	rssiValL = rxMessage.Payload[6];
//            	rssiVal = ((unsigned int)rssiValH << 8) + rssiValL;
//            	rssiVal = (WORD)((vddVal * (long)rssiVal) / 1024);
//            	
//            	GetRSSI();
            
            	
            	// Convert THERMISTOR ADC reading to mV
            	thermValH = rxMessage.Payload[4];
            	thermValL = rxMessage.Payload[5];
            	thermVal = ((unsigned int)thermValH << 8) + thermValL;
            	thermVal = (WORD)((vddVal * (long)thermVal) / 1024);

				// Convert the mV value from thermistor to temperature in C           
            	res = T_BIAS * ((float)thermVal / ((float)vddVal - (float)thermVal));
 				res = logf(res / R_0);
 				res = (1 / B_CONST) * res;
 				tem = 1 / ((1 / T_0) + res) - 273;
 				tempVal = ((tem * 9 / 5) + 32) * 10;	// convert degrees C to F, *10 for console 
			
			
				// Convert HUMIDITY ADC reading to mV
            	humValH = rxMessage.Payload[6];
            	humValL = rxMessage.Payload[7];
            	humVal = ((unsigned int) humValH << 8) + humValL;
            	humVal = (WORD)((vddVal * (long)humVal) / 1024);

				// Convert the mV value from humidity sensor to relative humidity
				// Equation from humidity sensor data sheet, *100 for console
				humVal = (((((long)humVal * 100000) / vddVal) - 15150) / 636);
			
			
				// Convert LIGHT ADC reading to mV
            	lightValH = rxMessage.Payload[8];
            	lightValL = rxMessage.Payload[9];
            	lightVal = ((unsigned int) lightValH << 8) + lightValL;  
				lightVal = (WORD)((vddVal * (long)lightVal) / 1024);
			
				// Convert the mV to illuminance
				if (lightVal > 10)
				{
            		res = vddVal - lightVal;					// get voltage across bias resistor
            		curr = res / (L_BIAS / 1000);				// get current through resistor
            		lightVal = 100 * curr / 50;					// convert current to illuminance Ev (lux)
    			} 
            	ConsolePutROMString((ROM char*)"Test in if end");
            	// Convert EXTERNAL ADC reading to mV
//            	extValH = rxMessage.Payload[13];
//            	extValL = rxMessage.Payload[14];
//            	extVal = ((unsigned int) extValH << 8) + extValL;
//            	extVal = (WORD)((vddVal * (long)extVal) / 1024);
			}
			else
			{
				// ************** USER MODE *********************
				
				// Add user created code here
				// Make sure to change USER section of PrintScreen() below
			}	
            ConsolePutROMString((ROM char*)"Test in PrintScreenbefore");
       		// Print data to terminal window, based on boardMode
           	PrintScreen();
            	
                // Reset time between packets
           	Sec_between_packet[nodeID] = 0;
            
       		// Flash LED2 to indicate receiving a packet. LED pin is inverted
           	LED_1 = 0;
           	for(temp_int = 0; temp_int < 800; temp_int++);
   			LED_1 = 1;

            /*******************************************************************/
            // Function MiApp_DiscardMessage is used to release the current 
            //  received packet.
            // After calling this function, the stack can start to process the
            //  next received frame 
            /*******************************************************************/  
            MiApp_DiscardMessage();
            
        }
        else
        {
           PressedButton = ButtonPressed();
            
            // Can only use S2 on board, because S3 is tied to INT pin of radio, and is always high
            if ( PressedButton == 1 )  
            {	            
				// Calculate total time
    	   		total_time_hr = Total_sec_cnt / 3600;
   				total_time_min = (Total_sec_cnt % 3600) / 60;
       			total_time_sec = Total_sec_cnt % 60;
        
	       		// Calulate time since last packet
//				delta_time_min = Sec_between_packet[nodeID] / 60;
//       			delta_time_sec = Sec_between_packet[nodeID] % 60;
            
        		// Time Display
	    		ConsolePutROMString((ROM char*)"\r\nTime ");
        
       			ConsolePut((total_time_hr / 10) % 10 + '0');
       			ConsolePut(total_time_hr % 10 + '0');
    	
	    		ConsolePutROMString((ROM char*)":");

       			ConsolePut((total_time_min / 10) % 10 + '0');
       			ConsolePut(total_time_min % 10 + '0');
    	
	    		ConsolePutROMString((ROM char*)":");

       			ConsolePut((total_time_sec / 10) % 10 + '0');
       			ConsolePut(total_time_sec % 10 + '0');
    	
        		ConsolePutROMString((ROM char*)" | ");
    	
    			// Seconds between packets
//	    		ConsolePutROMString((ROM char*)"dT ");
//            
//       			ConsolePut((delta_time_min / 10) % 10 + '0');
//       			ConsolePut(delta_time_min % 10 + '0');
//    	
//	    		ConsolePutROMString((ROM char*)":");
//
//       			ConsolePut((delta_time_sec / 10) % 10 + '0');
//       			ConsolePut(delta_time_sec % 10 + '0');
//    	
//        		ConsolePutROMString((ROM char*)" | Time since last packet\r\n");
			}
        }
    }
}


/*********************************************************************
* Function:         void T1Interrupt(void)
*
* PreCondition:     none
*
* Input:		    none
*
* Output:		    none
*
* Side Effects:	    none
*
* Overview:		    Interrupt function for Timer1.  Set up to time out
*					after 100ms, updates total seconds and seconds 
*					between packets counters.
*
* Note:			    
**********************************************************************/
void _ISRFAST __attribute__((interrupt, auto_psv)) _T1Interrupt(void) 
{
	int i = 0;
    Hundred_millisecond_cnt++;
   	IFS0bits.T1IF = 0;						// Clear Timer 1 interrupt flag
    TMR1 = 0x3CAF;       					// 0xFFFF - 0xC350 = 50,000 pulses = 100 msec count at Fosc=8MHz with Timer prescalar of 1:8
	if (Hundred_millisecond_cnt == 10)  	// 1 sec count
	{
		Hundred_millisecond_cnt = 0; 
		Total_sec_cnt++;
			  
		for(i = 0; i < 8; i++)
			Sec_between_packet[i]++;
	}
    	
	return;
}


/*********************************************************************
* Function:         void PrintScreen(void)
*
* PreCondition:     none
*
* Input:		    none
*
* Output:		    none
*
* Side Effects:	    none
*
* Overview:		    Prints received data to console, 
*					Dependent on sensor board mode
*
* Note:			    
**********************************************************************/
void PrintScreen(void)
{
	ConsolePutROMString((ROM char*)"\r\n");
    ConsolePutROMString((ROM char*)"1Test in PrintScreen");
   	// Check Mode of sensor board
   	if (boardMode == 0)
	{
		// ********* NORMAL MODE **************
		
		// Display all packet info on 2 lines
		
   		// Packet Count
    	ConsolePutROMString((ROM char*)"Packet #");
            
    	if ((Packet / 10000) != 0) 
		{
    		ConsolePut((Packet / 10000) % 10 + '0');
			ConsolePut((Packet / 1000) % 10 + '0');
			ConsolePut((Packet / 100) % 10 + '0');
    		ConsolePut((Packet / 10) % 10 + '0');
    	}
    	else 
   		{ 
       		ConsolePut(' ');
		    	    
       		if ((Packet / 1000) != 0) 
	   		{
    			ConsolePut((Packet / 1000) % 10 + '0');
   	   			ConsolePut((Packet / 100) % 10 + '0');
   	   			ConsolePut((Packet / 10) % 10 + '0');
    		}
       		else
       		{
     			ConsolePut(' ');
		    			
        		if ((Packet / 100) != 0) 
		   		{
    				ConsolePut((Packet / 100) % 10 + '0');
	   	   			ConsolePut((Packet / 10) % 10 + '0');
	   			}
       			else
       			{
	    			ConsolePut(' ');

	   	   			if ((Packet / 10) != 0)
	   	   				ConsolePut((Packet / 10) % 10 + '0');
	   	   			else
		   				ConsolePut(' ');
		   		}
			}
		}
   		ConsolePut(Packet % 10 + '0');
   		
   		ConsolePutROMString((ROM char*)" | ");


		// Node ID
		ConsolePutROMString((ROM char*)"Node   ");

    	ConsolePut(nodeID % 10 + '0');

   		ConsolePutROMString((ROM char*)" | ");
   	
   			
	   	// TX ID
//	    ConsolePutROMString((ROM char*)"TX ID  ");	
//	    
//	    if (TXID == 0xFF || TXID == 0x00)	// check for valid data
//	    	ConsolePutROMString((ROM char*)"---");
//        
//        else 
//        {
//			ConsolePut((TXID / 100) % 10 + '0');
//    		ConsolePut((TXID / 10) % 10 + '0');
//			ConsolePut(TXID % 10 + '0');
//	    }
  		ConsolePutROMString((ROM char*)"   | ");


        // Temperature sensor
        ConsolePutROMString((ROM char*)"Temp  ");
        
        if (tempVal < 250 || tempVal > 1250)			// check for valid data, 25 < T < 125
	    	ConsolePutROMString((ROM char*)"-----");

		else
		{
        	if ((tempVal / 1000) != 0) 
        	{
       	    	ConsolePut((tempVal / 1000) % 10 + '0');
   	        	ConsolePut((tempVal / 100) % 10 + '0');
           		ConsolePut((tempVal / 10) % 10 + '0');
        	}
        	else 
        	{ 
          		ConsolePut(' ');
                                 
	        	if ((tempVal / 100) != 0) 
		    	{
           			ConsolePut((tempVal / 100) % 10 + '0');
					ConsolePut((tempVal / 10) % 10 + '0');
            	}
   	        	else 
    	    	{ 
            		ConsolePut(' ');
	    	    
	        		if ((tempVal / 10) != 0) 
           				ConsolePut((tempVal / 10) % 10 + '0');
       				else
	     				ConsolePut(' ');
				}
			}
			ConsolePut('.');
   			ConsolePut(tempVal % 10 + '0');
		}
        ConsolePutROMString((ROM char*)" F | ");


       	// Light Sensor
        ConsolePutROMString((ROM char*)"Light ");
        
        if (lightVal < 10)								// check for valid data
	    	ConsolePutROMString((ROM char*)"-----");
	    	
	    else
	    {
    	    if ((lightVal / 10000) != 0) 
	    	{
       			ConsolePut((lightVal / 10000) % 10 + '0');
       			ConsolePut((lightVal / 1000) % 10 + '0');
       			ConsolePut((lightVal / 100) % 10 + '0');
       			ConsolePut((lightVal / 10) % 10 + '0');
        	}
	        else 
   		    { 
        	 	ConsolePut(' ');
	    	    
	        	if ((lightVal / 1000) != 0) 
	        	{
	       			ConsolePut((lightVal / 1000) % 10 + '0');
    	   			ConsolePut((lightVal / 100) % 10 + '0');
       				ConsolePut((lightVal / 10) % 10 + '0');
				}
   				else
   				{
     				ConsolePut(' ');

	        		if ((lightVal / 100) != 0) 
	        		{
		       			ConsolePut((lightVal / 100) % 10 + '0');
       					ConsolePut((lightVal / 10) % 10 + '0');
					}
					else
					{
						ConsolePut(' ');

           				if ((lightVal / 10) != 0) 
           					ConsolePut((lightVal / 10) % 10 + '0');
       					else
	     					ConsolePut(' ');
					}
				}
			}
			ConsolePut(lightVal % 10 + '0');
 		}
        ConsolePutROMString((ROM char*)" lx");


		// NEW LINE
   		ConsolePutROMString((ROM char*)"\r\n");

		
        // Time Display
	    ConsolePutROMString((ROM char*)"Time ");
        
       	ConsolePut((total_time_hr / 10) % 10 + '0');
       	ConsolePut(total_time_hr % 10 + '0');
    	
	    ConsolePutROMString((ROM char*)":");

       	ConsolePut((total_time_min / 10) % 10 + '0');
       	ConsolePut(total_time_min % 10 + '0');
    	
	    ConsolePutROMString((ROM char*)":");

       	ConsolePut((total_time_sec / 10) % 10 + '0');
       	ConsolePut(total_time_sec % 10 + '0');
    	
        ConsolePutROMString((ROM char*)" | ");
    	

    	// Seconds between packets
//	    ConsolePutROMString((ROM char*)"dT ");
//            
//       	ConsolePut((delta_time_min / 10) % 10 + '0');
//       	ConsolePut(delta_time_min % 10 + '0');
//    	
//	    ConsolePutROMString((ROM char*)":");
//
//       	ConsolePut((delta_time_sec / 10) % 10 + '0');
//       	ConsolePut(delta_time_sec % 10 + '0');
//    	
//        ConsolePutROMString((ROM char*)" | ");


		// RSSI
//        ConsolePutROMString((ROM char*)"RSSI ");
//            
//        if (rssiVal < 4)								// check for valid data
//	    	ConsolePutROMString((ROM char*)" --- ");
//	    	
//	    else
//	    {
//       		if ((rssiVal / 1000) != 0) 
//   				ConsolePut((rssiVal / 1000) % 10 + '0');
//			else
//    			ConsolePut(' ');
//                       
//        	ConsolePut((rssiVal / 100) % 10 + '0');
//			ConsolePut('.');
//        	ConsolePut((rssiVal / 10) % 10 + '0');
//        	ConsolePut(rssiVal % 10 + '0');
//     	}   	
//        ConsolePutROMString((ROM char*)"mW | ");


        // Humidity Sensor
        ConsolePutROMString((ROM char*)"Humidity ");

        if (humVal < 15 || humVal > 85)				// check for valid data
	    	ConsolePutROMString((ROM char*)"--");

		else
		{
  			ConsolePut((humVal / 10) % 10 + '0');
			ConsolePut(humVal % 10 + '0');
  		}      	
        ConsolePutROMString((ROM char*)" % | ");

 
        // External Sensor
//        ConsolePutROMString((ROM char*)"Extrnl ");
//
//        if ((extVal / 1000) != 0) 
//        {
//           	ConsolePut((extVal / 1000) % 10 + '0');
//           	ConsolePut((extVal / 100) % 10 + '0');
//           	ConsolePut((extVal / 10) % 10 + '0');
//        }
//        else 
//        { 
//          	ConsolePut(' ');
//                                 
//	        if ((extVal / 100) != 0) 
//		    {
//           		ConsolePut((extVal / 100) % 10 + '0');
//           		ConsolePut((extVal / 10) % 10 + '0');
//            }
//   	        else 
//    	    { 
//            	ConsolePut(' ');
//	    	    
//	        	if ((extVal / 10) != 0) 
//           			ConsolePut((extVal / 10) % 10 + '0');
//       			else
//	     			ConsolePut(' ');
//			}
//		}
//        ConsolePut(extVal % 10 + '0');
//            
//        ConsolePutROMString((ROM char*)" mV");
    }
    else
    {
	    // ************ USER MODE ******************
		
		// Display user created content here
		// This section shows the Packet count, Node ID and Time info
		
   		// Packet Count
//    	ConsolePutROMString((ROM char*)"Packet #");
//            
//    	if ((Packet / 10000) != 0) 
//		{
//    		ConsolePut((Packet / 10000) % 10 + '0');
//			ConsolePut((Packet / 1000) % 10 + '0');
//			ConsolePut((Packet / 100) % 10 + '0');
//    		ConsolePut((Packet / 10) % 10 + '0');
//    	}
//    	else 
//   		{ 
//       		ConsolePut(' ');
//		    	    
//       		if ((Packet / 1000) != 0) 
//	   		{
//    			ConsolePut((Packet / 1000) % 10 + '0');
//   	   			ConsolePut((Packet / 100) % 10 + '0');
//   	   			ConsolePut((Packet / 10) % 10 + '0');
//    		}
//       		else
//       		{
//     			ConsolePut(' ');
//		    			
//        		if ((Packet / 100) != 0) 
//		   		{
//    				ConsolePut((Packet / 100) % 10 + '0');
//	   	   			ConsolePut((Packet / 10) % 10 + '0');
//	   			}
//       			else
//       			{
//	    			ConsolePut(' ');
//
//	   	   			if ((Packet / 10) != 0)
//	   	   				ConsolePut((Packet / 10) % 10 + '0');
//	   	   			else
//		   				ConsolePut(' ');
//		   		}
//			}
//		}
//   		ConsolePut(Packet % 10 + '0');
//   		
//   		ConsolePutROMString((ROM char*)" | ");
//
//
//		// Node ID
//		ConsolePutROMString((ROM char*)"Node   ");
//
//    	ConsolePut(nodeID % 10 + '0');
//
//   		ConsolePutROMString((ROM char*)" | ");
//
//
//		// Nothing else implemented in the Mode
//		ConsolePutROMString((ROM char*)"This mode not implemented\r\n");
//		
//        // Time Display
//	    ConsolePutROMString((ROM char*)"Time ");
//        
//       	ConsolePut((total_time_hr / 10) % 10 + '0');
//       	ConsolePut(total_time_hr % 10 + '0');
//    	
//	    ConsolePutROMString((ROM char*)":");
//
//       	ConsolePut((total_time_min / 10) % 10 + '0');
//       	ConsolePut(total_time_min % 10 + '0');
//    	
//	    ConsolePutROMString((ROM char*)":");
//
//       	ConsolePut((total_time_sec / 10) % 10 + '0');
//       	ConsolePut(total_time_sec % 10 + '0');
//    	
//        ConsolePutROMString((ROM char*)" | ");
//    	
//
//    	// Seconds between packets
//	    ConsolePutROMString((ROM char*)"dT ");
//            
//       	ConsolePut((delta_time_min / 10) % 10 + '0');
//       	ConsolePut(delta_time_min % 10 + '0');
//    	
//	    ConsolePutROMString((ROM char*)":");
//
//       	ConsolePut((delta_time_sec / 10) % 10 + '0');
//       	ConsolePut(delta_time_sec % 10 + '0');
//    	
//        ConsolePutROMString((ROM char*)" | ");
    }    
   	ConsolePutROMString((ROM char*)"\r\n");
}	

/*********************************************************************
* Function:         void GetRSSI(void)
*
* PreCondition:     none
*
* Input:		    none
*
* Output:		    none
*
* Side Effects:	    none
*
* Overview:		    Uses lookup tables to get proper RSSI value
*					from node
*
* Note:			    
**********************************************************************/
/*void GetRSSI(void)
{

float rssi_lookup1[] = {		// this table contains RSSI data for low voltages (<40)
0.01	,
0.01	,
0.01	,
0.02	,
0.02	,
0.03	,
0.03	,
0.04	,
0.04	,
0.05	,
0.05	,
0.06	,
0.06	,
0.07	,
0.07	,
0.08	,
0.08	,
0.09	,
0.09	,
0.10	,
0.10	,
0.11	,
0.11	,
0.12	,
0.12	,
0.13	,
0.13	,
0.14	,
0.14	,
0.15	,
0.15	,
0.16	,
0.16	,
0.17	,
0.17	,
0.18	,
0.18	,
0.19	,
0.19	,
0.20	,
};

float rssi_lookup2[] = {		// this table contains RSSI data for med-low voltages (39<x<60)
0.20	,
0.21	,
0.22	,
0.23	,
0.24	,
0.25	,
0.26	,
0.27	,
0.29	,
0.30	,
};

float rssi_lookup3[] = {		// this table contains RSSI data for med voltages (59<x<200)
0.31	,
0.34	,
0.37	,
0.40	,
0.43	,
0.46	,
0.49	,
0.53	,
0.56	,
0.60	,
0.63	,
0.67	,
0.70	,
0.74	,
0.78	,
0.82	,
0.86	,
0.91	,
0.95	,
1.01	,
1.04	,
1.08	,
1.13	,
1.18	,
1.22	,
1.27	,
1.32	,
1.37	,
};

float rssi_lookup4[] = {		// this table contains RSSI data for med-high voltages (199<x<1000)
1.42	,
1.53	,
1.63	,
1.74	,
1.85	,
1.97	,
2.09	,
2.22	,
2.34	,
2.48	,
2.61	,
2.74	,
2.88	,
3.03	,
3.18	,
3.32	,
3.47	,
3.66	,
3.79	,
3.96	,
4.14	,
4.31	,
4.47	,
4.66	,
4.84	,
5.01	,
5.21	,
5.40	,
5.60	,
5.78	,
5.98	,
6.17	,
6.40	,
6.62	,
6.82	,
7.05	,
7.28	,
7.48	,
7.73	,
7.94	,
8.18	,
8.41	,
8.67	,
8.89	,
9.16	,
9.40	,
9.68	,
9.91	,
10.19	,
10.45	,
10.72	,
11.02	,
11.27	,
11.53	,
11.86	,
12.11	,
12.42	,
12.71	,
12.97	,
13.30	,
13.58	,
14.00	,
14.19	,
14.55	,
14.83	,
15.17	,
15.42	,
15.78	,
16.14	,
16.48	,
16.79	,
17.14	,
17.46	,
17.82	,
18.16	,
18.45	,
18.79	,
19.23	,
19.59	,
19.86	,
};

float rssi_lookup5[] = {		// this table contains RSSI data for high voltages (999<x<1640)
20.28	,
20.99	,
21.83	,
22.49	,
23.33	,
24.10	,
25.00	,
25.82	,
26.61	,
27.42	,
28.31	,
29.17	,
30.13	,
31.12	,
31.92	,
32.73	,
33.88	,
34.83	,
35.89	,
36.90	,
37.93	,
38.90	,
39.90	,
41.02	,
41.98	,
43.05	,
44.16	,
45.39	,
46.45	,
47.64	,
48.64	,
49.66	,
50.82	,
};

	if (rssiVal < 40)
		rssiVal = rssi_lookup1[rssiVal] * 100;
		
	else if (rssiVal >= 40 && rssiVal < 60)
		rssiVal = rssi_lookup2[(rssiVal - 40) / 2] * 100;
		
	else if (rssiVal >= 60 && rssiVal < 200)
		rssiVal = rssi_lookup3[(rssiVal - 60) / 5] * 100;

	else if (rssiVal >= 200 && rssiVal < 1000)
		rssiVal = rssi_lookup4[(rssiVal - 200) / 10] * 100;

	else if (rssiVal >= 1000 && rssiVal < 1640)
		rssiVal = rssi_lookup5[(rssiVal - 1000) / 20] * 100;
		
	else
		rssiVal = 0;
}*/
