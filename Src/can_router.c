/**
 * \file      can_router.c
 * \author    L. BOURDEL
 * \version   1.0
 * \date      2018
 * \brief     Main function
 *
 * \details   !!! Good Luck !!!
 */
 
#include "can_router.h"

uint8_t flagReceived=0;

/* Private variables ---------------------------------------------------------*/
uint8_t ubKeyNumber = 0x0;
CAN_HandleTypeDef     CanHandle;
CAN_TxHeaderTypeDef   TxHeader;

uint8_t               TxData[8];
uint32_t              TxMailbox;

CAN_FifoRxTypeDef CAN_FIFO_Rx = 
{
	.first=0,
	.last=0,	
	.NumberOfFrame=0
};

uint16_t CountTransmit=0;
uint16_t FailTransmit=0;
uint16_t CountReceivedFifo0=0;
uint16_t CountReceivedFifo1=0;


uint8_t destinataire_TX=0;
uint8_t commande_TX=0;
uint8_t carte_cible_TX=0;
uint8_t param_TX=0;

/**
* @brief  This function handles CAN1 RX0 interrupt request.
* @param  None
* @retval None
*/
void CANx_RX_IRQHandler(void)
{
  HAL_CAN_IRQHandler(&CanHandle);
}

/**
  * @brief  Get an CAN frame from the Rx FIFO zone into the message RAM.
  * @param  hcan pointer to an CAN_HandleTypeDef structure that contains
  *         the configuration information for the specified CAN.
  * @param  RxFifo Fifo number of the received message to be read.
  *         This parameter can be a value of @arg CAN_receive_FIFO_number.
  * @param  pHeader pointer to a CAN_RxHeaderTypeDef structure where the header
  *         of the Rx frame will be stored.
  * @param  aData array where the payload of the Rx frame will be stored.
  * @retval HAL status
  */
