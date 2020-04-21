/**
 * \file      timer.c
 * \author    L. BOURDEL
 * \version   1.0
 * \date      2017
 * \brief     Primitives functions for timer in compare (PWM) / capture mode
 *
 * \details   None
  @verbatim
TIMER   |CHANNEL 1                  |CHANNEL 2            |CHANNEL 3            |CHANNEL 4
TIM 1   |PA8    PB13   -            |PA9    PB0    PB14   |PA10   PB1   PB15    |PA11   -      -
TIM 2   |PA0    PA5    PA15   PB8   |PA1    PB3    -      |PA2    PB10   -      |PA3    PB2    PB11
TIM 3   |PA6    PB4    PC6          |PA7    PB5    PC7    |PB0    PC8    -      |PB1    PC9    -
TIM 4   |PB6    -      -            |PB7    -      -      |PB8    -   -         |PB9    -      -
TIM 5   |PA0    -      -            |PA1    -      -      |PA2    -   -         |PA3    -      -
TIM 8   |PC6    -      -            |PC7    -      -      |PC8    -    -        |PC9    -      -
TIM 9   |PA2    -      -            |PA3    -      -      |-      -      -      |-      -      -
TIM 10  |PB8    -      -            |-      -      -      |-      -      -      |-      -      -
TIM 11  |PB9    -      -            |-      -      -      |-      -      -      |-      -      -
TIM 12  |PB14   -      -            |PB15   -      -      |-      -      -      |-      -      -
TIM 13  |PA6    -      -            |-      -      -      |-      -      -      |-      -      -
TIM 14  |PA7    -      -            |-      -      -      |-      -      -      |-      -      -
@endverbatim
*/
#include <assert.h>
#include "timer.h"
const uint8_t IrqNumber[TIM_Max_index] ={
	0,  // Start from 1
	TIM1_CC_IRQn,
	TIM2_IRQn,
	TIM3_IRQn,
	TIM4_IRQn,
	TIM5_IRQn,
	TIM6_DAC_IRQn,
	TIM7_IRQn,
	TIM8_CC_IRQn,
	TIM1_BRK_TIM9_IRQn,
	TIM1_UP_TIM10_IRQn,
	TIM1_TRG_COM_TIM11_IRQn,
	TIM8_BRK_TIM12_IRQn,
	TIM8_UP_TIM13_IRQn,
	TIM8_TRG_COM_TIM14_IRQn,
};

/**
 * \brief     Default function when no call back configured for IT
 * \details   None
 * \param     ptr : pointer on timer structure
 * \return    \e none.
 */
void DefaultFunc( TIM_TypeDef *ptr )
{
	assert_failed(__FILE__,__LINE__);
}

void (*timer_callback[TIM_Max_index])( TIM_TypeDef * ) ={
	DefaultFunc,
	DefaultFunc,
	DefaultFunc,
	DefaultFunc,
	DefaultFunc,
	DefaultFunc,
	DefaultFunc,
	DefaultFunc,
	DefaultFunc,
	DefaultFunc,
	DefaultFunc,
	DefaultFunc,
	DefaultFunc,
	DefaultFunc,
	DefaultFunc
};
	


TIM_Base_InitTypeDef TIM_InitValue;  /*!<  structure variable for function parameter */

