/**
 * \file      can.c
 * \author    L. BOURDEL
 * \version   1.0
 * \date      2017
 * \brief     Primitives functions for Analog to Digital Converter
 *
 * \details   None
 */
 
#include "can.h"

CAN_InitTypeDef        CAN_InitStructure;
CAN_FilterInitTypeDef  CAN_FilterInitStructure;
CanTxMsg TxMessage;
uint8_t MsgCounter=0;

void RCC_APB1PeriphResetCmd(uint32_t RCC_APB1Periph, FunctionalState NewState);

void RCC_APB1PeriphResetCmd(uint32_t RCC_APB1Periph, FunctionalState NewState)
{
  /* Check the parameters */
  //assert_param(IS_RCC_APB1_PERIPH(RCC_APB1Periph));
  //assert_param(IS_FUNCTIONAL_STATE(NewState));
  if (NewState != DISABLE)
  {
    RCC->APB1RSTR |= RCC_APB1Periph;
  }
  else
  {
    RCC->APB1RSTR &= ~RCC_APB1Periph;
  }
}

/**
  * @brief  Deinitializes the CAN peripheral registers to their default reset values.
  * @param  CANx: where x can be 1,2 or 3 to select the CAN peripheral.
  * @note   CAN3 peripheral is available only for STM32F413_423xx devices
  * @retval None.
  */
void CAN_DeInit(CAN_TypeDef* CANx)
{
  /* Check the parameters */
  //assert_param(IS_CAN_ALL_PERIPH(CANx));
 
  if (CANx == CAN1)
  {
    /* Enable CAN1 reset state */
    RCC_APB1PeriphResetCmd(RCC_APB1RSTR_CAN1RST, ENABLE);
    /* Release CAN1 from reset state */
    RCC_APB1PeriphResetCmd(RCC_APB1RSTR_CAN1RST, DISABLE);
  }
#if defined(STM32F413_423xx)
  else if(CANx == CAN2)
  {  
    /* Enable CAN2 reset state */
    RCC_APB1PeriphResetCmd(RCC_APB1Periph_CAN2, ENABLE);
    /* Release CAN2 from reset state */
    RCC_APB1PeriphResetCmd(RCC_APB1Periph_CAN2, DISABLE);
  }
  
  else /* CAN3 available only for STM32F413_423xx */
  {
    /* Enable CAN3 reset state */
    RCC_APB1PeriphResetCmd(RCC_APB1Periph_CAN3, ENABLE);
    /* Release CAN3 from reset state */
    RCC_APB1PeriphResetCmd(RCC_APB1Periph_CAN3, DISABLE); 
  }
#else
  else
  {
    /* Enable CAN2 reset state */
    RCC_APB1PeriphResetCmd(RCC_APB1RSTR_CAN2RST, ENABLE);
    /* Release CAN2 from reset state */
    RCC_APB1PeriphResetCmd(RCC_APB1RSTR_CAN2RST, DISABLE);
  }
#endif /* STM32F413_423xx */
}


/**
 * \brief     Init function entry
 * \details   
 * \return    \e none.
 */
uint8_t CAN_Init_new(CAN_TypeDef* CANx, CAN_InitTypeDef* CAN_InitStruct)
{
  uint8_t InitStatus = CAN_InitStatus_Failed;
  uint32_t wait_ack = 0x00000000;
  /* Check the parameters */
  //assert_param(IS_CAN_ALL_PERIPH(CANx));
  //assert_param(IS_FUNCTIONAL_STATE(CAN_InitStruct->CAN_TTCM));
  //assert_param(IS_FUNCTIONAL_STATE(CAN_InitStruct->CAN_ABOM));
  //assert_param(IS_FUNCTIONAL_STATE(CAN_InitStruct->CAN_AWUM));
  //assert_param(IS_FUNCTIONAL_STATE(CAN_InitStruct->CAN_NART));
  //assert_param(IS_FUNCTIONAL_STATE(CAN_InitStruct->CAN_RFLM));
  //assert_param(IS_FUNCTIONAL_STATE(CAN_InitStruct->CAN_TXFP));
  //assert_param(IS_CAN_MODE(CAN_InitStruct->CAN_Mode));
  //assert_param(IS_CAN_SJW(CAN_InitStruct->CAN_SJW));
  //assert_param(IS_CAN_BS1(CAN_InitStruct->CAN_BS1));
  //assert_param(IS_CAN_BS2(CAN_InitStruct->CAN_BS2));
  //assert_param(IS_CAN_PRESCALER(CAN_InitStruct->CAN_Prescaler));

  /* Exit from sleep mode */
  CANx->MCR &= (~(uint32_t)CAN_MCR_SLEEP);

  /* Request initialisation */
  CANx->MCR |= CAN_MCR_INRQ ;

  /* Wait the acknowledge */
  while (((CANx->MSR & CAN_MSR_INAK) != CAN_MSR_INAK) && (wait_ack != INAK_TIMEOUT))
  {
    wait_ack++;
  }

  /* Check acknowledge */
  if ((CANx->MSR & CAN_MSR_INAK) != CAN_MSR_INAK)
  {
    InitStatus = CAN_InitStatus_Failed;
  }
  else 
  {
    /* Set the time triggered communication mode */
    if (CAN_InitStruct->CAN_TTCM == ENABLE)
    {
      CANx->MCR |= CAN_MCR_TTCM;
    }
    else
    {
      CANx->MCR &= ~(uint32_t)CAN_MCR_TTCM;
    }

    /* Set the automatic bus-off management */
    if (CAN_InitStruct->CAN_ABOM == ENABLE)
    {
      CANx->MCR |= CAN_MCR_ABOM;
    }
    else
    {
      CANx->MCR &= ~(uint32_t)CAN_MCR_ABOM;
    }

    /* Set the automatic wake-up mode */
    if (CAN_InitStruct->CAN_AWUM == ENABLE)
    {
      CANx->MCR |= CAN_MCR_AWUM;
    }
    else
    {
      CANx->MCR &= ~(uint32_t)CAN_MCR_AWUM;
    }

    /* Set the no automatic retransmission */
    if (CAN_InitStruct->CAN_NART == ENABLE)
    {
      CANx->MCR |= CAN_MCR_NART;
    }
    else
    {
      CANx->MCR &= ~(uint32_t)CAN_MCR_NART;
    }

    /* Set the receive FIFO locked mode */
    if (CAN_InitStruct->CAN_RFLM == ENABLE)
    {
      CANx->MCR |= CAN_MCR_RFLM;
    }
    else
    {
      CANx->MCR &= ~(uint32_t)CAN_MCR_RFLM;
    }

    /* Set the transmit FIFO priority */
    if (CAN_InitStruct->CAN_TXFP == ENABLE)
    {
      CANx->MCR |= CAN_MCR_TXFP;
    }
    else
    {
      CANx->MCR &= ~(uint32_t)CAN_MCR_TXFP;
    }

    /* Set the bit timing register */
    CANx->BTR = (uint32_t)((uint32_t)CAN_InitStruct->CAN_Mode << 30) | \
                ((uint32_t)CAN_InitStruct->CAN_SJW << 24) | \
                ((uint32_t)CAN_InitStruct->CAN_BS1 << 16) | \
                ((uint32_t)CAN_InitStruct->CAN_BS2 << 20) | \
               ((uint32_t)CAN_InitStruct->CAN_Prescaler - 1);

    /* Request leave initialisation */
    CANx->MCR &= ~(uint32_t)CAN_MCR_INRQ;

   /* Wait the acknowledge */
   wait_ack = 0;

   while (((CANx->MSR & CAN_MSR_INAK) == CAN_MSR_INAK) && (wait_ack != INAK_TIMEOUT))
   {
     wait_ack++;
   }

    /* ...and check acknowledged */
    if ((CANx->MSR & CAN_MSR_INAK) == CAN_MSR_INAK)
    {
      InitStatus = CAN_InitStatus_Failed;
    }
    else
    {
      InitStatus = CAN_InitStatus_Success ;
    }
  }

  /* At this step, return the status of initialization */
  return InitStatus;
}