HAL_StatusTypeDef CAN_GetRxMessage(CAN_HandleTypeDef *hcan, uint32_t RxFifo)
{
  HAL_CAN_StateTypeDef state = hcan->State;
	uint32_t ExtId = 0;
	CAN_FrameTypeDef* pFrameReceived;
	
  assert_param(IS_CAN_RX_FIFO(RxFifo));

  if ((state == HAL_CAN_STATE_READY) ||
      (state == HAL_CAN_STATE_LISTENING))
  {
    /* Check the Rx FIFO */
    if (RxFifo == CAN_RX_FIFO0) /* Rx element is assigned to Rx FIFO 0 */
    {
      /* Check that the Rx FIFO 0 is not empty */
      if ((hcan->Instance->RF0R & CAN_RF0R_FMP0) == 0U)
      {
        /* Update error code */
        hcan->ErrorCode |= HAL_CAN_ERROR_PARAM;

        return HAL_ERROR;
      }
    }
    else /* Rx element is assigned to Rx FIFO 1 */
    {
      /* Check that the Rx FIFO 1 is not empty */
      if ((hcan->Instance->RF1R & CAN_RF1R_FMP1) == 0U)
      {
        /* Update error code */
        hcan->ErrorCode |= HAL_CAN_ERROR_PARAM;

        return HAL_ERROR;
      }
    }

    /* Get the header */
#if 0
    pHeader->IDE = CAN_RI0R_IDE & hcan->Instance->sFIFOMailBox[RxFifo].RIR;
    if (pHeader->IDE == CAN_ID_STD)
    {
      pHeader->StdId = (CAN_RI0R_STID & hcan->Instance->sFIFOMailBox[RxFifo].RIR) >> CAN_TI0R_STID_Pos;
    }
    else
    {
      pHeader->ExtId = ((CAN_RI0R_EXID | CAN_RI0R_STID) & hcan->Instance->sFIFOMailBox[RxFifo].RIR) >> CAN_RI0R_EXID_Pos;
    }
#else
    //pHeader->IDE = CAN_RI0R_IDE & hcan->Instance->sFIFOMailBox[RxFifo].RIR;
    if ( (CAN_RI0R_IDE & hcan->Instance->sFIFOMailBox[RxFifo].RIR) == CAN_ID_STD)
    {
      //pHeader->StdId = (CAN_RI0R_STID & hcan->Instance->sFIFOMailBox[RxFifo].RIR) >> CAN_TI0R_STID_Pos;
			/* Update error code */
			hcan->ErrorCode |= HAL_CAN_ERROR_PARAM; // Domocan use only Can Ext Format

			return HAL_ERROR;
    }
    else
    {
      ExtId = ((CAN_RI0R_EXID | CAN_RI0R_STID) & hcan->Instance->sFIFOMailBox[RxFifo].RIR) >> CAN_RI0R_EXID_Pos;
    }
		
#endif
		// LBR FIFO Management
		if(CAN_FIFO_Rx.NumberOfFrame==Rx_Fifo_Size) {
					/* Update error code */
					hcan->ErrorCode |= HAL_CAN_ERROR_RX_FIFO_FULL;

					return HAL_ERROR;
		}else
		{
			
			pFrameReceived = &CAN_FIFO_Rx.CanRxFrame[CAN_FIFO_Rx.last];
			CAN_FIFO_Rx.last = (CAN_FIFO_Rx.last+1)%Rx_Fifo_Size;
			CAN_FIFO_Rx.NumberOfFrame++;

			// Max 7FFFFFD
			pFrameReceived->destinataire = ExtId >> 21;
			
			pFrameReceived->commande = (ExtId >> 16) & 0x1F;
			
			pFrameReceived->carte_cible = (ExtId >> 8) & 0xFF;

			pFrameReceived->param = (ExtId ) & 0xFF;		
			
			//pFrameReceived->RTR = (CAN_RI0R_RTR & hcan->Instance->sFIFOMailBox[RxFifo].RIR) >> CAN_RI0R_RTR_Pos;
			pFrameReceived->DLC = (CAN_RDT0R_DLC & hcan->Instance->sFIFOMailBox[RxFifo].RDTR) >> CAN_RDT0R_DLC_Pos;
			pFrameReceived->FilterMatchIndex = (CAN_RDT0R_FMI & hcan->Instance->sFIFOMailBox[RxFifo].RDTR) >> CAN_RDT0R_FMI_Pos;
			//pFrameReceived->Timestamp = (CAN_RDT0R_TIME & hcan->Instance->sFIFOMailBox[RxFifo].RDTR) >> CAN_RDT0R_TIME_Pos;

			/* Get the data */
			pFrameReceived->data[0] = (uint8_t)((CAN_RDL0R_DATA0 & hcan->Instance->sFIFOMailBox[RxFifo].RDLR) >> CAN_RDL0R_DATA0_Pos);
			pFrameReceived->data[1] = (uint8_t)((CAN_RDL0R_DATA1 & hcan->Instance->sFIFOMailBox[RxFifo].RDLR) >> CAN_RDL0R_DATA1_Pos);
			pFrameReceived->data[2] = (uint8_t)((CAN_RDL0R_DATA2 & hcan->Instance->sFIFOMailBox[RxFifo].RDLR) >> CAN_RDL0R_DATA2_Pos);
			pFrameReceived->data[3] = (uint8_t)((CAN_RDL0R_DATA3 & hcan->Instance->sFIFOMailBox[RxFifo].RDLR) >> CAN_RDL0R_DATA3_Pos);
			pFrameReceived->data[4] = (uint8_t)((CAN_RDH0R_DATA4 & hcan->Instance->sFIFOMailBox[RxFifo].RDHR) >> CAN_RDH0R_DATA4_Pos);
			pFrameReceived->data[5] = (uint8_t)((CAN_RDH0R_DATA5 & hcan->Instance->sFIFOMailBox[RxFifo].RDHR) >> CAN_RDH0R_DATA5_Pos);
			pFrameReceived->data[6] = (uint8_t)((CAN_RDH0R_DATA6 & hcan->Instance->sFIFOMailBox[RxFifo].RDHR) >> CAN_RDH0R_DATA6_Pos);
			pFrameReceived->data[7] = (uint8_t)((CAN_RDH0R_DATA7 & hcan->Instance->sFIFOMailBox[RxFifo].RDHR) >> CAN_RDH0R_DATA7_Pos);			
		}


    /* Release the FIFO */
    if (RxFifo == CAN_RX_FIFO0) /* Rx element is assigned to Rx FIFO 0 */
    {
      /* Release RX FIFO 0 */
      SET_BIT(hcan->Instance->RF0R, CAN_RF0R_RFOM0);
    }
    else /* Rx element is assigned to Rx FIFO 1 */
    {
      /* Release RX FIFO 1 */
      SET_BIT(hcan->Instance->RF1R, CAN_RF1R_RFOM1);
    }
    /* Return function status */
    return HAL_OK;
  }
  else
  {
    /* Update error code */
    hcan->ErrorCode |= HAL_CAN_ERROR_NOT_INITIALIZED;

    return HAL_ERROR;
  }
}

