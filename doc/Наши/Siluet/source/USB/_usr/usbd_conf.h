/**
  ******************************************************************************
  * @file    usbd_conf.h
  * @author  MCD Application Team
  * @version V1.1.0
  * @date    19-March-2012
  * @brief   USB Device configuration file
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2012 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software 
  * distributed under the License is distributed on an "AS IS" BASIS, 
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __USBD_CONF__H__
#define __USBD_CONF__H__

/* Includes ------------------------------------------------------------------*/
 #include "usb_conf.h"
 #include "stdbool.h"
//#include "..\..\Siluet_main.h"			// bool, true, false


/**  USBD_DESC_Private_Defines	**/

#define USBD_VID                        0x0483

#define USBD_PID                        0x5740

/**  USB_String_Descriptors		**/

#define USBD_LANGID_STRING              0x409
#define USBD_MANUFACTURER_STRING        "MNIPI"

#define USBD_PRODUCT_HS_STRING          "C8-52 Oscilloscope"
#define USBD_SERIALNUMBER_HS_STRING     "00000000050B"

#define USBD_PRODUCT_FS_STRING          "C8-52 Oscilloscope"
#define USBD_SERIALNUMBER_FS_STRING     "00000000050C"

#define USBD_CONFIGURATION_HS_STRING    "VCP Config"
#define USBD_INTERFACE_HS_STRING        "VCP Interface"

#define USBD_CONFIGURATION_FS_STRING    "VCP Config"
#define USBD_INTERFACE_FS_STRING        "VCP Interface"






#define USBD_CFG_MAX_NUM                1
#define USBD_ITF_MAX_NUM                1

#define USBD_SELF_POWERED               

#define USB_MAX_STR_DESC_SIZ            255 

/** @defgroup USB_VCP_Class_Layer_Parameter
  * @{
  */ 
#define CDC_IN_EP                       0x81  /* EP1 for data IN */
#define CDC_OUT_EP                      0x01  /* EP1 for data OUT */
#define CDC_CMD_EP                      0x82  /* EP2 for CDC commands */

/* CDC Endpoints parameters: you can fine tune these values depending on the needed baudrates and performance. */
// // #ifdef USE_USB_OTG_HS
// //  #define CDC_DATA_MAX_PACKET_SIZE       512  /* Endpoint IN & OUT Packet size */
// //  #define CDC_CMD_PACKET_SZE             8    /* Control Endpoint Packet size */

// //  #define CDC_IN_FRAME_INTERVAL          40   /* Number of micro-frames between IN transfers */
// //  #define APP_RX_DATA_SIZE               2048 /* Total size of IN buffer: 
// //                                                 APP_RX_DATA_SIZE*8/MAX_BAUDARATE*1000 should be > CDC_IN_FRAME_INTERVAL*8 */
// // #else
 #define CDC_DATA_MAX_PACKET_SIZE       64   /* Endpoint IN & OUT Packet size */		// will we see 63-64 bits data lenth problem??  
 #define CDC_CMD_PACKET_SZE             8    /* Control Endpoint Packet size */

 #define CDC_IN_FRAME_INTERVAL          5    /* Number of frames between IN transfers */
 //#define APP_RX_DATA_SIZE               2048 /* Total size of IN buffer: 
 //                                                APP_RX_DATA_SIZE*8/MAX_BAUDARATE*1000 should be > CDC_IN_FRAME_INTERVAL */
//#define USB_TX_DATA_SIZE               2048 /* Total size of IN buffer:	*/

// // #endif /* USE_USB_OTG_HS */

//#define APP_FOPS                        VCP_fops			// media acces callbacls struct		//no need


			//typedefs for buffers temporary placed here 


typedef struct	{
			bool 			Fresh;										// if TRUE - data is not processed yet
			uint32_t	Length;										// number of recieved databytes
			uint8_t 	Data[CDC_DATA_MAX_PACKET_SIZE];		// Buffer
} USB_Rx_Buffer_TypeDef;



typedef struct {
		bool			TxState;			//	Transfering status;
		bool 			Ready;				//	New Data ready flag;
		uint8_t 	*pData;				//	Buffer addres;
		uint16_t	Index;				//	Current data Index positon;
		uint16_t 	Length;				//	Total Data length;
		uint16_t	PcktLength;		// 	Data Packet Length;
} USB_Tx_Buffer_TypeDef;



#endif //__USBD_CONF__H__

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

