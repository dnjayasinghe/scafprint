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



/* Private function prototypes -----------------------------------------------*/
void Delay_hacked(uint32_t nCount);
/* Private functions ---------------------------------------------------------*/

typedef struct ECPoint{
  mpz_t x;
  mpz_t y;
  mpz_t z;
} ECPoint;

void print(mpz_t X,char* str){
  char* temp = malloc(sizeof(char) * 1000);
  mpz_get_str(temp,16,X);
  printf("%s%s",temp,str);
  free(temp);
}

void randk(mpz_t k,int size, mpz_t seed){
  unsigned int seed_ui;
  unsigned int m = 3365999333;
  char hexa[16]={'0','1','2','3','4','5','6','7','8','9','a','b','c','d','e','f'};
  char* k2 = malloc((size+1)*sizeof(char));
  mpz_t tmp;
  int i;
  mpz_init(tmp);
  mpz_mod_ui(tmp,seed,m);
  seed_ui = mpz_get_ui(tmp);
  srand(seed_ui);
  if(size > 23)
    k2[23]=hexa[rand()%15];
  for(i=0;i<size;i++){
    if(i!=23)
      k2[i]=hexa[rand()%16];
  }
  k2[size]= '\0';
  mpz_set_str(k,k2,16);
  mpz_clear(tmp);
  free(k2);
}

void print_ECPoint(ECPoint* e){
  print(e->x," <- X\r\n");
  print(e->y," <- Y\r\n");
  print(e->z," <- Z\r\n");
}

void EC_init(ECPoint* e,mpz_t x_coord, mpz_t y_coord, mpz_t z_coord){
  mpz_init(e->x);
  mpz_init(e->y);
  mpz_init(e->z);
  mpz_set(e->x,x_coord);
  mpz_set(e->y,y_coord);
  mpz_set(e->z,z_coord);
}

void EC_copy(ECPoint* e,ECPoint* e2){
  mpz_init(e->x);
  mpz_init(e->y);
  mpz_init(e->z);
  mpz_set(e->x,e2->x);
  mpz_set(e->y,e2->y);
  mpz_set(e->z,e2->z);
}

void EC_clear(ECPoint* e){
  mpz_clear(e->x);
  mpz_clear(e->y);
  mpz_clear(e->z);
}

void addmod(mpz_t res,mpz_t a, mpz_t b, mpz_t mod){
  mpz_add(res,a,b);
  mpz_mod(res,res,mod);
}

void submod(mpz_t res,mpz_t a, mpz_t b, mpz_t mod){
  mpz_sub(res,a,b);
  mpz_mod(res,res,mod);
}

void mulmod(mpz_t res,mpz_t a, mpz_t b, mpz_t mod){
  mpz_mul(res,a,b);
  mpz_mod(res,res,mod);
}

void mulmod_ui(mpz_t res,mpz_t a, unsigned int b, mpz_t mod){
  mpz_mul_ui(res,a,b);
  mpz_mod(res,res,mod);
}
void ecadd(ECPoint* res, ECPoint* P, ECPoint* Q,mpz_t mod){
  mpz_t U1,U2,V1,V2,U,V,W,A,VVV;
  mpz_init(U1);
  mpz_init(U2);
  mpz_init(V1);
  mpz_init(V2);
  mpz_init(U);
  mpz_init(V);
  mpz_init(W);
  mpz_init(A);
  mpz_init(VVV);

  mulmod(U1,Q->y,P->z,mod);
  mulmod(U2,P->y,Q->z,mod);
  mulmod(V1,Q->x,P->z,mod);
  mulmod(V2,P->x,Q->z,mod);
  submod(U,U1,U2,mod);
  submod(V,V1,V2,mod);
  mulmod(W,P->z,Q->z,mod);
  mulmod(A,U,U,mod);
  mulmod(A,A,W,mod);
  mulmod(VVV,V,V,mod);
  mulmod(VVV,VVV,V,mod);
  submod(A,A,VVV,mod);
  mulmod(VVV,V,V,mod);
  mulmod(VVV,VVV,V2,mod);
  mulmod_ui(VVV,VVV,2,mod);
  submod(A,A,VVV,mod);

  mulmod(res->x,V,A,mod);
  mulmod(VVV,V,V,mod);
  mulmod(VVV,VVV,V2,mod);
  submod(VVV,VVV,A,mod);
  mulmod(res->y,VVV,U,mod);
  mulmod(VVV,V,V,mod);
  mulmod(VVV,VVV,V,mod);
  mulmod(VVV,VVV,U2,mod);
  submod(res->y,res->y,VVV,mod);
  mulmod(res->z,V,V,mod);
  mulmod(res->z,res->z,V,mod);
  mulmod(res->z,res->z,W,mod);
  
  mpz_clear(U1);
  mpz_clear(U2);
  mpz_clear(V1);
  mpz_clear(V2);
  mpz_clear(U);
  mpz_clear(V);
  mpz_clear(W);
  mpz_clear(A);
  mpz_clear(VVV);
}


