/**
 * \file      main.c
 * \author    L. BOURDEL
 * \version   1.0
 * \date      2018
 * \brief     Main function
 *
 * \details   !!! Good Luck !!!
 */
 
#include "main.h"

/* FOR DEBUG
http://www.keil.com/support/man/docs/ulink2/ulink2_trace_itm_viewer.htm
http://www.keil.com/appnotes/docs/apnt_286.asp
19) Configuring Serial Wire Viewer (SWV):

DON'T FORGET to set SystemCoreClock in Debug/Trace windows
+
22) printf with ITM

*/


/*----------------------------------------------------------------------------
 MAIN function
 *----------------------------------------------------------------------------*/
void assert_failed(uint8_t* file, uint32_t line) {
      printf("Assert fail at File %s Line %d", file, (int)line);
      while(1);   // hang here (behavior is our your choice)
} 


/**
 * \brief     Main function entry
 * \details   
 * \return    \e none.
 */
int main(void){
	
	
// Switch System Clock to maximum
// F446 : 180Mhz, Apb1TimerClock = 90Mhz Apb2TimerClock = 180Mhz
// Apb1PeriphClock = 45Mhz Apb2PeriphClock = 90Mhz
//	JumpToBootloader();
	System_Clock_Init(); 

	SysTick_Initialize(1000);  // Interrupt period  = 1ms (1/1khz)

	/* Configure the CAN peripheral */
	CAN_Config();
		
	printf("Welcome STM32 Domocan Project\n");

	while(1)
	{
		//Delay_ms(100);


		CanRouter();
	}
}
