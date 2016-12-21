/**
  ******************************************************************************
  * @file    usbd_cdc_core.c
  * @author  MCD Application Team
  * @version V1.1.0
  * @date    19-March-2012
  * @brief   This file provides the high layer firmware functions to manage the 
  *          following functionalities of the USB CDC Class:
  *           - Initialization and Configuration of high and low layer
  *           - Enumeration as CDC Device (and enumeration for each implemented memory interface)
  *           - OUT/IN data transfer
  *           - Command IN transfer (class requests management)
  *           - Error management
  *           
  *  @verbatim
  *      
  *          ===================================================================      
  *                                CDC Class Driver Description
  *          =================================================================== 
  *           This driver manages the "Universal Serial Bus Class Definitions for Communications Devices
  *           Revision 1.2 November 16, 2007" and the sub-protocol specification of "Universal Serial Bus 
  *           Communications Class Subclass Specification for PSTN Devices Revision 1.2 February 9, 2007"
  *           This driver implements the following aspects of the specification:
  *             - Device descriptor management
  *             - Configuration descriptor management
  *             - Enumeration as CDC device with 2 data endpoints (IN and OUT) and 1 command endpoint (IN)
  *             - Requests management (as described in section 6.2 in specification)
  *             - Abstract Control Model compliant
  *             - Union Functional collection (using 1 IN endpoint for control)
  *             - Data interface class

  *           @note
  *             For the Abstract Control Model, this core allows only transmitting the requests to
  *             lower layer dispatcher (ie. usbd_cdc_vcp.c/.h) which should manage each request and
  *             perform relative actions.
  * 
  *           These aspects may be enriched or modified for a specific user application.
  *          
  *            This driver doesn't implement the following aspects of the specification 
  *            (but it is possible to manage these features with some modifications on this driver):
  *             - Any class-specific aspect relative to communication classes should be managed by user application.
  *             - All communication classes other than PSTN are not managed
  *      
  *  @endverbatim
  *                                  
  ******************************************************************************
  ******************************************************************************
  */ 

/* Includes ------------------------------------------------------------------*/
#include "usbd_cdc_core.h"
#include "usbd_desc.h"
#include "usbd_req.h"

#include "../../_SCPI/scpi_include.h"
#include "../Siluet_Main.h"

/*********************************************
   CDC Device library callbacks
 *********************************************/
static uint8_t  usbd_cdc_Init         (void  *pdev, uint8_t cfgidx);
static uint8_t  usbd_cdc_DeInit       (void  *pdev, uint8_t cfgidx);
static uint8_t  usbd_cdc_Setup        (void  *pdev, USB_SETUP_REQ *req);
static uint8_t  usbd_cdc_EP0_RxReady  (void *pdev);
static uint8_t  usbd_cdc_DataIn       (void *pdev, uint8_t epnum);
static uint8_t  usbd_cdc_DataOut      (void *pdev, uint8_t epnum);
static uint8_t  usbd_cdc_SOF          (void *pdev);

/*********************************************
   CDC specific management functions
 *********************************************/
static void Handle_USBAsynchXfer  (void *pdev);
static uint8_t  *USBD_cdc_GetCfgDesc (uint8_t speed, uint16_t *length);
#ifdef USE_USB_OTG_HS  
static uint8_t  *USBD_cdc_GetOtherCfgDesc (uint8_t speed, uint16_t *length);
#endif

/**  usbd_cdc_Private_Variables	  */ 

//extern CDC_IF_Prop_TypeDef  APP_FOPS;												//!!! do not forget this definition: #define APP_FOPS  VCP_fops
extern uint8_t USBD_DeviceDesc   [USB_SIZ_DEVICE_DESC];
extern osThreadId	SCPI_id;

			// Rx Tx Buffers structs declarations (global)
USB_Rx_Buffer_TypeDef 				USB_Rx_Buffer;
USB_Tx_Buffer_TypeDef 				USB_Tx_Buffer;

#ifdef USE_RING_BUFFER
Ring_Buffer_TypeDef RingRxBuffer;
#endif	/*	USE_RING_BUFFER	*/

 uint8_t usbd_cdc_CfgDesc  [USB_CDC_CONFIG_DESC_SIZ] ;
 uint8_t usbd_cdc_OtherCfgDesc  [USB_CDC_CONFIG_DESC_SIZ] ;
 static __IO uint32_t  usbd_cdc_AltSet = 0;

