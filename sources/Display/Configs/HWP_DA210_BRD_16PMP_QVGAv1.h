/*********************************************************************
 *
 *	Hardware specific definitions
 *
 *********************************************************************
 **/

#ifndef __HARDWARE_PROFILE_H
    #define __HARDWARE_PROFILE_H

/*********************************************************************
* PIC Device Specific includes
*********************************************************************/
    //#include "Compiler.h"

/*********************************************************************
* GetSystemClock() returns system clock frequency.
*
* GetPeripheralClock() returns peripheral clock frequency.
*
* GetInstructionClock() returns instruction clock frequency.
*
********************************************************************/

/*********************************************************************
* Macro: #define	GetSystemClock() 
*
* Overview: This macro returns the system clock frequency in Hertz.
*			* value is 8 MHz x 4 PLL for PIC24
*			* value is 8 MHz/2 x 18 PLL for PIC32
*
********************************************************************/
   
    #define GetSystemClock()    (32000000ul)    

/*********************************************************************
* Macro: #define	GetPeripheralClock() 
*
* Overview: This macro returns the peripheral clock frequency 
*			used in Hertz.
*			* value for PIC24 is <PRE>(GetSystemClock()/2) </PRE> 
*			* value for PIC32 is <PRE>(GetSystemClock()/(1<<OSCCONbits.PBDIV)) </PRE>
*
********************************************************************/
  
  #define GetPeripheralClock()    (GetSystemClock() / 2)
   
/*********************************************************************
* Macro: #define	GetInstructionClock() 
*
* Overview: This macro returns instruction clock frequency 
*			used in Hertz.
*			* value for PIC24 is <PRE>(GetSystemClock()/2) </PRE> 
*			* value for PIC32 is <PRE>(GetSystemClock()) </PRE> 
*
********************************************************************/
   
        #define GetInstructionClock()   (GetSystemClock() / 2)
  

/* ################################################################## */
/*********************************************************************
* START OF GRAPHICS RELATED MACROS
********************************************************************/
/* ################################################################## */

/*********************************************************************
* AUTO GENERATED CODE 
********************************************************************/

//Auto Generated Code
#define PIC24FJ256DA210_DEV_BOARD
//#define USE_16BIT_PMP
#define GFX_USE_DISPLAY_CONTROLLER_MCHP_DA210
#define GFX_USE_DISPLAY_PANEL_TFT_G240320LTSW_118W_E
#define GFX_GCLK_DIVIDER 57//61 //
//#define GFX_DISPLAY_BUFFER_START_ADDRESS 0x00020000ul
//#define GFX_DISPLAY_BUFFER_LENGTH 0x00025800ul
#define GFX_DISPLAY_BUFFER_START_ADDRESS 0x00004B00ul
#define GFX_DISPLAY_BUFFER_LENGTH 0x0009600ul
//#define GFX_EPMP_CS1_BASE_ADDRESS 0x00020000ul
//#define GFX_EPMP_CS1_MEMORY_SIZE 0x40000ul
#define GFX_WORK_AREA1_START_ADDRESS GFX_DISPLAY_BUFFER_START_ADDRESS  
#define GFX_WORK_AREA2_START_ADDRESS 0x000E100ul
#define GFX_WORK__ADDRESS 0x0000001ul
//End Auto Generated Code



/*********************************************************************
* END OF AUTO GENERATED CODE 
********************************************************************/

/*********************************************************************
* ERROR CHECKING
*********************************************************************/
/*
   This error check are intended for Microchip Development
   Boards and Modules. When using custom hardware, remove this part
   of the hardware profile.
*/
#if !defined (GFX_PICTAIL_V3) &&            \
    !defined (GFX_PICTAIL_V3E)&&            \
    !defined (PIC24FJ256DA210_DEV_BOARD) && \
    !defined (MEB_BOARD) &&                 \
    !defined (GFX_PICTAIL_LCC) &&           \
	!defined (MIKRO_BOARD)
	#error "Error: Graphics Hardware Platform is not defined! One of the following boards (GFX_PICTAIL_V3, GFX_PICTAIL_V3E, PIC24FJ256DA210_DEV_BOARD, MEB_BOARD, GFX_PICTAIL_LCC, MIKRO_BOARD) must be defined."
#endif

/*
   This check verifies that the selected PMP interface supports
   16-bit interface.
*/
#if defined (__PIC24FJ128GA010__) || defined (__PIC24FJ256GA110__) ||	\
	defined (__PIC24FJ256GB110__) || defined (__dsPIC33F__)        ||   \
	defined (__PIC24HJ128GP504__)
	#if defined (USE_16BIT_PMP)	
		#error "The device selected does not support 16 bit PMP interface."
	#endif
