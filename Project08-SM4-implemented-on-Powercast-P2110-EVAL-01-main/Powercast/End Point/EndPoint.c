/****************************************************************************
* FileName:		End_Point.c
* Dependencies: none   
* Processor:	PIC24
* Complier:     Microchip C30 v2.03 or higher	
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
****************************************************************************
* File Description:
*
* This is a simple example to demonstrate the capabilities of Powercast's
* technology, using the P2110 Energy Harvester to charge up a super
* capacitor and then turn on this code that reads sensors and sends back
* it's data to an Access Point.  There are 2 modes of operation, a normal 
* mode when the MODE jumper is connected, and a user mode when the MODE
* jumper otherwise.  
*
* Change History (Microchip Code):
*  Rev   Date         Author    Description
*  0.1   1/03/2008    yfy       Initial revision
*  2.0   4/15/2009    yfy       MiMAC and MiApp revision
*  3.1   5/28/2010    yfy       MiWi DE 3.1
*
* Change History (Powercast Code):
*  Ver   Date         Author    Description
*  1.0   10/05/2010   DWH       Initial version
*  1.1 	 04/29/2011   DWH		Added timeout counter for TXID
*								Fixed AN0 issue, disables SPI bus at startup
*									enables SPI before TX
*								Fixed humidity sensor reading, waits longer
*									to get accurate reading over 50%
**************************************************************************/

//************************ HEADERS ****************************************
#include "Console.h"
#include "ConfigApp.h"
#include "HardwareProfile.h"
#include "WirelessProtocols\MCHP_API.h"
#include "WirelessProtocols\SymbolTime.h"

//************************** FUNCTIONS ************************************
void GetBoardID(void);
int ADC_read(int);

//************************** VARIABLES ************************************
unsigned char boardID;
unsigned int TXID = 0xFFFF;		// TX ID set to error initially
unsigned int TXID_Check;		// Used to check for valid TX ID
unsigned int counter = 0;		// TX ID counter time out
int firstflag = 0;				// clears first UART byte
int TXIDdone = 0;				// flag set when TXID is received

// set from ADC
unsigned int vddVal;		
unsigned int tempVal;
unsigned int rssiVal;
unsigned int lightVal;
unsigned int humVal;
unsigned int extVal;

// need to break ADC data into 2 bytes to send
unsigned char modeVal;
unsigned char tempValH;
unsigned char tempValL;
unsigned char vddValH;
unsigned char vddValL;
unsigned char rssiValH;
unsigned char rssiValL;
unsigned char lightValH;
unsigned char lightValL;
unsigned char humValH;
unsigned char humValL;
unsigned char extValH;
unsigned char extValL;

// Sensor ADC inputs
#define TEMP_CHANNEL	5
#define HUMID_CHANNEL	1
#define LIGHT_CHANNEL	3
#define RSSI_CHANNEL	0
#define EXT_CHANNEL		2
#define VBG_CHANNEL     15


//*************************************************************************
// AdditionalNodeID variable array defines the additional 
// information to identify a device on a PAN. This array
// will be transmitted when initiate the connection between 
// the two devices. This  variable array will be stored in 
// the Connection Entry structure of the partner device. The 
// size of this array is ADDITIONAL_NODE_ID_SIZE, defined in 
// ConfigApp.h.
// In this demo, this variable array is set to be empty.
//*************************************************************************
#define LIGHT   0x01
#define SWITCH  0x02

#if ADDITIONAL_NODE_ID_SIZE > 0
    BYTE AdditionalNodeID[ADDITIONAL_NODE_ID_SIZE] = {LIGHT};
#endif

//*************************************************************************
// The variable myChannel defines the channel that the device
// is operate on. This variable will be only effective if energy scan
// (ENABLE_ED_SCAN) is not turned on. Once the energy scan is turned
// on, the operating channel will be one of the channels available with
// least amount of energy (or noise).
//*************************************************************************
BYTE myChannel = 25;

/*********************************************************************
* Function:         int main(void)
*
* PreCondition:     none
*
* Input:		    none
*
* Output:		    none
*
* Side Effects:	    none
*
* Overview:		    In this main function, the board is initialized
*					and the ID is read, then depending on the MODE
*					jumper, normal or user code is run.
* Note:			    
**********************************************************************/