uint8_t CAN_Init(CAN_TypeDef* CANx, CAN_InitTypeDef* CAN_InitStruct){
  uint32_t tickstart;

	/* Exit from sleep mode */   
  CANx->MCR &= (~(uint32_t)CAN_MCR_SLEEP); 
	
 /* Get tick */
  tickstart = SysTick_GetTick();
  /* Check Sleep mode leave acknowledge */
  while ((CANx->MSR & CAN_MSR_SLAK) != 0U)
  {
    if ((SysTick_GetTick() - tickstart) > 10)
    {
			return 1;
		}
  }
	
	/* Request initialisation */   
  CANx->MCR |= CAN_MCR_INRQ ;   
   
 /* Get tick */
  tickstart = SysTick_GetTick();
  /* Wait the acknowledge */   
  while (((CANx->MSR & CAN_MSR_INAK) != CAN_MSR_INAK))   
  {   
    if ((SysTick_GetTick() - tickstart) > 10)
    {
			return 1;
		}
  }   
   
  
    /* Set the time triggered communication mode */   
    if (CAN_InitStruct->CAN_TTCM == ENABLE)   
    {   
      CANx->MCR |= CAN_MCR_TTCM;   
    }   
    else   
    {   
      CANx->MCR &= ~(uint32_t)CAN_MCR_TTCM;   
    }   
   
    /* Set the automatic bus-off management */   
    if (CAN_InitStruct->CAN_ABOM == ENABLE)   
    {   
      CANx->MCR |= CAN_MCR_ABOM;   
    }   
    else   
    {   
      CANx->MCR &= ~(uint32_t)CAN_MCR_ABOM;   
    }   
   
    /* Set the automatic wake-up mode */   
    if (CAN_InitStruct->CAN_AWUM == ENABLE)   
    {   
      CANx->MCR |= CAN_MCR_AWUM;   
    }   
    else   
    {   
      CANx->MCR &= ~(uint32_t)CAN_MCR_AWUM;   
    }   
   
    /* Set the no automatic retransmission */   
    if (CAN_InitStruct->CAN_NART == ENABLE)   
    {   
      CANx->MCR |= CAN_MCR_NART;   
    }   
    else   
    {   
      CANx->MCR &= ~(uint32_t)CAN_MCR_NART;   
    }   
   
    /* Set the receive FIFO locked mode */   
    if (CAN_InitStruct->CAN_RFLM == ENABLE)   
    {   
      CANx->MCR |= CAN_MCR_RFLM;   
    }   
    else   
    {   
      CANx->MCR &= ~(uint32_t)CAN_MCR_RFLM;   
    }   
   
    /* Set the transmit FIFO priority */   
    if (CAN_InitStruct->CAN_TXFP == ENABLE)   
    {   
      CANx->MCR |= CAN_MCR_TXFP;   
    }   
    else   
    {   
      CANx->MCR &= ~(uint32_t)CAN_MCR_TXFP;   
    }   
   
    /* Set the bit timing register */   
    CANx->BTR = (uint32_t)((uint32_t)CAN_InitStruct->CAN_Mode << 30) |
                ((uint32_t)CAN_InitStruct->CAN_SJW << 24) |
                ((uint32_t)CAN_InitStruct->CAN_BS1 << 16) |
                ((uint32_t)CAN_InitStruct->CAN_BS2 << 20) | 
               ((uint32_t)CAN_InitStruct->CAN_Prescaler - 1);   
   
    /* Request leave initialisation */   
    CANx->MCR &= ~(uint32_t)CAN_MCR_INRQ;   
   
 /* Get tick */
  tickstart = SysTick_GetTick();
   while (((CANx->MSR & CAN_MSR_INAK) == CAN_MSR_INAK) )   
   {   
    if ((SysTick_GetTick() - tickstart) > 10)
    {
			return 1;
		}
   }   
   
    /* ...and check acknowledged */   
    if ((CANx->MSR & CAN_MSR_INAK) == CAN_MSR_INAK)   
    {   
			return 1;
    }   
    else   
    {   
			return 0;
    }   
}

/**
 * \brief     config function for CAN module
 * \details   
 * \return    \e none.
 */
void CAN_Config(CAN_TypeDef* CANx) {

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
	
  /* CAN cell init */
  CAN_InitStructure.CAN_TTCM = DISABLE;
  CAN_InitStructure.CAN_ABOM = DISABLE;
  CAN_InitStructure.CAN_AWUM = DISABLE;
  CAN_InitStructure.CAN_NART = DISABLE;
  CAN_InitStructure.CAN_RFLM = DISABLE;
  CAN_InitStructure.CAN_TXFP = DISABLE;
  CAN_InitStructure.CAN_Mode = CAN_Mode_Normal;
  CAN_InitStructure.CAN_SJW = CAN_SJW_1tq;
    
  /* CAN Baudrate = 1 MBps (CAN clocked at 30 MHz) */ // LBR
	/*
	Tq=(BRP[9:0]+1)*time period of APB clock = 2/30Mhz
	Tbs1=Tq*(TS1[3:0]+1) =Tq*(TS1[3:0]+1) = 6tq
	Tbs2=Tq*(TS2[2:0]+1) =Tq*(TS2[2:0]+1) = 8tq
	NominalBitTime = 1*Tq+Tbs1+Tbs2 = 15tq = 1/1Mhz
	Baud Rate=1/NominalBitTime = 1 MBps
	http://www.bittiming.can-wiki.info/

  CAN_InitStructure.CAN_BS1 = CAN_BS1_6tq;
  CAN_InitStructure.CAN_BS2 = CAN_BS2_8tq;
  CAN_InitStructure.CAN_Prescaler = 2;
  CAN_Init(CAN1, &CAN_InitStructure);
		*/
	
  /* CAN Baudrate = 250 KBps (CAN APB1 clocked at 45 MHz) */ // LBR
	/*
	Tq=(BRP[9:0]+1)*time period of APB clock = 6/45Mhz 
	Tbs1=Tq*(TS1[3:0]+1) =Tq*(TS1[3:0]+1) = 12tq
	Tbs2=Tq*(TS2[2:0]+1) =Tq*(TS2[2:0]+1) = 2tq
	NominalBitTime = 1*Tq+Tbs1+Tbs2 = 15tq = 15*6/45 = 2Mhz
	Baud Rate=1/NominalBitTime = 500 KBps
	http://www.bittiming.can-wiki.info/
	*/
  CAN_InitStructure.CAN_BS1 = CAN_BS1_12tq;
  CAN_InitStructure.CAN_BS2 = CAN_BS2_2tq;
  CAN_InitStructure.CAN_Prescaler = 6;
  CAN_Init(CAN1, &CAN_InitStructure);

  /* CAN filter init */
  CAN_FilterInitStructure.CAN_FilterNumber = 0;
  CAN_FilterInitStructure.CAN_FilterMode = CAN_FilterMode_IdMask;
  CAN_FilterInitStructure.CAN_FilterScale = CAN_FilterScale_32bit;
	// Ici, 000 est l'adresse de la carte. Il peux être différent pour chaque carte.
  CAN_FilterInitStructure.CAN_FilterIdHigh = 0x0000;
  CAN_FilterInitStructure.CAN_FilterIdLow = 0x0000;
	// Le masque peux servir à accepter une plage d'adresse au lieu d'une adresse unique.
  CAN_FilterInitStructure.CAN_FilterMaskIdHigh = 0x0000;
  CAN_FilterInitStructure.CAN_FilterMaskIdLow = 0x0000;
  CAN_FilterInitStructure.CAN_FilterFIFOAssignment = 0;
  CAN_FilterInitStructure.CAN_FilterActivation = ENABLE;
  CAN_FilterInit(&CAN_FilterInitStructure);
  
  /* Transmit Structure preparation */
	// Détermine l'adresse du périphérique au quel la trame est destiné. 
	// Si plusieurs périphériques sur le bus comprennent cette adresse dans leur filtre, ils recevront tous la trame.
//  TxMessage.StdId = 0x321;
  TxMessage.StdId = 0x4;
	// Adresse étendue, non utilisée dans note cas
  TxMessage.ExtId = 0x01;
	// Précise que la trame contient des données
  TxMessage.RTR = CAN_RTR_DATA;
	// Précise que la trame est de type Standard
  TxMessage.IDE = CAN_ID_STD;
	// Précise le nombre d'octets de données que la trame transporte ( De 0 à 8 )
  TxMessage.DLC = 3;
  
	// NVIC Config
  CAN_NVIC_Init();
	
  /* Enable FIFO 0 message pending Interrupt */
  CAN_ITConfig(CANx, CAN_IT_FMP0, ENABLE);
}