void ecdbl(ECPoint* res, ECPoint* P,mpz_t mod,mpz_t a){
  mpz_t W,S,B,H,temp;
  mpz_init(W);
  mpz_init(S);
  mpz_init(B);
  mpz_init(H);
  mpz_init(temp);
  mulmod(W,a,P->z,mod);
  mulmod(W,W,P->z,mod);
  mulmod(temp,P->x,P->x,mod);
  mulmod_ui(temp,temp,3,mod);
  addmod(W,W,temp,mod);
  mulmod(S,P->y,P->z,mod);
  mulmod(B,P->x,P->y,mod);
  mulmod(B,B,S,mod);
  mulmod(H,W,W,mod);
  mulmod_ui(temp,B,8,mod);
  submod(H,H,temp,mod);
  mulmod(res->x,H,S,mod);
  mulmod_ui(res->x,res->x,2,mod);
  mulmod_ui(res->y,B,4,mod);
  submod(res->y,res->y,H,mod);
  mulmod(res->y,res->y,W,mod);
  mulmod(temp,P->y,P->y,mod);
  mulmod(temp,temp,S,mod);
  mulmod(temp,temp,S,mod);
  mulmod_ui(temp,temp,8,mod);
  submod(res->y,res->y,temp,mod);
  mulmod(res->z,S,S,mod);
  mulmod(res->z,res->z,S,mod);
  mulmod_ui(res->z,res->z,8,mod);
  mpz_clear(W);
  mpz_clear(S);
  mpz_clear(B);
  mpz_clear(H);
  mpz_clear(temp);
}
void ecsm(ECPoint* res, ECPoint* P, mpz_t k, int size,mpz_t mod,mpz_t a){
  int i ;
  ECPoint tmp,tmp2;
  for (i=size-2;i>=0;i--){
    EC_copy(&tmp,res);
    ecdbl(res,&tmp,mod,a);
    EC_clear(&tmp);
    if(mpz_tstbit(k,i)){
      EC_copy(&tmp2,res);
      ecadd(res,&tmp2,P,mod);
      EC_clear(&tmp2);
    }
  }
}



void EC_to_affine(ECPoint* e,mpz_t p,mpz_t mod){
  mpz_t inv_z,p_2,tmp;mpz_init(inv_z);mpz_init(p_2);mpz_init(tmp);
  if(mpz_invert(inv_z,e->z,mod)){
    mulmod(e->x,e->x,inv_z,mod);
    mulmod(e->y,e->y,inv_z,mod);
     
    mulmod(e->z,e->z,inv_z,mod);
  }else{
    mpz_sub_ui(p_2,p,2);
    mpz_powm(tmp,e->z,p_2,mod);
    mulmod(e->x,e->x,tmp,mod);
    mulmod(e->y,e->y,tmp,mod);
    
    mulmod(e->z,e->z,tmp,mod);
  }
  mpz_clear(inv_z);
  mpz_clear(p_2);
  mpz_clear(tmp);
}

