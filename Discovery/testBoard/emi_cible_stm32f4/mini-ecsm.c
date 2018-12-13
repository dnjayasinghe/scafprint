#include <stdio.h>
#include <stdlib.h>
#include "mini-gmp.h"

typedef struct ECPoint{
  mpz_t x;
  mpz_t y;
  mpz_t z;
} ECPoint;

void print(mpz_t X,char* str){
  char* temp = malloc(sizeof(char) * 1000);
  mpz_get_str(temp,10,X);
  printf("%s%s",temp,str);
  free(temp);
}

void print_ECPoint(ECPoint* e){
  print(e->x," <- X\n");
  print(e->y," <- Y\n");
  print(e->z," <- Z\n");
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

void ecsm(ECPoint* res, ECPoint* P, mpz_t k, int size,mpz_t mod,mpz_t a){
  int i;
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

void EC_to_affine(ECPoint* e,mpz_t p,mpz_t mod){
  mpz_t inv_z,p_2;
  mpz_init(inv_z);
  mpz_init(p_2);
  if(mpz_invert(inv_z,e->z,mod)){
    mulmod(e->x,e->x,inv_z,mod);
    mulmod(e->y,e->y,inv_z,mod);
    mpz_set_ui(e->z,1); 
  }else{
    mpz_sub_ui(p_2,p,2);
    mpz_powm(e->x,e->x,p_2,mod);
    mpz_powm(e->y,e->y,p_2,mod);
    mpz_set_ui(e->z,1);
  }
  mpz_clear(inv_z);
  mpz_clear(p_2);
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

int main(int argc, char* argv[]){

  
  mpz_t Fp,Fr,Zpr,ord,a,b,Gx,Gy,Gz,k;
  ECPoint P;
  ECPoint Q,Qpr,Qr;
  int size;
  mpz_init(Fp);
  mpz_init(Fr);
  mpz_init(Zpr);
  mpz_init(ord);
  mpz_init(a);
  mpz_init(b);
  mpz_init(Gx);
  mpz_init(Gy);
  mpz_init(Gz);
  mpz_init(k);
  
  mpz_set_str(Fp,"fffffffffffffffffffffffffffffffeffffffffffffffff",16);
  mpz_set_str(Fr,"eed669c334e98d8d",16);
  mpz_mul(Zpr,Fp,Fr);
  mpz_set_str(ord,"ffffffffffffffffffffffff99def836146bc9b1b4d22831",16);
  mpz_set_str(a,"fffffffffffffffffffffffffffffffefffffffffffffffc",16);
  mpz_set_str(b,"64210519e59c80e70fa7e9ab72243049feb8deecc146b9b1",16);
  mpz_set_str(Gx,"188da80eb03090f67cbf20eb43a18800f4ff0afd82ff1012",16);
  mpz_set_str(Gy,"07192b95ffc8da78631011ed6b24cdd573f977a11e794811",16);
  mpz_set_ui(Gz,1);
  mpz_set_str(k,"28757b2",16);
  size = mpz_sizeinbase(k,2);
  print(k,"\n");
  printf("size : %d\n",size);
  EC_init(&P,Gx,Gy,Gz);
  EC_copy(&Q,&P);
  EC_copy(&Qpr,&P);
  EC_copy(&Qr,&P);

  ecsm(&Q,&P,k,size,Fp,a);
  EC_to_affine(&Q,Fp,Fp);
  print_ECPoint(&Q);

  
  ecsm(&Qpr,&P,k,size,Zpr,a);
  ecsm(&Qr,&P,k,size,Fr,a);
  EC_to_affine(&Qpr,Fp,Zpr);
  EC_to_affine(&Qr,Fp,Fr);
  print_ECPoint(&Qpr);
  print_ECPoint(&Qr);

  if(EC_is_equal_in_Fr(&Qpr,&Qr,Fr)){
    EC_in(&Qpr,Fp);
    print_ECPoint(&Qpr);
    printf("Résultats identiques\n");
  }else
    printf("Erreur : résultats différents\n");


  EC_clear(&P);
  EC_clear(&Q);
  EC_clear(&Qpr);
  EC_clear(&Qr);
  mpz_clear(Fp);
  mpz_clear(Fr);
  mpz_clear(Zpr);
  mpz_clear(ord);
  mpz_clear(a);
  mpz_clear(b);
  mpz_clear(Gx);
  mpz_clear(Gy);
  mpz_clear(Gz);  
  mpz_clear(k);
  return 0;
}
