/**
 * \file      main.h
 * \author    L. BOURDEL
 * \version   1.0
 * \date      2017
 * \brief     Interfaces for main program
 *
 * \details   !!! Good Luck !!!
 */

#ifndef MAIN_H
#define MAIN_H
#include <stdio.h>    // For printf (icon manage run time/compiler/IO/Stdout/ITM)
#include <stdlib.h>
#include <stdbool.h>
#include <stm32f4xx.h>
#include "PinNames.h"
#include "stm32f4xx_hal_gpio_ex.h"

#include "SysClock.h"
#include "gpio.h"
#include "SysTick.h"
#include "interrupt.h"
#include "timer.h"
#include "stm32f4xx_hal_can.h"
#include "can_router.h"
#include "can_common_cmds.h"

//void assert_failed(uint8_t* file, uint32_t line) ;




/**
  * @brief  The assert_param macro is used for function's parameters check.
  * @param  expr If expr is false, it calls assert_failed function
  *         which reports the name of the source file and the source
  *         line number of the call that failed. 
  *         If expr is true, it returns no value.
  * @retval None
  */
#define assert_param(expr) ((expr) ? (void)0U : assert_failed((uint8_t *)__FILE__, __LINE__))
/* Exported functions ------------------------------------------------------- */
  void assert_failed(uint8_t* file, uint32_t line);
	
	
	
#endif /* MAIN_H */

