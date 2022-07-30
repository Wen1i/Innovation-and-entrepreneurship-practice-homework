/********************************************************************
* FileName:		HardwareProfile.h
* Dependencies:    
* Processor:	PIC18, PIC24, PIC32, dsPIC30, dsPIC33
*               tested with 18F4620, dsPIC33FJ256GP710	
* Complier:     Microchip C18 v3.04 or higher
*				Microchip C30 v2.03 or higher
*               Microchip C32 v1.02 or higher		
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
*  3.1   5/28/2010    yfy       MiWi DE 3.1
*
* Change History (Powercast Code):
*  Ver   Date         Author    Description
*  1.0   10/05/2010   DWH       Initial revision
********************************************************************/

#ifndef _HARDWARE_PROFILE_H
    #define _HARDWARE_PROFILE_H

    #include "GenericTypeDefs.h"
    #include "ConfigApp.h"
    #ifdef __PIC32MX__
        #define RCON_SLEEP_MASK (0x8)
        #define RCON_IDLE_MASK  (0x4)
        #define Sleep()         PowerSaveSleep()   
        void Enable_PB_1_2_Interrupts(void);
    #endif
    

    /*********************************************************************/
    // Choose one of the demo board that support the demo out of the box
    /*********************************************************************/
    //#define PICDEMZ
    //#define PIC18_EXPLORER
    //#define EXPLORER16
    //#define EIGHT_BIT_WIRELESS_BOARD
    #define XLP16
    
    /*********************************************************************/
    // there are three ways to use NVM to store data: External EPROM, 
    // Data EEPROM and programming space, with following definitions:
    //  #define USE_EXTERNAL_EEPROM
    //  #define USE_DATA_EEPROM
    //  #define USE_PROGRAMMING_SPACE  
    // Each demo board has defined the method of using NVM, as
    // required by Network Freezer feature.
    /*********************************************************************/
    
    #if defined(XLP16)
	    #if defined(__PIC32MX__)
    	    #define CLOCK_FREQ 64000000    
        	#define RFIF IFS0bits.INT1IF
	        #define RFIE IEC0bits.INT1IE
    	#else
        	#define CLOCK_FREQ 8000000
	        #define RFIF IFS1bits.INT1IF
    	    #define RFIE IEC1bits.INT1IE
	    #endif
    
    	// This is incorrect because the correct pin, RB14, is also tied to the switch S3
    	#if defined(__PIC24F__) || defined(__PIC24FK__)    
        	#define RF_INT_PIN PORTAbits.RA0
	    #elif defined(__dsPIC33F__) || defined(__PIC24H__)
    	    #define RF_INT_PIN PORTAbits.RA12
	    #endif

	    #define TMRL TMR2
		// Transceiver Configuration
	    #define PHY_CS LATBbits.LATB3
    	#define PHY_CS_TRIS TRISBbits.TRISB3
    	#define PHY_RESETn LATBbits.LATB5
	   	#define PHY_RESETn_TRIS TRISBbits.TRISB5
    	#define PHY_WAKE LATBbits.LATB12
    	#define PHY_WAKE_TRIS TRISBbits.TRISB12

    	#define PUSH_BUTTON_1_PULLUP CNPU2bits.CN23PUE
	    #define PUSH_BUTTON_2_PULLUP CNPU1bits.CN12PUE
    
    	#define PUSH_BUTTON_1 PORTBbits.RB7
	    #define PUSH_BUTTON_2 PORTBbits.RB14  // This pin also tied to INT from radio, cannot be used because the switch doens't work
    	#define LED_2 LATBbits.LATB8
	    #define LED_1 LATBbits.LATB15
    
    	#define PUSH_BUTTON_1_TRIS TRISBbits.TRISB7
	    #define PUSH_BUTTON_2_TRIS TRISBbits.TRISB14
    	#define LED_1_TRIS TRISBbits.TRISB8
	    #define LED_2_TRIS TRISBbits.TRISB15
	#endif


    #if defined(EIGHT_BIT_WIRELESS_BOARD)
        #define CLOCK_FREQ          8000000
        #define USE_EXTERNAL_EEPROM
        
        // Define EEPROM_SHARE_SPI if external EEPROM shares the same 
        // SPI with transceiver
        #define EEPROM_SHARE_SPI
        
        // 8-bit wireless demo board can use the sensor port to
        // connect to either a LCD or a RS232 serial port
        #define SENSOR_PORT_LCD
        #define SENSOR_PORT_UART
        
        #if defined(MRF49XA)
            #define RFIF            INTCON3bits.INT3IF
            #define RFIE            INTCON3bits.INT3IE
            #define PHY_CS          LATCbits.LATC2
            #define PHY_CS_TRIS     TRISCbits.TRISC2
            #define RF_INT_PIN      PORTBbits.RB3
            #define RF_INT_TRIS     TRISBbits.TRISB3
            #define nFSEL           LATEbits.LATE1
            #define nFSEL_TRIS      TRISEbits.TRISE1
            #define FINT            PORTDbits.RD3
            #define FINT_TRIS       TRISDbits.TRISD3
        #elif defined(MRF24J40)
            #define RFIF            INTCON3bits.INT1IF
            #define RFIE            INTCON3bits.INT1IE
            #define PHY_CS          LATBbits.LATB3
            #define PHY_CS_TRIS     TRISBbits.TRISB3
            #define RF_INT_PIN      PORTBbits.RB1
            #define RF_INT_TRIS     TRISBbits.TRISB1
            #define PHY_WAKE        LATDbits.LATD3
            #define PHY_WAKE_TRIS   TRISDbits.TRISD3
		#elif defined(MRF89XA)
			#define RFIF            INTCON3bits.INT3IF
		    #define RFIE            INTCON3bits.INT3IE
			#define RF_INT_PIN	    PORTBbits.RB3
			#define RF_INT_TRIS	    TRISBbits.TRISB3
			#define PHY_IRQ1        INTCON3bits.INT3IF
			#define PHY_IRQ1_En   INTCON3bits.INT3IE	//Port connection - direction needs to be defined
			#define PHY_IRQ0_TRIS	TRISDbits.TRISD3
			#define Config_nCS      LATEbits.LATE1
		    #define Config_nCS_TRIS TRISEbits.TRISE1
			#define Data_nCS        LATCbits.LATC2
			#define Data_nCS_TRIS   TRISCbits.TRISC2
			#define IRQ0_Received   PORTDbits.RD3
        #endif
        
        #define SPI_SDI             PORTBbits.RB5               
        #define SDI_TRIS            TRISBbits.TRISB5
        #define SPI_SDO             LATCbits.LATC7               
        #define SDO_TRIS            TRISCbits.TRISC7
        #define SPI_SCK             LATBbits.LATB4               
        #define SCK_TRIS            TRISBbits.TRISB4
        
		#if defined(MRF24J40) || defined(MRF49XA)
        #define PHY_RESETn          LATDbits.LATD7
        #define PHY_RESETn_TRIS     TRISDbits.TRISD7
        #endif
        
        #define PUSH_BUTTON_1       PORTBbits.RB0
        #define BUTTON_1_TRIS       TRISBbits.TRISB0
        #define PUSH_BUTTON_2       PORTBbits.RB2
        #define BUTTON_2_TRIS       TRISBbits.TRISB2
        
        
        #define LED_1               LATAbits.LATA2
        #define LED_1_TRIS          TRISAbits.TRISA2
        #define LED_2               LATAbits.LATA3
        #define LED_2_TRIS          TRISAbits.TRISA3
    
        #define EE_nCS_TRIS         TRISDbits.TRISD5
        #define EE_nCS              LATDbits.LATD5
        #define TMRL                TMR0L   
        
        #if defined(SENSOR_PORT_LCD) && defined(SENSOR_PORT_UART)
            //#error  "The Sensor Port can only be used to connect to LCD OR UART board.
        #endif
               
    #endif
    
    #if defined(PIC18_EXPLORER) 
        #define CLOCK_FREQ          10000000
        #define USE_EXTERNAL_EEPROM
        
        // Define EEPROM_SHARE_SPI if external EEPROM shares the same 
        // SPI with transceiver
        #define EEPROM_SHARE_SPI
        
        // Transceiver Configuration
        #if defined(MRF49XA)
            #define RFIF            INTCON3bits.INT3IF
            #define RFIE            INTCON3bits.INT3IE
            #define PHY_CS          LATCbits.LATC2
            #define PHY_CS_TRIS     TRISCbits.TRISC2
            #define RF_INT_PIN      PORTBbits.RB3
            #define RF_INT_TRIS     TRISBbits.TRISB3
            #define nFSEL           LATEbits.LATE3             
            #define nFSEL_TRIS      TRISEbits.TRISE3
            #define FINT		    PORTBbits.RB4               
            #define FINT_TRIS       TRISBbits.TRISB4
        #elif defined(MRF24J40)
            #define RFIF            INTCON3bits.INT1IF
            #define RFIE            INTCON3bits.INT1IE
            #define PHY_CS          LATBbits.LATB3
            #define PHY_CS_TRIS     TRISBbits.TRISB3
            #define RF_INT_PIN      PORTBbits.RB1
            #define RF_INT_TRIS     TRISBbits.TRISB1

            #define PHY_WAKE        LATBbits.LATB4
            #define PHY_WAKE_TRIS   TRISBbits.TRISB4
		#elif defined(MRF89XA)
			#define RFIF            INTCON3bits.INT3IF
		    #define RFIE            INTCON3bits.INT3IE
			#define RF_INT_PIN	    PORTBbits.RB3
			#define RF_INT_TRIS	    TRISBbits.TRISB3
			#define PHY_IRQ1        INTCON3bits.INT3IF
			#define PHY_IRQ1_En     INTCON3bits.INT3IE	
			#define PHY_IRQ0_TRIS	TRISBbits.TRISB4
			#define Config_nCS      LATEbits.LATE3
		    #define Config_nCS_TRIS TRISEbits.TRISE3
			#define Data_nCS        LATCbits.LATC2
			#define Data_nCS_TRIS   TRISCbits.TRISC2
			#define IRQ0_Received   PORTBbits.RB4
			
        #endif
    
        #define SPI_SDI             PORTCbits.RC4               
        #define SDI_TRIS            TRISCbits.TRISC4
        #define SPI_SDO             LATCbits.LATC5               
        #define SDO_TRIS            TRISCbits.TRISC5
        #define SPI_SCK             LATCbits.LATC3               
        #define SCK_TRIS            TRISCbits.TRISC3

        #define PHY_RESETn          LATBbits.LATB5
        #define PHY_RESETn_TRIS     TRISBbits.TRISB5
        
        #define PUSH_BUTTON_1       PORTBbits.RB0
        #define BUTTON_1_TRIS       TRISBbits.TRISB0
        #define PUSH_BUTTON_2       PORTAbits.RA5
        #define BUTTON_2_TRIS       TRISAbits.TRISA5
        
        
        #define LED_1               LATDbits.LATD7
        #define LED_1_TRIS          TRISDbits.TRISD7
        #define LED_2               LATDbits.LATD6
        #define LED_2_TRIS          TRISDbits.TRISD6
    
    
        #define EE_nCS_TRIS         TRISAbits.TRISA3
        #define EE_nCS              LATAbits.LATA3
        #define TMRL                TMR0L
        
    #endif
    
    
    
    #if defined(PICDEMZ)
        #define CLOCK_FREQ          16000000
        #define USE_DATA_EEPROM
        
        // Transceiver Configuration
        #define RFIF                INTCONbits.INT0IF
        #define RFIE                INTCONbits.INT0IE
        #define PHY_CS              LATCbits.LATC0
        #define PHY_CS_TRIS         TRISCbits.TRISC0
        #define PHY_RESETn          LATCbits.LATC2
        #define PHY_RESETn_TRIS     TRISCbits.TRISC2
        #define PHY_WAKE            LATCbits.LATC1
        #define PHY_WAKE_TRIS       TRISCbits.TRISC1
        
        #define PUSH_BUTTON_1       PORTBbits.RB5
        #define PUSH_BUTTON_2       PORTBbits.RB4
        #define LED_1               LATAbits.LATA0
        #define LED_2               LATAbits.LATA1
        
        #define BUTTON_1_TRIS       TRISBbits.TRISB5
        #define BUTTON_2_TRIS       TRISBbits.TRISB4
        #define LED_1_TRIS          TRISAbits.TRISA0
        #define LED_2_TRIS          TRISAbits.TRISA1
        
        #define RF_INT_PIN          PORTBbits.RB0
        #define RF_INT_TRIS         TRISBbits.TRISB0
        
        #define SPI_SDI             PORTCbits.RC4               
        #define SDI_TRIS            TRISCbits.TRISC4
        #define SPI_SDO             LATCbits.LATC5               
        #define SDO_TRIS            TRISCbits.TRISC5
        #define SPI_SCK             LATCbits.LATC3               
        #define SCK_TRIS            TRISCbits.TRISC3
        
        #define TMRL                TMR0L
    #endif
    
    
    #if defined(EXPLORER16) 
    
        #if defined(__PIC32MX__)
            #define CLOCK_FREQ      64000000    
            #define RFIF            IFS0bits.INT1IF
            #define RFIE            IEC0bits.INT1IE
        #else
            #define CLOCK_FREQ      8000000
            #define RFIF            IFS1bits.INT1IF
            #define RFIE            IEC1bits.INT1IE
        #endif
    
        #if defined(__PIC24F__) || defined(__PIC24FK__) || defined(__PIC32MX__)   
            #define RF_INT_PIN      PORTEbits.RE8   
            #define RF_INT_TRIS     TRISEbits.TRISE8 
        #elif defined(__dsPIC33F__) || defined(__PIC24H__)
            #define RF_INT_PIN      PORTAbits.RA12
            #define RF_INT_TRIS     TRISAbits.TRISA12
        #endif
        
        #define USE_EXTERNAL_EEPROM
        
        // Transceiver Configuration
		#if (defined(MRF24J40) || defined(MRF49XA))
            #define PHY_CS          LATBbits.LATB2
            #define PHY_CS_TRIS     TRISBbits.TRISB2
            #define PHY_RESETn      LATGbits.LATG2
            #define PHY_RESETn_TRIS TRISGbits.TRISG2
        #endif

        #define SPI_SDI             PORTFbits.RF7 
        #define SDI_TRIS            TRISFbits.TRISF7
        #define SPI_SDO             LATFbits.LATF8  
        #define SDO_TRIS            TRISFbits.TRISF8
        #define SPI_SCK             LATFbits.LATF6 
        #define SCK_TRIS            TRISFbits.TRISF6
        
        #if defined(MRF49XA)
            #define nFSEL           LATBbits.LATB1              
            #define nFSEL_TRIS      TRISBbits.TRISB1
            #define FINT		    PORTEbits.RE9 
            #define FINT_TRIS       TRISEbits.TRISE9
        #elif defined(MRF24J40)
            #define PHY_WAKE        LATGbits.LATG3
            #define PHY_WAKE_TRIS   TRISGbits.TRISG3
		#else
			#if defined(__PIC32MX__)
				#define PHY_IRQ1        IFS0bits.INT2IF
				#define PHY_IRQ1_En     IEC0bits.INT2IE			//Second interrupt from MRF89XA
			#else
				#define PHY_IRQ1        IFS1bits.INT2IF
				#define PHY_IRQ1_En     IEC1bits.INT2IE			//Second interrupt from MRF89XA
			#endif
			#define PHY_IRQ1_TRIS   TRISEbits.TRISE9
			#define Config_nCS      LATBbits.LATB1
		    #define Config_nCS_TRIS TRISBbits.TRISB1
			#define Data_nCS        LATBbits.LATB2
			#define Data_nCS_TRIS   TRISBbits.TRISB2
        #endif
        
        #define PUSH_BUTTON_1       PORTDbits.RD6
        #define PUSH_BUTTON_2       PORTDbits.RD7
        #define LED_1               LATAbits.LATA7
        #define LED_2               LATAbits.LATA6
        
        #define BUTTON_1_TRIS       TRISDbits.TRISD6
        #define BUTTON_2_TRIS       TRISDbits.TRISD7
        #define LED_1_TRIS          TRISAbits.TRISA7
        #define LED_2_TRIS          TRISAbits.TRISA6
        
        // Define SUPPORT_TWO_SPI if external EEPROM use the second SPI
        // port alone, not sharing SPI port with the transceiver
        #define SUPPORT_TWO_SPI
        
        // External EEPROM SPI chip select pin definition
        #define EE_nCS_TRIS         TRISDbits.TRISD12
        #define EE_nCS              LATDbits.LATD12
    
        #define TMRL                TMR2
        
    #endif
    
    
    
    
    #if defined(PICDEMZ)
        #define CLOCK_FREQ          16000000
        
        // Transceiver Configuration
        #define RFIF                INTCONbits.INT0IF
        #define RFIE                INTCONbits.INT0IE
        #define PHY_CS              LATCbits.LATC0
        #define PHY_CS_TRIS         TRISCbits.TRISC0
        #define PHY_RESETn          LATCbits.LATC2
        #define PHY_RESETn_TRIS     TRISCbits.TRISC2
        #define PHY_WAKE            LATCbits.LATC1
        #define PHY_WAKE_TRIS       TRISCbits.TRISC1
        
        #define PUSH_BUTTON_1       PORTBbits.RB5
        #define PUSH_BUTTON_2       PORTBbits.RB4
        #define LED_1               LATAbits.LATA0
        #define LED_2               LATAbits.LATA1
        
        #define BUTTON_1_TRIS       TRISBbits.TRISB5
        #define BUTTON_2_TRIS       TRISBbits.TRISB4
        #define LED_1_TRIS          TRISAbits.TRISA0
        #define LED_2_TRIS          TRISAbits.TRISA1
        
        #define RF_INT_PIN          PORTBbits.RB0
        #define RF_INT_TRIS         TRISBbits.TRISB0
        
        #define SPI_SDI             PORTCbits.RC4               
        #define SDI_TRIS            TRISCbits.TRISC4
        #define SPI_SDO             LATCbits.LATC5               
        #define SDO_TRIS            TRISCbits.TRISC5
        #define SPI_SCK             LATCbits.LATC3               
        #define SCK_TRIS            TRISCbits.TRISC3
        
        #define TMRL                TMR0L
        
    #endif

    // Following definitions are used for LCD display on the demo board
    #if defined(EXPLORER16)
    
    	#define LCD_DATA0_TRIS		(TRISEbits.TRISE0)		// Multiplexed with LED6
    	#define LCD_DATA0_IO		(LATEbits.LATE0)
    	#define LCD_DATA1_TRIS		(TRISEbits.TRISE1)
    	#define LCD_DATA1_IO		(LATEbits.LATE1)
    	#define LCD_DATA2_TRIS		(TRISEbits.TRISE2)
    	#define LCD_DATA2_IO		(LATEbits.LATE2)
    	#define LCD_DATA3_TRIS		(TRISEbits.TRISE3)		// Multiplexed with LED3
    	#define LCD_DATA3_IO		(LATEbits.LATE3)
    	#define LCD_DATA4_TRIS		(TRISEbits.TRISE4)		// Multiplexed with LED2
    	#define LCD_DATA4_IO		(LATEbits.LATE4)
    	#define LCD_DATA5_TRIS		(TRISEbits.TRISE5)
    	#define LCD_DATA5_IO		(LATEbits.LATE5)
    	#define LCD_DATA6_TRIS		(TRISEbits.TRISE6)
    	#define LCD_DATA6_IO		(LATEbits.LATE6)
    	#define LCD_DATA7_TRIS		(TRISEbits.TRISE7)
    	#define LCD_DATA7_IO		(LATEbits.LATE7)
    	#define LCD_RD_WR_TRIS		(TRISDbits.TRISD5)
    	#define LCD_RD_WR_IO		(LATDbits.LATD5)
    	#define LCD_RS_TRIS			(TRISBbits.TRISB15)
    	#define LCD_RS_IO			(LATBbits.LATB15)
    	#define LCD_E_TRIS			(TRISDbits.TRISD4)
    	#define LCD_E_IO			(LATDbits.LATD4)
    
    #elif defined(PIC18_EXPLORER)
    
        #define LCD_CS_TRIS         (TRISAbits.TRISA2)
        #define LCD_CS              (LATAbits.LATA2)
        #define LCD_RESET_TRIS      (TRISFbits.TRISF6)
        #define LCD_RESET           (LATFbits.LATF6)
    
    #elif defined(EIGHT_BIT_WIRELESS_BOARD)
    
        #define SUPPORT_TWO_SPI
        
        #define SPInCS_TRIS         TRISCbits.TRISC6
        #define SPInCS_LAT	        LATCbits.LATC6
    	#define	SPISSPSTAT	        SSP2STAT
    	#define SPISSPCON	        SSP2CON1
    	#define SPISSPBUF	        SSP2BUF
    	#define SPIWCOL		        SSP2CON1bits.WCOL
    	#define SPISSPIE	        PIE3bits.SSP2IE
    	#define SPISSPIF	        PIR3bits.SSP2IF
    	
    #endif


    // Following definition is for delay functionality
    #if defined(__18CXX)
        #define GetInstructionClock()	(CLOCK_FREQ/4)
    #elif defined(__C30__) 
        #define GetInstructionClock()	(CLOCK_FREQ/2)
    #elif defined(__PIC32MX__)
        #define GetInstructionClock()	(CLOCK_FREQ)
    #endif

    BYTE ButtonPressed(void);
    void BoardInit(void);

    void LCDDisplay(char *, BYTE, BOOL);
    void LCDTRXCount(BYTE, BYTE);
    