int main(void)
{   
    int i;
	    
    // Initialize the system
    BoardInit();      
	GetBoardID();
    
    // Loop through code forever while power is applied,  10s delay after reset at end
    while(1)
    {
	    // Check Mode select pin 
    	if(MODE == 0)				// if mode jumper is on, do normal code, pin is inverted
	    {
			// ********* NORMAL CODE ******************
	    	modeVal = 0;
    
			// Turn on circuit to read TX ID, and enable reading of RSSI later
	   		PC_DSET = 1;
    
			// Enable UART1 to read TXID, enable interrupt
			IFS0bits.U1RXIF = 0;	// clear interrupt flag
			IEC0bits.U1RXIE = 1;	// enable interrupt
	    	U1MODEbits.UARTEN = 1;

			// Read sensors while waiting for TXID
	
		    // Measure Reference voltage, used for getting VDD
		    vddVal = ADC_read(VBG_CHANNEL);
	    	vddValH = vddVal >> 8;			// split result into 2 bytes to send
    		vddValL = vddVal;
    
	    	// Measure Temperature
	    	tempVal = ADC_read(TEMP_CHANNEL);
    		tempValH = tempVal >> 8;		// split result into 2 bytes to send
    		tempValL = tempVal;
    
	    	// Measure RSSI
    		counter = 0;
	    	rssiVal = ADC_read(RSSI_CHANNEL);
	        while (rssiVal < 5 && counter < 10)		// if read during TXID, sample again
    		{
	    		rssiVal = ADC_read(RSSI_CHANNEL);
        		counter++;
			} 
			rssiValH = rssiVal >> 8;		// split result into 2 bytes to send
    		rssiValL = rssiVal;
    		
	    	// Measure Light
	    	lightVal = ADC_read(LIGHT_CHANNEL);
    		lightValH = lightVal >> 8;		// split result into 2 bytes to send
    		lightValL = lightVal;
     		
	    	// Measure External Sensor
	    	extVal = ADC_read(EXT_CHANNEL);
    		extValH = extVal >> 8;		// split result into 2 bytes to send
    		extValL = extVal;
     
	    	// Measure Humidity
    		DelayMs(5);						// delay to let sensor warmup
    		humVal = ADC_read(HUMID_CHANNEL);
    		humValH = humVal >> 8;			// split result into 2 bytes to send
    		humValL = humVal;
    	
    		// Disable VDD to sensors
			SENS_PWR = 1;

		    // Wait for UART1 to see a valid TXID, or timeout if no TXID
		    // If timeout occurs, TXID is 0xFF
    		counter = 0;
			while(!TXIDdone && counter < 9)
	    	{
		    	DelayMs(1);
				counter++;
			}	
		
		    // Disable UART1 and interrupt
    		IEC0bits.U1RXIE = 0;
		    U1MODEbits.UARTEN = 0;
		
			// Turn off RSSI/TXID circuit
   			PC_DSET = 0;
   		}	
		else
		{
			// ********* USER CODE ******************
			modeVal = 1;
		
			// Do alternate code.  This is where to put user created code.
			// Make sure to add user created data to packet below
		}	

	   	// Send MIWI packet
		// Enable SPI bus to talk to radio
   		SPI1STAT = 0x8000;

	//*******************************************************************
    // Initialize Microchip proprietary protocol. Which protocol to use
    // depends on the configuration in ConfigApp.h
    //*******************************************************************
    //*******************************************************************
    // Function MiApp_ProtocolInit initialize the protocol stack. The
    // only input parameter indicates if previous network configuration
    // should be restored. In this simple example, we assume that the 
    // network starts from scratch.
    //*******************************************************************
		MiApp_ProtocolInit(FALSE);   
	
    	MiApp_SetChannel(myChannel);      // Set default channel
    
	    MiApp_FlushTx();
	    MiApp_WriteData(modeVal);
		MiApp_WriteData(boardID);

		// Check Mode select pin 
	    if(MODE == 0)					// if mode jumper is off send rest of data
    	{
//    		MiApp_WriteData(TXID);
	    	MiApp_WriteData(vddValH);
    		MiApp_WriteData(vddValL);
//    		MiApp_WriteData(rssiValH);
//    		MiApp_WriteData(rssiValL);
    
	    	// Sensor Data
    		MiApp_WriteData(tempValH);
    		MiApp_WriteData(tempValL);
	    	MiApp_WriteData(humValH);
    		MiApp_WriteData(humValL);
    		MiApp_WriteData(lightValH);
	    	MiApp_WriteData(lightValL);
//    		MiApp_WriteData(extValH);
//    		MiApp_WriteData(extValL);
            
            BYTE SM4KEY[] = { 0x01,0x23,0x45,0x67,0x89,0xab,0xcd,0xef,0xfe,0xdc,0xba,0x98,0x76,0x54,0x32,0x10 };
            SM4Enc(TxBuffer, 10, SM4KEY); 
            TxData += 6; 
    	}
    	else
    	{
	    	// Add user created data to packet
		} 

		// Send Packet
    	MiApp_BroadcastPacket(FALSE);

		// Turn off radio
    	PHY_RESETn = 0;						
    
	    // Shut down power supply, this should hardware reset the circuit, which reinitializes again at startup
    	PC_Reset = 1;
    
    	// 10 sec delay, then repeat if constantly powered
    	DelayMs(10000);
    	
    	// Re-enable sensor VDD for next run through loop
    	SENS_PWR = 0;
    	
    }	// END WHILE

    return 0;
}