/**
  * @brief  Rx Fifo 0 message pending callback
  * @param  hcan: pointer to a CAN_HandleTypeDef structure that contains
  *         the configuration information for the specified CAN.
  * @retval None
  */
void IRQ_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan)
{
  /* Get RX message */
  if (CAN_GetRxMessage(hcan, CAN_RX_FIFO0) != HAL_OK)
  {
    /* Reception Error */
    Error_Handler();
  }

	if(CAN_FIFO_Rx.CanRxFrame[0].destinataire>0xFD) // 7 bits recessifs impossible
	{
		// CanSendError( Err_G15 );
	}

	CountReceivedFifo0++;

//	printf("Msg Received \n");
	flagReceived=1;

}

void CanRouter(void)
{
	
		if (CAN_FIFO_Rx.NumberOfFrame)
		{
			switch(CAN_FIFO_Rx.CanRxFrame[CAN_FIFO_Rx.first].commande)
			{
				case CMD_COMM:
					CanCommonCmdRouter();
					break;
				case CMD_INPUT:
					break;
				default:
					break;
			}
		}

		flagReceived=0;

			// Error Passive Need to automotic wakeup !!! LBR HAL_CAN_RequestSleep(&CanHandle);
}

void Can_SendMessage(void)
{
	TxHeader.ExtId = (destinataire_TX<<21);
	TxHeader.ExtId |= (commande_TX<<16);
	TxHeader.ExtId |= (carte_cible_TX<<8);
	TxHeader.ExtId |= (param_TX);
	
	/* Start the Transmission process */
	if (HAL_CAN_AddTxMessage(&CanHandle, &TxHeader, TxData, &TxMailbox) != HAL_OK)
	{
		/* Transmission request Error */
		Error_Handler();
		FailTransmit++;
	}
	else
	{// Keep current received msg to transmit new answer
		CAN_FIFO_Rx.first = (CAN_FIFO_Rx.first+1)%Rx_Fifo_Size;
		CAN_FIFO_Rx.NumberOfFrame--;
	}
}

void CanSendError( uint8_t ErrorCode )
{
	destinataire_TX = destinataire_RX;
	commande_TX = 0x1F;
	carte_cible_TX = 0x00;
	param_TX = ErrorCode;

	TxHeader.DLC = 4;
	TxData[0] = destinataire_RX;
	TxData[1] = commande_RX;
	TxData[2] = carte_cible_RX;
	TxData[3] = param_RX;
	
	Can_SendMessage();
}

/**
  * @brief  Initializes the NVIC peripheral according to the specified
  *         parameters in the NVIC_InitStruct.
  * @note   To configure interrupts priority correctly, the NVIC_PriorityGroupConfig()
  *         function should be called before. 
  * @param  NVIC_InitStruct: pointer to a NVIC_InitTypeDef structure that contains
  *         the configuration information for the specified NVIC peripheral.
  * @retval None
  */
void CAN_NVIC_Init(void)
{
	  uint32_t prioritygroup = 0x00U;
  
  /* Check the parameters */
// assert_param(IS_NVIC_SUB_PRIORITY(SubPriority));
//  assert_param(IS_NVIC_PREEMPTION_PRIORITY(PreemptPriority));
  
  prioritygroup = NVIC_GetPriorityGrouping();
  
  //NVIC_SetPriority(IRQn, NVIC_EncodePriority(prioritygroup, PreemptPriority, SubPriority));
  NVIC_SetPriority(CANx_RX_IRQn, NVIC_EncodePriority(prioritygroup, 1, 0));

	  /* Enable interrupt */
  NVIC_EnableIRQ(CANx_RX_IRQn);
}


