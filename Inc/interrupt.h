/**
 * \file      interrupt.h
 * \author    L. BOURDEL
 * \version   1.0
 * \date      2017
 * \brief     Interfaces for interrupt services
 *
 * \details   !!! Good Luck !!!
 */
 
 /*----------------------------------------------------------------------------
LAB INTERRUPT

- define Interfaces

	GOOD LUCK!
 *----------------------------------------------------------------------------*/


#ifndef INTERRUPT_H
#define INTERRUPT_H

#include "main.h"

// ************************************************************************************
// *********************************** ENUM *******************************************
// ************************************************************************************

/*! Defines the triggering mode of an interrupt. */
typedef enum {
	None,   //!< Disables the interrupt.
	Rising, //!< Trigger interrupt on rising edge on line, pull down resistor active
	Falling, //!< Trigger interrupt on falling edge on line, pull up resistor active
	RisingAndFalling /*!< Trigger interrupt on any edge on line, no pull resistor active */
} TriggerModeEnum;

// ************************************************************************************
// ********************************** STRUCT*******************************************
// ************************************************************************************

// ************************************************************************************
// ********************************** DEFINE ******************************************
// ************************************************************************************

// ************************************************************************************
// ********************************* FUNCTION *****************************************
// ************************************************************************************

/**
  \defgroup ExtI_Group 02 - External Interrupt
  \brief    Functions to connect Input as Interrupt.
	@{
*/
void EXTI_Interrupt_Init(PinNameEnum , TriggerModeEnum , void (*callback)(int ));
/**@} end of interrupt interface */
#endif
