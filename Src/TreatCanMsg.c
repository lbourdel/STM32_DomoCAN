/**
 * \file      TreatCanMsg.c
 * \author    L. BOURDEL
 * \version   1.0
 * \date      2020
 * \brief     Primitives functions for CAM msg received
 *
 * \details   !!! Good Luck !!!
 */

#include "TreatCanMsg.h"


/**
 * \brief     Configure I/O pin
 * \note   This function also enables clock for GPIO port (RCC->AHB1ENR)
 * \details   
 * \param			PinId : Id enumeration see PinNames.h
 * \param			moder : ->MODER register, Select GPIO mode. This parameter can be a value of @ref GpioPinFunctionEnum enumeration
 * \param			output_type : ->OTYPER register, Select GPIO Output type. This parameter can be a value of @ref GpioOutputTypeEnum enumeration
 * \param			output_speed : ->OSPEEDR, Select GPIO speed. This parameter can be a value of @ref GpioOutputSpeedEnum enumeration
 * \param			pull_up_down : ->PUPDR register, Select GPIO pull resistor. This parameter can be a value of @ref GpioPullUpDownEnum enumeration GPIO Push-Pull
 * \retval None  
 */