/**
 * \brief     Config counter from Timer X
 * \details   Used only with define TIMx_CounterInit
 * \param     TIMx : Timer Id (ie : TIM4)
 * \param     ClockBusId : see @ref TimClockTimerEnum
 * \param     TIMxInitValue : see config struct @ref TIM_Base_InitTypeDef
 * \return    \e none.
 */
 void timer_Init_( TIM_TypeDef *TIMx, TimClockTimerEnum ClockBusId, TIM_Base_InitTypeDef* TIMxInitValue )
{
	uint32_t TimInputClock = TIMx_Enable_Clock_Timer_(ClockBusId);
	
// SystemClock as Input by default
	
	/* Select the Counter Mode */
	TIMx->CR1 &= ~(TIM_CR1_DIR | TIM_CR1_CMS);
	TIMx->CR1 |= (TIMxInitValue->CounterDir << TIM_CR1_DIR_Pos) | (TIMxInitValue->CounterMode << TIM_CR1_CMS_Pos) ;
	/* Set the clock division */
#if defined(TIM6) || defined(TIM7)
  if((TIMx != TIM6) && (TIMx != TIM7))
  {
    /* Set the clock division */
    TIMx->CR1 &=  ~TIM_CR1_CKD;
    TIMx->CR1 |= TIM_CKD_DIV1;
  }
#endif
	
  /* Set the Prescaler value */
	if( (TimInputClock/TIMxInitValue->InputClk_Counter) > 65535) // PSC can't be more than 0xFFFF
		assert_failed(__FILE__,__LINE__);
	TIMx->PSC = TimInputClock/TIMxInitValue->InputClk_Counter-1;

  /* Set the Auto-reload value */
	TIMx->ARR = TIMxInitValue->Period;
	
	if( TIM_InitValue.CounterConfig & DIER_UIE)
		TIMx->DIER |= TIM_DIER_UIE;   											// Enable interrupt on update event or CNT reset

  /* Generate an update event to reload the Prescaler */
// LBR	TIMx->EGR |= TIM_EGR_UG;  // or TIM_PSCReloadMode_Immediate

	if( (TIM_InitValue.CounterConfig) & RESET_COUNTER_ON_TRIG) // Specific feature to reset CNT when capture occured
	{
	// To reset Counter when capture occurs, set in SMCR with trig:
	//  TS: Trigger selection to 
	//	101: Filtered Timer Input 1 (TI1FP1) or 
	//	110: Filtered Timer Input 2 (TI2FP2)
		TIMx->SMCR &= ~TIM_SMCR_TS_Msk;
	//	TIMx->SMCR |= (TIM_SMCR_TS_2 | TIM_SMCR_TS_0); // For TI1FP1 : 101
		TIMx->SMCR |= (TIM_SMCR_TS_2 | TIM_SMCR_TS_1); // For TI2FP2 : 110
	//	SMS: Slave mode selection to
	//	100: Reset Mode - Rising edge of the selected trigger input (TRGI) reinitializes
	//	                  the counter and generates an update of the registers.
		TIMx->SMCR &= ~TIM_SMCR_SMS_Msk;
	// LBR Pb when PWM & Capture on same Timer	
		TIMx->SMCR |= TIM_SMCR_SMS_2; // 100
	}
	
	// Advanced-control timers
  if((TIMx == TIM1) || (TIMx == TIM8) ) 
  {
    /* Set the Repetition Counter value (only for TIM1 and TIM8) */
    TIMx->RCR = 0;
	}
		
// Only counter overflow/underflow generates an update interrupt
// LBR no needed ?	TIMx->CR1 |= TIM_CR1_URS;
}

/**
 * \brief     Config PWM channel for Timer X
 * \param     TIMx : Timer Id (ie : TIM4)
 * \param     PwmConfig : see structure TIM_PWM_Config
 * \return    \e none.
 */
