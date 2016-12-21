
/*!*****************************************************************************
 @file		s1d13706.h
 @author	Козлячков В.К.
 @date		2012.00
 @version	V1.0
 @brief		
 @details	
 @note		
 *******************************************************************************/

/*! @addtogroup	s1d13706
    @{ */

#ifndef S1D13706_H			// Включить файл только один раз
#define S1D13706_H

/*__ Заголовочные файлы _______________________________________________________*/

#include <stdint.h>

/*__ Глобальные типы данных ___________________________________________________*/

//! Read-Only Configuration Registers
typedef struct {
  __I uint8_t RCR;	//!< Revision Code Register,			0x00
  __I uint8_t DBSR;	//!< Dislpay Buffer Size Register,		0x01
  __I uint8_t CRR;	//!< Configuration Readback Register,		0x02
} ReadOnlyConf_t;

//! Clock Configuration Registers
typedef struct {
  __IO uint8_t MCCR;	//!< Memory Clock Configuration Register,	0x04
  __IO uint8_t PCCR;	//!< Pixel Clock Configuration Register,	0x05
} ClockConf_t;

//! Look-Up Table Registers
typedef struct {
  __O uint8_t LUTBWDR;	//!< Look-Up Table Blue Write Data Register,	0x08
  __O uint8_t LUTGWDR;	//!< Look-Up Table Green Write Data Register,	0x09
  __O uint8_t LUTRWDR;	//!< Look-Up Table Red Write Data Register,	0x0A
  __O uint8_t LUTWAR;	//!< Look-Up Table Write Address Register,	0x0B
  __I uint8_t LUTBRDR;	//!< Look-Up Table Blue Read Data Register,	0x0C
  __I uint8_t LUTGRDR;	//!< Look-Up Table Green Read Data Register,	0x0D
  __I uint8_t LUTRRDR;	//!< Look-Up Table Red Read Data Register,	0x0E
  __O uint8_t LUTRAR;	//!< Look-Up Table Read Address Register,	0x0F
} LookUpTable_t;

//! Panel Configuration Registers
typedef struct { 
  __IO uint8_t PTR;	//!< Panel Type Register,			0x10
  __IO uint8_t MODRR;	//!< MOD Rate Register,				0x11
  __IO uint8_t HTR;	//!< Horizontal Total Register,			0x12
  uint8_t RESERVED13;	//						0x13
  __IO uint8_t HDPR;	//!< Horizontal Display Period Register,	0x14
  uint8_t RESERVED15;	//						0x15
  __IO uint8_t HDPSPR0;	//!< Horiz Display Period Start Position Reg 0,	0x16
  __IO uint8_t HDPSPR1;	//!< Horiz Display Period Start Position Reg 1,	0x17
  __IO uint8_t VTR0;	//!< Vertical Total Register 0,			0x18
  __IO uint8_t VTR1;	//!< Vertical Total Register 1,			0x19
  uint8_t RESERVED1A;	//						0x1A
  uint8_t RESERVED1B;	//						0x1B
  __IO uint8_t VDPR0;	//!< Vertical Display Period Register 0,	0x1C
  __IO uint8_t VDPR1;	//!< Vertical Display Period Register 1,	0x1D
  __IO uint8_t VDPSPR0;	//!< Vert Display Period Start Position Reg 0,	0x1E
  __IO uint8_t VDPSPR1;	//!< Vert Display Period Start Position Reg 1,	0x1F
  __IO uint8_t FPLPWR;	//!< FPLINE Pulse Width Register,		0x20
  uint8_t RESERVED21;	//						0x21
  __IO uint8_t FPLPSPR0; //!< FPLINE Pulse Start Position Register 0,	0x22
  __IO uint8_t FPLPSPR1; //!< FPLINE Pulse Start Position Register 1,	0x23
  __IO uint8_t FPFPWR;	//!< FPFRAME Pulse Width Register,		0x24
  uint8_t RESERVED25;	//						0x25
  __IO uint8_t FPFPSPR0; //!< FPFRAME Pulse Start Position Register 0,	0x26
  __IO uint8_t FPFPSPR1; //!< FPFRAME Pulse Start Position Register 1,	0x27
  __IO uint8_t DTFDGCPIR; //!< D-TFD GCP Index Register,		0x28
  uint8_t RESERVED29;	//						0x29
  uint8_t RESERVED2A;	//						0x2A
  uint8_t RESERVED2B;	//						0x2B
  __IO uint8_t DTFDGCPDR; //!< D-TFD GCP Data Register,			0x2C
} PanelConf_t;

//! Display Mode Registers
typedef struct {
  __IO uint8_t DMR;	//!< Display Mode Register,			0x70
  __IO uint8_t SER;	//!< Special Effects Register			0x71
  uint8_t RESERVED72;	//						0x72
  uint8_t RESERVED73;	//						0x73
  __IO uint8_t MWDSAR0;	//!< Main Window Display Start Address Reg 0,	0x74
  __IO uint8_t MWDSAR1;	//!< Main Window Display Start Address Reg 1,	0x75
  __IO uint8_t MWDSAR2;	//!< Main Window Display Start Address Reg 2,	0x76
  uint8_t RESERVED77;	//						0x77
  __IO uint8_t MWLAOR0;	//!< Main Window Line Address Offset Reg 0,	0x78
  __IO uint8_t MWLAOR1;	//!< Main Window Line Address Offset Reg 1,	0x79
} DisplayMode_t;