void CAN_SendTestMsg(CAN_TypeDef* CANx)
{
	TxMessage.Data[0] = 0xAA;
	TxMessage.Data[1] = 0xBB;
	TxMessage.Data[2] = 2+MsgCounter++;
	CAN_Transmit(CANx, &TxMessage);
		/* Wait until one of the mailboxes is empty */
	while((CAN_GetFlagStatus(CANx, CAN_FLAG_RQCP0) !=RESET) || 
				(CAN_GetFlagStatus(CANx, CAN_FLAG_RQCP1) !=RESET) || 
				(CAN_GetFlagStatus(CANx, CAN_FLAG_RQCP2) !=RESET));
}

/**
  * @brief  Configures the CAN reception filter according to the specified
  *         parameters in the CAN_FilterInitStruct.
  * @param  CAN_FilterInitStruct: pointer to a CAN_FilterInitTypeDef structure that
  *         contains the configuration information.
  * @retval None
  */
void CAN_FilterInit(CAN_FilterInitTypeDef* CAN_FilterInitStruct)
{
  uint32_t filter_number_bit_pos = 0;

  filter_number_bit_pos = ((uint32_t)1) << CAN_FilterInitStruct->CAN_FilterNumber;

  /* Initialisation mode for the filter */
  CAN1->FMR |= FMR_FINIT;

  /* Filter Deactivation */
  CAN1->FA1R &= ~(uint32_t)filter_number_bit_pos;

  /* Filter Scale */
  if (CAN_FilterInitStruct->CAN_FilterScale == CAN_FilterScale_16bit)
  {
    /* 16-bit scale for the filter */
    CAN1->FS1R &= ~(uint32_t)filter_number_bit_pos;

    /* First 16-bit identifier and First 16-bit mask */
    /* Or First 16-bit identifier and Second 16-bit identifier */
    CAN1->sFilterRegister[CAN_FilterInitStruct->CAN_FilterNumber].FR1 = 
       ((0x0000FFFF & (uint32_t)CAN_FilterInitStruct->CAN_FilterMaskIdLow) << 16) |
        (0x0000FFFF & (uint32_t)CAN_FilterInitStruct->CAN_FilterIdLow);

    /* Second 16-bit identifier and Second 16-bit mask */
    /* Or Third 16-bit identifier and Fourth 16-bit identifier */
    CAN1->sFilterRegister[CAN_FilterInitStruct->CAN_FilterNumber].FR2 = 
       ((0x0000FFFF & (uint32_t)CAN_FilterInitStruct->CAN_FilterMaskIdHigh) << 16) |
        (0x0000FFFF & (uint32_t)CAN_FilterInitStruct->CAN_FilterIdHigh);
  }

  if (CAN_FilterInitStruct->CAN_FilterScale == CAN_FilterScale_32bit)
  {
    /* 32-bit scale for the filter */
    CAN1->FS1R |= filter_number_bit_pos;
    /* 32-bit identifier or First 32-bit identifier */
    CAN1->sFilterRegister[CAN_FilterInitStruct->CAN_FilterNumber].FR1 = 
       ((0x0000FFFF & (uint32_t)CAN_FilterInitStruct->CAN_FilterIdHigh) << 16) |
        (0x0000FFFF & (uint32_t)CAN_FilterInitStruct->CAN_FilterIdLow);
    /* 32-bit mask or Second 32-bit identifier */
    CAN1->sFilterRegister[CAN_FilterInitStruct->CAN_FilterNumber].FR2 = 
       ((0x0000FFFF & (uint32_t)CAN_FilterInitStruct->CAN_FilterMaskIdHigh) << 16) |
        (0x0000FFFF & (uint32_t)CAN_FilterInitStruct->CAN_FilterMaskIdLow);
  }

  /* Filter Mode */
  if (CAN_FilterInitStruct->CAN_FilterMode == CAN_FilterMode_IdMask)
  {
    /*Id/Mask mode for the filter*/
    CAN1->FM1R &= ~(uint32_t)filter_number_bit_pos;
  }
  else /* CAN_FilterInitStruct->CAN_FilterMode == CAN_FilterMode_IdList */
  {
    /*Identifier list mode for the filter*/
    CAN1->FM1R |= (uint32_t)filter_number_bit_pos;
  }

  /* Filter FIFO assignment */
  if (CAN_FilterInitStruct->CAN_FilterFIFOAssignment == CAN_Filter_FIFO0)
  {
    /* FIFO 0 assignation for the filter */
    CAN1->FFA1R &= ~(uint32_t)filter_number_bit_pos;
  }

  if (CAN_FilterInitStruct->CAN_FilterFIFOAssignment == CAN_Filter_FIFO1)
  {
    /* FIFO 1 assignation for the filter */
    CAN1->FFA1R |= (uint32_t)filter_number_bit_pos;
  }
  
  /* Filter activation */
  if (CAN_FilterInitStruct->CAN_FilterActivation == ENABLE)
  {
    CAN1->FA1R |= filter_number_bit_pos;
  }

  /* Leave the initialisation mode for the filter */
  CAN1->FMR &= ~FMR_FINIT;
}


