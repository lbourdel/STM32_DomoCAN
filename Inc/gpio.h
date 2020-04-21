/**
 * \file      gpio.h
 * \author    L. BOURDEL
 * \version   1.0
 * \date      2017
 * \brief     Interfaces for I/O
 *
 * \details   !!! Good Luck !!!
 */


#ifndef GPIO_H
#define GPIO_H

#include "main.h"

// ************************************************************************************
// *********************************** ENUM *******************************************
// ************************************************************************************

// Pin Function
/**
 * \brief     I/O direction and function
 * \details   Used for config I/O.
 */
 typedef enum {
    PIN_INPUT = 0,			/*!< GPIO Pin as General Purpose Input */
    PIN_OUTPUT = 1,			/*!< GPIO Pin as General Purpose Output */
    PIN_ALTERNATE = 2,	/*!< GPIO Pin as Alternate Function */
    PIN_ANALOG = 3,			/*!< GPIO Pin as Analog */
}GpioPinFunctionEnum;

// Output Type
/**
 * \brief     Output Type
 * \details   Used for config I/O.
 */
typedef enum {
    OUTPUT_PUSHPULL          = 0,	/*!< GPIO Output Type Push-Pull */
		OUTPULL_OUTPUT_DEFAULT		 = OUTPUT_PUSHPULL,
    OUTPUT_OPENDRAIN         = 1	/*!< GPIO Output Type Open-Drain */
}GpioOutputTypeEnum;

// Output Speed
/**
 * \brief     Output Speed
 * \details   Used for config I/O.
 */
typedef enum {
    OUTPUT_SPEED_LOW          	= 0,	/*!< GPIO Speed Low */
		OUTPUT_SPEED_DEFAULT    		= OUTPUT_SPEED_LOW,
    OUTPUT_SPEED_MEDIUM         = 1,	/*!< GPIO Speed Medium */
    OUTPUT_SPEED_FAST         	= 2,	/*!< GPIO Speed Fast */
    OUTPUT_SPEED_HS             = 3		/*!< GPIO Speed High */
}GpioOutputSpeedEnum;

// Input Type
/**
 * \brief     Pull-Up Pull-Out config
 * \details   Used for config I/O.
 */
typedef enum {
    INPUT_NO_PULLUP_PULLDOWN          = 0,	/*!< No pull resistor */
    INPUT_PUPDR_DEFAULT        				= INPUT_NO_PULLUP_PULLDOWN,
    INPUT_PULL_UP        					    = 1,	/*!< Pull up resistor enabled */
    INPUT_PULL_DOWN     					    = 2		/*!< Pull down resistor enabled */
}GpioPullUpDownEnum; 

// ************************************************************************************
// ********************************** STRUCT*******************************************
// ************************************************************************************

// ************************************************************************************
// ********************************** DEFINE ******************************************
// ************************************************************************************


// ************************************************************************************
// ********************************* FUNCTION *****************************************
// ************************************************************************************

// Config
/* ##################################### Debug In/Output function ########################################### */
/**
  \defgroup GpioGroup 01 - GPIO Interface
  \brief    Functions to manipulate I/O. See PinNameEnum definition in PinNames.h.
 @{
 */
void GPIOx_Pin_Configure(PinNameEnum      ,
												 GpioPinFunctionEnum  , 
												 GpioOutputTypeEnum   , 
												 GpioOutputSpeedEnum  ,
												 GpioPullUpDownEnum  );

void GPIOx_PinAF (PinNameEnum ,
                 uint8_t    );

void GPIOx_Set_Output_Pin(PinNameEnum );

void GPIOx_Reset_Output_Pin(PinNameEnum );
	
void GPIOx_Toggle_Output_Pin(PinNameEnum );
	
void GPIOx_Set_Output_Port(GPIO_TypeDef* , uint16_t , uint16_t );

bool GPIOx_Read_Input_Pin(PinNameEnum );
/**@} end of GPIO interface */
bool GPIO_PinAF(uint8_t PinId,
                 uint8_t AlternateFunction,
								 uint32_t AFR_Register[2]	);

#endif
