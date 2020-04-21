/*! \mainpage STM32 LAB
 *
 * \section td2 TD 2 : GPIO
 *
 * \subsection step1 Step 1: Opening the box
 * This is the introduction.
 *
 * \section td3 TD 3 : External interrupt
 *
 * \subsection step1 Step 1: Opening the box
 * This is the introduction.
 *
 * \section td4 TD 4 : UltraSonic Sensor
 *
 * \subsection step1 Step 1: Opening the box
 * This is the introduction.
 *
 * \section td5 TD 2 : Remote Control
 *
 * \subsection step1 Step 1: Opening the box
 * This is the introduction.
 *
 *
 * etc...
 */
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
uint8_t flagReceived=0;
int main(void){
// Switch System Clock to maximum
// F446 : 180Mhz, Apb1TimerClock = 90Mhz Apb2TimerClock = 180Mhz
// Apb1PeriphClock = 45Mhz Apb2PeriphClock = 90Mhz
	System_Clock_Init(); 

	SysTick_Initialize(1000);  // Interrupt period  = 1ms (1/1khz)

	CAN_Config(CAN1);
	
	// Init Can ùsg
	CAN_SendTestMsg(CAN1);
	printf("Welcome STM32 Domocan Project\n");

	while(1)
	{
		Delay_ms(100);
		if (flagReceived)
		{
			flagReceived=0;
			CAN_SendTestMsg(CAN1);
		}
	}
}

