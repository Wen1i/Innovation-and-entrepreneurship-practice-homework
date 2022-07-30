/*****************************************************************************
*
*              HardwareProfile.c -- Hardware Profile
*
*****************************************************************************
* FileName:        HardwareProfile.c
* Dependencies:
* Processor:	PIC24
* Complier:     Microchip C30 v2.03 or higher	
* Linker:       MPLINK 03.40.00 or higher
* Company:      Microchip Technology Incorporated
*
* Software License Agreement
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
*****************************************************************************
* File Description:
*
*   This file provides configuration and basic hardware functionality 
*   based on chosen hardware demo boards.
*
* Change History:
*  Rev   Date         Description
*  0.1   2/17/2009    Initial revision
*  3.1   5/28/2010    MiWi DE 3.1
*
* Change History (Powercast Code):
*  Ver   Date         Author    Description
*  1.0   10/05/2010   DWH       Initial version
*  1.1   3/3/2011     DWH       Disable SPI bus in BoardInit()
******************************************************************************/
#include "SystemProfile.h"
#include "Compiler.h"
#include "Console.h"
#include "HardwareProfile.h"

                                                         
_FBS(BSS_OFF & BWRP_OFF)
_FGS(GCP_OFF & GWRP_OFF)
_FOSCSEL(FNOSC_FRC & IESO_ON)
_FOSC(FCKSM_CSDCMD & POSCFREQ_MS & OSCIOFNC_ON & POSCMOD_NONE )
_FWDT(FWDTEN_OFF)
_FPOR(MCLRE_ON & BOREN_BOR0 & PWRTEN_OFF)
_FICD(BKBUG_OFF & ICS_PGx3)
_FDS(DSWDTEN_OFF & DSBOREN_OFF) 



/*********************************************************************
 * Function:        void BoardInit( void )
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          None
 *
 * Side Effects:    Board is initialized for P2P usage
 *
 * Overview:        This function configures the board 
 *
 * Note:            Routine to setup the processor registers
 
 This routine needs to be called before the function 
 *                  to initialize P2P stack or any other function that
 *                  operates on the stack
 ********************************************************************/

void BoardInit(void)
{
	//Setup ADC for reading sensor inputs
	AD1CON1 = 0x00E0;   //Autoconvert, manual Sample
	AD1CON2 = 0x0000;   //int ref, 16-word buffer, mux A
    AD1CON3 = 0x1F00;	//SAMC = 31 TAD for all reads
    AD1PCFG = 0x1C10;	//AN4,10,11,12 set as digital
    					//AN0,1,2,3,5 set as analog
    
    //Turn off other peripherals
    CTMUCON = 0x0000;	//CTMU disabled
    CM1CON = 0x0000;	//COMP1 disabled
    CM2CON = 0x0000;	//COMP2 disabled
    CVRCON = 0x0000; 	//CVR disabled
    U2MODE = 0x0000;	//UART2 disabled
    HLVDCON = 0x0000;	//HLVD disabled
    RCFGCAL = 0x0000;	//RTC disabled
    
    //Set up UART1 to read in TX ID
    U1BRG = 14; 			//baud rate = 16.66kbps
    U1MODE = 0x0006;		//9-bit, no parity
	U1STA = 0x0000;			//interrupt after 1 byte received
    
    //Set transceiver I/O
    PHY_CS_TRIS = 0;
    PHY_RESETn_TRIS = 0;
    RF_INT_TRIS = 1;
    PHY_WAKE_TRIS = 0;
    SDI_TRIS = 1;
    SDO_TRIS = 0;
    SCK_TRIS = 0;
    
    PHY_CS = 1;
    PHY_RESETn = 0;
    PHY_WAKE = 1;
    SPI_SDO = 0;        
    SPI_SCK = 0;    
    
    // Setup SPI bus, but leave disabled for now
    #if defined(HARDWARE_SPI)
     	SPI1CON1 = 0b0000000100111110;
     	SPI1STAT = 0x0000;
    #endif

	

    INTCON2bits.INT1EP = 1;

    RFIF = 0;
        
    if( RF_INT_PIN == 0 )
    {
    	RFIF = 1;
    }

    //Set Powercast Module I/O
    PC_Reset_TRIS = 0;
    PC_DSET_TRIS = 0;         
    PC_INT_TRIS = 1;

    PC_Reset = 0;
    PC_DSET = 0;
    
    //Set Board I/O
    SENS_PWR_TRIS = 0;
    SENS_PWR = 0;			// enable sensors at startup
    
    //Dip Switch pins set up as inputs for reading ID
    ID1_TRIS = 1;
    ID2_TRIS = 1;
    ID3_TRIS = 1;
    
    // Set Mode select pin for input
    MODE_TRIS = 1;

    //Enable internal pull-ups
    ID1_PUE = 1;
    ID2_PUE = 1;
    ID3_PUE = 1;
    MODE_PUE = 1;
}