void TIMx_PWM_Config(TIM_TypeDef *TIMx, TIM_PWM_Config PwmConfig )
{
	switch(PwmConfig.ChannelId)
	{
		case TIM_CHANNEL_1:
// Output compare mode
//	These bits define the behavior of the output reference signal OC1REF from which OC1 and 
//	OC1N are derived
			TIMx->CCMR1 &= ~TIM_CCMR1_OC1M_Msk;
			TIMx->CCMR1 |= ( PwmConfig.OutputMode << TIM_CCMR1_OC1M_Pos);
// Output Compare preload enable
//	0: Preload register on TIMx_CCR1 disabled. TIMx_CCR1 can be written at anytime, the 
//	new value is taken in account immediately.
//	1: Preload register on TIMx_CCR1 enabled. Read/Write operations access the preload 
//	register. TIMx_CCR1 preload value is loaded in the active register at each update event.
		TIMx->CCMR1 &= ~TIM_CCMR1_OC1PE_Msk;
			TIMx->CCMR1 |= ( PwmConfig.Preload << TIM_CCMR1_OC1PE_Pos);
//  capture/compare register 1
// CCR1 is the value to be loaded in the actual capture/compare 1 register (preload value).
// It is loaded permanently if the preload feature is not selected in the TIMx_CCMR1 register(bit OC1PE).
// Else the preload value is copied in the active capture/compare 1 register when an update event occurs.
// The active capture/compare register contains the value to be compared to the counter TIMx_CNT and signaled on OC1 output.
//			TIMx->CCR1 	 = ((float)PwmConfig.DutyCycle /100) * TIMx->ARR;                                 // Output Compare Register for channel 1
			TIMx->CCR1 	 = PwmConfig.CompareValue-1;                                 // Output Compare Register for channel 1
// Bit 0  CC1E: Compare 1 output enable
// 0: Off - OC1 is not active. OC1 level is then function of MOE, OSSI, OSSR, OIS1, OIS1N and CC1NE bits.
// 1: On - OC1 signal is output on the corresponding output pin depending on MOE, OSSI, OSSR, OIS1, OIS1N and CC1NE bits.
			TIMx->CCER 	 	&= ~TIM_CCER_CC1E_Msk;                       // Enable output for ch1
			TIMx->CCER 	 	|= (PwmConfig.OutputEnable << TIM_CCER_CC1E_Pos );          // Enable output for ch1

		break;
		case TIM_CHANNEL_2:
			// Output compare mode
			TIMx->CCMR1 &= ~TIM_CCMR1_OC2M_Msk;
			TIMx->CCMR1 |= ( PwmConfig.OutputMode << TIM_CCMR1_OC2M_Pos);
			// Output Compare preload enable
			TIMx->CCMR1 &= ~TIM_CCMR1_OC2PE_Msk;
			TIMx->CCMR1 |= ( PwmConfig.Preload << TIM_CCMR1_OC2PE_Pos);
			TIMx->CCR2 	 = PwmConfig.CompareValue-1;                                 // Output Compare Register for channel 2
// Bit 0  CC1E: Compare 1 output enable
			TIMx->CCER 	 	&= ~TIM_CCER_CC2E_Msk;                       // Enable output for ch1
			TIMx->CCER 	 	|= (PwmConfig.OutputEnable << TIM_CCER_CC2E_Pos );          // Enable output for ch1
			break;
		case TIM_CHANNEL_3:
			// Output compare mode
			TIMx->CCMR2 &= ~TIM_CCMR2_OC3M_Msk;
			TIMx->CCMR2 |= ( PwmConfig.OutputMode << TIM_CCMR2_OC3M_Pos);
			// Output Compare preload enable
			TIMx->CCMR2 &= ~TIM_CCMR1_OC1PE_Msk;
			TIMx->CCMR2 |= ( PwmConfig.Preload << TIM_CCMR2_OC3PE_Pos);
			TIMx->CCR3 	 = PwmConfig.CompareValue-1;                                 // Output Compare Register for channel 3
// Bit 0  CC1E: Compare 1 output enable
			TIMx->CCER 	 	&= ~TIM_CCER_CC3E_Msk;                       // Enable output for ch1
			TIMx->CCER 	 	|= (PwmConfig.OutputEnable << TIM_CCER_CC3E_Pos );          // Enable output for ch1
			break;
		case TIM_CHANNEL_4:
			// Output compare mode
			TIMx->CCMR2 &= ~TIM_CCMR2_OC4M_Msk;
			TIMx->CCMR2 |= ( PwmConfig.OutputMode << TIM_CCMR2_OC4M_Pos);
			// Output Compare preload enable
			TIMx->CCMR2 &= ~TIM_CCMR1_OC1PE_Msk;
			TIMx->CCMR2 |= ( PwmConfig.Preload << TIM_CCMR2_OC4PE_Pos);
			TIMx->CCR4 	 = PwmConfig.CompareValue-1;                                 // Output Compare Register for channel 4
// Bit 0  CC1E: Compare 1 output enable
			TIMx->CCER 	 	&= ~TIM_CCER_CC4E_Msk;                       // Enable output for ch1
			TIMx->CCER 	 	|= (PwmConfig.OutputEnable << TIM_CCER_CC4E_Pos );          // Enable output for ch1
			break;
		default:
			// Not possible because enum needed
			break;
	}

	//TIMx->CR1 	 		|= TIM_CR1_ARPE;                        // Auto-reload preload enable

// Advanced-control timers
  if((TIMx == TIM1) || (TIMx == TIM8) ) 
  {
// MOE: Main output enable
// This bit is cleared asynchronously by hardware as soon as the break input is active. It is set by software
// or automatically depending on the AOE bit. It is acting only on the channels which are configured in output.
// 0: OC and OCN outputs are disabled or forced to idle state.
// 1: OC and OCN outputs are enabled if their respective enable bits are set (CCxE, CCxNE in TIMx_CCER register).
			TIMx->BDTR |= TIM_BDTR_MOE;  
		
// CC1NE: Capture/Compare 1 complementary output enable
// 0: Off - OC1N is not active. OC1N level is then function of MOE, OSSI, OSSR, OIS1, OIS1N and CC1E bits.
// 1: On - OC1N signal is output on the corresponding output pin depending on MOE, OSSI, OSSR, OIS1, OIS1N and CC1E bits.		
		TIMx->CCER |= (TIM_CCER_CC1NE | TIM_CCER_CC2NE | TIM_CCER_CC3NE); 
	}
}