uint8_t CmdBuff[CDC_CMD_PACKET_SZE] ;

static uint32_t cdcCmd = 0xFF;
//static uint32_t cdcLen = 0;

/* CDC interface class callbacks structure */
USBD_Class_cb_TypeDef  USBD_CDC_cb = 
{
  usbd_cdc_Init,
  usbd_cdc_DeInit,
  usbd_cdc_Setup,
  NULL,                 /* EP0_TxSent, */
  usbd_cdc_EP0_RxReady,
  usbd_cdc_DataIn,
  usbd_cdc_DataOut,
  usbd_cdc_SOF,
  NULL,
  NULL,     
  USBD_cdc_GetCfgDesc,
#ifdef USE_USB_OTG_HS   
  USBD_cdc_GetOtherCfgDesc, /* use same cobfig as per FS */
#endif /* USE_USB_OTG_HS  */
};

/* USB CDC device Configuration Descriptor */
//__ALIGN_BEGIN uint8_t usbd_cdc_CfgDesc[USB_CDC_CONFIG_DESC_SIZ]  __ALIGN_END =
uint8_t usbd_cdc_CfgDesc[USB_CDC_CONFIG_DESC_SIZ] = 
{
  /*Configuration Descriptor*/
  0x09,   /* bLength: Configuration Descriptor size */
  USB_CONFIGURATION_DESCRIPTOR_TYPE,      /* bDescriptorType: Configuration */
  USB_CDC_CONFIG_DESC_SIZ,                /* wTotalLength:no of returned bytes */
  0x00,
  0x02,   /* bNumInterfaces: 2 interface */
  0x01,   /* bConfigurationValue: Configuration value */
  0x00,   /* iConfiguration: Index of string descriptor describing the configuration */
  0xC0,   /* bmAttributes: self powered */
  0x32,   /* MaxPower 0 mA */
  
  /*---------------------------------------------------------------------------*/
  
  /*Interface Descriptor */
  0x09,   /* bLength: Interface Descriptor size */
  USB_INTERFACE_DESCRIPTOR_TYPE,  /* bDescriptorType: Interface */
  /* Interface descriptor type */
  0x00,   /* bInterfaceNumber: Number of Interface */
  0x00,   /* bAlternateSetting: Alternate setting */
  0x01,   /* bNumEndpoints: One endpoints used */
  0x02,   /* bInterfaceClass: Communication Interface Class */
  0x02,   /* bInterfaceSubClass: Abstract Control Model */
  0x01,   /* bInterfaceProtocol: Common AT commands */
  0x00,   /* iInterface: */
  
  /*Header Functional Descriptor*/
  0x05,   /* bLength: Endpoint Descriptor size */
  0x24,   /* bDescriptorType: CS_INTERFACE */
  0x00,   /* bDescriptorSubtype: Header Func Desc */
  0x10,   /* bcdCDC: spec release number */
  0x01,
  
  /*Call Management Functional Descriptor*/
  0x05,   /* bFunctionLength */
  0x24,   /* bDescriptorType: CS_INTERFACE */
  0x01,   /* bDescriptorSubtype: Call Management Func Desc */
  0x00,   /* bmCapabilities: D0+D1 */
  0x01,   /* bDataInterface: 1 */
  
  /*ACM Functional Descriptor*/
  0x04,   /* bFunctionLength */
  0x24,   /* bDescriptorType: CS_INTERFACE */
  0x02,   /* bDescriptorSubtype: Abstract Control Management desc */
  0x02,   /* bmCapabilities */
  
  /*Union Functional Descriptor*/
  0x05,   /* bFunctionLength */
  0x24,   /* bDescriptorType: CS_INTERFACE */
  0x06,   /* bDescriptorSubtype: Union func desc */
  0x00,   /* bMasterInterface: Communication class interface */
  0x01,   /* bSlaveInterface0: Data Class Interface */
  
  /*Endpoint 2 Descriptor*/
  0x07,                           /* bLength: Endpoint Descriptor size */
  USB_ENDPOINT_DESCRIPTOR_TYPE,   /* bDescriptorType: Endpoint */
  CDC_CMD_EP,                     /* bEndpointAddress */
  0x03,                           /* bmAttributes: Interrupt */
  LOBYTE(CDC_CMD_PACKET_SZE),     /* wMaxPacketSize: */
  HIBYTE(CDC_CMD_PACKET_SZE),

  0xFF,                           /* bInterval: */
  
  /*---------------------------------------------------------------------------*/
  
  /*Data class interface descriptor*/
  0x09,   /* bLength: Endpoint Descriptor size */
  USB_INTERFACE_DESCRIPTOR_TYPE,  /* bDescriptorType: */
  0x01,   /* bInterfaceNumber: Number of Interface */
  0x00,   /* bAlternateSetting: Alternate setting */
  0x02,   /* bNumEndpoints: Two endpoints used */
  0x0A,   /* bInterfaceClass: CDC */
  0x00,   /* bInterfaceSubClass: */
  0x00,   /* bInterfaceProtocol: */
  0x00,   /* iInterface: */
  
  /*Endpoint OUT Descriptor*/
  0x07,   /* bLength: Endpoint Descriptor size */
  USB_ENDPOINT_DESCRIPTOR_TYPE,      /* bDescriptorType: Endpoint */
  CDC_OUT_EP,                        /* bEndpointAddress */
  0x02,                              /* bmAttributes: Bulk */
  LOBYTE(CDC_DATA_MAX_PACKET_SIZE),  /* wMaxPacketSize: */
  HIBYTE(CDC_DATA_MAX_PACKET_SIZE),
  0x00,                              /* bInterval: ignore for Bulk transfer */
  
  /*Endpoint IN Descriptor*/
  0x07,   /* bLength: Endpoint Descriptor size */
  USB_ENDPOINT_DESCRIPTOR_TYPE,      /* bDescriptorType: Endpoint */
  CDC_IN_EP,                         /* bEndpointAddress */
  0x02,                              /* bmAttributes: Bulk */
  LOBYTE(CDC_DATA_MAX_PACKET_SIZE),  /* wMaxPacketSize: */
  HIBYTE(CDC_DATA_MAX_PACKET_SIZE),
  0x00                               /* bInterval: ignore for Bulk transfer */
} ;