/*float rssi_lookup[] = {		// this table contains RSSI data based on voltage
0.027669416	,
0.058479008	,
0.086896043	,
0.111943788	,
0.138675583	,
0.16519618	,
0.193642196	,
0.222843515	,
0.253512863	,
0.284446111	,
0.316956746	,
0.351560441	,
0.386366977	,
0.422668614	,
0.460256574	,
0.498310456	,
0.539510623	,
0.580764418	,
0.622300285	,
0.666806769	,
0.722769804	,
0.76913044	,
0.814704284	,
0.866961876	,
0.91622049	,
0.968277856	,
1.020939484	,
1.071519305	,
1.127197456	,
1.183041556	,
1.241652308	,
1.297179271	,
1.358313447	,
1.419057522	,
1.479108388	,
1.54525444	,
1.606941253	,
1.674942876	,
1.737800829	,
1.807174126	,
1.874994508	,
1.9498446	,
2.018366364	,
2.089296131	,
2.162718524	,
2.238721139	,
2.31739465	,
2.398832919	,
2.477422058	,
2.552701303	,
2.630267992	,
2.716439269	,
2.792543841	,
2.877398415	,
2.96483139	,
3.04789499	,
3.140508694	,
3.228494122	,
3.318944576	,
3.411929116	,
3.50751874	,
3.597493352	,
3.698281798	,
3.79314985	,
3.899419867	,
3.990249024	,
4.092606597	,
4.19758984	,
4.295364268	,
4.395416154	,
4.508167045	,
4.613175746	,
4.720630413	,
4.819477976	,
4.94310687	,
5.046612976	,
5.152286446	,
5.272298614	,
5.395106225	,
5.508076964	,
5.623413252	,
5.741164622	,
5.861381645	,
5.970352866	,
6.095368972	,
6.237348355	,
6.353309319	,
6.486344335	,
6.60693448	,
6.729766563	,
6.854882265	,
6.99841996	,
7.128530301	,
7.261059574	,
7.396052751	,
7.533555637	,
7.655966069	,
7.816278046	,
7.943282347	,
8.072350302	,
8.222426499	,
8.356030182	,
8.511380382	,
8.649679188	,
8.790225168	,
8.933054837	,
9.078205302	,
9.225714272	,
9.375620069	,
9.52796164	,
9.682778563	,
9.840111058	,
9.977000638	,
10.11579454	,
10.28016298	,
10.44720219	,
10.59253725	,
10.76465214	,
10.91440336	,
11.06623784	,
11.22018454	,
11.40249788	,
11.56112242	,
11.69499391	,
11.88502227	,
12.0503594	,
12.2179966	,
12.35947433	,
12.53141175	,
12.70574105	,
12.88249552	,
13.06170888	,
13.21295634	,
13.39676687	,
13.55189412	,
13.74041975	,
13.93156803	,
14.09288798	,
14.25607594	,
14.42115352	,
14.62177174	,
14.79108388	,
14.99684836	,
15.17050367	,
15.34616983	,
15.5238701	,
15.70362804	,
15.88546749	,
16.06941253	,
16.21810097	,
16.44371723	,
16.6341265	,
16.78804018	,
16.98243652	,
17.13957308	,
17.33803998	,
17.53880502	,
17.70108958	,
17.90605854	,
18.11340093	,
18.28100216	,
18.49268619	,
18.66379691	,
18.87991349	,
19.09853259	,
19.31968317	,
19.54339456	
};
*/


#endif