/** @defgroup CAN_Group6 Interrupts and flags management functions
 *  @brief   Interrupts and flags management functions
 *
@verbatim   
 ===============================================================================
              ##### Interrupts and flags management functions #####
 ===============================================================================  

     [..] This section provides functions allowing to configure the CAN Interrupts 
          and to get the status and clear flags and Interrupts pending bits.
  
          The CAN provides 14 Interrupts sources and 15 Flags:

   
  *** Flags ***
  =============
    [..] The 15 flags can be divided on 4 groups: 

      (+) Transmit Flags
        (++) CAN_FLAG_RQCP0, 
        (++) CAN_FLAG_RQCP1, 
        (++) CAN_FLAG_RQCP2  : Request completed MailBoxes 0, 1 and 2  Flags
                               Set when the last request (transmit or abort)
                               has been performed. 

      (+) Receive Flags


        (++) CAN_FLAG_FMP0,
        (++) CAN_FLAG_FMP1   : FIFO 0 and 1 Message Pending Flags 
                               set to signal that messages are pending in the receive 
                               FIFO.
                               These Flags are cleared only by hardware. 

        (++) CAN_FLAG_FF0,
        (++) CAN_FLAG_FF1    : FIFO 0 and 1 Full Flags
                               set when three messages are stored in the selected 
                               FIFO.                        

        (++) CAN_FLAG_FOV0              
        (++) CAN_FLAG_FOV1   : FIFO 0 and 1 Overrun Flags
                               set when a new message has been received and passed 
                               the filter while the FIFO was full.         

      (+) Operating Mode Flags

        (++) CAN_FLAG_WKU    : Wake up Flag
                               set to signal that a SOF bit has been detected while 
                               the CAN hardware was in Sleep mode. 
        
        (++) CAN_FLAG_SLAK   : Sleep acknowledge Flag
                               Set to signal that the CAN has entered Sleep Mode. 
    
      (+) Error Flags

        (++) CAN_FLAG_EWG    : Error Warning Flag
                               Set when the warning limit has been reached (Receive 
                               Error Counter or Transmit Error Counter greater than 96). 
                               This Flag is cleared only by hardware.
                            
        (++) CAN_FLAG_EPV    : Error Passive Flag
                               Set when the Error Passive limit has been reached 
                               (Receive Error Counter or Transmit Error Counter 
                               greater than 127).
                               This Flag is cleared only by hardware.
                             
        (++) CAN_FLAG_BOF    : Bus-Off Flag
                               set when CAN enters the bus-off state. The bus-off 
                               state is entered on TEC overflow, greater than 255.
                               This Flag is cleared only by hardware.
                                   
        (++) CAN_FLAG_LEC    : Last error code Flag
                               set If a message has been transferred (reception or
                               transmission) with error, and the error code is hold.              
                           
  *** Interrupts ***
  ==================
    [..] The 14 interrupts can be divided on 4 groups: 
  
      (+) Transmit interrupt
  
        (++) CAN_IT_TME   :  Transmit mailbox empty Interrupt
                             if enabled, this interrupt source is pending when 
                             no transmit request are pending for Tx mailboxes.      

      (+) Receive Interrupts
         
        (++) CAN_IT_FMP0,
        (++) CAN_IT_FMP1    :  FIFO 0 and FIFO1 message pending Interrupts
                               if enabled, these interrupt sources are pending 
                               when messages are pending in the receive FIFO.
                               The corresponding interrupt pending bits are cleared 
                               only by hardware.
                
        (++) CAN_IT_FF0,              
        (++) CAN_IT_FF1     :  FIFO 0 and FIFO1 full Interrupts
                               if enabled, these interrupt sources are pending 
                               when three messages are stored in the selected FIFO.
        
        (++) CAN_IT_FOV0,        
        (++) CAN_IT_FOV1    :  FIFO 0 and FIFO1 overrun Interrupts        
                               if enabled, these interrupt sources are pending 
                               when a new message has been received and passed 
                               the filter while the FIFO was full.

      (+) Operating Mode Interrupts
         
        (++) CAN_IT_WKU     :  Wake-up Interrupt
                               if enabled, this interrupt source is pending when 
                               a SOF bit has been detected while the CAN hardware 
                               was in Sleep mode.
                                  
        (++) CAN_IT_SLK     :  Sleep acknowledge Interrupt
                               if enabled, this interrupt source is pending when 
                               the CAN has entered Sleep Mode.       

      (+) Error Interrupts 
        
        (++) CAN_IT_EWG     :  Error warning Interrupt 
                               if enabled, this interrupt source is pending when
                               the warning limit has been reached (Receive Error 
                               Counter or Transmit Error Counter=96). 
                               
        (++) CAN_IT_EPV     :  Error passive Interrupt        
                               if enabled, this interrupt source is pending when
                               the Error Passive limit has been reached (Receive 
                               Error Counter or Transmit Error Counter>127).
                          
        (++) CAN_IT_BOF     :  Bus-off Interrupt
                               if enabled, this interrupt source is pending when
                               CAN enters the bus-off state. The bus-off state is 
                               entered on TEC overflow, greater than 255.
                               This Flag is cleared only by hardware.
                                  
        (++) CAN_IT_LEC     :  Last error code Interrupt        
                               if enabled, this interrupt source is pending  when
                               a message has been transferred (reception or
                               transmission) with error, and the error code is hold.
                          
        (++) CAN_IT_ERR     :  Error Interrupt
                               if enabled, this interrupt source is pending when 
                               an error condition is pending.      
                      
    [..] Managing the CAN controller events :
 
         The user should identify which mode will be used in his application to 
         manage the CAN controller events: Polling mode or Interrupt mode.
  
      (#) In the Polling Mode it is advised to use the following functions:
        (++) CAN_GetFlagStatus() : to check if flags events occur. 
        (++) CAN_ClearFlag()     : to clear the flags events.
  

  
      (#) In the Interrupt Mode it is advised to use the following functions:
        (++) CAN_ITConfig()       : to enable or disable the interrupt source.
        (++) CAN_GetITStatus()    : to check if Interrupt occurs.
        (++) CAN_ClearITPendingBit() : to clear the Interrupt pending Bit 
            (corresponding Flag).
        -@@-  This function has no impact on CAN_IT_FMP0 and CAN_IT_FMP1 Interrupts 
             pending bits since there are cleared only by hardware. 
  
@endverbatim
  * @{
  */ 
/**
  * @brief  Enables or disables the specified CANx interrupts.
  * @param  CANx: where x can be 1,2 or 3 to select the CAN peripheral.
  * @param  CAN_IT: specifies the CAN interrupt sources to be enabled or disabled.
  *          This parameter can be: 
  *            @arg CAN_IT_TME: Transmit mailbox empty Interrupt 
  *            @arg CAN_IT_FMP0: FIFO 0 message pending Interrupt 
  *            @arg CAN_IT_FF0: FIFO 0 full Interrupt
  *            @arg CAN_IT_FOV0: FIFO 0 overrun Interrupt
  *            @arg CAN_IT_FMP1: FIFO 1 message pending Interrupt 
  *            @arg CAN_IT_FF1: FIFO 1 full Interrupt
  *            @arg CAN_IT_FOV1: FIFO 1 overrun Interrupt
  *            @arg CAN_IT_WKU: Wake-up Interrupt
  *            @arg CAN_IT_SLK: Sleep acknowledge Interrupt  
  *            @arg CAN_IT_EWG: Error warning Interrupt
  *            @arg CAN_IT_EPV: Error passive Interrupt
  *            @arg CAN_IT_BOF: Bus-off Interrupt  
  *            @arg CAN_IT_LEC: Last error code Interrupt
  *            @arg CAN_IT_ERR: Error Interrupt
  * @param  NewState: new state of the CAN interrupts.
  * @note   CAN3 peripheral is available only for STM32F413_423xx devices
  *          This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
void CAN_ITConfig(CAN_TypeDef* CANx, uint32_t CAN_IT, FunctionalState NewState)
{
  if (NewState != DISABLE)
  {
    /* Enable the selected CANx interrupt */
    CANx->IER |= CAN_IT;
  }
  else
  {
    /* Disable the selected CANx interrupt */
    CANx->IER &= ~CAN_IT;
  }
}