#ifdef USE_USB_OTG_HS
//__ALIGN_BEGIN uint8_t usbd_cdc_OtherCfgDesc[USB_CDC_CONFIG_DESC_SIZ]  __ALIGN_END =
uint8_t usbd_cdc_OtherCfgDesc[USB_CDC_CONFIG_DESC_SIZ]	=
{ 
  0x09,   /* bLength: Configuation Descriptor size */
  USB_DESC_TYPE_OTHER_SPEED_CONFIGURATION,   
  USB_CDC_CONFIG_DESC_SIZ,
  0x00,
  0x02,   /* bNumInterfaces: 2 interfaces */
  0x01,   /* bConfigurationValue: */
  0x04,   /* iConfiguration: */
  0xC0,   /* bmAttributes: */
  0x32,   /* MaxPower 100 mA */  
  
  /*Interface Descriptor */
  0x09,   /* bLength: Interface Descriptor size */
  USB_INTERFACE_DESCRIPTOR_TYPE,  /* bDescriptorType: Interface */
  /* Interface descriptor type */
  0x00,   /* bInterfaceNumber: Number of Interface */
  0x00,   /* bAlternateSetting: Alternate setting */
  0x01,   /* bNumEndpoints: One endpoints used */
  0x02,   /* bInterfaceClass: Communication Interface Class */
  0x02,   /* bInterfaceSubClass: Abstract Control Model */
  0x01,   /* bInterfaceProtocol: Common AT commands */
  0x00,   /* iInterface: */
  
  /*Header Functional Descriptor*/
  0x05,   /* bLength: Endpoint Descriptor size */
  0x24,   /* bDescriptorType: CS_INTERFACE */
  0x00,   /* bDescriptorSubtype: Header Func Desc */
  0x10,   /* bcdCDC: spec release number */
  0x01,
  
  /*Call Management Functional Descriptor*/
  0x05,   /* bFunctionLength */
  0x24,   /* bDescriptorType: CS_INTERFACE */
  0x01,   /* bDescriptorSubtype: Call Management Func Desc */
  0x00,   /* bmCapabilities: D0+D1 */
  0x01,   /* bDataInterface: 1 */
  
  /*ACM Functional Descriptor*/
  0x04,   /* bFunctionLength */
  0x24,   /* bDescriptorType: CS_INTERFACE */
  0x02,   /* bDescriptorSubtype: Abstract Control Management desc */
  0x02,   /* bmCapabilities */
  
  /*Union Functional Descriptor*/
  0x05,   /* bFunctionLength */
  0x24,   /* bDescriptorType: CS_INTERFACE */
  0x06,   /* bDescriptorSubtype: Union func desc */
  0x00,   /* bMasterInterface: Communication class interface */
  0x01,   /* bSlaveInterface0: Data Class Interface */
  
  /*Endpoint 2 Descriptor*/
  0x07,                           /* bLength: Endpoint Descriptor size */
  USB_ENDPOINT_DESCRIPTOR_TYPE,   /* bDescriptorType: Endpoint */
  CDC_CMD_EP,                     /* bEndpointAddress */
  0x03,                           /* bmAttributes: Interrupt */
  LOBYTE(CDC_CMD_PACKET_SZE),     /* wMaxPacketSize: */
  HIBYTE(CDC_CMD_PACKET_SZE),
  0xFF,                           /* bInterval: */
  
  /*---------------------------------------------------------------------------*/
  
  /*Data class interface descriptor*/
  0x09,   /* bLength: Endpoint Descriptor size */
  USB_INTERFACE_DESCRIPTOR_TYPE,  /* bDescriptorType: */
  0x01,   /* bInterfaceNumber: Number of Interface */
  0x00,   /* bAlternateSetting: Alternate setting */
  0x02,   /* bNumEndpoints: Two endpoints used */
  0x0A,   /* bInterfaceClass: CDC */
  0x00,   /* bInterfaceSubClass: */
  0x00,   /* bInterfaceProtocol: */
  0x00,   /* iInterface: */
  
  /*Endpoint OUT Descriptor*/
  0x07,   /* bLength: Endpoint Descriptor size */
  USB_ENDPOINT_DESCRIPTOR_TYPE,      /* bDescriptorType: Endpoint */
  CDC_OUT_EP,                        /* bEndpointAddress */
  0x02,                              /* bmAttributes: Bulk */
  0x40,                              /* wMaxPacketSize: */
  0x00,
  0x00,                              /* bInterval: ignore for Bulk transfer */
  
  /*Endpoint IN Descriptor*/
  0x07,   /* bLength: Endpoint Descriptor size */
  USB_ENDPOINT_DESCRIPTOR_TYPE,     /* bDescriptorType: Endpoint */
  CDC_IN_EP,                        /* bEndpointAddress */
  0x02,                             /* bmAttributes: Bulk */
  0x40,                             /* wMaxPacketSize: */
  0x00,
  0x00                              /* bInterval */
};
#endif /* USE_USB_OTG_HS  */





			/********************************************/
			/** 				usbd_cdc_Private_Functions 		 	*/ 
			/********************************************/