void Error_Handler(void)
{
		printf("Error_Handler 0x%08x\n", CanHandle.ErrorCode);

}


void assert_print(CAN_HandleTypeDef *_hcan) {
      printf("ErrorCode hcan 0x%08x\n",  _hcan->ErrorCode);

}

void print_info(CAN_HandleTypeDef *_hcan) {
      printf("CAN Entered in SLEEP 0x%08x\n",  _hcan->ErrorCode);

}

void print_info_WAKEUP_FROM_RX_MSG(CAN_HandleTypeDef *_hcan) {
      printf("CAN Entered is HAL_CAN_WAKEUP_FROM_RX_MSG_CB_ID 0x%08x\n",  _hcan->ErrorCode);

}


void CAN_ConfigureCallBackFct(CAN_HandleTypeDef *hcan)
{
//                               HAL_CAN_TX_MAILBOX0_COMPLETE_CB_ID Tx Mailbox 0 Complete callback ID
//                               HAL_CAN_TX_MAILBOX1_COMPLETE_CB_ID Tx Mailbox 1 Complete callback ID
//                               HAL_CAN_TX_MAILBOX2_COMPLETE_CB_ID Tx Mailbox 2 Complete callback ID
//                               HAL_CAN_TX_MAILBOX0_ABORT_CB_ID Tx Mailbox 0 Abort callback ID
//                               HAL_CAN_TX_MAILBOX1_ABORT_CB_ID Tx Mailbox 1 Abort callback ID
//                               HAL_CAN_TX_MAILBOX2_ABORT_CB_ID Tx Mailbox 2 Abort callback ID
//                               HAL_CAN_RX_FIFO0_MSG_PENDING_CB_ID Rx Fifo 0 message pending callback ID
	HAL_CAN_RegisterCallback(hcan, HAL_CAN_RX_FIFO0_MSG_PENDING_CB_ID, IRQ_CAN_RxFifo0MsgPendingCallback);
//                               HAL_CAN_RX_FIFO0_FULL_CB_ID Rx Fifo 0 full callback ID
	HAL_CAN_RegisterCallback(hcan, HAL_CAN_RX_FIFO0_FULL_CB_ID, assert_print);

//                               HAL_CAN_RX_FIFO1_FULL_CB_ID Rx Fifo 1 full callback ID
	HAL_CAN_RegisterCallback(hcan, HAL_CAN_RX_FIFO1_FULL_CB_ID, assert_print);
//                               HAL_CAN_SLEEP_CB_ID Sleep callback ID
	HAL_CAN_RegisterCallback(hcan, HAL_CAN_SLEEP_CB_ID, print_info);
//                               HAL_CAN_WAKEUP_FROM_RX_MSG_CB_ID Wake Up from Rx message callback ID
	HAL_CAN_RegisterCallback(hcan, HAL_CAN_SLEEP_CB_ID, print_info_WAKEUP_FROM_RX_MSG);
//                               HAL_CAN_ERROR_CB_ID Error callback ID
	HAL_CAN_RegisterCallback(hcan, HAL_CAN_ERROR_CB_ID, assert_print);
//                               HAL_CAN_MSPINIT_CB_ID MspInit callback ID
//                               HAL_CAN_MSPDEINIT_CB_ID MspDeInit callback ID	
	
}



/**
  * @brief  Configures the CAN.
  * @param  None
  * @retval None
  */