/**
  * @brief  Checks whether the specified CAN flag is set or not.
  * @param  CANx: where x can be 1,2 or 3 to select the CAN peripheral.
  * @param  CAN_FLAG: specifies the flag to check.
  *          This parameter can be one of the following values:
  *            @arg CAN_FLAG_RQCP0: Request MailBox0 Flag
  *            @arg CAN_FLAG_RQCP1: Request MailBox1 Flag
  *            @arg CAN_FLAG_RQCP2: Request MailBox2 Flag
  *            @arg CAN_FLAG_FMP0: FIFO 0 Message Pending Flag   
  *            @arg CAN_FLAG_FF0: FIFO 0 Full Flag       
  *            @arg CAN_FLAG_FOV0: FIFO 0 Overrun Flag 
  *            @arg CAN_FLAG_FMP1: FIFO 1 Message Pending Flag   
  *            @arg CAN_FLAG_FF1: FIFO 1 Full Flag        
  *            @arg CAN_FLAG_FOV1: FIFO 1 Overrun Flag     
  *            @arg CAN_FLAG_WKU: Wake up Flag
  *            @arg CAN_FLAG_SLAK: Sleep acknowledge Flag 
  *            @arg CAN_FLAG_EWG: Error Warning Flag
  *            @arg CAN_FLAG_EPV: Error Passive Flag  
  *            @arg CAN_FLAG_BOF: Bus-Off Flag    
  *            @arg CAN_FLAG_LEC: Last error code Flag
  * @note   CAN3 peripheral is available only for STM32F413_423xx devices
  * @retval The new state of CAN_FLAG (SET or RESET).
  */
FlagStatus CAN_GetFlagStatus(CAN_TypeDef* CANx, uint32_t CAN_FLAG)
{
  FlagStatus bitstatus = RESET;
  
  /* Check the parameters */
//  assert_param(IS_CAN_ALL_PERIPH(CANx));
//  assert_param(IS_CAN_GET_FLAG(CAN_FLAG));
  

  if((CAN_FLAG & CAN_FLAGS_ESR) != (uint32_t)RESET)
  { 
    /* Check the status of the specified CAN flag */
    if ((CANx->ESR & (CAN_FLAG & 0x000FFFFF)) != (uint32_t)RESET)
    { 
      /* CAN_FLAG is set */
      bitstatus = SET;
    }
    else
    { 
      /* CAN_FLAG is reset */
      bitstatus = RESET;
    }
  }
  else if((CAN_FLAG & CAN_FLAGS_MSR) != (uint32_t)RESET)
  { 
    /* Check the status of the specified CAN flag */
    if ((CANx->MSR & (CAN_FLAG & 0x000FFFFF)) != (uint32_t)RESET)
    { 
      /* CAN_FLAG is set */
      bitstatus = SET;
    }
    else
    { 
      /* CAN_FLAG is reset */
      bitstatus = RESET;
    }
  }
  else if((CAN_FLAG & CAN_FLAGS_TSR) != (uint32_t)RESET)
  { 
    /* Check the status of the specified CAN flag */
    if ((CANx->TSR & (CAN_FLAG & 0x000FFFFF)) != (uint32_t)RESET)
    { 
      /* CAN_FLAG is set */
      bitstatus = SET;
    }
    else
    { 
      /* CAN_FLAG is reset */
      bitstatus = RESET;
    }
  }
  else if((CAN_FLAG & CAN_FLAGS_RF0R) != (uint32_t)RESET)
  { 
    /* Check the status of the specified CAN flag */
    if ((CANx->RF0R & (CAN_FLAG & 0x000FFFFF)) != (uint32_t)RESET)
    { 
      /* CAN_FLAG is set */
      bitstatus = SET;
    }
    else
    { 
      /* CAN_FLAG is reset */
      bitstatus = RESET;
    }
  }
  else /* If(CAN_FLAG & CAN_FLAGS_RF1R != (uint32_t)RESET) */
  { 
    /* Check the status of the specified CAN flag */
    if ((uint32_t)(CANx->RF1R & (CAN_FLAG & 0x000FFFFF)) != (uint32_t)RESET)
    { 
      /* CAN_FLAG is set */
      bitstatus = SET;
    }
    else
    { 
      /* CAN_FLAG is reset */
      bitstatus = RESET;
    }
  }
  /* Return the CAN_FLAG status */
  return  bitstatus;
}

/**
  * @brief  Clears the CAN's pending flags.
  * @param  CANx: where x can be 1,2 or 3 to select the CAN peripheral.
  * @param  CAN_FLAG: specifies the flag to clear.
  *          This parameter can be one of the following values:
  *            @arg CAN_FLAG_RQCP0: Request MailBox0 Flag
  *            @arg CAN_FLAG_RQCP1: Request MailBox1 Flag
  *            @arg CAN_FLAG_RQCP2: Request MailBox2 Flag 
  *            @arg CAN_FLAG_FF0: FIFO 0 Full Flag       
  *            @arg CAN_FLAG_FOV0: FIFO 0 Overrun Flag  
  *            @arg CAN_FLAG_FF1: FIFO 1 Full Flag        
  *            @arg CAN_FLAG_FOV1: FIFO 1 Overrun Flag     
  *            @arg CAN_FLAG_WKU: Wake up Flag
  *            @arg CAN_FLAG_SLAK: Sleep acknowledge Flag    
  *            @arg CAN_FLAG_LEC: Last error code Flag
  * @note   CAN3 peripheral is available only for STM32F413_423xx devices
  * @retval None
  */
void CAN_ClearFlag(CAN_TypeDef* CANx, uint32_t CAN_FLAG)
{
  uint32_t flagtmp=0;
  /* Check the parameters */
 // assert_param(IS_CAN_ALL_PERIPH(CANx));
 // assert_param(IS_CAN_CLEAR_FLAG(CAN_FLAG));
  
  if (CAN_FLAG == CAN_FLAG_LEC) /* ESR register */
  {
    /* Clear the selected CAN flags */
    CANx->ESR = (uint32_t)RESET;
  }
  else /* MSR or TSR or RF0R or RF1R */
  {
    flagtmp = CAN_FLAG & 0x000FFFFF;

    if ((CAN_FLAG & CAN_FLAGS_RF0R)!=(uint32_t)RESET)
    {
      /* Receive Flags */
      CANx->RF0R = (uint32_t)(flagtmp);
    }
    else if ((CAN_FLAG & CAN_FLAGS_RF1R)!=(uint32_t)RESET)
    {
      /* Receive Flags */
      CANx->RF1R = (uint32_t)(flagtmp);
    }
    else if ((CAN_FLAG & CAN_FLAGS_TSR)!=(uint32_t)RESET)
    {
      /* Transmit Flags */
      CANx->TSR = (uint32_t)(flagtmp);
    }
    else /* If((CAN_FLAG & CAN_FLAGS_MSR)!=(uint32_t)RESET) */
    {
      /* Operating mode Flags */
      CANx->MSR = (uint32_t)(flagtmp);
    }
  }
}

/**
  * @brief  Checks whether the specified CANx interrupt has occurred or not.
  * @param  CANx: where x can be 1,2 or 3 to select the CAN peripheral.
  * @param  CAN_IT: specifies the CAN interrupt source to check.
  *          This parameter can be one of the following values:
  *            @arg CAN_IT_TME: Transmit mailbox empty Interrupt 
  *            @arg CAN_IT_FMP0: FIFO 0 message pending Interrupt 
  *            @arg CAN_IT_FF0: FIFO 0 full Interrupt
  *            @arg CAN_IT_FOV0: FIFO 0 overrun Interrupt
  *            @arg CAN_IT_FMP1: FIFO 1 message pending Interrupt 
  *            @arg CAN_IT_FF1: FIFO 1 full Interrupt
  *            @arg CAN_IT_FOV1: FIFO 1 overrun Interrupt
  *            @arg CAN_IT_WKU: Wake-up Interrupt
  *            @arg CAN_IT_SLK: Sleep acknowledge Interrupt  
  *            @arg CAN_IT_EWG: Error warning Interrupt
  *            @arg CAN_IT_EPV: Error passive Interrupt
  *            @arg CAN_IT_BOF: Bus-off Interrupt  
  *            @arg CAN_IT_LEC: Last error code Interrupt
  *            @arg CAN_IT_ERR: Error Interrupt
  * @note   CAN3 peripheral is available only for STM32F413_423xx devices
  * @retval The current state of CAN_IT (SET or RESET).
  */
