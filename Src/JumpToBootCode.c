/**
 * Function to perform jump to system memory boot from user application
 *
 * Call function when you want to jump to system memory
 */
 
// https://stm32f4-discovery.net/2017/04/tutorial-jump-system-memory-software-stm32/
/*
RAM - 12 Kbyte starting from address 0x20000000 are used by the bootloader firmware
System memory - 29 Kbyte starting from address 0x1FFF0000, contain the bootloader firmware

CAN2 bootloader
CAN2 Enabled Once initialized the CAN2 configuration is:Baudrate 125 kbps, 11-bit identifier.
Note: CAN1 is clocked during CAN2 bootloader execution because CAN1 manages the
      communication between CAN2 and SRAM.
CAN2_RX pin Input PB05 pin: CAN2 in reception mode
CAN2_TX pin Output PB13 pin: CAN2 in transmission mode

CLI Tool in python using socketCAN : https://github.com/marcinbor85/can-prog
*/

#include "main.h"

/**
  * @brief  Resets the RCC clock configuration to the default reset state.
  * @note   The default reset state of the clock configuration is given below:
  *            - HSI ON and used as system clock source
  *            - HSE, PLL and PLLI2S OFF
  *            - AHB, APB1 and APB2 prescaler set to 1.
  *            - CSS, MCO1 and MCO2 OFF
  *            - All interrupts disabled
  * @note   This function doesn't modify the configuration of the
  *            - Peripheral clocks  
  *            - LSI, LSE and RTC clocks 
  * @param  None
  * @retval None
  */
void RCC_DeInit(void)
{
  /* Set HSION bit */
  RCC->CR |= (uint32_t)0x00000001;

  /* Reset CFGR register */
  RCC->CFGR = 0x00000000;

  /* Reset HSEON, CSSON, PLLON, PLLI2S and PLLSAI(STM32F42xxx/43xxx/446xx/469xx/479xx devices) bits */
  RCC->CR &= (uint32_t)0xEAF6FFFF;
  
  /* Reset PLLCFGR register */
  RCC->PLLCFGR = 0x24003010;

#if defined(STM32F40_41xxx) || defined(STM32F427_437xx) || defined(STM32F429_439xx) || defined(STM32F401xx) || defined(STM32F411xE) || defined(STM32F446xx) || defined(STM32F413_423xx) || defined(STM32F469_479xx)  
  /* Reset PLLI2SCFGR register */
  RCC->PLLI2SCFGR = 0x20003000;
#endif /* STM32F40_41xxx || STM32F427_437xx || STM32F429_439xx || STM32F401xx || STM32F411xE || STM32F446xx || STM32F413_423xx || STM32F469_479xx */

#if defined(STM32F40_41xxx) || defined(STM32F427_437xx) || defined(STM32F429_439xx) || defined(STM32F446xx) || defined(STM32F469_479xx) 
  /* Reset PLLSAICFGR register, only available for STM32F42xxx/43xxx/446xx/469xx/479xx devices */
  RCC->PLLSAICFGR = 0x24003000;
#endif /* STM32F40_41xxx || STM32F427_437xx || STM32F429_439xx || STM32F446xx || STM32F469_479xx */
  
  /* Reset HSEBYP bit */
  RCC->CR &= (uint32_t)0xFFFBFFFF;

  /* Disable all interrupts */
  RCC->CIR = 0x00000000;

  /* Disable Timers clock prescalers selection, only available for STM32F42/43xxx and STM32F413_423xx devices */
  RCC->DCKCFGR = 0x00000000;
  
#if defined(STM32F410xx) || defined(STM32F413_423xx)
  /* Disable LPTIM and FMPI2C clock prescalers selection, only available for STM32F410xx and STM32F413_423xx devices */
  RCC->DCKCFGR2 = 0x00000000;
#endif /* STM32F410xx || STM32F413_423xx */  
}