/**
 * \brief      Modify PWM DutyCycle
 * \details    Call to update Duty Cycle on Channel X from Timer Y
 * \param    TIMx       			Timer Id (ie : TIM4)
 * \param    ChannelId        Channel Number.
 * \param    DutyCycle        Duty Cycle in %.
 * \return   \e none.
 */
void TIMx_PWM_UpdateDutyCycle(TIM_TypeDef *TIMx, TimChannelEnum ChannelId, uint8_t DutyCycle  )
{
	switch(ChannelId)
	{
		case TIM_CHANNEL_1:
			TIMx->CCR1 	 = ((float)DutyCycle /100) * TIMx->ARR;                                 // Output Compare Register for channel 1
		break;
		case TIM_CHANNEL_2:
			TIMx->CCR2 	 = ((float)DutyCycle /100) * TIMx->ARR;                                 // Output Compare Register for channel 2
			break;
		case TIM_CHANNEL_3:
			TIMx->CCR3 	 = ((float)DutyCycle /100) * TIMx->ARR;                                 // Output Compare Register for channel 3
			break;
		case TIM_CHANNEL_4:
			TIMx->CCR4 	 = ((float)DutyCycle /100) * TIMx->ARR;                                 // Output Compare Register for channel 4
			break;
		default:
			// Not possible because enum needed
			break;
	}
}

/**
 * \brief     Config Capture channel for Timer X
 * \param     TIMx : Timer Id (ie : TIM4)
 * \param     Edge : configure edge type for trigger
 * \param     ChannelSelect : structure TimChannelEnum
 * \return    \e none.
 */