/*********************************************************************
* Function:         void GetBoardID(void)
*
* PreCondition:     none
*
* Input:		    none
*
* Output:		    none
*
* Side Effects:	    none
*
* Overview:		    Reads DIP switches and sets boardID accordingly
*
* Note:			    
********************************************************************/
void GetBoardID(void) 
{
	boardID = 0;
	
	// DIP switches are inverted, when switch is on, pin reads as 0;
	if (!ID1)	boardID = boardID | 0b001;
    if (!ID2)	boardID = boardID | 0b010;
    if (!ID3)	boardID = boardID | 0b100;
}


/*********************************************************************
* Function:         void U1RXInterrupt(void)
*
* PreCondition:     none
*
* Input:		    none
*
* Output:		    none
*
* Side Effects:	    none
*
* Overview:		    Interrupt routine for UART1 RX, getting TXID
*
* Note:			    
**********************************************************************/
void _ISRFAST __attribute__((interrupt, auto_psv)) _U1RXInterrupt(void) 
{
    IFS0bits.U1RXIF = 0;		// clear flag
	
	// UART1 contains invalid data for the first byte, clear it out
	if (firstflag == 0)
	{
		firstflag = 1;			// set flag so this is only done once

	    // Clear UART1 read buffer
   		U1RXREG;
    	U1RXREG;
    	U1RXREG;
    	U1RXREG;
 	}
 	else
 	{   	
	    TXIDdone = 1; 						// done with uart
    	IEC0bits.U1RXIE = 0;				// disable interrupt, only read TXID once
    	TXID = U1RXREG;						// read UART1 buffer
		TXID_Check = TXID & 0x0100;			// used to check for valid ID
	
		// If an error occured while reading the TXID, read as a read error
		if (U1STAbits.FERR == 1 || TXID_Check != 0)
			TXID = 0;
	}
}


/*********************************************************************
* Function:         int ADC_read(int)
*
* PreCondition:     none
*
* Input:		    Channel to read
*
* Output:		    ADC value
*
* Side Effects:	    none
*
* Overview:		    Does an ADC read for the channel.
*
* Note:			    
**********************************************************************/
int ADC_read(int channel)
{
	int i = 0;
	int tempvar = 0;
	
	AD1CHS = channel;				// set channel to measure 
   	AD1CON1bits.ADON = 1;			// turn ADC on for taking readings
    for (i = 0; i < 50; i++);
	AD1CON1bits.SAMP = 1;   		// start sampling
    while (!AD1CON1bits.DONE);		// wait for ADC to complete
    tempvar = ADC1BUF0;				// read ADC result
    AD1CON1bits.ADON = 0;			// turn ADC off for before taking next reading
	
	return tempvar;
}
