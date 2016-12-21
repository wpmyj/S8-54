/**
  ******************************************************************************
  * @file    usb_add_module.h
  * @author  
  * @version V1.1.0
  * @date    06-June-2013
  * @brief   header for addiction USB CDC module to main
  ******************************************************************************
  * 
  *
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __USB_ADD_MODULE__H__
#define __USB_ADD_MODULE__H__

// config defines
//USE_STDPERIPH_DRIVER, STM32F2XX, USE_USB_OTG_FS
//USE_STDPERIPH_DRIVER, STM32F2XX, USE_USB_OTG_HS, USE_EMBEDDED_PHY


//#define	USE_STDPERIPH_DRIVER
// #define	STM32F2XX
// #define	USE_USB_OTG_FS

 #include "usbd_cdc_core.h"
 #include "usbd_usr.h"
 #include "usb_conf.h"
 #include "usbd_desc.h"

// global struct with device type
// #if !defined(USB_OTG_dev)
// 	USB_OTG_CORE_HANDLE    USB_OTG_dev;
// #endif



/* Includes ------------------------------------------------------------------*/



#endif //__USB_ADD_MODULE__H__



/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