ITStatus CAN_GetITStatus(CAN_TypeDef* CANx, uint32_t CAN_IT)
{
  ITStatus itstatus = RESET;
  /* Check the parameters */
 // assert_param(IS_CAN_ALL_PERIPH(CANx));
 // assert_param(IS_CAN_IT(CAN_IT));
  
  /* check the interrupt enable bit */
 if((CANx->IER & CAN_IT) != RESET)
 {
   /* in case the Interrupt is enabled, .... */
    switch (CAN_IT)
    {
      case CAN_IT_TME:
        /* Check CAN_TSR_RQCPx bits */
        itstatus = CheckITStatus(CANx->TSR, CAN_TSR_RQCP0|CAN_TSR_RQCP1|CAN_TSR_RQCP2);  
        break;
      case CAN_IT_FMP0:
        /* Check CAN_RF0R_FMP0 bit */
        itstatus = CheckITStatus(CANx->RF0R, CAN_RF0R_FMP0);  
        break;
      case CAN_IT_FF0:
        /* Check CAN_RF0R_FULL0 bit */
        itstatus = CheckITStatus(CANx->RF0R, CAN_RF0R_FULL0);  
        break;
      case CAN_IT_FOV0:
        /* Check CAN_RF0R_FOVR0 bit */
        itstatus = CheckITStatus(CANx->RF0R, CAN_RF0R_FOVR0);  
        break;
      case CAN_IT_FMP1:
        /* Check CAN_RF1R_FMP1 bit */
        itstatus = CheckITStatus(CANx->RF1R, CAN_RF1R_FMP1);  
        break;
      case CAN_IT_FF1:
        /* Check CAN_RF1R_FULL1 bit */
        itstatus = CheckITStatus(CANx->RF1R, CAN_RF1R_FULL1);  
        break;
      case CAN_IT_FOV1:
        /* Check CAN_RF1R_FOVR1 bit */
        itstatus = CheckITStatus(CANx->RF1R, CAN_RF1R_FOVR1);  
        break;
      case CAN_IT_WKU:
        /* Check CAN_MSR_WKUI bit */
        itstatus = CheckITStatus(CANx->MSR, CAN_MSR_WKUI);  
        break;
      case CAN_IT_SLK:
        /* Check CAN_MSR_SLAKI bit */
        itstatus = CheckITStatus(CANx->MSR, CAN_MSR_SLAKI);  
        break;
      case CAN_IT_EWG:
        /* Check CAN_ESR_EWGF bit */
        itstatus = CheckITStatus(CANx->ESR, CAN_ESR_EWGF);  
        break;
      case CAN_IT_EPV:
        /* Check CAN_ESR_EPVF bit */
        itstatus = CheckITStatus(CANx->ESR, CAN_ESR_EPVF);  
        break;
      case CAN_IT_BOF:
        /* Check CAN_ESR_BOFF bit */
        itstatus = CheckITStatus(CANx->ESR, CAN_ESR_BOFF);  
        break;
      case CAN_IT_LEC:
        /* Check CAN_ESR_LEC bit */
        itstatus = CheckITStatus(CANx->ESR, CAN_ESR_LEC);  
        break;
      case CAN_IT_ERR:
        /* Check CAN_MSR_ERRI bit */ 
        itstatus = CheckITStatus(CANx->MSR, CAN_MSR_ERRI); 
        break;
      default:
        /* in case of error, return RESET */
        itstatus = RESET;
        break;
    }
  }
  else
  {
   /* in case the Interrupt is not enabled, return RESET */
    itstatus  = RESET;
  }
  
  /* Return the CAN_IT status */
  return  itstatus;
}

/**
  * @brief  Clears the CANx's interrupt pending bits.
  * @param  CANx: where x can be 1,2 or 3 to select the CAN peripheral.
  * @param  CAN_IT: specifies the interrupt pending bit to clear.
  *          This parameter can be one of the following values:
  *            @arg CAN_IT_TME: Transmit mailbox empty Interrupt
  *            @arg CAN_IT_FF0: FIFO 0 full Interrupt
  *            @arg CAN_IT_FOV0: FIFO 0 overrun Interrupt
  *            @arg CAN_IT_FF1: FIFO 1 full Interrupt
  *            @arg CAN_IT_FOV1: FIFO 1 overrun Interrupt
  *            @arg CAN_IT_WKU: Wake-up Interrupt
  *            @arg CAN_IT_SLK: Sleep acknowledge Interrupt  
  *            @arg CAN_IT_EWG: Error warning Interrupt
  *            @arg CAN_IT_EPV: Error passive Interrupt
  *            @arg CAN_IT_BOF: Bus-off Interrupt  
  *            @arg CAN_IT_LEC: Last error code Interrupt
  *            @arg CAN_IT_ERR: Error Interrupt
  * @note   CAN3 peripheral is available only for STM32F413_423xx devices
  * @retval None
  */
void CAN_ClearITPendingBit(CAN_TypeDef* CANx, uint32_t CAN_IT)
{
  /* Check the parameters */
 // assert_param(IS_CAN_ALL_PERIPH(CANx));
 // assert_param(IS_CAN_CLEAR_IT(CAN_IT));

  switch (CAN_IT)
  {
    case CAN_IT_TME:
      /* Clear CAN_TSR_RQCPx (rc_w1)*/
      CANx->TSR = CAN_TSR_RQCP0|CAN_TSR_RQCP1|CAN_TSR_RQCP2;  
      break;
    case CAN_IT_FF0:
      /* Clear CAN_RF0R_FULL0 (rc_w1)*/
      CANx->RF0R = CAN_RF0R_FULL0; 
      break;
    case CAN_IT_FOV0:
      /* Clear CAN_RF0R_FOVR0 (rc_w1)*/
      CANx->RF0R = CAN_RF0R_FOVR0; 
      break;
    case CAN_IT_FF1:
      /* Clear CAN_RF1R_FULL1 (rc_w1)*/
      CANx->RF1R = CAN_RF1R_FULL1;  
      break;
    case CAN_IT_FOV1:
      /* Clear CAN_RF1R_FOVR1 (rc_w1)*/
      CANx->RF1R = CAN_RF1R_FOVR1; 
      break;
    case CAN_IT_WKU:
      /* Clear CAN_MSR_WKUI (rc_w1)*/
      CANx->MSR = CAN_MSR_WKUI;  
      break;
    case CAN_IT_SLK:
      /* Clear CAN_MSR_SLAKI (rc_w1)*/ 
      CANx->MSR = CAN_MSR_SLAKI;   
      break;
    case CAN_IT_EWG:
      /* Clear CAN_MSR_ERRI (rc_w1) */
      CANx->MSR = CAN_MSR_ERRI;
       /* @note the corresponding Flag is cleared by hardware depending on the CAN Bus status*/ 
      break;
    case CAN_IT_EPV:
      /* Clear CAN_MSR_ERRI (rc_w1) */
      CANx->MSR = CAN_MSR_ERRI; 
       /* @note the corresponding Flag is cleared by hardware depending on the CAN Bus status*/
      break;
    case CAN_IT_BOF:
      /* Clear CAN_MSR_ERRI (rc_w1) */ 
      CANx->MSR = CAN_MSR_ERRI; 
       /* @note the corresponding Flag is cleared by hardware depending on the CAN Bus status*/
       break;
    case CAN_IT_LEC:
      /*  Clear LEC bits */
      CANx->ESR = RESET; 
      /* Clear CAN_MSR_ERRI (rc_w1) */
      CANx->MSR = CAN_MSR_ERRI; 
      break;
    case CAN_IT_ERR:
      /*Clear LEC bits */
      CANx->ESR = RESET; 
      /* Clear CAN_MSR_ERRI (rc_w1) */
      CANx->MSR = CAN_MSR_ERRI; 
       /* @note BOFF, EPVF and EWGF Flags are cleared by hardware depending on the CAN Bus status*/
       break;
    default:
       break;
   }
}
 /**
  * @}
  */