void TIMx_Capture_Config(TIM_TypeDef *TIMx, TimEdgeModeEnum Edge, TimChannelEnum ChannelSelect )
{
	switch(ChannelSelect)
	{
		case TIM_CHANNEL_1:
		// Setup capture settings for TIMx

		// 00 = output
		// 01 = input, CC1 is mapped on timer Input 1
		// 10 = input, CC1 is mapped on timer Input 2
		// 11 = input, CC1 is mapped on slave timer
			TIMx->CCMR1 &= ~TIM_CCMR1_CC1S; // Reset default
			TIMx->CCMR1 |= TIM_CCMR1_CC1S_0; // Channel : CAPTURE TI1

		// Input Capture 1 Filter disabled
		// because we want to capture every event
			TIMx->CCMR1 &= ~TIM_CCMR1_IC1F;
			//TIMx->CCMR1 |= TIM_CCMR1_IC1F_0 | TIM_CCMR1_IC1F_1;
			
		// Capture edge config
			TIMx->CCER &= ~(TIM_CCER_CC1P | TIM_CCER_CC1NP); // reset first
			TIMx->CCER |= Edge << TIM_CCER_CC1P_Pos; // config edges

		// Program input prescaller
		// To capture each valid transition, set input prescaler to zero (default)
			TIMx->CCMR1 &= ~TIM_CCMR1_IC1PSC;

		// Capture enable
			TIMx->CCER |= TIM_CCER_CC1E; // Capture Enable
		// Capture interrupt enable	
			TIMx->DIER |= TIM_DIER_CC1IE;

		// Clear the Capture event flag for channel 1
			TIMx->SR = ~TIM_SR_CC1IF;
		break;
		case TIM_CHANNEL_2:
		// Setup input timer settings for TIMx
			TIMx->CCMR1 &= ~TIM_CCMR1_CC2S; // Reset default
			TIMx->CCMR1 |= TIM_CCMR1_CC2S_0; // Channel : CAPTURE TI1

			TIMx->CCMR1 &= ~TIM_CCMR1_IC2F;
			//TIMx->CCMR1 |= TIM_CCMR1_IC2F_0 | TIM_CCMR1_IC2F_1;
			
			TIMx->CCER &= ~(TIM_CCER_CC2P | TIM_CCER_CC2NP); // reset
			TIMx->CCER |= Edge << TIM_CCER_CC2P_Pos; // config edges

			TIMx->CCMR1 &= ~TIM_CCMR1_IC2PSC;

			TIMx->CCER |= TIM_CCER_CC2E; // Capture Enable
			TIMx->DIER |= TIM_DIER_CC2IE;
		// Clear the Capture event flag for channel 2
			TIMx->SR = ~TIM_SR_CC2IF;
	
		break;
		case TIM_CHANNEL_3:
		// Setup input timer settings for TIMx
			TIMx->CCMR2 &= ~TIM_CCMR2_CC3S; // Reset default
			TIMx->CCMR2 |= TIM_CCMR2_CC3S_0; // Channel : CAPTURE TI1

			TIMx->CCMR2 &= ~TIM_CCMR2_IC3F;
			//TIMx->CCMR2 |= TIM_CCMR2_IC3F_0 | TIM_CCMR2_IC3F_1;
			
			TIMx->CCER &= ~(TIM_CCER_CC3P | TIM_CCER_CC3NP); // reset
			TIMx->CCER |= Edge << TIM_CCER_CC3P_Pos; // config edges

			TIMx->CCMR2 &= ~TIM_CCMR2_IC3PSC;

			TIMx->CCER |= TIM_CCER_CC3E; // Capture Enable
			TIMx->DIER |= TIM_DIER_CC3IE;
		// Clear the Capture event flag for channel 3
			TIMx->SR = ~TIM_SR_CC3IF;
		
			break;
		case TIM_CHANNEL_4:
		// Setup input timer settings for TIMx
			TIMx->CCMR2 &= ~TIM_CCMR2_CC4S; // Reset default
			TIMx->CCMR2 |= TIM_CCMR2_CC4S_0; // Channel : CAPTURE TI1

			TIMx->CCMR2 &= ~TIM_CCMR2_IC4F;
			//TIMx->CCMR2 |= TIM_CCMR2_IC4F_0 | TIM_CCMR2_IC4F_1;
			
			TIMx->CCER &= ~(TIM_CCER_CC4P | TIM_CCER_CC4NP); // reset
			TIMx->CCER |= Edge << TIM_CCER_CC4P_Pos; // config edges

			TIMx->CCMR2 &= ~TIM_CCMR2_IC4PSC;

			TIMx->CCER |= TIM_CCER_CC4E; // Capture Enable
			TIMx->DIER |= TIM_DIER_CC4IE;
		// Clear the Capture event flag for channel 4
			TIMx->SR = ~TIM_SR_CC4IF;

			break;
		default:
			// Not possible because enum needed
			break;
	}
// Re-initialize the counter and generates an update of the registers
// LBR	TIMx->EGR |= TIM_EGR_UG;

}