#endif

/*********************************************************************
* DISPLAY PARAMETERS 
*********************************************************************/

// -----------------------------------
// For RGB GLASS
// -----------------------------------
// Using TFT_G240320LTSW_118W_E, TFT2N0369_E, DT032TFT_TS, DT032TFT  Display Panel.
// These panels uses Solomon Systech SSD1289 as their controller. 
	#if defined (GFX_USE_DISPLAY_PANEL_TFT_G240320LTSW_118W_E)
	   
		/* note: For SSD1289 
			
			tHBP (horizontal back porch)= DISP_HOR_BACK_PORCH + DISP_HOR_PULSE_WIDTH
			tVBP (vertical back porch)  = DISP_VER_BACK_PORCH + DISP_VER_PULSE_WIDTH

			Horizontal Cycle (280) = tHBP + DISP_HOR_FRONT_PORCH + DISP_HOR_RESOLUTION
			Vertical Cycle (326)   = tVBP + DISP_VER_FRONT_PORCH + DISP_VER_RESOLUTION

			In summary, the DISP_HOR_PULSE_WIDTH should not exceed tHBP and 
			the DISP_VER_PULSE_WIDTH should not exceed tVBP. See SSD1289 data sheet for 
			details.
		*/
		#define DISP_ORIENTATION		0 // 90
        #define DISP_HOR_RESOLUTION		320// //320
        #define DISP_VER_RESOLUTION		240 //240 
        #define DISP_DATA_WIDTH			18 //18
        #define DISP_INV_LSHIFT
        #define DISP_HOR_PULSE_WIDTH    96//96//25
        #define DISP_HOR_BACK_PORCH     2//3
        #define DISP_HOR_FRONT_PORCH    8//10
        #define DISP_VER_PULSE_WIDTH    4//4
        #define DISP_VER_BACK_PORCH     5// 0//
        #define DISP_VER_FRONT_PORCH    2//2
    	#define GFX_LCD_TYPE            GFX_LCD_TFT

		#if defined(GFX_USE_DISPLAY_CONTROLLER_MCHP_DA210)
            #define GFX_DISPLAYENABLE_ENABLE
            #define GFX_HSYNC_ENABLE
            #define GFX_VSYNC_ENABLE
            // #define GFX_DISPLAYPOWER_ENABLE
            #define GFX_CLOCK_POLARITY                  GFX_ACTIVE_HIGH
            #define GFX_DISPLAYENABLE_POLARITY          GFX_ACTIVE_HIGH
            #define GFX_HSYNC_POLARITY                  GFX_ACTIVE_LOW
            #define GFX_VSYNC_POLARITY                  GFX_ACTIVE_LOW
        #endif
	#endif // #if defined (GFX_USE_DISPLAY_PANEL_TFT_G240320LTSW_118W_E) || defined (GFX_USE_DISPLAY_PANEL_TFT2N0369_E) ||....
	

// -----------------------------------
// Other driver dependencies
// -----------------------------------
#if defined (GFX_USE_DISPLAY_PANEL_TFT_G240320LTSW_118W_E)
    #define USE_TCON_SSD1289
    #define USE_TCON_MODULE
    

#endif	

