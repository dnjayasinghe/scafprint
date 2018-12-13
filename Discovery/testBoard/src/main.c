/***************************************************
** HAL pour Test Implementation ECC sur stm32f4 : 
** Author: ZN, MD
** Date de creation : 		14 juillet 2014
** Description : HAL + interface VCP pour essais SCA
** Sur STM32F407
** utilisation du port usb OTG comme bridge 
** USB <-> Serie . Peripherique compatible avec 
** le driver VCP de linux 2.6. Periphérique dispo
** sous le nom /dev/ttyACMx
** redirection de stdout vers ce port serie (utilisation
** de printf() possible )
****************************************************/
//#include "config.h"
 

#include "stm32f4xx_conf.h"
#include "stm32f4xx.h"   //pour le trigger
//#include "main.h"

#include "usbd_cdc_core.h"
#include "usbd_usr.h"
#include "usbd_desc.h"
#include "usbd_cdc_vcp.h"
#include <stdio.h>
#include <stdlib.h>
#include "mini-gmp.h"
#include "tm_stm32f4_usb_vcp.h"
#include "aes.h"
#include "aes0.h"
#include <time.h>

void calculation_test();
void init();
void TobyteArray(char *, unsigned char *);
unsigned int SendDataCCycles(unsigned int *);
unsigned int SendDataCCyclesandCT(unsigned int *, unsigned char * );
unsigned int WaitSignal(uint8_t );
void ResetCYCCNT(void);
void StartCYCCNT(void);
void InitCYCCNT(void);

#define five_nop {__asm("nop");__asm("nop");__asm("nop");__asm("nop");__asm("nop");}
#define ten_nop five_nop five_nop
#define twenty_nop ten_nop ten_nop
#define fifty_nop twenty_nop twenty_nop ten_nop

#define KEY_SIZE 16
#define PT_SIZE 16
#define CT_SIZE PT_SIZE
#define TRACES 256
#define ROUNDS 2048000

 /*  Global variables for to set the cycle counter  */
 volatile unsigned int *DWT_CYCCNT     = (volatile unsigned int *)0xE0001004; //address of the register
 volatile unsigned int *DWT_CONTROL    = (volatile unsigned int *)0xE0001000; //address of the register
 volatile unsigned int *SCB_DEMCR      = (volatile unsigned int *)0xE000EDFC; //address of the register


volatile uint32_t time_var1, time_var2;
__ALIGN_BEGIN USB_OTG_CORE_HANDLE  USB_OTG_dev __ALIGN_END;


uint8_t TM_INT_USB_VCP_ReceiveBuffer[USB_VCP_RECEIVE_BUFFER_LENGTH];
uint32_t tm_int_usb_vcp_buf_in, tm_int_usb_vcp_buf_out, tm_int_usb_vcp_buf_num;
extern TM_USB_VCP_Result TM_USB_VCP_INT_Status;
extern LINE_CODING linecoding;
uint8_t TM_USB_VCP_INT_Init = 0;
USB_OTG_CORE_HANDLE	USB_OTG_dev;
extern uint8_t TM_INT_USB_VCP_ReceiveBuffer[USB_VCP_RECEIVE_BUFFER_LENGTH];

void load_ccm_section () __attribute__ ((section (".init")));
void load_ccm_section (){
    extern char _edata, _sccm, _eccm;
 
    char *src = &_edata;
    char *dst = &_sccm;
    while (dst < &_eccm) {
        *dst++ = *src++;
    }
}

