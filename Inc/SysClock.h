/**
 * \file      SysClock.h
 * \author    L. BOURDEL
 * \version   1.0
 * \date      2017
 * \brief     Interfaces for System Clock config
 *
 * \details   !!! Good Luck !!!
 */
 
#ifndef SYSCLOCK_H
#define SYSCLOCK_H

#include "main.h"

extern uint32_t Apb1TimerClock ;  /*!< Contains the APB1 Timer clock speed in Hz */
extern uint32_t Apb2TimerClock ;  /*!< Contains the APB2 Timer clock speed in Hz  */

extern uint32_t Apb1PeriphClock ;  /*!< Contains the APB1 Periph clock speed in Hz */
extern uint32_t Apb2PeriphClock ;  /*!< Contains the APB2 Periph clock speed in Hz */


// ************************************************************************************
// *********************************** ENUM *******************************************
// ************************************************************************************


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
  \defgroup SystemClk_Group 03 - System Clock primitives
  \brief    System Clock primitives.
	@{
*/

void System_Clock_Init(void);
/**@} end of System Clock interface */
#endif /* SYSCLOCK_H */