/********************************************************************************
  * @brief  usbd_cdc_Init
  *         Initilaize the CDC interface
  * @param  pdev: device instance
  * @param  cfgidx: Configuration index
  * @retval status
	* @note 	calls on cable connection, init EPs, organize enumeration process
  *******************************************************************************/
static uint8_t  usbd_cdc_Init (void  *pdev,	uint8_t cfgidx)	{
  
	uint8_t *pbuf;

  /* Open all EPs  */
  DCD_EP_Open(pdev, CDC_IN_EP, CDC_DATA_IN_PACKET_SIZE, USB_OTG_EP_BULK);
  DCD_EP_Open(pdev, CDC_OUT_EP, CDC_DATA_OUT_PACKET_SIZE, USB_OTG_EP_BULK);
  DCD_EP_Open(pdev, CDC_CMD_EP, CDC_CMD_PACKET_SZE, USB_OTG_EP_INT);
  
  pbuf = (uint8_t *)USBD_DeviceDesc;
  pbuf[4] = DEVICE_CLASS_CDC;
  pbuf[5] = DEVICE_SUBCLASS_CDC;
  
  /* Initialize the Interface physical components */
  //APP_FOPS.pIf_Init();		// no need at all anymore
	// Maybe here will be initialization of some datastructs

  /* Prepare Out endpoint to receive next packet */
  //DCD_EP_PrepareRx(pdev, CDC_OUT_EP, (uint8_t*)(USB_Rx_Buffer), CDC_DATA_OUT_PACKET_SIZE);
  DCD_EP_PrepareRx(pdev, CDC_OUT_EP, (uint8_t*)(&USB_Rx_Buffer.Data), CDC_DATA_OUT_PACKET_SIZE);
	
  return USBD_OK;
}


