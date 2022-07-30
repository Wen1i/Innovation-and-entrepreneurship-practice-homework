/********************************************************************
* FileName:		HardwareProfile.h
* Dependencies:    
* Processor:	PIC24
* Complier:     Microchip C30 v2.03 or higher	
* Company:		Microchip Technology, Inc.
*
* Copyright and Disclaimer Notice
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
*  This file defines functions used for demo board hardware
*
* Change History:
*  Rev   Date         Author    Description
*  1.0   2/15/2009    yfy       Initial revision
*  2.0   4/15/2009    yfy       MiMAC and MiApp revision
*  2.1	 8/31/2010	  DWH       Edited to work with Powercast Demo
*
* Change History (Powercast Code):
*  Ver   Date         Author    Description
*  1.0   10/05/2010   DWH       Initial version
********************************************************************/

#ifndef _HARDWARE_PROFILE_H
    #define _HARDWARE_PROFILE_H
    
    #include "GenericTypeDefs.h"
    #include "ConfigApp.h"
    
    void BoardInit(void);

    #define CLOCK_FREQ      8000000
    #define RFIF            IFS1bits.INT1IF
    #define RFIE            IEC1bits.INT1IE
    #define TMRL			TMR2

    #define GetInstructionClock()	(CLOCK_FREQ/2)

    // Transceiver Configuration
    #define RF_INT_PIN      PORTBbits.RB14   
    #define RF_INT_TRIS     TRISBbits.TRISB14 
    #define PHY_CS 			LATAbits.LATA7
    #define PHY_CS_TRIS 	TRISAbits.TRISA7
    #define PHY_RESETn 		LATAbits.LATA6
    #define PHY_RESETn_TRIS TRISAbits.TRISA6
    #define PHY_WAKE 		LATBbits.LATB15
    #define PHY_WAKE_TRIS 	TRISBbits.TRISB15
    #define SPI_SDI     	PORTBbits.RB10           
    #define SDI_TRIS		TRISBbits.TRISB10
    #define SPI_SDO    		LATBbits.LATB13              
    #define SDO_TRIS  		TRISBbits.TRISB13
    #define SPI_SCK   		LATBbits.LATB11              
    #define SCK_TRIS  		TRISBbits.TRISB11
       
    // Powercast Module Pins
    #define PC_Reset		LATBbits.LATB4
    #define PC_Reset_TRIS	TRISBbits.TRISB4
    #define PC_DSET			LATAbits.LATA3
    #define PC_DSET_TRIS	TRISAbits.TRISA3
    #define PC_INT			PORTAbits.RA4
    #define PC_INT_TRIS		TRISAbits.TRISA4

	// Sensor Power Control
    #define SENS_PWR 		LATAbits.LATA2
    #define SENS_PWR_TRIS	TRISAbits.TRISA2
      
    // Dip Switches for ID
    #define ID1 			PORTBbits.RB9
    #define ID1_TRIS		TRISBbits.TRISB9
    #define ID1_PUE			CNPU2bits.CN21PUE
    #define ID2 			PORTBbits.RB8
    #define ID2_TRIS		TRISBbits.TRISB8
    #define ID2_PUE			CNPU2bits.CN22PUE
    #define ID3 			PORTBbits.RB7
    #define ID3_TRIS		TRISBbits.TRISB7
    #define ID3_PUE			CNPU2bits.CN23PUE
    
    // Mode Select
    #define MODE 			PORTBbits.RB12
    #define MODE_TRIS		TRISBbits.TRISB12
    #define MODE_PUE		CNPU1bits.CN14PUE
    
#endif