/*********************************************************************
* PARALLEL MASTER PORT INTERFACE TIMING AND POLARITIES 
*********************************************************************/
/*
    This portion sets up the EPMP port timing when Graphics Module is
    used as a Alternate Master in EPMP (See Family Reference Manual (FRM)
    Section 42. Enhanced Parallel Master Port (EPMP)) for details.

    Macros to define the access timing of the parallel device in EPMP chip 
	selects.  This value is used to calculate the AMWAIT cycles needed in EPMP
	Alternate Master I/Os direct access (EPMP bypass mode) (MSTSEL = 0x3). 
	When in this mode the Alternate Master (in this case Graphics Module)
	will use the clock specified by the G1CLKSEL bits of CLKDIV register
	in the oscillator module. There are two possible values: 48 Mhz and 
	96 Mhz. The equation used is:
	    temp = (1000000000ul) / ((CLKDIVbits.G1CLKSEL == 1)? 960000000ul : 480000000);
		
		if (EPMPCSx_ACCESS_TIME <= (temp*3))
		    PMCSxMDbits.AMWAIT = 0;
		else if (EPMPCSx_ACCESS_TIME > temp)
	    {
            PMCSxMDbits.AMWAIT = (EPMPCSx_ACCESS_TIME / temp);
            if ((EPMPCSx_ACCESS_TIME % temp) > 0)
                PMCSxMDbits.AMWAIT += 1;
        }          
		 
        where:   
            x = 1 for CS1 
            x = 2 for CS2
            the factor of 3 is used because of the fact that an AMWAIT of 0 means
            that 3 alternate master cycles are used per read/write.
            Actual cycles used is 3 + AMWAIT cycles per read/write access.

*/
#ifdef PIC24FJ256DA210_DEV_BOARD

	#if defined (GFX_EPMP_CS1_BASE_ADDRESS)
		#define EPMPCS1_CS_POLARITY	GFX_ACTIVE_LOW
		#define EPMPCS1_WR_POLARITY	GFX_ACTIVE_LOW
		#define EPMPCS1_RD_POLARITY	GFX_ACTIVE_LOW
		#define EPMPCS1_BE_POLARITY	GFX_ACTIVE_LOW


		#define EPMPCS1_ACCESS_TIME                 (10)		// chip select 1 Alternate Master access time.

	#endif

	#if defined (GFX_EPMP_CS2_BASE_ADDRESS)
		//#define EPMPCS2_CS_POLARITY	GFX_ACTIVE_LOW
		////#define EPMPCS2_WR_POLARITY	GFX_ACTIVE_LOW
		//#define EPMPCS2_RD_POLARITY	GFX_ACTIVE_LOW
		//#define EPMPCS2_BE_POLARITY	GFX_ACTIVE_LOW

		// macros to define the access timing of the parallel device in EPMP CS2
		//#define EPMPCS2_ACCESS_TIME                 (55)		// chip select 1 Alternate Master access time.

	#endif
#endif


/*********************************************************************
* HARDWARE PROFILE FOR DISPLAY CONTROLLER INTERFACE
*********************************************************************/
/* 
    The following defines the different IO signals used to connect
    to the display controller.
    The hardware profile used is dependent on the PIC device 
    and the development board used.
    
    The following are used to control the following signals to the
    Display Controller:
    1. Reset Signal - this signal controls the reset pin of the display
            controller if available.
            
            Example:
            #define DisplayResetConfig()        TRISAbits.TRISA4 = 0    
            #define DisplayResetEnable()        LATAbits.LATA4 = 0
            #define DisplayResetDisable()       LATAbits.LATA4 = 1

    2. Command/Data Signal - this signal indicates to the controller
            if the command or data is being sent to the display controller.

            Example:
            #define DisplayCmdDataConfig()      TRISAbits.TRISA1 = 0
            #define DisplaySetCommand()         LATAbits.LATA1 = 0
            #define DisplaySetData()            LATAbits.LATA1 = 1

    3. Controller Chip Select Signal - this signal controls the chip select
            pin of the display controller is available.

            Example:
            #define DisplayConfig()             TRISBbits.TRISB15 = 0             
            #define DisplayEnable()             LATBbits.LATB15 = 0
            #define DisplayDisable()            LATBbits.LATB15 = 1
            
    4. Display Module Power Control Signal - this signal turns on or off
            the display module if available. In some cases, this can be an 
            IO port that will enable the power circuitry of your 
            display module.

            Example:
            #define DisplayPowerConfig()        TRISAbits.TRISA5 = 0       
            #define DisplayPowerOn()            LATAbits.LATA5 = 1           
            #define DisplayPowerOff()           LATAbits.LATA5 = 0

    5. Display Backlight Brightness Control - this controls the backlight
            brightness or ON or OFF states if available. If brightness is 
            controllable, this signal may be driven by some PWM signal. 

            Example:
            #define DisplayBacklightConfig()    TRISDbits.TRISD0 = 0  
            #define DisplayBacklightOn()        LATDbits.LATD0 = 0
            #define DisplayBacklightOff()       LATDbits.LATD0 = 1               
    
*/


    // Depending on the display panel used, the backlight control may be
    // different. This portion sets up the correct backlight level to 
    // turn on or off the backlight.
    #if defined (GFX_USE_DISPLAY_PANEL_TFT_G240320LTSW_118W_E)
        #define BACKLIGHT_ENABLE_LEVEL      0
        #define BACKLIGHT_DISABLE_LEVEL     1
   
    #endif    

typedef enum
{
    ANSEL_DIGITAL = 0,
    ANSEL_ANALOG = 1
}ANSEL_BIT_STATE;
    

