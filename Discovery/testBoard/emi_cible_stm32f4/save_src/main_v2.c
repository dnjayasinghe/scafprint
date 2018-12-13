
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


  for(cpt = 0; cpt < 2*100 ; cpt++){
	dword_state_tab[cpt] = 0 ;
	}

  while (1)
  {
    /* PD12 to be toggled */
        GPIO_SetBits(GPIOD, GPIO_Pin_12);
	Delay(TEMPO);
	GPIO_SetBits(GPIOD, GPIO_Pin_15);
	asm(
		"mov r7, #0xA5A5A5A5\n\t"
		"mov r1 , #0\n\t" // r1=cpt =0
		"mov r2, #0\n\t"  // r2=index=0
		"ldr r12, =0x20000200\n\t"//adresse table
	);

	asm(
		"1:\n\t"
		"str r2, [r12, r1]\n\t" 	// r6 = @(r0+r2)[]
		"add r2, #0x1\n\t"	// r6 += 1
		"add r1, #0x4\n\t"	// r6 += 4
		"str r7, [r12, r1]\n\t"	// @(r0 + r2)[] = r7
		"add r1, #0x4\n\t"	// r6+=4
		"cmp r2, #100\n\t"	// cmp r6, nb_tests
		"bne 1b \n\t"
	);
	GPIO_Write(GPIOD, 0xFF);

	asm("ldr	r1, =4096");
    
  }
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