int EC_is_equal_in_Fr(ECPoint* e, ECPoint* e1, mpz_t mod){
  mpz_t temp_x,temp_y,temp_z;
  mpz_init(temp_x);
  mpz_init(temp_y);
  mpz_init(temp_z);
  mpz_mod(temp_x,e->x,mod);
  mpz_mod(temp_y,e->y,mod);
  mpz_mod(temp_z,e->z,mod);
  if((mpz_cmp(temp_x,e1->x)==0)&&(mpz_cmp(temp_y,e1->y)==0)&&(mpz_cmp(temp_z,e1->z)==0)){
    mpz_clear(temp_x);
    mpz_clear(temp_y);
    mpz_clear(temp_z);
    return 1;
  }else{
    mpz_clear(temp_x);
    mpz_clear(temp_y);
    mpz_clear(temp_z);
    return 0;
  }
}

void EC_in(ECPoint* e, mpz_t mod){
  mpz_mod(e->x,e->x,mod);
  mpz_mod(e->y,e->y,mod);
  mpz_mod(e->z,e->z,mod);
}

// Private variables
volatile uint32_t time_var1, time_var2;
__ALIGN_BEGIN USB_OTG_CORE_HANDLE  USB_OTG_dev __ALIGN_END;

// Private function prototypes
void Delay(volatile uint32_t nCount);
void init();
void hacked_ms_delay(int ms){
	while(ms--> 0){
			volatile int x = 5971;
			while(x-->0)
				__asm("nop");
	}	

}
void calculation_test();
static long BUFFER[30];
int main(void) {
  init(); printf("BUFFER: %p\n", &BUFFER);
	/*
	 * désactivation du buffering de STDOUT. Sinon rien n'est affiché avant 
	 * d'avoir envoyé un carctère "\n" ou avoir fait un flush du buff000001	0x6eede67f	0xe51er de l'UART.
	 */
	setbuf(stdout, NULL);
	fflush(stdout);
	calculation_test();
	for(;;) {
		GPIO_SetBits(GPIOD, GPIO_Pin_13);
		Delay(500);
		GPIO_ResetBits(GPIOD, GPIO_Pin_13);
		Delay(500);
	}

	return 0;
}