int main(void) {
	printf("main\n\r");
	init();

	unsigned char *key =  malloc( sizeof(unsigned char) * KEY_SIZE ) ;
	unsigned char *pt  =  malloc( sizeof(unsigned char) * PT_SIZE ) ;
	unsigned char *ct  =  malloc( sizeof(unsigned char) * CT_SIZE ) ;

	unsigned char *PTGENkey =  malloc( sizeof(unsigned char) * KEY_SIZE ) ;
	unsigned char *PTGENpt  =  malloc( sizeof(unsigned char) * PT_SIZE ) ;
	unsigned char *PTGENct  =  malloc( sizeof(unsigned char) * CT_SIZE ) ;

	int i=0; int j=0; int k=0; int dir=0; int omit=0;

unsigned int *ClkCycles   =  malloc(TRACES*sizeof(unsigned int*));
//unsigned char *ctData     =  malloc( sizeof(unsigned char) * CT_SIZE * TRACES ) ;
  if(ClkCycles==NULL);
	 // MemAllocationERROR();

/* readkey     */
 // ReadKey(key);

  char SKey[]="00000000000000000000000000000000";
  char SPt[] ="00000000000000000000000000000000";
  TobyteArray(SKey,key);
 unsigned char keyAT[] = {
   0x01, 0xab, 0x02, 0xcd, 0x04, 0x05, 0xef, 0x00,
   0xb2, 0xc3, 0xd4, 0xe5, 0xf6, 0x7f, 0x00, 0x00,
         };
//0xb2, 0xc3, 0xd4, 0xe5, 0x54, 0x76, 0x98, 0xbf,
 unsigned char keyST[] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
         };

  TobyteArray(SPt,pt);

 char PTGENSKey[]="00000000000000000000000000000000";
 char PTGENSPt[] ="00000000000000000000000000000000";

 TobyteArray(PTGENSKey,PTGENkey);
 TobyteArray(PTGENSPt,PTGENpt);

  InitCYCCNT();
  unsigned int cyc[2];
  WaitSignal(0x45);
  ResetCYCCNT();
  StartCYCCNT();


  /*   Init AES */
	
	 AES_KEY wctx; AES_KEY PTGENwctx;
         private_AES_set_encrypt_key(keyAT, 128, &wctx);
         //private_AES_set_encrypt_key(PTGENkey, 128, &PTGENwctx);

  /*  Main loop  */
 	 for(i=0;i<ROUNDS;i++){
 		 // encrypt TRACES number of encryptions
 		 for(j=0;j<TRACES;j++){
 			 //Generate PT
			//pt[15] = pt[15]+1;
 			 //save data
			__disable_irq();
  			ResetCYCCNT();
 			cyc[0]= *DWT_CYCCNT;

 			 //Encrypt
 			//AES_encrypt(pt, ct, &wctx);  
			AES_encrypt(pt, ct, &wctx);  
			//aes_crypt_ecb(&aes, AES_ENCRYPT, pt, ct);
			
			
 			cyc[1]=*DWT_CYCCNT;
 			ClkCycles[j] = cyc[1]-cyc[0];
			__enable_irq();
			// for (k = 0; k < CT_SIZE; k++) {
			//	ctData[j*CT_SIZE+k] = pt[k];
				//pt[k]=ct[k];
		
			//	 } 

			//AES_encrypt(PTGENpt, PTGENct, &PTGENwctx); 
			 AES128_ECB_encrypt(PTGENpt, PTGENkey, PTGENct);
			for (k = 0; k < CT_SIZE; k++) { PTGENpt[k]= PTGENct[k];  }
pt[0]= PTGENct[0]; 
pt[5]= PTGENct[5]; 
pt[10]= PTGENct[10]; 
 
pt[1]= PTGENct[1];
pt[6]= PTGENct[6];
pt[11]= PTGENct[11];
pt[12]= PTGENct[12];

pt[2]= PTGENct[2]; 
pt[8]= PTGENct[8]; 
pt[13]= PTGENct[13]; 

pt[3]= PTGENct[3]; 
pt[4]= PTGENct[4];
pt[9]= PTGENct[9];
			/*
			if(pt[0]==255)
				pt[5] = pt[5] + 1;

			pt[0] = pt[0] + 1;
			*/
				
			
		/*	if(pt[0]==0 && omit==0)
			{
				dir=1;
				omit=1;
			}	
			else if (pt[0]==255 && omit ==0)
			{
				dir=0;
				omit=1;
			}
			else if (pt[0]==0 || pt[0]==255)
			{
				omit=0;			
			}

			if(dir==1 && omit==1)			
				pt[0] = pt[0] + 1;	
			else if (dir==0 && omit==1)
				pt[0] = pt[0] - 1;	
			*/				
			//pt=ct;
			
 			//ClkCycles[j] =cyc[1];
		

 		 }

 		 // send data to PC
 	//	SendData(CTtraces, start, end, TRACES);
 	if	(SendDataCCycles(ClkCycles)==1);// ComunicationPC2BoardERROR() ;
		//SendDataCCyclesandCT(ClkCycles, ctData);
 	 }

 	while (1);
	
	return 0;
}

/* wait for the signal from the computer */
unsigned int WaitSignal(uint8_t c){
	  uint8_t  read;
		int i=0;
		while(1){
			 //while(TM_USB_VCP_BufferEmpty==0);
			if(VCP_get_char(&read) >0 && read==c) // if some data arrived
						return 0;	// and that data is 'c'
			else
				fifty_nop  //some delay
			i++;
		}
		return 1;
}
unsigned int SendDataCCycles(unsigned int *CCyles){

	int i=0;
	GPIO_SetBits(GPIOD, GPIO_Pin_13);
	
	for(i=0;i<TRACES;i++){

		VCP_put_char((CCyles[i] >>24)&0xff);
			//return 1;
		VCP_put_char((CCyles[i] >>16)&0xff);
			//return 1;
		VCP_put_char((CCyles[i] >>8)&0xff);
			//return 1;
		VCP_put_char((CCyles[i] >>0)&0xff);
			//return 1;
		 // WaitSignal(0x46);
	}
	GPIO_ResetBits(GPIOD, GPIO_Pin_13);
for(i=0;i<TRACES*1000;i++);
	return 0;
}