/**
 * \brief     Run Timer X counter
 * \param     TIMx : Timer Id (ie : TIM4)
 * \return    \e none.
 */
void TIMx_Counter_Enable(TIM_TypeDef *TIMx) {
	// Enable the timer.
	//SysTick_Config(timer_period);
	TIMx->CR1 |= TIM_CR1_CEN;
}

/**
 * \brief     Stop Timer X counter
 * \param     TIMx : Timer Id (ie : TIM4)
 * \return    \e none.
 */
void TIMx_Counter_Disable(TIM_TypeDef *TIMx) {
	// Disable the timer.
	TIMx->CR1 &= ~TIM_CR1_CEN;
	//SysTick->CTRL  &= ~SysTick_CTRL_ENABLE_Msk; 
}

/**
 * \brief     Set CallBack function for TimerX/ChannelY
 * \details   Call only by TIMx_SetCallBack macro
 * \param     TIMx : Timer Id (ie : TIM4)
 * \param     TIMx_IRQn : IRQ id 
 * \param     TIMx_index : Timer Id (ie : TIM4)
 * \param     callback : pointer to callback function
 * \return    \e none.
 */
void timer_set_callback_(TIM_TypeDef *TIMx, int TIMx_IRQn, TimCallBackFuncIndexEnum TIMx_index,void (*callback)( TIM_TypeDef * ) ) {
		
	timer_callback[TIMx_index] = callback;

	TIMx->SR &= ~TIM_SR_UIF;
	
	NVIC_SetPriority( (IRQn_Type)TIMx_IRQn,3);
	NVIC_ClearPendingIRQ( (IRQn_Type)TIMx_IRQn);
	NVIC_EnableIRQ( (IRQn_Type)TIMx_IRQn);

}

/**
 * \brief     Enable Timer bus clock
 * \details    Call only by 
 * \param     ClockBusId : ClockBus Id from TimClockTimerEnum enum
 * \return    \e none.
 */
uint32_t TIMx_Enable_Clock_Timer_(TimClockTimerEnum ClockBusId) {
		switch( ((ClockBusId&0xFF00) >>8) )
		{
			case 1:
				RCC->APB1ENR |= ( 0x1U << (ClockBusId&0x0FF));
				return Apb1TimerClock;
			case 2:
				RCC->APB2ENR |= ( 0x1U << (ClockBusId&0x0FF));
				return Apb2TimerClock;
			default:
				return 0;
		}
}

/**
 * \brief     Handler for Timer1
 * \details      
 * \return    Return : \e none.
 */
void TIM1_CC_IRQHandler(void)
{
	  timer_callback[TIM1_index](TIM1);
}

/**
 * \brief     Handler for Timer2
 * \details      
 * \return    Return : \e none.
 */
void TIM2_IRQHandler(void)
{
	  timer_callback[TIM2_index](TIM2);

//// Update interrupt flag
//// This bit is set by hardware on an update event. It is cleared by software.
//	if((TIM2->SR)&TIM_SR_UIF){
//	  timer_callback[TIM2_index](TIM2);
//		TIM2->SR &= ~TIM_SR_UIF;		
//		NVIC_ClearPendingIRQ(TIM2_IRQn);	
//	}
//// For TIMER CAPTURE
////	This flag is set by hardware only when the corresponding channel is configured in input 
////	capture mode. It is cleared by software by writing it to ‘0.
////	0: No overcapture has been detected
////	1: The counter value has been captured in TIMx_CCR1 register while CC1IF flag was 
////	already set
//	if(TIM2->SR & TIM_SR_CC1OF){ // OverFlow
//		timer_callback[TIM2_index](TIM2);
////		TIM2->SR &= ~TIM_SR_UIF;
//		TIM2->SR &= ~TIM_SR_CC1OF;	
//	}
////	If channel CC1 is configured as input:
////	This bit is set by hardware on a capture. It is cleared by software or by reading the 
////	TIMx_CCR1 register.
//	if((TIM2->SR & TIM_SR_CC1IF) != 0){ 
//		timer_callback[TIM2_index](TIM2);
//		TIM2->SR &= ~TIM_SR_CC1IF;
////		TIM2->SR &= ~TIM_SR_CC1OF;	
//	}
//	TIM2->SR &= ~TIM_SR_UIF;

}