#if defined (PIC24FJ256DA210_DEV_BOARD)
	
    // Definitions for POWER ON pin
    #define DisplayPowerConfig()            TRISAbits.TRISA5 = 0       
    #define DisplayPowerOn()                LATAbits.LATA5 = 1           
    #define DisplayPowerOff()               LATAbits.LATA5 = 0

    // Definitions for backlight control pin
    #define DisplayBacklightConfig()        
    #define DisplayBacklightOn()        
    #define DisplayBacklightOff()          
    
    // Dummy definitions
    // Definitions for reset pin
    #define DisplayResetConfig()           
    #define DisplayResetEnable()           
    #define DisplayResetDisable()          

    // Definitions for RS pin
    #define DisplayCmdDataConfig()         
    #define DisplaySetCommand()            
    #define DisplaySetData()               

    // Definitions for CS pin
    #define DisplayConfig()                
    #define DisplayEnable()                
    #define DisplayDisable()               
 

#endif 


/*
	These are the hardware settings for the SPI peripherals used.
*/
 

/*********************************************************************
* HARDWARE PROFILE FOR THE TIMING CONTROLLER
*********************************************************************/
/*
	Timing Controllers (TCON) are used by some displays.
	The display selected will determine the TCON that will be used
	in the build. 
	When using SSD1926, the TCON is connected through the SSD1926 
	GPIO and SPI transfers is BitBanged on these GPIO ports.
	Refer to SSD1926 implementation for details.
*/
	/*********************************************************************
	* I/Os for the TCON 
	*********************************************************************/

	/* ----------------------------------------- */
	#if defined (PIC24FJ256DA210_DEV_BOARD) 
		#if defined (GFX_USE_DISPLAY_PANEL_TFT_G240320LTSW_118W_E) //|| defined (GFX_USE_DISPLAY_PANEL_35QVW0T)
	/* ----------------------------------------- */
		
		#define TCON_CS_LAT      LATAbits.LATA0   //_RA0
		#define TCON_CS_TRIS     TRISAbits.TRISA0 //_TRISA0
		#define TCON_CS_DIG()
	    
		#define TCON_SCL_LAT     LATDbits.LATD8   //_RD8
		#define TCON_SCL_TRIS    TRISDbits.TRISD8 //_TRISD8
		#define TCON_SCL_DIG()
	    
		#define TCON_SDO_LAT     LATBbits.LATB1   //_RB1
		#define TCON_SDO_TRIS    TRISBbits.TRISB1 //_TRISB1
		#define TCON_SDO_DIG()   _ANSB1 = ANSEL_DIGITAL;
	    
		#define TCON_DC_LAT      LATBbits.LATB0   //_RB0
		#define TCON_DC_TRIS     TRISBbits.TRISB0 //_TRISB0
		#define TCON_DC_DIG()    _ANSB0 = ANSEL_DIGITAL;
		
		#endif // #if defined (GFX_USE_DISPLAY_PANEL_TFT_G240320LTSW_118W_E)
	#endif // #if defined (DA210_DEV_BOARD)...

//***************

typedef enum
{
    HW_BUTTON_PRESS = 0,
    HW_BUTTON_RELEASE = 1
}HW_BUTTON_STATE;

#if defined (PIC24FJ256DA210_DEV_BOARD)
    #if defined(__PIC24FJ256DA210__)
        #define HardwareButtonInit()
        #define GetHWButtonProgram()        (PORTEbits.RE9)
        #define GetHWButtonScanDown()       (HW_BUTTON_RELEASE)
        #define GetHWButtonScanUp()         (HW_BUTTON_RELEASE) 
        #define GetHWButtonCR()             (PORTGbits.RG8)
        #define GetHWButtonFocus()          (PORTEbits.RE9)
        #define GetHWButtonLeft()           (PORTGbits.RG8)
        #define GetHWButtonCenter()         (PORTEbits.RE9)
        #define GetHWButtonRight()          (PORTBbits.RB5)
    #endif 
#endif 
/*********************************************************************
* IOS FOR THE UART
*********************************************************************/

#if defined(__PIC24FJ256DA210__)    
    #define TX_TRIS TRISFbits.TRISF3
    #define RX_TRIS TRISDbits.TRISD0
#endif

/*********************************************************************
* RTCC DEFAULT INITIALIZATION (these are values to initialize the RTCC
*********************************************************************/
#define RTCC_DEFAULT_DAY        13      // 13th
#define RTCC_DEFAULT_MONTH      2       // February
#define RTCC_DEFAULT_YEAR       12      // 2012
#define RTCC_DEFAULT_WEEKDAY    1       // Monday
#define RTCC_DEFAULT_HOUR       10      // 10:10:01
#define RTCC_DEFAULT_MINUTE     10
#define RTCC_DEFAULT_SECOND     1



#endif // __HARDWARE_PROFILE_H