/**
  * @brief  Checks whether the CAN interrupt has occurred or not.
  * @param  CAN_Reg: specifies the CAN interrupt register to check.
  * @param  It_Bit: specifies the interrupt source bit to check.
  * @retval The new state of the CAN Interrupt (SET or RESET).
  */
static ITStatus CheckITStatus(uint32_t CAN_Reg, uint32_t It_Bit)
{
  ITStatus pendingbitstatus = RESET;
  
  if ((CAN_Reg & It_Bit) != (uint32_t)RESET)
  {
    /* CAN_IT is set */
    pendingbitstatus = SET;
  }
  else
  {
    /* CAN_IT is reset */
    pendingbitstatus = RESET;
  }
  return pendingbitstatus;
}



















/**
  * @brief  Initiates and transmits a CAN frame message.
  * @param  CANx: where x can be 1,2 or 3 to select the CAN peripheral.
  * @param  TxMessage: pointer to a structure which contains CAN Id, CAN DLC and CAN data.
  * @note   CAN3 peripheral is available only for STM32F413_423xx devices
  * @retval The number of the mailbox that is used for transmission or
  *         CAN_TxStatus_NoMailBox if there is no empty mailbox.
  */
uint8_t CAN_Transmit(CAN_TypeDef* CANx, CanTxMsg* TxMessage)
{
  uint8_t transmit_mailbox = 0;

  /* Select one empty transmit mailbox */
  if ((CANx->TSR&CAN_TSR_TME0) == CAN_TSR_TME0)
  {
    transmit_mailbox = 0;
  }
  else if ((CANx->TSR&CAN_TSR_TME1) == CAN_TSR_TME1)
  {
    transmit_mailbox = 1;
  }
  else if ((CANx->TSR&CAN_TSR_TME2) == CAN_TSR_TME2)
  {
    transmit_mailbox = 2;
  }
  else
  {
    transmit_mailbox = CAN_TxStatus_NoMailBox;
  }

  if (transmit_mailbox != CAN_TxStatus_NoMailBox)
  {
    /* Set up the Id */
    CANx->sTxMailBox[transmit_mailbox].TIR &= TMIDxR_TXRQ;
    if (TxMessage->IDE == CAN_Id_Standard)
    {
      //assert_param(IS_CAN_STDID(TxMessage->StdId));  
      CANx->sTxMailBox[transmit_mailbox].TIR |= ((TxMessage->StdId << 21) | \
                                                  TxMessage->RTR);
    }
    else
    {
      //assert_param(IS_CAN_EXTID(TxMessage->ExtId));
      CANx->sTxMailBox[transmit_mailbox].TIR |= ((TxMessage->ExtId << 3) | \
                                                  TxMessage->IDE | \
                                                  TxMessage->RTR);
    }
    
    /* Set up the DLC */
    TxMessage->DLC &= (uint8_t)0x0000000F;
    CANx->sTxMailBox[transmit_mailbox].TDTR &= (uint32_t)0xFFFFFFF0;
    CANx->sTxMailBox[transmit_mailbox].TDTR |= TxMessage->DLC;

    /* Set up the data field */
    CANx->sTxMailBox[transmit_mailbox].TDLR = (((uint32_t)TxMessage->Data[3] << 24) | 
                                             ((uint32_t)TxMessage->Data[2] << 16) |
                                             ((uint32_t)TxMessage->Data[1] << 8) | 
                                             ((uint32_t)TxMessage->Data[0]));
    CANx->sTxMailBox[transmit_mailbox].TDHR = (((uint32_t)TxMessage->Data[7] << 24) | 
                                             ((uint32_t)TxMessage->Data[6] << 16) |
                                             ((uint32_t)TxMessage->Data[5] << 8) |
                                             ((uint32_t)TxMessage->Data[4]));
    /* Request transmission */
    CANx->sTxMailBox[transmit_mailbox].TIR |= TMIDxR_TXRQ;
  }
  return transmit_mailbox;
}

/**
  * @brief  Checks the transmission status of a CAN Frame.
  * @param  CANx: where x can be 1,2 or 3 to select the CAN peripheral.
  * @param  TransmitMailbox: the number of the mailbox that is used for transmission.
  * @note   CAN3 peripheral is available only for STM32F413_423xx devices
  * @retval CAN_TxStatus_Ok if the CAN driver transmits the message, 
  *         CAN_TxStatus_Failed in an other case.
  */
uint8_t CAN_TransmitStatus(CAN_TypeDef* CANx, uint8_t TransmitMailbox)
{
  uint32_t state = 0;

  /* Check the parameters */
//  assert_param(IS_CAN_ALL_PERIPH(CANx));
//  assert_param(IS_CAN_TRANSMITMAILBOX(TransmitMailbox));
 
  switch (TransmitMailbox)
  {
    case (CAN_TXMAILBOX_0): 
      state =   CANx->TSR &  (CAN_TSR_RQCP0 | CAN_TSR_TXOK0 | CAN_TSR_TME0);
      break;
    case (CAN_TXMAILBOX_1): 
      state =   CANx->TSR &  (CAN_TSR_RQCP1 | CAN_TSR_TXOK1 | CAN_TSR_TME1);
      break;
    case (CAN_TXMAILBOX_2): 
      state =   CANx->TSR &  (CAN_TSR_RQCP2 | CAN_TSR_TXOK2 | CAN_TSR_TME2);
      break;
    default:
      state = CAN_TxStatus_Failed;
      break;
  }
  switch (state)
  {
      /* transmit pending  */
    case (0x0): state = CAN_TxStatus_Pending;
      break;
      /* transmit failed  */
     case (CAN_TSR_RQCP0 | CAN_TSR_TME0): state = CAN_TxStatus_Failed;
      break;
     case (CAN_TSR_RQCP1 | CAN_TSR_TME1): state = CAN_TxStatus_Failed;
      break;
     case (CAN_TSR_RQCP2 | CAN_TSR_TME2): state = CAN_TxStatus_Failed;
      break;
      /* transmit succeeded  */
    case (CAN_TSR_RQCP0 | CAN_TSR_TXOK0 | CAN_TSR_TME0):state = CAN_TxStatus_Ok;
      break;
    case (CAN_TSR_RQCP1 | CAN_TSR_TXOK1 | CAN_TSR_TME1):state = CAN_TxStatus_Ok;
      break;
    case (CAN_TSR_RQCP2 | CAN_TSR_TXOK2 | CAN_TSR_TME2):state = CAN_TxStatus_Ok;
      break;
    default: state = CAN_TxStatus_Failed;
      break;
  }
  return (uint8_t) state;
}

/**
  * @brief  Cancels a transmit request.
  * @param  CANx: where x can be 1,2 or 3 to select the CAN peripheral.
  * @param  Mailbox: Mailbox number.
  * @note   CAN3 peripheral is available only for STM32F413_423xx devices
  * @retval None
  */
