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
#include "can.h"

void assert_failed(uint8_t* file, uint32_t line) ;

#endif /* MAIN_H */

