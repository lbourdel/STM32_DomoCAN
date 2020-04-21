/*! \mainpage STM32 LAB
 *
 * \section td2 TD 2 : GPIO
 *
 * \subsection step1 Step 1: Opening the box
 * This is the introduction.
 *
 * \section td3 TD 3 : External interrupt
 *
 * \subsection step1 Step 1: Opening the box
 * This is the introduction.
 *
 * \section td4 TD 4 : UltraSonic Sensor
 *
 * \subsection step1 Step 1: Opening the box
 * This is the introduction.
 *
 * \section td5 TD 2 : Remote Control
 *
 * \subsection step1 Step 1: Opening the box
 * This is the introduction.
 *
 *
 * etc...
 */
/**
 * \file      main.c
 * \author    L. BOURDEL
 * \version   1.0
 * \date      2018
 * \brief     Main function
 *
 * \details   !!! Good Luck !!!
 */
 
#include "main.h"

/* FOR DEBUG
http://www.keil.com/support/man/docs/ulink2/ulink2_trace_itm_viewer.htm
http://www.keil.com/appnotes/docs/apnt_286.asp
19) Configuring Serial Wire Viewer (SWV):

DON'T FORGET to set SystemCoreClock in Debug/Trace windows
+
22) printf with ITM

*/




uint8_t flagReceived=0;

/* Private variables ---------------------------------------------------------*/
uint8_t ubKeyNumber = 0x0;
CAN_HandleTypeDef     CanHandle;
CAN_TxHeaderTypeDef   TxHeader;
CAN_RxHeaderTypeDef   RxHeader;
uint8_t               TxData[8];
uint8_t               RxData[8];
uint32_t              TxMailbox;


uint16_t CountTransmit=0;
uint16_t CountReceivedFifo0=0;
uint16_t CountReceivedFifo1=0;

/**
* @brief  This function handles CAN1 RX0 interrupt request.
* @param  None
* @retval None
*/
void CANx_RX_IRQHandler(void)
{
  HAL_CAN_IRQHandler(&CanHandle);
}



/*----------------------------------------------------------------------------
 MAIN function
 *----------------------------------------------------------------------------*/
void assert_failed(uint8_t* file, uint32_t line) {
      printf("Assert fail at File %s Line %d", file, (int)line);
      while(1);   // hang here (behavior is our your choice)
} 


static void Error_Handler(void)
{
		printf("ERROR");

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

/**
  * @brief  Rx Fifo 0 message pending callback
  * @param  hcan: pointer to a CAN_HandleTypeDef structure that contains
  *         the configuration information for the specified CAN.
  * @retval None
  */
void IRQ_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan)
{
  /* Get RX message */
  if (HAL_CAN_GetRxMessage(hcan, CAN_RX_FIFO0, &RxHeader, RxData) != HAL_OK)
  {
    /* Reception Error */
    Error_Handler();
  }
	CountReceivedFifo0++;

//	printf("Msg Received \n");
	flagReceived=1;

}

/**
  * @brief  Rx Fifo 1 message pending callback
  * @param  hcan: pointer to a CAN_HandleTypeDef structure that contains
  *         the configuration information for the specified CAN.
  * @retval None
  */
void IRQ_CAN_RxFifo1MsgPendingCallback(CAN_HandleTypeDef *hcan)
{
  /* Get RX message */
  if (HAL_CAN_GetRxMessage(hcan, CAN_RX_FIFO1, &RxHeader, RxData) != HAL_OK)
  {
    /* Reception Error */
    Error_Handler();
  }
	CountReceivedFifo1++;

//	printf("Msg Received \n");
	flagReceived=1;

}


void assert_print(CAN_HandleTypeDef *_hcan) {
      printf("ErrorCode hcan %d",  (int)_hcan->ErrorCode);

}

void print_info(CAN_HandleTypeDef *_hcan) {
      printf("CAN Entered in SLEEP %d",  (int)_hcan->ErrorCode);

}

