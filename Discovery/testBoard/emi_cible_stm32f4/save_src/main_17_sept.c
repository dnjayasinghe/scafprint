
/* Includes ------------------------------------------------------------------*/
#include "stm32f4_discovery.h"
#include "stm32f4xx_conf.h"

/* Private typedef -----------------------------------------------------------*/
GPIO_InitTypeDef  GPIO_InitStructure;

/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
void Delay(__IO uint32_t nCount);
/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Main program
  * @param  None
  * @retval None
  */
int main(void)
{
  /*!< At this stage the microcontroller clock setting is already configured, 
       this is done through SystemInit() function which is called from startup
       file (startup_stm32f4xx.s) before to branch to application main.
       To reconfigure the default setting of SystemInit() function, refer to
        system_stm32f4xx.c file
     */

  /* GPIOD Periph clock enable */
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);

  /* Configure PD12, PD13, PD14 and PD15 in output pushpull mode */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12 | GPIO_Pin_13| GPIO_Pin_14| GPIO_Pin_15;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(GPIOD, &GPIO_InitStructure);


  int cpt =0;
  static uint32_t * dword_state_tab = ( uint32_t *)0x20000200 ;


  for(cpt = 0; cpt < 800 ; cpt++){
	dword_state_tab[cpt] = (cpt%2)? 0x44444444 : 0x77777777 ;
	}
	asm(
		"nop\n\t"
	   );
	

    /* PD12 to be toggled */
        GPIO_SetBits(GPIOD, GPIO_Pin_12);
	Delay(TEMPO);
	asm(	
		"ldr r2 , =0x00001111\n\t"
		"ldr r4,  =0x22220000\n\t"
		"ldr r5,  =0x00000005\n\t"
		"ldr r6 , =0x00000006\n\t"
		"ldr r7 , =0x00005555\n\t"
		"ldr r8 , =0xA5A50000\n\t"
		"ldr r9 , =0xAAAA5555\n\t"
		"ldr r10 ,=0x0000A5A5\n\t"
		"ldr r11, =0x20000600\n\t"
		"ldr r12, =0x20000200\n\t");//adresse table

	GPIO_SetBits(GPIOD, GPIO_Pin_15);
	asm(
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		//"MCR p15, 0, r0, c7, c5, 0 \n\t" // Flush entire ICache
	    "isb\n\t"//  try to Clean the entire pipeline stages and lock it
		"isb\n\t" // try several time since the isb may be skipped due 
		"isb\n\t" // to EMI : 4 instruction may be read from flash + 3 instruction may be skipped due to previous "nop" instructions
		"isb\n\t"
		"ldr r7 , =0xA5A5A5A5\n\t" // now pipeline should be clean and only ldr instruction fetched									<-This is used as marker for fault attack
								          // decoded/ 
		"isb\n\t"						  // Clean the entire pipeline stages and lock it
		//"MCR p15, 0, r0, c7, c5, 0 \n\t"  // Flush entire ICache
		"isb\n\t"						  // Clean the entire pipeline stages and lock it here only isb, add add mov should be read from flash memory
										  // and the pipeline should then be filled (1 cycle + time for ICash to refile (6 cycles))
		"add r2, #0x1\n\t" // 2 5 6 7 8 10 11		"add r7, #0x1\n\t"
		"add r4, #0x1\n\t"

		"rrx r8, r5 \n\t" // save flags 
		"rrx r9, r6 \n\t" // save flags 

		"subs r10 , r11, r5\n\t"	
		"subs r8 , r11, r6\n\t"	
		
	
		"str r7, [r12], #4\n\t"	// st r7, r12+
		"str r7, [r12], #4\n\t"	// st r7, r12+
		"str r7, [r12], #4\n\t"	// st r7, r12+
		"str r7, [r12], #4\n\t"	// st r7, r12+
		"str r7, [r12], #4\n\t"	// st r7, r12+	
        "str r7, [r12], #4\n\t"	// st r7, r12+
        "str r7, [r12], #4\n\t"	// st r7, r12+
        "str r7, [r12], #4\n\t"	// st r7, r12+
		"str r7, [r12], #4\n\t" // st r7, r12+
		"str r7, [r12], #4\n\t"	// st r7, r12+
        "str r7, [r12], #4\n\t"	// st r7, r12+
        "str r7, [r12], #4\n\t"	// st r7, r12+
        "str r7, [r12], #4\n\t"	// st r7, r12+
		"str r7, [r12], #4\n\t"	// st r7, r12+
        "str r7, [r12], #4\n\t"	// st r7, r12+
        "str r7, [r12], #4\n\t"	// st r7, r12+
        "str r7, [r12], #4\n\t"	// st r7, r12+
		"str r7, [r12], #4\n\t"	// st r7, r12+
        "str r7, [r12], #4\n\t"	// st r7, r12+
        "str r7, [r12], #4\n\t"	// st r7, r12+
        "str r7, [r12], #4\n\t"	// st r7, r12+
		"str r7, [r12], #4\n\t"	// st r7, r12+
        "str r7, [r12], #4\n\t"	// st r7, r12+
        "str r7, [r12], #4\n\t"	// st r7, r12+
        "str r7, [r12], #4\n\t"	// st r7, r12+
        "str r7, [r12], #4\n\t"	// st r7, r12+
        "str r7, [r12], #4\n\t"	// st r7, r12+
        "str r7, [r12], #4\n\t"	// st r7, r12+
        "str r7, [r12], #4\n\t"	// st r7, r12+
        "str r7, [r12], #4\n\t"	// st r7, r12+
        "str r7, [r12], #4\n\t"	// st r7, r12+
        "str r7, [r12], #4\n\t"	// st r7, r12+
        "str r7, [r12], #4\n\t"	// st r7, r12+
        "str r7, [r12], #4\n\t"	// st r7, r12+
        "str r7, [r12], #4\n\t"	// st r7, r12+
        "str r7, [r12], #4\n\t"	// st r7, r12+
        "str r7, [r12], #4\n\t"	// st r7, r12+
        "str r7, [r12], #4\n\t"	// st r7, r12+
        "str r7, [r12], #4\n\t"	// st r7, r12+
        "str r7, [r12], #4\n\t"	// st r7, r12+
        "str r7, [r12], #4\n\t"	// st r7, r12+
		"str r7, [r12], #4\n\t"	// st r7, r12+
        "str r7, [r12], #4\n\t"	// st r7, r12+
        "str r7, [r12], #4\n\t"	// st r7, r12+
        "str r7, [r12], #4\n\t"	// st r7, r12+
        "str r7, [r12], #4\n\t"	// st r7, r12+
        "str r7, [r12], #4\n\t"	// st r7, r12+
        "str r7, [r12], #4\n\t"	// st r7, r12+
        "str r7, [r12], #4\n\t"	// st r7, r12+
        "str r7, [r12], #4\n\t"	// st r7, r12+
        "str r7, [r12], #4\n\t"	// st r7, r12+
        "str r7, [r12], #4\n\t"	// st r7, r12+
        "str r7, [r12], #4\n\t"	// st r7, r12+
        "str r7, [r12], #4\n\t"	// st r7, r12+
        "str r7, [r12], #4\n\t"	// st r7, r12+
        "str r7, [r12], #4\n\t"	// st r7, r12+
        "str r7, [r12], #4\n\t"	// st r7, r12+
        "str r7, [r12], #4\n\t"	// st r7, r12+
        "str r7, [r12], #4\n\t"	// st r7, r12+
        "str r7, [r12], #4\n\t"	// st r7, r12+
        "str r7, [r12], #4\n\t"	// st r7, r12+
		"str r7, [r12], #4\n\t"	// st r7, r12+
        "str r7, [r12], #4\n\t"	// st r7, r12+
        "str r7, [r12], #4\n\t"	// st r7, r12+
        "str r7, [r12], #4\n\t"	// st r7, r12+
        "str r7, [r12], #4\n\t"	// st r7, r12+
        "str r7, [r12], #4\n\t"	// st r7, r12+
        "str r7, [r12], #4\n\t"	// st r7, r12+
        "str r7, [r12], #4\n\t"	// st r7, r12+
        "str r7, [r12], #4\n\t"	// st r7, r12+
        "str r7, [r12], #4\n\t"	// st r7, r12+
        "str r7, [r12], #4\n\t"	// st r7, r12+
        "str r7, [r12], #4\n\t"	// st r7, r12+
        "str r7, [r12], #4\n\t"	// st r7, r12+
        "str r7, [r12], #4\n\t"	// st r7, r12+
        "str r7, [r12], #4\n\t"	// st r7, r12+
        "str r7, [r12], #4\n\t"	// st r7, r12+
        "str r7, [r12], #4\n\t"	// st r7, r12+
        "str r7, [r12], #4\n\t"	// st r7, r12+
        "str r7, [r12], #4\n\t"	// st r7, r12+
        "str r7, [r12], #4\n\t"	// st r7, r12+
		"str r7, [r12], #4\n\t"	// st r7, r12+
        "str r7, [r12], #4\n\t"	// st r7, r12+
        "str r7, [r12], #4\n\t"	// st r7, r12+
        "str r7, [r12], #4\n\t"	// st r7, r12+
        "str r7, [r12], #4\n\t"	// st r7, r12+
        "str r7, [r12], #4\n\t"	// st r7, r12+
        "str r7, [r12], #4\n\t"	// st r7, r12+
        "str r7, [r12], #4\n\t"	// st r7, r12+
        "str r7, [r12], #4\n\t"	// st r7, r12+
        "str r7, [r12], #4\n\t"	// st r7, r12+
        "str r7, [r12], #4\n\t"	// st r7, r12+
        "str r7, [r12], #4\n\t"	// st r7, r12+
        "str r7, [r12], #4\n\t"	// st r7, r12+
        "str r7, [r12], #4\n\t"	// st r7, r12+
        "str r7, [r12], #4\n\t"	// st r7, r12+
        "str r7, [r12], #4\n\t"	// st r7, r12+
        "str r7, [r12], #4\n\t"	// st r7, r12+
        "str r7, [r12], #4\n\t"	// st r7, r12+
        "str r7, [r12], #4\n\t"	// st r7, r12+
        "str r7, [r12], #4\n\t"	// st r7, r12+
		"str r7, [r12], #4\n\t"	// st r7, r12+
        "str r7, [r12], #4\n\t"	// st r7, r12+
        "str r7, [r12], #4\n\t"	// st r7, r12+
        "str r7, [r12], #4\n\t"	// st r7, r12+
        "str r7, [r12], #4\n\t"	// st r7, r12+
        "str r7, [r12], #4\n\t"	// st r7, r12+
        "str r7, [r12], #4\n\t"	// st r7, r12+
        "str r7, [r12], #4\n\t"	// st r7, r12+
        "str r7, [r12], #4\n\t"	// st r7, r12+
        "str r7, [r12], #4\n\t"	// st r7, r12+
        "str r7, [r12], #4\n\t"	// st r7, r12+
        "str r7, [r12], #4\n\t"	// st r7, r12+
        "str r7, [r12], #4\n\t"	// st r7, r12+
        "str r7, [r12], #4\n\t"	// st r7, r12+
        "str r7, [r12], #4\n\t"	// st r7, r12+
        "str r7, [r12], #4\n\t"	// st r7, r12+
        "str r7, [r12], #4\n\t"	// st r7, r12+
        "str r7, [r12], #4\n\t"	// st r7, r12+
        "str r7, [r12], #4\n\t"	// st r7, r12+
        "str r7, [r12], #4\n\t"	// st r7, r12+
		"str r7, [r12], #4\n\t"	// st r7, r12+
        "str r7, [r12], #4\n\t"	// st r7, r12+
        "str r7, [r12], #4\n\t"	// st r7, r12+
        "str r7, [r12], #4\n\t"	// st r7, r12+
        "str r7, [r12], #4\n\t"	// st r7, r12+
        "str r7, [r12], #4\n\t"	// st r7, r12+
        "str r7, [r12], #4\n\t"	// st r7, r12+
        "str r7, [r12], #4\n\t"	// st r7, r12+
        "str r7, [r12], #4\n\t"	// st r7, r12+
        "str r7, [r12], #4\n\t"	// st r7, r12+
        "str r7, [r12], #4\n\t"	// st r7, r12+
        "str r7, [r12], #4\n\t"	// st r7, r12+
        "str r7, [r12], #4\n\t"	// st r7, r12+
        "str r7, [r12], #4\n\t"	// st r7, r12+
        "str r7, [r12], #4\n\t"	// st r7, r12+
        "str r7, [r12], #4\n\t"	// st r7, r12+
        "str r7, [r12], #4\n\t"	// st r7, r12+
        "str r7, [r12], #4\n\t"	// st r7, r12+
        "str r7, [r12], #4\n\t"	// st r7, r12+
        "str r7, [r12], #4\n\t"	// st r7, r12+
		"str r7, [r12], #4\n\t"	// st r7, r12+
        "str r7, [r12], #4\n\t"	// st r7, r12+
        "str r7, [r12], #4\n\t"	// st r7, r12+
        "str r7, [r12], #4\n\t"	// st r7, r12+
        "str r7, [r12], #4\n\t"	// st r7, r12+
        "str r7, [r12], #4\n\t"	// st r7, r12+
        "str r7, [r12], #4\n\t"	// st r7, r12+
        "str r7, [r12], #4\n\t"	// st r7, r12+
        "str r7, [r12], #4\n\t"	// st r7, r12+
        "str r7, [r12], #4\n\t"	// st r7, r12+
        "str r7, [r12], #4\n\t"	// st r7, r12+
        "str r7, [r12], #4\n\t"	// st r7, r12+
        "str r7, [r12], #4\n\t"	// st r7, r12+
        "str r7, [r12], #4\n\t"	// st r7, r12+
        "str r7, [r12], #4\n\t"	// st r7, r12+
        "str r7, [r12], #4\n\t"	// st r7, r12+
        "str r7, [r12], #4\n\t"	// st r7, r12+
        "str r7, [r12], #4\n\t"	// st r7, r12+
        "str r7, [r12], #4\n\t"	// st r7, r12+
        "str r7, [r12], #4\n\t"	// st r7, r12+
		"str r7, [r12], #4\n\t"	// st r7, r12+
        "str r7, [r12], #4\n\t"	// st r7, r12+
        "str r7, [r12], #4\n\t"	// st r7, r12+
        "str r7, [r12], #4\n\t"	// st r7, r12+
        "str r7, [r12], #4\n\t"	// st r7, r12+
        "str r7, [r12], #4\n\t"	// st r7, r12+
        "str r7, [r12], #4\n\t"	// st r7, r12+
        "str r7, [r12], #4\n\t"	// st r7, r12+
        "str r7, [r12], #4\n\t"	// st r7, r12+
        "str r7, [r12], #4\n\t"	// st r7, r12+
        "str r7, [r12], #4\n\t"	// st r7, r12+
        "str r7, [r12], #4\n\t"	// st r7, r12+
        "str r7, [r12], #4\n\t"	// st r7, r12+
        "str r7, [r12], #4\n\t"	// st r7, r12+
        "str r7, [r12], #4\n\t"	// st r7, r12+
        "str r7, [r12], #4\n\t"	// st r7, r12+
        "str r7, [r12], #4\n\t"	// st r7, r12+
        "str r7, [r12], #4\n\t"	// st r7, r12+
        "str r7, [r12], #4\n\t"	// st r7, r12+
        "str r7, [r12], #4\n\t"	// st r7, r12+
		"str r7, [r12], #4\n\t"	// st r7, r12+
        "str r7, [r12], #4\n\t"	// st r7, r12+
        "str r7, [r12], #4\n\t"	// st r7, r12+
        "str r7, [r12], #4\n\t"	// st r7, r12+
        "str r7, [r12], #4\n\t"	// st r7, r12+
        "str r7, [r12], #4\n\t"	// st r7, r12+
        "str r7, [r12], #4\n\t"	// st r7, r12+
        "str r7, [r12], #4\n\t"	// st r7, r12+
        "str r7, [r12], #4\n\t"	// st r7, r12+
        "str r7, [r12], #4\n\t"	// st r7, r12+
        "str r7, [r12], #4\n\t"	// st r7, r12+
        "str r7, [r12], #4\n\t"	// st r7, r12+
        "str r7, [r12], #4\n\t"	// st r7, r12+
        "str r7, [r12], #4\n\t"	// st r7, r12+
        "str r7, [r12], #4\n\t"	// st r7, r12+
        "str r7, [r12], #4\n\t"	// st r7, r12+
        "str r7, [r12], #4\n\t"	// st r7, r12+
        "str r7, [r12], #4\n\t"	// st r7, r12+
        "str r7, [r12], #4\n\t"	// st r7, r12+
        "str r7, [r12], #4\n\t"	// st r7, r12+
		"str r7, [r12], #4\n\t"	// st r7, r12+
        "str r7, [r12], #4\n\t"	// st r7, r12+
        "str r7, [r12], #4\n\t"	// st r7, r12+
        "str r7, [r12], #4\n\t"	// st r7, r12+
        "str r7, [r12], #4\n\t"	// st r7, r12+
        "str r7, [r12], #4\n\t"	// st r7, r12+
        "str r7, [r12], #4\n\t"	// st r7, r12+
        "str r7, [r12], #4\n\t"	// st r7, r12+
        "str r7, [r12], #4\n\t"	// st r7, r12+
        "str r7, [r12], #4\n\t"	// st r7, r12+
        "str r7, [r12], #4\n\t"	// st r7, r12+
		"str r7, [r12], #4\n\t"	// st r7, r12+
        "str r7, [r12], #4\n\t"	// st r7, r12+
		"str r7, [r12], #4\n\t"	// st r7, r12+
        "str r7, [r12], #4\n\t"	// st r7, r12+
		"str r7, [r12], #4\n\t"	// st r7, r12+
        "str r7, [r12], #4\n\t"	// st r7, r12+
	);
	GPIO_Write(GPIOD, 0xFF);
	asm(
		"nop\n\t"
	   );


}

/**
  * @brief  Delay Function.
  * @param  nCount:specifies the Delay time length.
  * @retval None
  */
void Delay(__IO uint32_t nCount)
{
  while(nCount--)
  {
  }
}


#ifdef  USE_FULL_ASSERT

/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{ 
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}
#endif

/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/
