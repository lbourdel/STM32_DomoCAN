/**
 * \file      timer.h
 * \author    L. BOURDEL
 * \version   1.0
 * \date      2017
 * \brief     Interfaces for timer in compare (PWM) / capture mode
 *
 * \details   !!! Good Luck !!!
*/

#ifndef TIMER_H
#define TIMER_H

#include "main.h"

// *************************************************************************************
// ************************************ ENUM *******************************************
// *************************************************************************************
/**
 * \brief     Enum used for functions callback array.
 * \details   Index of array.
 */
 
typedef enum {
	TIM1_index=1,
	TIM2_index,
	TIM3_index,
	TIM4_index,
	TIM5_index,
	TIM6_index,
	TIM7_index,
	TIM8_index,
	TIM9_index,
	TIM10_index,
	TIM11_index,
	TIM12_index,
	TIM13_index,
	TIM14_index,
	TIM_Max_index
}TimCallBackFuncIndexEnum;

extern const uint8_t IrqNumber[TIM_Max_index];

#define TIM_APB1_BUS 0x100
#define TIM_APB2_BUS 0x200

/**
 * \brief     Enum to enable corresponding clock timer .
 * \details   Enable clock timer by macro call.
 */
typedef enum {
// APB1 bus
	TIM2_ClockTimer=0x100 | RCC_APB1ENR_TIM2EN_Pos,
	TIM3_ClockTimer=0x100 | RCC_APB1ENR_TIM3EN_Pos,
	TIM4_ClockTimer=0x100 | RCC_APB1ENR_TIM4EN_Pos,
	TIM5_ClockTimer=0x100 | RCC_APB1ENR_TIM5EN_Pos,
#ifdef TIM6
	TIM6_ClockTimer=0x100 | RCC_APB1ENR_TIM6EN_Pos,
#endif
#ifdef TIM7
	TIM7_ClockTimer=0x100 | RCC_APB1ENR_TIM7EN_Pos,
#endif
#ifdef TIM12
	TIM12_ClockTimer=0x100 | RCC_APB1ENR_TIM12EN_Pos,
#endif
#ifdef TIM13
	TIM13_ClockTimer=0x100 | RCC_APB1ENR_TIM13EN_Pos,
#endif
#ifdef TIM14
	TIM14_ClockTimer=0x100 | RCC_APB1ENR_TIM14EN_Pos,
#endif

// APB2 bus
#ifdef TIM1
	TIM1_ClockTimer=0x200 | RCC_APB2ENR_TIM1EN_Pos,
#endif
#ifdef TIM8
	TIM8_ClockTimer=0x200 | RCC_APB2ENR_TIM8EN_Pos,
#endif
	TIM9_ClockTimer=0x200 | RCC_APB2ENR_TIM9EN_Pos,
	TIM10_ClockTimer=0x200 | RCC_APB2ENR_TIM10EN_Pos,
	TIM11_ClockTimer=0x200 | RCC_APB2ENR_TIM11EN_Pos
}TimClockTimerEnum;

/**
 * \brief     Enum for compare/capture timer.
 * \details   Used for config functions.
 */
typedef enum {
	TIM_CHANNEL_1=1,
	TIM_CHANNEL_2=2,
	TIM_CHANNEL_3=3,
	TIM_CHANNEL_4=4
}TimChannelEnum;

/**
 * \brief    Output mode for compare time.
 * \details   ie : Toggle, PWM1, PWM2, ....
 */
typedef enum {
	TIM_OCMODE_TIMING = (0x0U),
	TIM_OCMODE_ACTIVE = (0x01U),
	TIM_OCMODE_INACTIVE = (0x02U),
	TIM_OCMODE_FORCED_ACTIVE = (0x03U),
	TIM_OCMODE_TOGGLE = (0x04U),
	TIM_OCMODE_FORCED_INACTIVE = (0x05U),
	TIM_OCMODE_PWM1 = (0x06U),
	TIM_OCMODE_PWM2 = (0x07U)
}TimOutputCompareModeEnum;

/**
 * \brief    Preload setting for timer compare module.
 * \details   Enable or Disable
 */
typedef enum {
	TIM_PreloadDisable = (0x0U),
	TIM_PreloadEnable = (0x1U)
}TimOutputPreloadModeEnum;

/**
 * \brief    Output setting for timer compare module.
 * \details   Enable or Disable
 */
typedef enum {
	TIM_OutputDisable = (0x0U),
	TIM_OutputEnable = (0x1U)
}TimOutputEnableEnum;

/**
 * \brief    Counter mode setting for timer.
 * \details   
 */
typedef enum {
	TIM_CounterModeEdgeAligned = (0x00U),			
	TIM_CounterModeCenterAligned1 = (0x01U),	
	TIM_CounterModeCenterAligned2 = (0x02U),	
	TIM_CounterModeCenterAligned3 = (0x03U)		
}TimCounterModeEnum;

/**
 * \brief    Direction Counter setting for timer.
 * \details   Up or Down
 */
typedef enum {
	TIM_CounterDirUp = (0x00U),		/*!< Up direction   */
	TIM_CounterDirDown = (0x01U)	/*!< Down direction */
}TimCounterDirEnum;