//! Picture-in-Picture Plus (PIP+) Registers
typedef struct {
  __IO uint8_t PIPWDSAR0; //!< PIP+ Window Display Start Address Reg 0,	0x7C
  __IO uint8_t PIPWDSAR1; //!< PIP+ Window Display Start Address Reg 1,	0x7D
  __IO uint8_t PIPWDSAR2; //!< PIP+ Window Display Start Address Reg 2,	0x7E
  uint8_t RESERVED7F;	//						0x7F
  __IO uint8_t PIPWLAOR0; //!< PIP+ Window Line Address Offset Reg 0,	0x80
  __IO uint8_t PIPWLAOR1; //!< PIP+ Window Line Address Offset Reg 1,	0x81
  uint8_t RESERVED82;	//						0x82
  uint8_t RESERVED83;	//						0x83
  __IO uint8_t PIPWXSPR0; //!< PIP+ Window X Start Position Register 0,	0x84
  __IO uint8_t PIPWXSPR1; //!< PIP+ Window X Start Position Register 1,	0x85
  uint8_t RESERVED86;	//						0x86
  uint8_t RESERVED87;	//						0x87
  __IO uint8_t PIPWYSPR0; //!< PIP+ Window Y Start Position Register 0,	0x88
  __IO uint8_t PIPWYSPR1; //!< PIP+ Window Y Start Position Register 1,	0x89
  uint8_t RESERVED8A;	//						0x8A
  uint8_t RESERVED8B;	//						0x8B
  __IO uint8_t PIPWXEPR0; //!< PIP+ Window X End Position Register 0,	0x8C
  __IO uint8_t PIPWXEPR1; //!< PIP+ Window X End Position Register 1,	0x8D
  uint8_t RESERVED8E;	//						0x8E
  uint8_t RESERVED8F;	//						0x8F
  __IO uint8_t PIPWYEPR0; //!< PIP+ Window Y End Position Register 0,	0x90
  __IO uint8_t PIPWYEPR1; //!< PIP+ Window Y End Position Register 1,	0x91
} PIP_t;

//! Miscellaneous Registers
typedef struct {
  __IO uint8_t PSCR;	//!< Power Save Configuration Register,		0xA0
  __IO uint8_t RESERVEDA1; //!< Reserved,				0xA1
  __IO uint8_t RESERVEDA2; //!< Reserved,				0xA2
  __IO uint8_t RESERVEDA3; //!< Reserved,				0xA3
  __IO uint8_t SPR0;	//!< Scratch Pad Register 0,			0xA4
  __IO uint8_t SPR1;	//!< Scratch Pad Register 1,			0xA5
} Miscell_t;

//! General Purpose IO Pins Registers
typedef struct {
  __IO uint8_t GPIOPCFR0; //!< General Purpose IO Pins Configuration Reg 0, 0xA8
  __IO uint8_t GPIOPCFR1; //!< General Purpose IO Pins Configuration Reg 1, 0xA9
  uint8_t RESERVEDAA;	//						0xAA
  uint8_t RESERVEDAB;	//						0xAB
  __IO uint8_t GPIOPSCR0; //!< General Purpose IO Pins Status/Control Reg 0, 0xAC
  __IO uint8_t GPIOPSCR1; //!< General Purpose IO Pins Status/Control Reg 1, 0xAD
} GPIOP_t;

//! Pulse Width Modulation Clock and Contrast Voltage Pulse
typedef struct {
  __IO uint8_t PWMCVCTR; //!< PWM Clock / CV Pulse Control Register,	0xB0
  __IO uint8_t PWMCVCFR; //!< PWM Clock / CV Pulse Configuration Reg,	0xB1
  __IO uint8_t CVPBLR;	//!< CV Pulse Burst Length Register,		0xB2
  __IO uint8_t PWMODCR;	//!< PWMOUT Duty Cycle Register,		0xB3
} PWMCVConf_t;


/*__ Глобальные определения констант __________________________________________*/

/* Базовый адрес контроллера ЖКИ в пространстве адресов MCU
   должен быть скорректирован для конкретного применения */
// #define DPYCTRL_BASE	((uint32_t)0x6C000000)	//!< Dysplay base address
#define DPYREGS_BASE	DPYCTRL_BASE		//!< S1D13706 registers base
#define DPYMEM_BASE	(DPYCTRL_BASE + 0x00020000) //!< S1D13706 memory base
#define DPYMEM_SIZE	((uint32_t)(80 * 1024))	//!< S1D13706 memory size 80KB
// #define DPYMEM_SIZE	81920

#define READONLYCONF_BASE	(DPYREGS_BASE)
#define CLOCKCONF_BASE		(DPYREGS_BASE + 0x04)
#define LOOKUPTABLE_BASE	(DPYREGS_BASE + 0x08)
#define PANELCONF_BASE		(DPYREGS_BASE + 0x10)
#define DISPLAYMODE_BASE	(DPYREGS_BASE + 0x70)
#define PIP_BASE		(DPYREGS_BASE + 0x7C)
#define MISCELL_BASE		(DPYREGS_BASE + 0xA0)
#define GPIOP_BASE		(DPYREGS_BASE + 0xA8)
#define PWMCVCONF_BASE		(DPYREGS_BASE + 0xB0)

#define READONLYCONF	((ReadOnlyConf_t *) READONLYCONF_BASE)
#define CLOCKCONF	((ClockConf_t *) CLOCKCONF_BASE)
#define LOOKUPTABLE	((LookUpTable_t *) LOOKUPTABLE_BASE)
#define PANELCONF	((PanelConf_t *) PANELCONF_BASE)
#define DISPLAYMODE	((DisplayMode_t *) DISPLAYMODE_BASE)
#define PIP		((PIP_t *) PIP_BASE)
#define MISCELL		((Miscell_t *) MISCELL_BASE)
#define GPIOP		((GPIOP_t *) GPIOP_BASE)
#define PWMCVCONF	((PWMCVConf_t *) PWMCVCONF_BASE)

#endif					// S1D13706_H

/*! @} */