void JumpToBootloader(void)
{
	typedef void (*pFunction)(void);
	
	pFunction JumpToApplication;
// LBR	uint32_t JumpAddress;
	RCC_DeInit();
	//HAL_RCC_DeInit();
	SysTick->CTRL = 0;
	SysTick->LOAD = 0;
	SysTick->VAL = 0;
	/** * Step: Disable all interrupts */
	__disable_irq();
	/* ARM Cortex-M Programming Guide to Memory Barrier Instructions.*/
	__DSB();
	// LBR Code follow __HAL_SYSCFG_REMAPMEMORY_SYSTEMFLASH();
	SYSCFG->MEMRMP &= ~(SYSCFG_MEMRMP_MEM_MODE);
	SYSCFG->MEMRMP |= SYSCFG_MEMRMP_MEM_MODE_0;
	
	/* Remap is bot visible at once. Execute some unrelated command! */
	__DSB();
	__ISB();
	JumpToApplication = (void (*)(void)) (*((uint32_t *)(0x1FFF0000 + 4))); // PC
	/* Initialize user application's Stack Pointer */
	__set_MSP(*(__IO uint32_t*) 0x1FFF0000);
	JumpToApplication();
}

#if 0
// LBR
#define USE_STDPERIPH_DRIVER


void JumpToBootloader(void) {
	void (*SysMemBootJump)(void);
	
	/**
	 * Step: Set system memory address. 
	 *       
	 *       For STM32F429, system memory is on 0x1FFF 0000
	 *       For other families, check AN2606 document table 110 with descriptions of memory addresses 
	 */
	volatile uint32_t addr = 0x1FFF0000;
	
	/**
	 * Step: Disable RCC, set it to default (after reset) settings
	 *       Internal clock, no PLL, etc.
	 */
#if defined(USE_HAL_DRIVER)
	HAL_RCC_DeInit();
#endif /* defined(USE_HAL_DRIVER) */
#if defined(USE_STDPERIPH_DRIVER)
	RCC_DeInit();
#endif /* defined(USE_STDPERIPH_DRIVER) */
	
	/**
	 * Step: Disable systick timer and reset it to default values
	 */
	SysTick->CTRL = 0;
	SysTick->LOAD = 0;
	SysTick->VAL = 0;

	/**
	 * Step: Disable all interrupts
	 */
	__disable_irq();
	
	/**
	 * Step: Remap system memory to address 0x0000 0000 in address space
	 *       For each family registers may be different. 
	 *       Check reference manual for each family.
	 *
	 *       For STM32F4xx, MEMRMP register in SYSCFG is used (bits[1:0])
	 *       For STM32F0xx, CFGR1 register in SYSCFG is used (bits[1:0])
	 *       For others, check family reference manual
	 */
	//Remap by hand... {
#if defined(STM32F4)
	SYSCFG->MEMRMP = 0x01;
#endif
#if defined(STM32F0)
	SYSCFG->CFGR1 = 0x01;
#endif
	//} ...or if you use HAL drivers
	//__HAL_SYSCFG_REMAPMEMORY_SYSTEMFLASH();	//Call HAL macro to do this for you
	
	/**
	 * Step: Set jump memory location for system memory
	 *       Use address with 4 bytes offset which specifies jump location where program starts
	 */
	SysMemBootJump = (void (*)(void)) (*((uint32_t *)(addr + 4)));
	
	/**
	 * Step: Set main stack pointer.
	 *       This step must be done last otherwise local variables in this function
	 *       don't have proper value since stack pointer is located on different position
	 *
	 *       Set direct address location which specifies stack pointer in SRAM location
	 */
	__set_MSP(*(uint32_t *)addr);
	
	/**
	 * Step: Actually call our function to jump to set location
	 *       This will start system memory execution
	 */
	SysMemBootJump();
	
	/**
	 * Step: Connect USB<->UART converter to dedicated USART pins and test
	 *       and test with bootloader works with STM32 Flash Loader Demonstrator software
	 */
}
#endif
