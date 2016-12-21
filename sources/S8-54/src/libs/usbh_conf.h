/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __USBH_CONF_H
#define __USBH_CONF_H

#include <stdlib.h>
#include <string.h>


/* Exported types ------------------------------------------------------------*/
#define USBH_MAX_NUM_ENDPOINTS                2
#define USBH_MAX_NUM_INTERFACES               2
#define USBH_MAX_NUM_CONFIGURATION            1
#define USBH_MAX_NUM_SUPPORTED_CLASS          1
#define USBH_KEEP_CFG_DESCRIPTOR              0
#define USBH_MAX_SIZE_CONFIGURATION           0x200
#define USBH_MAX_DATA_BUFFER                  0x200
#define USBH_DEBUG_LEVEL                      0
#define USBH_USE_OS                           0

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx.h"
    
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* CMSIS OS macros */ 
#if (USBH_USE_OS == 1)
  #include "cmsis_os.h"
  #define   USBH_PROCESS_PRIO    osPriorityNormal
#endif    

/* Memory management macros */    
#define USBH_malloc               malloc
#define USBH_free                 free
#define USBH_memset               memset
#define USBH_memcpy               memcpy


#if (USBH_DEBUG_LEVEL > 0)
#define USBH_UsrLog(...)  Log_Write(__VA_ARGS__)
#define USBH_ErrLog(...)  Log_Write(__VA_ARGS__)
#define USBH_DbgLog(...)  Log_Write(__VA_ARGS__)
#else
#define USBH_UsrLog(...) {}
#define USBH_ErrLog(...) {}
#define USBH_DbgLog(...) {}
#endif

/* Exported functions ------------------------------------------------------- */

#endif /* __USB_CONF_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