unsigned int SendDataCCyclesandCT(unsigned int *CCyles, unsigned char * Data){

	int i=0; int j=0;
	GPIO_SetBits(GPIOD, GPIO_Pin_13);
	
	for(i=0;i<TRACES;i++){

		VCP_put_char((CCyles[i] >>24)&0xff);
			//return 1;
		VCP_put_char((CCyles[i] >>16)&0xff);
			//return 1;
		VCP_put_char((CCyles[i] >>8)&0xff);
			//return 1;
		VCP_put_char((CCyles[i] >>0)&0xff);
			//return 1;
		//for(j=0;j<16;j++)
		VCP_put_char(Data[i*CT_SIZE+0]);
		VCP_put_char(Data[i*CT_SIZE+1]);
		VCP_put_char(Data[i*CT_SIZE+2]);
		VCP_put_char(Data[i*CT_SIZE+3]);

		VCP_put_char(Data[i*CT_SIZE+4]);
		VCP_put_char(Data[i*CT_SIZE+5]);
		VCP_put_char(Data[i*CT_SIZE+6]);
		VCP_put_char(Data[i*CT_SIZE+7]);

		VCP_put_char(Data[i*CT_SIZE+8]);
		VCP_put_char(Data[i*CT_SIZE+9]);
		VCP_put_char(Data[i*CT_SIZE+10]);
		VCP_put_char(Data[i*CT_SIZE+11]);

		VCP_put_char(Data[i*CT_SIZE+12]);
		VCP_put_char(Data[i*CT_SIZE+13]);
		VCP_put_char(Data[i*CT_SIZE+14]);
		VCP_put_char(Data[i*CT_SIZE+15]);

		 //WaitSignal(0x46);
	}
	GPIO_ResetBits(GPIOD, GPIO_Pin_13);
	for(i=0;i<TRACES*10000;i++);  // for windows 7
	//for(i=0;i<TRACES*100;i++);
	return 0;
}

void TobyteArray(char *CharString, unsigned char *UCharArray ){

	 unsigned int i, t, hn, ln;

	    for (t = 0,i = 0; i < KEY_SIZE * 2 ; i+=2,++t) {

	            hn = CharString[i] > '9' ? (CharString[i]|32) - 'a' + 10 : CharString[i] - '0';
	            ln = CharString[i+1] > '9' ? (CharString[i+1]|32) - 'a' + 10 : CharString[i+1] - '0';

	            UCharArray[t] = (hn << 4 ) | ln;
	    }
	 return ;

}

void calculation_test() {
    for(;;) {

		//#if defined(__UART__)
		char x[100]={0};
		char y[65]={0};
		
		GPIO_SetBits(GPIOD, GPIO_Pin_13);
		fgets(x,65,stdin); //get point coordinates from usb port
		fgets(y,65,stdin);
		GPIO_ResetBits(GPIOD, GPIO_Pin_13);
		
		
		//code start here
  
	}
}

void init() {
	GPIO_InitTypeDef  GPIO_InitStructure;
	// ---------- SysTick timer -------- //
	if (SysTick_Config(SystemCoreClock / 1000)) {
		// Capture error
		while (1){};
	}
	// ---------- GPIO -------- //
	// GPIOD Periph clock enable
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);
	// Configure PD12, PD13, PD14 and PD15 in output pushpull mode
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13 | GPIO_Pin_13| GPIO_Pin_14| GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOD, &GPIO_InitStructure);

	// ------------- USB -------------- //
 USBD_Init(&USB_OTG_dev,
	            USB_OTG_FS_CORE_ID,
	            &USR_desc,
	            &USBD_CDC_cb,
	            &USR_cb);
	
	/* Return OK */
	//return TM_USB_VCP_OK;



}


/*
 * Called from systick handler
 */

void timing_handler() {
	if (time_var1) {
		time_var1--;
	}
	time_var2++;
}
void Delay(volatile uint32_t nCount) {
	time_var1 = nCount;
	while(time_var1){};
}

void _init() {

}

/* Initialize Counter */
void InitCYCCNT(){
	    *SCB_DEMCR = *SCB_DEMCR | 0x01000000;
		*DWT_CYCCNT = 0; // reset the counter
}

/* Initialize Counter */
void ResetCYCCNT(){
		*DWT_CYCCNT = 0; // reset the counter
}

/* start counter */
void StartCYCCNT(){
		*DWT_CONTROL = *DWT_CONTROL | 1 ; // enable the counter
}

/* start counter */
uint32_t  ReadCYCCNT(){
	return *DWT_CYCCNT;
}