void print_info_WAKEUP_FROM_RX_MSG(CAN_HandleTypeDef *_hcan) {
      printf("CAN Entered is HAL_CAN_WAKEUP_FROM_RX_MSG_CB_ID %d",  (int)_hcan->ErrorCode);

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
//                               HAL_CAN_RX_FIFO1_MSG_PENDING_CB_ID Rx Fifo 1 message pending callback ID
	HAL_CAN_RegisterCallback(hcan, HAL_CAN_RX_FIFO1_MSG_PENDING_CB_ID, IRQ_CAN_RxFifo1MsgPendingCallback);
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
static void CAN_Config(void)
{
  CAN_FilterTypeDef  sFilterConfig;
	
		// GPIO Config
	// PB8 CAN1_RX
	// PB9 CAN1 TX
	GPIOx_Pin_Configure(PB_8, PIN_ALTERNATE, OUTPUT_PUSHPULL, OUTPUT_SPEED_HS, INPUT_PULL_UP);
	GPIOx_PinAF (PB_8, GPIO_AF9_CAN1);

	GPIOx_Pin_Configure(PB_9, PIN_ALTERNATE, OUTPUT_PUSHPULL, OUTPUT_SPEED_HS, INPUT_PULL_UP);
	GPIOx_PinAF (PB_9, GPIO_AF9_CAN1);

	// Set up the clock	
	RCC->APB1ENR|=RCC_APB1ENR_CAN1EN; // CAN1 clock enable
	
	/* CAN register init */
  //CAN_DeInit(CANx);
	
	

  /*##-1- Configure the CAN peripheral #######################################*/
  CanHandle.Instance = CANx;

  CanHandle.Init.TimeTriggeredMode = DISABLE;
  CanHandle.Init.AutoBusOff = DISABLE;
  CanHandle.Init.AutoWakeUp = DISABLE;
  CanHandle.Init.AutoRetransmission = ENABLE;
  CanHandle.Init.ReceiveFifoLocked = DISABLE;
  CanHandle.Init.TransmitFifoPriority = DISABLE;
  CanHandle.Init.Mode = CAN_MODE_NORMAL;
  CanHandle.Init.SyncJumpWidth = CAN_SJW_1TQ;
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
  CanHandle.Init.TimeSeg1 = CAN_BS1_12TQ;
  CanHandle.Init.TimeSeg2 = CAN_BS2_2TQ;
  CanHandle.Init.Prescaler = 6;

  if (HAL_CAN_Init(&CanHandle) != HAL_OK)
  {
    /* Initialization Error */
    Error_Handler();
  }

  /*##-2- Configure the CAN Filter ###########################################*/
  sFilterConfig.FilterBank = 0;
  sFilterConfig.FilterMode = CAN_FILTERMODE_IDMASK;
  sFilterConfig.FilterScale = CAN_FILTERSCALE_32BIT;
  sFilterConfig.FilterIdHigh = 0x0000;
  sFilterConfig.FilterIdLow = 0x0000;
  sFilterConfig.FilterMaskIdHigh = 0x0000;
  sFilterConfig.FilterMaskIdLow = 0x0000;
  sFilterConfig.FilterFIFOAssignment = CAN_RX_FIFO0;
  sFilterConfig.FilterActivation = ENABLE;
  sFilterConfig.SlaveStartFilterBank = 14;

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

  if (HAL_CAN_ActivateNotification(&CanHandle, CAN_IT_ERROR_WARNING) != HAL_OK)
  {
    /* Notification Error */
    Error_Handler();
  }

  if (HAL_CAN_ActivateNotification(&CanHandle, CAN_IT_ERROR_PASSIVE) != HAL_OK)
  {
    /* Notification Error */
    Error_Handler();
  }

  if (HAL_CAN_ActivateNotification(&CanHandle, CAN_IT_BUSOFF) != HAL_OK)
  {
    /* Notification Error */
    Error_Handler();
  }

  if (HAL_CAN_ActivateNotification(&CanHandle, CAN_IT_LAST_ERROR_CODE) != HAL_OK)
  {
    /* Notification Error */
    Error_Handler();
  }

  if (HAL_CAN_ActivateNotification(&CanHandle, CAN_IT_ERROR) != HAL_OK)
  {
    /* Notification Error */
    Error_Handler();
  }

  /*##-5- Configure Transmission process #####################################*/
  TxHeader.StdId = 0x321;
  TxHeader.ExtId = 0x01;
  TxHeader.RTR = CAN_RTR_DATA;
  TxHeader.IDE = CAN_ID_STD;
  TxHeader.DLC = 3;
  TxHeader.TransmitGlobalTime = DISABLE;
	TxData[0]=0xAA;
	TxData[1]=0xBB;
	TxData[2]=0xCC;


}


/**
 * \brief     Main function entry
 * \details   
 * \return    \e none.
 */

int main(void){
// Switch System Clock to maximum
// F446 : 180Mhz, Apb1TimerClock = 90Mhz Apb2TimerClock = 180Mhz
// Apb1PeriphClock = 45Mhz Apb2PeriphClock = 90Mhz
	System_Clock_Init(); 

	SysTick_Initialize(1000);  // Interrupt period  = 1ms (1/1khz)

	  /* Configure the CAN peripheral */
	CAN_Config();

	CAN_NVIC_Init();
		
	/* Start the Transmission process */
	if (HAL_CAN_AddTxMessage(&CanHandle, &TxHeader, TxData, &TxMailbox) != HAL_OK)
	{
		/* Transmission request Error */
		Error_Handler();
	}
	printf("Welcome STM32 Domocan Project\n");

	while(1)
	{
		Delay_ms(100);

		if (flagReceived)
		{
			flagReceived=0;
			/* Start the Transmission process */
			if (HAL_CAN_AddTxMessage(&CanHandle, &TxHeader, TxData, &TxMailbox) != HAL_OK)
			{
				/* Transmission request Error */
				Error_Handler();
			}
			CountTransmit++;
			// Erroe Passive !!! LBR HAL_CAN_RequestSleep(&CanHandle);

		}
	}
}

