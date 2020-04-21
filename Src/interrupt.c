/**
 * \file      interrupt.c
 * \author    L. BOURDEL
 * \version   1.1
 * \date      2018
 * \brief     Primitives functions for interrupt
 *
 * \details   !!! Good Luck !!!
 */
/*----------------------------------------------------------------------------
LAB INTERRUPT

- define Interrupt init function
- define 

	GOOD LUCK!
 *----------------------------------------------------------------------------*/

#include "interrupt.h"

static void (*EXTI_callback[16])(int EXTI_ID_Pending);

/**
 * \brief     Configure I/O pin for interrupt
 * \note   This function also enables clock for SYSCFG (RCC->APB2ENR)
 * \details   
 * \param			PinId : Id enumeration see PinNames.h
 * \param			trig : ->IMR, ->RTSR, ->FTSR register, 
 * \param			callback : Function called by interrupt handler
 * \return    \b None.
 */
void EXTI_Interrupt_Init(PinNameEnum PinId, TriggerModeEnum trig, void (*callback)(int)) 	
{
	int IRQ_port_num,IRQ_pin_index;
	
//  IRQ_status = 0;
	IRQ_port_num = GET_PORT_INDEX(PinId);
	IRQ_pin_index = GET_PIN_INDEX(PinId);	

		/* Check if line is already in use */
	if (
		(EXTI->IMR & (0x01<<IRQ_pin_index)) || /* !< Interrupt already attached */
		(EXTI->EMR & (0x01<<IRQ_pin_index))    /* !< Event already attached */
	) {
		/* Return error */
		assert_failed(__FILE__,__LINE__);
	}

	// Enable SYSCFG clock
	RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;

	SYSCFG->EXTICR[IRQ_pin_index/4] &= ~(0xF << (IRQ_pin_index%4)*4);
  SYSCFG->EXTICR[IRQ_pin_index/4] |= (IRQ_port_num<<(IRQ_pin_index%4)*4);

	/* Clear first */
	EXTI->FTSR &= ~(0x01<<IRQ_pin_index);
	EXTI->RTSR &= ~(0x01<<IRQ_pin_index);

	switch(trig){
		case None:
			EXTI->IMR &= ~(1<<IRQ_pin_index);
		break;
		case Rising:
			EXTI->RTSR|= (1<<IRQ_pin_index);
		break;
		case Falling:
			EXTI->FTSR|= (1<<IRQ_pin_index);
		break;
		case RisingAndFalling:
			EXTI->RTSR|= (1<<IRQ_pin_index);
			EXTI->FTSR|= (1<<IRQ_pin_index);
		break;
		default:
		/* Return error */
#ifdef _DEBUG
			printf(" !!!ERROR!!! line=%d file=%s\n", __LINE__, __FILE__);
#endif
		break;
	}

	/* set by default */
	EXTI->IMR |= (1<<IRQ_pin_index);
	
	//Connect the pin to external interrupt line
	switch(IRQ_pin_index){
		case 0:
			NVIC_SetPriority(EXTI0_IRQn,3);
		  NVIC_ClearPendingIRQ(EXTI0_IRQn);
		  NVIC_EnableIRQ(EXTI0_IRQn);
			break;
		case 1:
			NVIC_SetPriority(EXTI1_IRQn,3);
		  NVIC_ClearPendingIRQ(EXTI1_IRQn);
		  NVIC_EnableIRQ(EXTI1_IRQn);
			break;
    case 2:
			NVIC_SetPriority(EXTI2_IRQn,3);
		  NVIC_ClearPendingIRQ(EXTI2_IRQn);
		  NVIC_EnableIRQ(EXTI2_IRQn);
			break;
		case 3:
			NVIC_SetPriority(EXTI3_IRQn,3);
		  NVIC_ClearPendingIRQ(EXTI3_IRQn);
		  NVIC_EnableIRQ(EXTI3_IRQn);
			break;		
		case 4:
			NVIC_SetPriority(EXTI4_IRQn,3);
		  NVIC_ClearPendingIRQ(EXTI4_IRQn);
		  NVIC_EnableIRQ(EXTI4_IRQn);
			break;		
		case 5:
    case 6:
		case 7:
		case 8:
		case 9:
			NVIC_SetPriority(EXTI9_5_IRQn,3);
		  NVIC_ClearPendingIRQ(EXTI9_5_IRQn);
		  NVIC_EnableIRQ(EXTI9_5_IRQn);
			break;
    case 10:
		case 11:
		case 12:
		case 13:
    case 14:
		case 15:
			NVIC_SetPriority(EXTI15_10_IRQn,3);
		  NVIC_ClearPendingIRQ(EXTI15_10_IRQn);
		  NVIC_EnableIRQ(EXTI15_10_IRQn);
			break;
		default:
		/* Return error */
#ifdef _DEBUG
			printf(" !!!ERROR!!! line=%d file=%s\n", __LINE__, __FILE__);
#endif
		break;
	}

// Registering CallBack function
	EXTI_callback[IRQ_pin_index]=callback;

// LBR Not needed		__enable_irq(); // ; Enable IRQ by clearing PRIMASK

}