void calculation_test() {
	//#if defined(__UART__)
	//GPIO_SetBits(GPIOD, GPIO_Pin_13);
	//code start here
  
	//GPIO_SetBits(GPIOD, GPIO_Pin_14);
	//Delay_hacked(TEMPO);
	//

	char * J = "000000000000000000000fffffffffffffffffffffffffff";
	char * H = "0472a1cb39961e381e048f1d7810cb98a1f0bc02b4a1fa2e";

	char* R = malloc(sizeof(char)*49);
	char* K = malloc(sizeof(char)*49);

	int v;
	for(v=0;v<48;v++){
		R[v]=J[v];
		K[v]=H[v];
	}

	printf("K: %p R: %p \r\n",K,R);
	K[48] = '\0';
	R[48] = '\0';
	mpz_t Fp,Fr,Zpr,ord,a,b,Gx,Gy,Gz,k;
	ECPoint P;
	ECPoint Qpr,Qr;
	int size=192;
	 //TODO : Mesurer les temps de calculs (demander à Zak)
  
	
	int flag;
	mpz_init(k);
  
	mpz_init(Fp);
	mpz_init(Fr);
  
	
	mpz_init(Zpr);
	mpz_init(ord);
	mpz_init(a);
	mpz_init(b);
	mpz_init(Gx);
	mpz_init(Gy);
	mpz_init(Gz);
	//printf("%s\r\n",K);
	mpz_set_str(Fp,"fffffffffffffffffffffffffffffffeffffffffffffffff",16);
	mpz_set_str(k,K,16);
	mpz_set_str(Fr,R,16);mpz_set_str(Fr,"3fd",16);

	mpz_mul(Zpr,Fp,Fr);
	mpz_set_str(ord,"ffffffffffffffffffffffff99def836146bc9b1b4d22831",16);
	mpz_set_str(a,"fffffffffffffffffffffffffffffffefffffffffffffffc",16);
	mpz_set_str(b,"64210519e59c80e70fa7e9ab72243049feb8deecc146b9b1",16);
	mpz_set_str(Gx,"188da80eb03090f67cbf20eb43a18800f4ff0afd82ff1012",16);
	mpz_set_str(Gy,"07192b95ffc8da78631011ed6b24cdd573f977a11e794811",16);
	mpz_set_ui(Gz,1);


//for(;;) {

	fflush(stdout);
	asm volatile ("MOVS r0, #255\n\
	MSR BASEPRI, r0");
	
	print(k," <-- k alea 192 bits\r\n");
  
	EC_init(&P,Gx,Gy,Gz);
	EC_copy(&Qpr,&P);
	EC_copy(&Qr,&P);

  // PROTECTED
	GPIO_SetBits(GPIOD, GPIO_Pin_14);
	Delay_hacked(TEMPO);
	GPIO_SetBits(GPIOD, GPIO_Pin_15);//<-----
	ecsm(&Qpr,&P,k,size,Zpr,a); // Faute là
	GPIO_Write(GPIOD, 0xFF);//<------
	EC_to_affine(&Qpr,Fp,Zpr);
	ecsm(&Qr,&P,k,size,Fr,a); // Faute là
	EC_to_affine(&Qr,Fp,Fr);
	flag = EC_is_equal_in_Fr(&Qpr,&Qr,Fr);
	if(flag){ // Faute là
	        EC_in(&Qpr,Fp);
		print_ECPoint(&Qpr);
		printf("Résultats identiques\r\n\n"); //TODO : Print un tableau pour les tests : 1 ou -1 (detected or not), res protected, res non protected
	}else{
		EC_in(&Qpr,Fp);
		printf("Erreur : résultats différents\r\n");
	}
	for (v = 0; v < Qpr.x->_mp_size; v++) BUFFER[v]=Qpr.x->_mp_d[v];
	for (v = 0; v < Qpr.y->_mp_size; v++) BUFFER[v+10]=Qpr.y->_mp_d[v];
	for (v = 0; v < Qpr.z->_mp_size; v++) BUFFER[v+20]=Qpr.z->_mp_d[v];
        for (v = 0; v < 30; v++) printf("%lx ", BUFFER[v]); printf("\r\n");
	printf("QprX: %p QprY %p QprZ %p flag: %p \r\n",&(Qpr.x),&Qpr.y, &Qpr.z,&flag);
	printf("size(x) = %d, addr(x) = %p\r\n", Qpr.x->_mp_size, Qpr.x->_mp_d);
	printf("size(y) = %d, addr(y) = %p\r\n", Qpr.y->_mp_size, Qpr.y->_mp_d);
	printf("size(z) = %d, addr(z) = %p\r\n", Qpr.z->_mp_size, Qpr.z->_mp_d);
	//dump Qpr,k,flag
	EC_clear(&P); 
	EC_clear(&Qpr);
	EC_clear(&Qr);
	asm volatile ("MOVS r0, #0\n\
	MSR BASEPRI, r0");	
	fflush(stdout);
	setbuf(stdout, NULL);
  
	//}
	mpz_clear(Zpr);
	mpz_clear(ord);
	mpz_clear(a);
	mpz_clear(b);
	mpz_clear(Gx);
	mpz_clear(Gy);
	mpz_clear(Gz);  

	mpz_clear(Fp);
	mpz_clear(Fr);
	free(R);
	free(K);
	mpz_clear(k);	
	//print(k,"\n");

		//end code
		
		
	////GPIO_Write(GPIOD, 0xFF);
	//GPIO_ResetBits(GPIOD, GPIO_Pin_13);
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
/**
  * @brief  Delay Function.
  * @param  nCount:specifies the Delay time length.
  * @retval None
  */
void Delay_hacked(uint32_t nCount)
{
  while(nCount--)
  {
	  asm(
		"nop\n\t"
	   );

  }
}

/*
 * Delay a number of systick cycles (1ms)
 */
void Delay(volatile uint32_t nCount) {
	time_var1 = nCount;
	while(time_var1){};
}

/*
 * Dummy function to avoid compiler error
 */
void _init() {

}


