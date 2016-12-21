/**
  ******************************************************************************
  * @file    usbd_usr.c
  * @author  MCD Application Team
  * @version V1.1.0
  * @date    19-March-2012
  * @brief   This file includes the user application layer
  ******************************************************************************
  * @note
  *
  *					Implicates device reaction on diffrent USB-line events, like 	 
	*					Connection, Disconnection, Reset, Resume, Suspend, Configure, 
	*					Initiation etc.
	*					It is template. If no functionality need, just put it as is.
  ******************************************************************************
  */ 

/* Includes ------------------------------------------------------------------*/
//#include	"usb_common_module.h"

 #include "usbd_usr.h"
 #include "usbd_ioreq.h"

/**  USBD_USR_Private_Variables	**/

USBD_Usr_cb_TypeDef USR_cb =
{
  USBD_USR_Init,
  USBD_USR_DeviceReset,
  USBD_USR_DeviceConfigured,
  USBD_USR_DeviceSuspended,
  USBD_USR_DeviceResumed,
  
  
  USBD_USR_DeviceConnected,
  USBD_USR_DeviceDisconnected,    
};



/********************************************************************************
* @brief  USBD_USR_Init 
*         Displays the message on LCD for host lib initialization
* @param  None
* @retval None
********************************************************************************/
void USBD_USR_Init(void)	{  

}



/********************************************************************************
* @brief  USBD_USR_DeviceReset 
*         Reset event placeholder
* @param  speed : device speed
* @retval None
********************************************************************************/
void USBD_USR_DeviceReset(uint8_t speed )	{								// one more pack of messages

}



/********************************************************************************
* @brief  USBD_USR_DeviceConfigured
*         Device configuration Event Placeholder
* @param  None
* @retval Staus
********************************************************************************/
void USBD_USR_DeviceConfigured (void)	{

}



/********************************************************************************
* @brief  USBD_USR_DeviceSuspended 
*         Device suspend Event Placeholder
* @param  None
* @retval None
********************************************************************************/
void USBD_USR_DeviceSuspended(void)		{

}



/********************************************************************************
* @brief  USBD_USR_DeviceResumed 
*         Device resume Event Placeholder
* @param  None
* @retval None
********************************************************************************/
void USBD_USR_DeviceResumed(void)	{

}



/********************************************************************************
* @brief  USBD_USR_DeviceConnected
*         Device connection Event Placeholder
* @param  None
* @retval Staus
********************************************************************************/
void USBD_USR_DeviceConnected (void)	{
		// for example, here can be realized message like "scope connected to PC"
}



/********************************************************************************
* @brief  USBD_USR_DeviceDisonnected
*         Device disconnection Event Placeholder
* @param  None
* @retval Staus
********************************************************************************/
void USBD_USR_DeviceDisconnected (void)	{
		// for example, here can be realized message like "scope disconnected from PC"
}




/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