/**
 * \brief     Handler for Timer3
 * \details    
 * \return    Return : \e none.
 */
void TIM3_IRQHandler(void)
{
	  timer_callback[TIM3_index](TIM3);

//	if((TIM3->SR)&TIM_SR_UIF){
//	  timer_callback[TIM3_index](TIM3);
//		TIM3->SR &= ~TIM_SR_UIF;		
//		NVIC_ClearPendingIRQ(TIM3_IRQn);	
//	}
}

/**
 * \brief     Handler for Timer4
 * \details    
 * \return    Return : \e none.
 */
void TIM4_IRQHandler(void)
{
  timer_callback[TIM4_index](TIM4);

//	if(TIM4->SR & TIM_SR_CC3IF){ // Capture occured
//		TIM4->SR &= ~TIM_SR_CC3IF; // reset flag
//	}

//	if(TIM4->SR & TIM_SR_CC3OF){ // OverFlow Channelx
//		TIM4->SR &= ~TIM_SR_CC3OF; // CCxIF automatically reset
//	}

//	if((TIM4->SR)&TIM_SR_UIF){ // OverFlow
//		TIM4->SR &= ~TIM_SR_UIF;		
//	}

//	NVIC_ClearPendingIRQ(TIM4_IRQn);	

}

/**
 * \brief     Handler for Timer5
 * \details    
 * \return    Return : \e none.
 */
void TIM5_IRQHandler(void)
{
	  timer_callback[TIM5_index](TIM5);
//	if((TIM5->SR)&TIM_SR_UIF){
//	  timer_callback[TIM5_index](TIM5);
//		TIM5->SR &= ~TIM_SR_UIF;		
//		NVIC_ClearPendingIRQ(TIM5_IRQn);	
//	}
}


/**
 * \brief     Handler for Timer6
 * \details      
 * \return    Return : \e none.
 */
void TIM6_DAC_IRQHandler(void)
{
	  timer_callback[TIM6_index](TIM6);
}

/**
 * \brief     Handler for Timer7
 * \details      
 * \return    Return : \e none.
 */
void TIM7_IRQHandler(void)
{
	  timer_callback[TIM7_index](TIM7);
}

/**
 * \brief     Handler for Timer8
 * \details      
 * \return    Return : \e none.
 */
void TIM8_CC_IRQHandler(void)
{
	  timer_callback[TIM8_index](TIM8);
}

/**
 * \brief     Handler for Timer9
 * \details      
 * \return    Return : \e none.
 */
void TIM1_BRK_TIM9_IRQHandler(void)
{
	  timer_callback[TIM9_index](TIM9);
}

/**
 * \brief     Handler for Timer10
 * \details      
 * \return    Return : \e none.
 */
void TIM1_UP_TIM10_IRQHandler(void)
{
	  timer_callback[TIM10_index](TIM10);
}

/**
 * \brief     Handler for Timer11
 * \details      
 * \return    Return : \e none.
 */
void TIM1_TRG_COM_TIM11_IRQHandler(void)
{
	  timer_callback[TIM11_index](TIM11);
}

/**
 * \brief     Handler for Timer12
 * \details      
 * \return    Return : \e none.
 */
void TIM8_BRK_TIM12_IRQHandler(void)
{
	  timer_callback[TIM12_index](TIM12);
}

/**
 * \brief     Handler for Timer13
 * \details      
 * \return    Return : \e none.
 */
void TIM8_UP_TIM13_IRQHandler(void)
{
	  timer_callback[TIM13_index](TIM13);
}

/**
 * \brief     Handler for Timer14
 * \details      
 * \return    Return : \e none.
 */
void TIM8_TRG_COM_TIM14_IRQHandler(void)
{
	  timer_callback[TIM14_index](TIM14);
}