/********************************************************************************
  * @brief  usbd_cdc_Init
  *         DeInitialize the CDC layer
  * @param  pdev: device instance
  * @param  cfgidx: Configuration index
  * @retval status
 ********************************************************************************/
static uint8_t  usbd_cdc_DeInit (void  *pdev, uint8_t cfgidx)	{
 
	/* Close all EPs  */
  DCD_EP_Close(pdev, CDC_IN_EP);
  DCD_EP_Close(pdev, CDC_OUT_EP);
  DCD_EP_Close(pdev, CDC_CMD_EP);

  /* Restore default state of the Interface physical components */
  //APP_FOPS.pIf_DeInit();			// deactivate hardvare USART
  
  return USBD_OK;
}


/********************************************************************************
  * @brief  usbd_cdc_Setup
  *         Handle the CDC specific requests
  * @param  pdev: instance
  * @param  req: usb requests
  * @retval status
	* @note connect-disconnect terminal handled here
********************************************************************************/		
static uint8_t  usbd_cdc_Setup (void  *pdev, USB_SETUP_REQ *req)	{
  uint16_t len=USB_CDC_DESC_SIZ;
  uint8_t  *pbuf=usbd_cdc_CfgDesc + 9;
  
  switch (req->bmRequest & USB_REQ_TYPE_MASK)	{
    /* CDC Class Requests -------------------------------*/
  case USB_REQ_TYPE_CLASS :
      /* Check if the request is a data setup packet */
      if (req->wLength)	{
        /* Check if the request is Device-to-Host */
        if (req->bmRequest & 0x80)	{
          /* Get the data to be sent to Host from interface layer */
//          APP_FOPS.pIf_Ctrl(req->bRequest, CmdBuff, req->wLength);			// controll - state & linecoding
          
          /* Send the data to the host */
          USBD_CtlSendData (pdev,	CmdBuff, req->wLength);          
        }
        else	{ /* Host-to-Device requeset */	
          /* Set the value of the current command to be processed */
          cdcCmd = req->bRequest;
//          cdcLen = req->wLength;
          
          /* Prepare the reception of the buffer over EP0
          Next step: the received data will be managed in usbd_cdc_EP0_TxSent() 
          function. */
          USBD_CtlPrepareRx (pdev, CmdBuff, req->wLength);          
        }
      }
      else	{ /* No Data request */
        /* Transfer the command to the interface layer */
//        APP_FOPS.pIf_Ctrl(req->bRequest, NULL, 0);											// controll - state & linecoding
      }
      
      return USBD_OK;
      
    default:
      USBD_CtlError (pdev, req);
			return USBD_FAIL;
   
      
    /* Standard Requests -------------------------------*/
  case USB_REQ_TYPE_STANDARD:
    switch (req->bRequest)	{
    case USB_REQ_GET_DESCRIPTOR: 
      if( (req->wValue >> 8) == CDC_DESCRIPTOR_TYPE)	{
        pbuf = usbd_cdc_CfgDesc + 9 + (9 * USBD_ITF_MAX_NUM);
        len = MIN(USB_CDC_DESC_SIZ , req->wLength);
      }
      
      USBD_CtlSendData (pdev,	pbuf, len);
      break;
      
    case USB_REQ_GET_INTERFACE :
      USBD_CtlSendData (pdev, (uint8_t *)&usbd_cdc_AltSet, 1);
      break;
      
    case USB_REQ_SET_INTERFACE :
      if ((uint8_t)(req->wValue) < USBD_ITF_MAX_NUM)	{
        usbd_cdc_AltSet = (uint8_t)(req->wValue);
      }
      else {
        /* Call the error management function (command will be nacked */
        USBD_CtlError (pdev, req);
      }
      break;
    }
  }
  return USBD_OK;
}