//Note: only four interrupt lines are implemented i.e. only use pin 0-4

/**
 * \brief     IRQ Handler for EXTI0
 * \details   
 * \return    \e none.
 */
void EXTI0_IRQHandler(void)
{	
	if ((EXTI->PR & EXTI_PR_PR0) != 0)
	{
		EXTI_callback[0](EXTI_PR_PR0);
		
		EXTI->PR |= EXTI_PR_PR0; // Clear EXTI0 pending interrupt
	}
}

/**
 * \brief     IRQ Handler for EXTI1
 * \details   
 * \return    \e none.
 */
void EXTI1_IRQHandler(void){
	
	if ((EXTI->PR & EXTI_PR_PR1) != 0)
	{
		// Code TODO
		EXTI_callback[1](EXTI_PR_PR1);
		
		EXTI->PR |= EXTI_PR_PR1; // Clear EXTI0 pending interrupt
	}
	
}

/**
 * \brief     IRQ Handler for EXTI2
 * \details   
 * \return    \e none.
 */
void EXTI2_IRQHandler(void){
	
	if ((EXTI->PR & EXTI_PR_PR2) != 0)
	{
		// Code TODO
		EXTI_callback[2](EXTI_PR_PR2);
		
		EXTI->PR |= EXTI_PR_PR2; // Clear EXTI0 pending interrupt
	}
	
}

/**
 * \brief     IRQ Handler for EXTI3
 * \details   
 * \return    \e none.
 */
void EXTI3_IRQHandler(void){
	
	if ((EXTI->PR & EXTI_PR_PR3) != 0)
	{
		// Code TODO
		EXTI_callback[3](EXTI_PR_PR3);
		
		EXTI->PR |= EXTI_PR_PR3; // Clear EXTI0 pending interrupt
	}
	
}

/**
 * \brief     IRQ Handler for EXTI4
 * \details   
 * \return    \e none.
 */
void EXTI4_IRQHandler(void){
	
	if ((EXTI->PR & EXTI_PR_PR4) != 0)
	{
		// Code TODO
		EXTI_callback[4](EXTI_PR_PR4);
		
		EXTI->PR |= EXTI_PR_PR4; // Clear EXTI0 pending interrupt
	}
	
}

/**
 * \brief     IRQ Handler for EXTI 5 to 9
 * \details   
 * \return    \e none.
 */
void EXTI9_5_IRQHandler(void){
	int i;
	
	for(i=5;i<=9;i++){
		if ((EXTI->PR & (0x1U << i)) != 0)
		{
			// Code TODO
			EXTI_callback[i]((0x1U << i));
			
			EXTI->PR |= (0x1U << i); // Clear EXTI0 pending interrupt
		}
	}
}

/**
 * \brief     IRQ Handler for EXTI 10 to 15
 * \details   
 * \return    \e none.
 */
void EXTI15_10_IRQHandler(void){
	int i;
	
	for(i=10;i<=15;i++){
		if ((EXTI->PR & (0x1U << i)) != 0)
		{
			// Code TODO
			EXTI_callback[i]((0x1U << i));
			
			EXTI->PR |= (0x1U << i); // Clear EXTI0 pending interrupt
		}
	}	
}

CanRxMsg RxMessage;
extern uint8_t ubKeyNumber;

/**
  * @brief  This function handles CAN1 RX0 request.
  * @param  None
  * @retval None
  */
void CAN1_RX0_IRQHandler(void)
{
	
	if (CAN_GetITStatus(CAN1, CAN_IT_FMP0 ) == RESET )
		assert_failed(__FILE__,__LINE__);
	
  CAN_Receive(CAN1, CAN_FIFO0, &RxMessage);
  
//  if ((RxMessage.StdId == 0x321)&&(RxMessage.IDE == CAN_ID_STD) && (RxMessage.DLC == 1))
//  {
//    LED_Display(RxMessage.Data[0]);
//    ubKeyNumber = RxMessage.Data[0];
//  }
}

/**
  * @brief  This function handles CAN1 RX0 request.
  * @param  None
  * @retval None
  */
void CAN1_RX1_IRQHandler(void)
{
	if (CAN_GetITStatus(CAN1, CAN_IT_FMP1 ) == RESET )
		assert_failed(__FILE__,__LINE__);
	
  CAN_Receive(CAN1, CAN_FIFO1, &RxMessage);
  
//  if ((RxMessage.StdId == 0x321)&&(RxMessage.IDE == CAN_ID_STD) && (RxMessage.DLC == 1))
//  {
//    LED_Display(RxMessage.Data[0]);
//    ubKeyNumber = RxMessage.Data[0];
//  }
}