void CAN_Config(void)
{
  CAN_FilterTypeDef  sFilterConfig;
	
		// GPIO Config
	// CAN1
	// PB8 CAN1_RX
	// PB9 CAN1 TX
	/*
	GPIOx_Pin_Configure(PB_8, PIN_ALTERNATE, OUTPUT_PUSHPULL, OUTPUT_SPEED_HS, INPUT_PULL_UP);
	GPIOx_PinAF (PB_8, GPIO_AF9_CAN1);

	GPIOx_Pin_Configure(PB_9, PIN_ALTERNATE, OUTPUT_PUSHPULL, OUTPUT_SPEED_HS, INPUT_PULL_UP);
	GPIOx_PinAF (PB_9, GPIO_AF9_CAN1);
	*/

	// CAN2 Bootloader use
	// CAN2_RX pin Input PB05
	// CAN2_TX pin Output PB13
	GPIOx_Pin_Configure(PB_5, PIN_ALTERNATE, OUTPUT_PUSHPULL, OUTPUT_SPEED_HS, INPUT_PULL_UP);
	GPIOx_PinAF (PB_5, GPIO_AF9_CAN2);

	GPIOx_Pin_Configure(PB_13, PIN_ALTERNATE, OUTPUT_PUSHPULL, OUTPUT_SPEED_HS, INPUT_PULL_UP);
	GPIOx_PinAF (PB_13, GPIO_AF9_CAN2);

	// Set up the CAN clock
	if( CANx==CAN1)
	{
		RCC->APB1ENR|=RCC_APB1ENR_CAN1EN; // CAN1 clock enable
	}
	else if( CANx==CAN2)
	{
		RCC->APB1ENR|=RCC_APB1ENR_CAN1EN; // CAN1 clock enable
		RCC->APB1ENR|=RCC_APB1ENR_CAN2EN; // CAN2 clock enable
	}
	
	/* CAN register init */
  //CAN_DeInit(CANx);
	
	

  /*##-1- Configure the CAN peripheral #######################################*/
  CanHandle.Instance = CANx;

  CanHandle.Init.TimeTriggeredMode = DISABLE; // MCR->TTCM :Internal counter for Timestamp
  CanHandle.Init.AutoBusOff = ENABLE; // MCR->ABOM :Bus-Off recovery LBR
  CanHandle.Init.AutoWakeUp = DISABLE; // MCR->AWUM :Automatic wakeup when CAN is in SLEEP mode
  CanHandle.Init.AutoRetransmission = ENABLE; // MCR->NART
  CanHandle.Init.ReceiveFifoLocked = DISABLE; // MCR->RFLM :FIFO locked on overrun
  CanHandle.Init.TransmitFifoPriority = DISABLE; // MCR->TXFP
  CanHandle.Init.Mode = CAN_MODE_NORMAL; // BTR->xx no debug loop back used

	CanHandle.Init.SyncJumpWidth = CAN_SJW_1TQ; // BTR->SJW
	// LBR 
  /* CAN Baudrate = 500 KBps (CAN APB1 clocked at 45 MHz) */ // LBR
	/*
	Tq=(BRP[9:0]+1)*time period of APB clock = 6/45Mhz 
	Tbs1=Tq*(TS1[3:0]+1) =Tq*(TS1[3:0]+1) = 12tq
	Tbs2=Tq*(TS2[2:0]+1) =Tq*(TS2[2:0]+1) = 2tq
	NominalBitTime = 1*Tq+Tbs1+Tbs2 = 15tq = 15*6/45 = 2Mhz
	Baud Rate=1/NominalBitTime = 500 KBps
	http://www.bittiming.can-wiki.info/
	*/
  CanHandle.Init.TimeSeg1 = CAN_BS1_12TQ; // BTR->TS1
  CanHandle.Init.TimeSeg2 = CAN_BS2_2TQ; // BTR->TS2
  CanHandle.Init.Prescaler = 6; // BTR->BRP

  if (HAL_CAN_Init(&CanHandle) != HAL_OK)
  {
    /* Initialization Error */
    Error_Handler();
  }

  /*##-2- Configure the CAN Filter ###########################################*/
	// Filter 0 to 13 : CAN1, Filter 14 to 27 : CAN2
  sFilterConfig.SlaveStartFilterBank = 14; // FMR->CAN2SB : use 14 filters for each CANx
	// Init 1st filter of CAN2
  sFilterConfig.FilterBank = 14; // number of filter bank to initialized
  sFilterConfig.FilterMode = CAN_FILTERMODE_IDMASK; // Identifier mask mode or list mode
  sFilterConfig.FilterScale = CAN_FILTERSCALE_32BIT; // for extended identifiers (29-bit)
  sFilterConfig.FilterIdHigh = 0x0000; // No filter
  sFilterConfig.FilterIdLow = 0x0000;
  sFilterConfig.FilterMaskIdHigh = 0x0000;
  sFilterConfig.FilterMaskIdLow = 0x0000;
  sFilterConfig.FilterFIFOAssignment = CAN_RX_FIFO0; // filter set to FIFO0
  sFilterConfig.FilterActivation = ENABLE;

  if (HAL_CAN_ConfigFilter(&CanHandle, &sFilterConfig) != HAL_OK)
  {
    /* Filter configuration Error */
    Error_Handler();
  }
	// LBR Ready state now, configure callback fcts here
	CAN_ConfigureCallBackFct(&CanHandle);
	
  /*##-3- Start the CAN peripheral ###########################################*/
  if (HAL_CAN_Start(&CanHandle) != HAL_OK)
  {
    /* Start Error */
    Error_Handler();
  }

  /*##-4- Activate CAN RX notification #######################################*/
	// LBR only Pending IT needed
  if (HAL_CAN_ActivateNotification(&CanHandle, CAN_IT_RX_FIFO0_MSG_PENDING) != HAL_OK)
  {
    /* Notification Error */
    Error_Handler();
  }

  if (HAL_CAN_ActivateNotification(&CanHandle, CAN_IT_RX_FIFO0_FULL) != HAL_OK)
  {
    /* Notification Error */
    Error_Handler();
  }

  if (HAL_CAN_ActivateNotification(&CanHandle, CAN_IT_RX_FIFO0_OVERRUN) != HAL_OK)
  {
    /* Notification Error */
    Error_Handler();
  }

	  if (HAL_CAN_ActivateNotification(&CanHandle, CAN_IT_RX_FIFO1_MSG_PENDING) != HAL_OK)
  {
    /* Notification Error */
    Error_Handler();
  }

  if (HAL_CAN_ActivateNotification(&CanHandle, CAN_IT_RX_FIFO1_FULL) != HAL_OK)
  {
    /* Notification Error */
    Error_Handler();
  }

  if (HAL_CAN_ActivateNotification(&CanHandle, CAN_IT_RX_FIFO1_OVERRUN) != HAL_OK)
  {
    /* Notification Error */
    Error_Handler();
  }
//FOR DEBUG LBR
//  if (HAL_CAN_ActivateNotification(&CanHandle, CAN_IT_ERROR_WARNING) != HAL_OK)
//  {
//    /* Notification Error */
//    Error_Handler();
//  }

//  if (HAL_CAN_ActivateNotification(&CanHandle, CAN_IT_ERROR_PASSIVE) != HAL_OK)
//  {
//    /* Notification Error */
//    Error_Handler();
//  }

//  if (HAL_CAN_ActivateNotification(&CanHandle, CAN_IT_BUSOFF) != HAL_OK)
//  {
//    /* Notification Error */
//    Error_Handler();
//  }

//  if (HAL_CAN_ActivateNotification(&CanHandle, CAN_IT_LAST_ERROR_CODE) != HAL_OK)
//  {
//    /* Notification Error */
//    Error_Handler();
//  }

//  if (HAL_CAN_ActivateNotification(&CanHandle, CAN_IT_ERROR) != HAL_OK)
//  {
//    /* Notification Error */
//    Error_Handler();
//  }

  /*##-5- Configure Transmission process #####################################*/
  TxHeader.StdId = 0x321;
  TxHeader.ExtId = 0x01321;
  TxHeader.RTR = CAN_RTR_DATA; // Remote transmission request : DATA or REMOTE
  TxHeader.IDE = CAN_ID_EXT; // Standard identifiers (11-bit)/CAN_ID_STD and extended identifiers (29-bit)/CAN_ID_EXT
  TxHeader.DLC = 3;
  TxHeader.TransmitGlobalTime = DISABLE; // To transmit timestamp
	TxData[0]=0xAA;
	TxData[1]=0xBB;
	TxData[2]=0xCC;

	// config IT
	CAN_NVIC_Init();
	
	/* Start the Transmission process */
	if (HAL_CAN_AddTxMessage(&CanHandle, &TxHeader, TxData, &TxMailbox) != HAL_OK)
	{
		/* Transmission request Error */
		Error_Handler();
	}

}