/********************************************************************************
  * @brief  usbd_cdc_EP0_RxReady
  *         Data received on control endpoint
  * @param  pdev: device device instance
  * @retval status
 ********************************************************************************/
static uint8_t  usbd_cdc_EP0_RxReady (void  *pdev)	{ 
  if (cdcCmd != NO_CMD)	{
    /* Process the data */
//    APP_FOPS.pIf_Ctrl(cdcCmd, CmdBuff, cdcLen);
    
    /* Reset the command variable to default value */
    cdcCmd = NO_CMD;
  }
  
  return USBD_OK;
}

/********************************************************************************
  * @brief  usbd_cdc_DataOut
  *         Data received on non-control Out endpoint
  * @param  pdev: device instance
  * @param  epnum: endpoint number
  * @retval status
********************************************************************************/
static uint8_t  usbd_cdc_DataOut (void *pdev, uint8_t epnum) {      
  uint32_t i;
  /* Get the received data buffer and update the counter */
  USB_Rx_Buffer.Length = ((USB_OTG_CORE_HANDLE*)pdev)->dev.out_ep[epnum].xfer_count;						// num of recieved data
  USB_Rx_Buffer.Fresh = true;																																		// mark data as fresh recieved
  
	/* USB data will be immediately processed, this allow next USB traffic being 
     NAKed till the end of the application Xfer */

	
#ifdef	USE_RING_BUFFER	
//	CopyRxBufferToRing(&USB_Rx_Buffer, &RingRxBuffer);
//	SCPI_ParseOneCommandFromBuffer( &RingRxBuffer);
	
	if(USB_Rx_Buffer.Data[USB_Rx_Buffer.Length])	USB_Rx_Buffer.Data[USB_Rx_Buffer.Length] = NULL;	// Check for NULL terminator at the end of string
	
	for(i = 0; i < USB_Rx_Buffer.Length; i++)	RING_BUFF_WRITE(RingRxBuffer, USB_Rx_Buffer.Data[i]);	
	RingRxBuffer.source = USB;
	
	osSignalSet(SCPI_id, 2);
#else
	SCPI_ParseOneCommandFromBuffer(USB_Rx_Buffer.Data);
#endif /*	USE_RING_BUFFER	*/
	
	/* Prepare Out endpoint to receive next packet */
	DCD_EP_PrepareRx(pdev, CDC_OUT_EP, (uint8_t*)(USB_Rx_Buffer.Data), CDC_DATA_OUT_PACKET_SIZE);

  return USBD_OK;
}


/********************************************************************************
  * @brief  usbd_audio_DataIn
  *         Data sent on non-control IN endpoint
  * @param  pdev: device instance
  * @param  epnum: endpoint number
  * @retval status
 ********************************************************************************/
static uint8_t  usbd_cdc_DataIn (void *pdev, uint8_t epnum)	{
  if (true == USB_Tx_Buffer.TxState)	{										// execute algorutn
    if (0 == USB_Tx_Buffer.Length)	{											// nothing to transfer
      USB_Tx_Buffer.TxState = false;
			return USBD_OK;			
    }

		if (USB_Tx_Buffer.Length > TX1_FIFO_HS_SIZE)	{
			USB_Tx_Buffer.Length -= TX1_FIFO_HS_SIZE;
			USB_Tx_Buffer.PcktLength = TX1_FIFO_HS_SIZE;
		}
		else	{
			USB_Tx_Buffer.PcktLength = USB_Tx_Buffer.Length;
			USB_Tx_Buffer.Length = 0;
			
			USB_Tx_Buffer.TxState = false;
		}
		
		/* Prepare the available data buffer to be sent on IN endpoint */
		DCD_EP_Tx (pdev, CDC_IN_EP, (uint8_t*)&USB_Tx_Buffer.pData[USB_Tx_Buffer.Index], USB_Tx_Buffer.PcktLength);
		
		USB_Tx_Buffer.Index += USB_Tx_Buffer.PcktLength;
    
  }  
  
  return USBD_OK;
}