/**
 * \brief    Behavior of PSC reload.
 * \details   ->EGR field
 */
typedef enum {
	TIM_CounterReloadUpdate = (0x00U),		
	TIM_CounterReloadImmediate = (0x01U)	
}TimCounterPscReloadModeEnum;

/**
 * @brief    Divider for internal clk CK_INT.
 * \details   by /1 or /2 or /4
 */
typedef enum {
	TIM_CKD_DIV1 = (0x00U),	
	TIM_CKD_DIV2 = (0x01U),	
	TIM_CKD_DIV4 = (0x02U)	
}TimCounterClockDivisionEnum;

/*! Defines the triggering mode of edge. */
typedef enum {
	RisingEdge = (0x00U), //!<  inverted/falling edge
	FallingEdge = (0x01U), //!<  reserved, do not use this configuration.
	RisingAndFallingEdge = (0x03U) // noninverted/both edges
} TimEdgeModeEnum;

// *************************************************************************************
// *********************************** STRUCT*******************************************
// *************************************************************************************

/** 
  * @brief  PWM Configuration Structure definition  
  */
typedef struct
{
	TimChannelEnum ChannelId:3;    					/*!<  Channel Id  */
	TimOutputCompareModeEnum OutputMode:3;	/*!< Output compare mode Toggle, PWM1, ...  */
	TimOutputPreloadModeEnum Preload:1;			/*!<  Preload setting for timer compare module */
	TimOutputEnableEnum OutputEnable:1;			/*!<  Output setting for timer compare module */
	
// Old	uint8_t DutyCycle:8;										/*!< Duty Cycle in %.  */
	uint32_t align:24;											/*!<  to align on 32 bits */
	uint32_t CompareValue;									/*!<  to set DutyCle=CompareValue/Period=CCRx/ARR */
} TIM_PWM_Config;

/** 
  * @brief  TIM Time base Configuration Structure definition  
  */
#define  DIER_UIE 0x01 /* Specifies When Event is enabled  */
#define  RESET_COUNTER_ON_TRIG 0x02 /* Specifies When Event is enabled  */

typedef struct
{
  uint32_t InputClk_Counter;         /*!< Specifies Clk Input value requested */

  TimCounterModeEnum CounterMode;       /*!< Specifies the counter mode.
                                   This parameter can be a value of @ref TimCounterModeEnum */

  TimCounterDirEnum CounterDir;       /*!< Specifies the counter dir.
                                   This parameter can be a value of @ref TimCounterDirEnum */

  uint32_t Period;            /*!< Specifies period value to be loaded into the active
                                   Auto-Reload Register at the next update event.
                                   This parameter can be a number between Min_Data = 0x0000U and Max_Data = 0xFFFF.  */

  uint32_t CounterConfig;            /*!< Several config option, see bitfield definition */
} TIM_Base_InitTypeDef;

// *************************************************************************************
// *********************************** VARIABLE ******************************************
// *************************************************************************************

extern TIM_Base_InitTypeDef TIM_InitValue;  /*!<  structure variable for function parameter */

/**
  \defgroup Timer_Group 05 - Timer primitives
  \brief    Timer primitives.
	@{
*/
// *************************************************************************************
// *********************************** DEFINE ******************************************
// *************************************************************************************

/**
 * \brief     Macro to set callback function
 * \details    
 * \param     a : Timer Id (ie : TIM4).
 * \param     CallBackFct : CallBack function.
 */
#define TIMx_SetCallBack(a, CallBackFct)    timer_set_callback_(a, IrqNumber[##a##_index], ##a##_index, CallBackFct)

/**
 * \brief     Macro to init Timer Counter
 * \details   see @ref timer_Init_
* \param     TIMx : Timer Id (ie : TIM4).
* \param    TIMxInitValue :  TIMxInitValue : see config struct TIM_Base_InitTypeDef
 */
#define TIMx_CounterInit(TIMx, TIMxInitValue)    timer_Init_(TIMx, ##TIMx##_ClockTimer, TIMxInitValue)

// *************************************************************************************
// ********************************** FUNCTION *****************************************
// *************************************************************************************

void timer_set_callback_(TIM_TypeDef*, int , TimCallBackFuncIndexEnum ,void (*callback)(TIM_TypeDef * ));

void timer_Init_( TIM_TypeDef*, TimClockTimerEnum, TIM_Base_InitTypeDef* );

uint32_t TIMx_Enable_Clock_Timer_(TimClockTimerEnum );

void TIMx_PWM_Config(TIM_TypeDef* , TIM_PWM_Config);

void TIMx_PWM_UpdateDutyCycle(TIM_TypeDef* , TimChannelEnum, uint8_t);

void TIMx_Capture_Config(TIM_TypeDef *, TimEdgeModeEnum, TimChannelEnum  );

void TIMx_Counter_Enable(TIM_TypeDef *);

void TIMx_Counter_Disable(TIM_TypeDef *);

void TIM2_IRQHandler(void);

void TIM3_IRQHandler(void);

void TIM4_IRQHandler(void);

void TIM5_IRQHandler(void);
/**@} end of timer interface */

#endif /* TIMER_H */