void CAN_CancelTransmit(CAN_TypeDef* CANx, uint8_t Mailbox)
{
  /* Check the parameters */
  //assert_param(IS_CAN_ALL_PERIPH(CANx));
  //assert_param(IS_CAN_TRANSMITMAILBOX(Mailbox));
  /* abort transmission */
  switch (Mailbox)
  {
    case (CAN_TXMAILBOX_0): CANx->TSR |= CAN_TSR_ABRQ0;
      break;
    case (CAN_TXMAILBOX_1): CANx->TSR |= CAN_TSR_ABRQ1;
      break;
    case (CAN_TXMAILBOX_2): CANx->TSR |= CAN_TSR_ABRQ2;
      break;
    default:
      break;
  }
}
/**
  * @}
  */

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
  uint8_t tmppriority = 0x00, tmppre = 0x00, tmpsub = 0x0F;

  CAN_NVIC_InitTypeDef  NVIC_InitStruct;

  NVIC_InitStruct.NVIC_IRQChannel = CAN1_RX0_IRQn;
  NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 0x0;
  NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0x0;
  NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;

  
  /* Check the parameters */
  //assert_param(IS_FUNCTIONAL_STATE(NVIC_InitStruct->NVIC_IRQChannelCmd));
  //assert_param(IS_NVIC_PREEMPTION_PRIORITY(NVIC_InitStruct->NVIC_IRQChannelPreemptionPriority));  
  //assert_param(IS_NVIC_SUB_PRIORITY(NVIC_InitStruct->NVIC_IRQChannelSubPriority));
    
  if (NVIC_InitStruct.NVIC_IRQChannelCmd != DISABLE)
  {
    /* Compute the Corresponding IRQ Priority --------------------------------*/    
    tmppriority = (0x700 - ((SCB->AIRCR) & (uint32_t)0x700))>> 0x08;
    tmppre = (0x4 - tmppriority);
    tmpsub = tmpsub >> tmppriority;

    tmppriority = NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority << tmppre;
    tmppriority |=  (uint8_t)(NVIC_InitStruct.NVIC_IRQChannelSubPriority & tmpsub);
        
    tmppriority = tmppriority << 0x04;
        
    NVIC->IP[NVIC_InitStruct.NVIC_IRQChannel] = tmppriority;
    
    /* Enable the Selected IRQ Channels --------------------------------------*/
    NVIC->ISER[NVIC_InitStruct.NVIC_IRQChannel >> 0x05] =
      (uint32_t)0x01 << (NVIC_InitStruct.NVIC_IRQChannel & (uint8_t)0x1F);
  }
  else
  {
    /* Disable the Selected IRQ Channels -------------------------------------*/
    NVIC->ICER[NVIC_InitStruct.NVIC_IRQChannel >> 0x05] =
      (uint32_t)0x01 << (NVIC_InitStruct.NVIC_IRQChannel & (uint8_t)0x1F);
  }
}





/**
  * @brief  Receives a correct CAN frame.
  * @param  CANx: where x can be 1,2 or 3 to select the CAN peripheral.
  * @param  FIFONumber: Receive FIFO number, CAN_FIFO0 or CAN_FIFO1.
  * @param  RxMessage: pointer to a structure receive frame which contains CAN Id,
  *         CAN DLC, CAN data and FMI number.
  * @note   CAN3 peripheral is available only for STM32F413_423xx devices
  * @retval None
  */
void CAN_Receive(CAN_TypeDef* CANx, uint8_t FIFONumber, CanRxMsg* RxMessage)
{
	flagReceived=1;
  /* Check the parameters */
//  assert_param(IS_CAN_ALL_PERIPH(CANx));
  //assert_param(IS_CAN_FIFO(FIFONumber));
  /* Get the Id */
  RxMessage->IDE = (uint8_t)0x04 & CANx->sFIFOMailBox[FIFONumber].RIR;
  if (RxMessage->IDE == CAN_Id_Standard)
  {
    RxMessage->StdId = (uint32_t)0x000007FF & (CANx->sFIFOMailBox[FIFONumber].RIR >> 21);
  }
  else
  {
    RxMessage->ExtId = (uint32_t)0x1FFFFFFF & (CANx->sFIFOMailBox[FIFONumber].RIR >> 3);
  }
  
  RxMessage->RTR = (uint8_t)0x02 & CANx->sFIFOMailBox[FIFONumber].RIR;
  /* Get the DLC */
  RxMessage->DLC = (uint8_t)0x0F & CANx->sFIFOMailBox[FIFONumber].RDTR;
  /* Get the FMI */
  RxMessage->FMI = (uint8_t)0xFF & (CANx->sFIFOMailBox[FIFONumber].RDTR >> 8);
  /* Get the data field */
  RxMessage->Data[0] = (uint8_t)0xFF & CANx->sFIFOMailBox[FIFONumber].RDLR;
  RxMessage->Data[1] = (uint8_t)0xFF & (CANx->sFIFOMailBox[FIFONumber].RDLR >> 8);
  RxMessage->Data[2] = (uint8_t)0xFF & (CANx->sFIFOMailBox[FIFONumber].RDLR >> 16);
  RxMessage->Data[3] = (uint8_t)0xFF & (CANx->sFIFOMailBox[FIFONumber].RDLR >> 24);
  RxMessage->Data[4] = (uint8_t)0xFF & CANx->sFIFOMailBox[FIFONumber].RDHR;
  RxMessage->Data[5] = (uint8_t)0xFF & (CANx->sFIFOMailBox[FIFONumber].RDHR >> 8);
  RxMessage->Data[6] = (uint8_t)0xFF & (CANx->sFIFOMailBox[FIFONumber].RDHR >> 16);
  RxMessage->Data[7] = (uint8_t)0xFF & (CANx->sFIFOMailBox[FIFONumber].RDHR >> 24);
  /* Release the FIFO */
  /* Release FIFO0 */
  if (FIFONumber == CAN_FIFO0)
  {
    CANx->RF0R |= CAN_RF0R_RFOM0;
  }
  /* Release FIFO1 */
  else /* FIFONumber == CAN_FIFO1 */
  {
    CANx->RF1R |= CAN_RF1R_RFOM1;
  }
}

/**
  * @brief  Releases the specified receive FIFO.
  * @param  CANx: where x can be 1,2 or 3 to select the CAN peripheral.
  * @param  FIFONumber: FIFO to release, CAN_FIFO0 or CAN_FIFO1.
  * @note   CAN3 peripheral is available only for STM32F413_423xx devices
  * @retval None
  */
void CAN_FIFORelease(CAN_TypeDef* CANx, uint8_t FIFONumber)
{
  /* Check the parameters */
//  assert_param(IS_CAN_ALL_PERIPH(CANx));
  //assert_param(IS_CAN_FIFO(FIFONumber));
  /* Release FIFO0 */
  if (FIFONumber == CAN_FIFO0)
  {
    CANx->RF0R |= CAN_RF0R_RFOM0;
  }
  /* Release FIFO1 */
  else /* FIFONumber == CAN_FIFO1 */
  {
    CANx->RF1R |= CAN_RF1R_RFOM1;
  }
}

/**
  * @brief  Returns the number of pending received messages.
  * @param  CANx: where x can be 1,2 or 3 to select the CAN peripheral.
  * @param  FIFONumber: Receive FIFO number, CAN_FIFO0 or CAN_FIFO1.
  * @note   CAN3 peripheral is available only for STM32F413_423xx devices
  * @retval NbMessage : which is the number of pending message.
  */
uint8_t CAN_MessagePending(CAN_TypeDef* CANx, uint8_t FIFONumber)
{
  uint8_t message_pending=0;
  /* Check the parameters */
  //assert_param(IS_CAN_ALL_PERIPH(CANx));
  //assert_param(IS_CAN_FIFO(FIFONumber));
  if (FIFONumber == CAN_FIFO0)
  {
    message_pending = (uint8_t)(CANx->RF0R&(uint32_t)0x03);
  }
  else if (FIFONumber == CAN_FIFO1)
  {
    message_pending = (uint8_t)(CANx->RF1R&(uint32_t)0x03);
  }
  else
  {
    message_pending = 0;
  }
  return message_pending;
}
/**
  * @}
  */