/********************************************************************************
  * @brief  usbd_cdc_SOF											// LOL, here was a mark of audio device
  *         Start Of Frame event management
  * @param  pdev: instance
  * @param  epnum: endpoint number
  * @retval status
	* @brief	counting frames & 
********************************************************************************/
static uint8_t  usbd_cdc_SOF (void *pdev)	{      
  static uint32_t FrameCount = 0;
  
  if (FrameCount++ == CDC_IN_FRAME_INTERVAL)	{
    /* Reset the frame counter */
    FrameCount = 0;
    
    /* Check the data to be sent through IN pipe */
		if((false == USB_Tx_Buffer.TxState) && (true == USB_Tx_Buffer.Ready))	{
			Handle_USBAsynchXfer(pdev);
		}
	}
  
  return USBD_OK;
}

/********************************************************************************
  * @brief  Handle_USBAsynchXfer
  *         Send data to USB
  * @param  pdev: instance
  * @retval None				// TX1_FIFO_HS_SIZE				// CDC_DATA_IN_PACKET_SIZE
********************************************************************************/
static void Handle_USBAsynchXfer (void *pdev)	{

// struct USB_Tx_Buffer:	
// 		bool 			Transfer;			//	Data transfer permission
// 		bool			TxState;			//	strange shit (transfer status)	-used
// 		bool 			Ready;				//	Data ready flag									-used
// 		uint8_t 	*pData;				//	Buffer addres										-used
// 		uint16_t	Index;				//	Current data Index positon			-used
// 		uint16_t 	Length;				//	Total Data length								-used
// 		uint16_t	PcktLength;		// 	Data Packet Length  						-used
	if(true == USB_Tx_Buffer.Ready)	{
			USB_Tx_Buffer.Index = 0;
			
			if(0 == USB_Tx_Buffer.Length)	{				// here is nothing to transfer
				USB_Tx_Buffer.Ready = false;
				return;															// return
			}
			
			if(USB_Tx_Buffer.Length > TX1_FIFO_HS_SIZE)	{
				USB_Tx_Buffer.Length -= TX1_FIFO_HS_SIZE;				// calc rest of data
				USB_Tx_Buffer.PcktLength = TX1_FIFO_HS_SIZE;
				USB_Tx_Buffer.TxState = true;														// set Tx active
			}
			else	{
				USB_Tx_Buffer.PcktLength = USB_Tx_Buffer.Length;				// calculate packet length if data length less, then TX1_FIFO_HS_SIZE;
			}

			DCD_EP_Tx (pdev, CDC_IN_EP, (uint8_t*)&USB_Tx_Buffer.pData[USB_Tx_Buffer.Index], USB_Tx_Buffer.PcktLength);
			
			USB_Tx_Buffer.Index += USB_Tx_Buffer.PcktLength;					// change index value 
			USB_Tx_Buffer.Ready = false;
	}														
		return;	
}


/********************************************************************************
  * @brief  USBD_cdc_GetCfgDesc 
  *         Return configuration descriptor
  * @param  speed : current device speed
  * @param  length : pointer data length
  * @retval pointer to descriptor buffer
********************************************************************************/
static uint8_t  *USBD_cdc_GetCfgDesc (uint8_t speed, uint16_t *length)	{
  *length = sizeof (usbd_cdc_CfgDesc);
  return usbd_cdc_CfgDesc;
}

/********************************************************************************
  * @brief  USBD_cdc_GetCfgDesc 
  *         Return configuration descriptor
  * @param  speed : current device speed
  * @param  length : pointer data length
  * @retval pointer to descriptor buffer
********************************************************************************/
#ifdef USE_USB_OTG_HS 
static uint8_t  *USBD_cdc_GetOtherCfgDesc (uint8_t speed, uint16_t *length)
{
  *length = sizeof (usbd_cdc_OtherCfgDesc);
  return usbd_cdc_OtherCfgDesc;
}
#endif



/*******************************************************
   USB interrupt handler declarations & handler itself 
********************************************************/


extern USB_OTG_CORE_HANDLE           USB_OTG_dev;
extern uint32_t USBD_OTG_ISR_Handler (USB_OTG_CORE_HANDLE *pdev);

/********************************************************************************
  * @brief  This function handles OTG_HS Handler.
  * @param  None
  * @retval None
********************************************************************************/


#ifdef USE_USB_OTG_HS  
void OTG_HS_IRQHandler(void)
#else
void OTG_FS_IRQHandler(void)
#endif
{
  USBD_OTG_ISR_Handler (&USB_OTG_dev);
}


/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
