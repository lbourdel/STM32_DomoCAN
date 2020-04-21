/**
 * \file      gpio.c
 * \author    L. BOURDEL
 * \version   1.0
 * \date      2017
 * \brief     Primitives functions for GPIO
 *
 * \details   !!! Good Luck !!!
 */

#include "gpio.h"


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
void GPIOx_Pin_Configure(PinNameEnum      PinId,  // I.E. : PC_5
                       GpioPinFunctionEnum  moder,	// GPIO Mode: Input(00, reset), Output(01), AlterFunc(10), Analog(11, reset)
																											
                       GpioOutputTypeEnum   output_type,		// GPIO Output Type: Output push-pull (0, reset), Output open drain (1)
																											
                       GpioOutputSpeedEnum  output_speed,		// GPIO Speed: Low speed (00), Medium speed (01), Fast speed (10), High speed (11)
                       GpioPullUpDownEnum  pull_up_down) 	// GPIO Push-Pull: No pull-up, pull-down (00), Pull-up (01), Pull-down (10), Reserved (11)
{
	GPIO_TypeDef* GPIOx;
												 
	int PinNum=GET_PIN_INDEX(PinId);
	int PortNum=GET_PORT_INDEX(PinId);

// Enable GPIO clock
	RCC->AHB1ENR |= (0x1U << PortNum);
	
	GPIOx=(GPIO_TypeDef*)(GPIOA_BASE+(PortNum*0x0400U)); // uint16_t = unsigned int

  GPIOx->MODER   = (GPIOx->MODER   & ~(0x3 << (2*PinNum))) | (moder         << (2*PinNum));
  GPIOx->OTYPER  = (GPIOx->OTYPER  & ~(0x1 <<    PinNum))  | (output_type  <<    PinNum);
  GPIOx->OSPEEDR = (GPIOx->OSPEEDR & ~(0x3 << (2*PinNum))) | (output_speed << (2*PinNum));
  GPIOx->PUPDR   = (GPIOx->PUPDR   & ~(0x3 << (2*PinNum))) | (pull_up_down << (2*PinNum));
}

/**
 * \brief     Configure I/O as Alternate Function for peripheral
 * \details   
 * \param			PinId : Id enumeration see PinNames.h
 * \param  		AlternateFunction : ->AFR register, See stm32f4xx_gpio_AF.h & Excel file for Pin peripheral functionnality 
 * \retval None  
 */
void GPIOx_PinAF (PinNameEnum      PinId,
                 uint8_t    AlternateFunction) {

	GPIO_TypeDef* GPIOx;
												 
	int PinNum=GET_PIN_INDEX(PinId);
		 
	if( IS_GPIO_AF(AlternateFunction)!=true)
		assert_failed(__FILE__,__LINE__);
												 
	GPIOx=(GPIO_TypeDef*)(GPIOA_BASE+(GET_PORT_INDEX(PinId)*0x0400U));

  if (PinNum >= 8) {
		PinNum%=8;
    GPIOx->AFR[1] = (GPIOx->AFR[1] & ~(0xF << (4*PinNum))) | (AlternateFunction << (4*PinNum));
  } else {
    GPIOx->AFR[0] = (GPIOx->AFR[0] & ~(0xF << (4*PinNum))) | (AlternateFunction << (4*PinNum));
  }
}

/**
 * \brief     Force output pin to '1'
 * \details   
 * \param			PinId : Id enumeration see PinNames.h
 * \return    \e none.
 */
void GPIOx_Set_Output_Pin(PinNameEnum PinId)
{
	GPIO_TypeDef* GPIOx;
												 
	GPIOx=(GPIO_TypeDef*)(GPIOA_BASE+(GET_PORT_INDEX(PinId)*0x0400U));
	
	GPIOx->ODR|= (0x1 << GET_PIN_INDEX(PinId));
}

/**
 * \brief     Force output pin to '0'
 * \details   
 * \param			PinId : Id enumeration see PinNames.h
 * \return    \e none.
 */
void GPIOx_Reset_Output_Pin(PinNameEnum PinId)
{
	GPIO_TypeDef* GPIOx;
	
	GPIOx=(GPIO_TypeDef*)(GPIOA_BASE+(GET_PORT_INDEX(PinId)*0x0400U));

  GPIOx->ODR  = (GPIOx->ODR  & ~(0x1 << GET_PIN_INDEX(PinId)));
}

/**
 * \brief     Toggle output pin between VCC and GND
 * \details   
 * \param			PinId : Id enumeration see PinNames.h
 * \return    \e none.
 */
void GPIOx_Toggle_Output_Pin(PinNameEnum PinId)
{
	GPIO_TypeDef* GPIOx;
											
	GPIOx=(GPIO_TypeDef*)(GPIOA_BASE+(GET_PORT_INDEX(PinId)*0x0400U));
	
	GPIOx->ODR^= (0x1 << GET_PIN_INDEX(PinId));
}

/**
 * \brief     Set output PORT I/O
 * \details   val  : value to set
 * \details   mask : modify only bit specified by mask
 * \return    \e none.
 */
void GPIOx_Set_Output_Port(GPIO_TypeDef* GPIOx, uint16_t val, uint16_t mask)
{	
	GPIOx->ODR  = (GPIOx->ODR  & ~mask)  | val;
}

/**
 * \brief     Read input status
 * \details   val  : value to set
 * \param			PinId : Id enumeration see PinNames.h
 * \return    \e boolean : 1 or 0.
 */
bool GPIOx_Read_Input_Pin(PinNameEnum PinId)
{
	GPIO_TypeDef* GPIOx;
	
	GPIOx=(GPIO_TypeDef*)(GPIOA_BASE+(GET_PORT_INDEX(PinId)*0x0400U));

  return (GPIOx->IDR & (0x01<<GET_PIN_INDEX(PinId)) );
}

/**
 * \brief     Configure Alternate Function
 * \param			PinId : Id enumeration see PinNames.h
 * \param			AlternateFunction : between 0 to 15
 * \param			AFR_Register : register to modify
 * \return    \e boolean : true if succeed, otherwise false.
 */
bool GPIO_PinAF(uint8_t PinId,
                uint8_t AlternateFunction,
								uint32_t AFR_Register[2]	) 
{
  if ( PinId >= 16) return false;
	
	PinId/=8;
	PinId%=8;
	AFR_Register[(PinId/8)] = (AFR_Register[1] & ~(0xF << (4*(PinId%8)))) | (AlternateFunction << (4*(PinId%8)));

	
  if (PinId >= 8) {
		PinId%=8;
    AFR_Register[1] = (AFR_Register[1] & ~(0xF << (4*PinId))) | (AlternateFunction << (4*PinId));
  } else {
    AFR_Register[0] = (AFR_Register[0] & ~(0xF << (4*PinId))) | (AlternateFunction << (4*PinId));
  }

  return true;
}
