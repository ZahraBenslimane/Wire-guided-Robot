#include "LPC23xx.h"
#include "math.h"
#define freq_ech 0.0001 // la freq d'ech
#define freq_sin 1 // la freq su sinus
#define per_timer 12000 // periode du timer pour avoir une inter et calculer vout
#define Vref 3.3  
#define Vmax 1.65
#define pi 3.1415926535897932
float Vout ;
float t=0;

unsigned int son1k[37] = {511,611};

void isr_timer0 (void)__irq
{

Vout= Vmax + Vmax*sin(2*pi*t) ;

DACR = (int)((1024/Vref)*Vout) << 6 ;  

t= t + freq_ech  ;
VICVectAddr = 0;
T0IR =1 ;
}

void init_timer ()
{
T0MR0 =  per_timer  ;   // T0MR0= T X F(PCLK)
T0MCR = 0x3 ;     // interruption + reset
VICIntEnable = ( 1 << 4) ;     // vecteur d'interruption du timer 0
VICVectAddr4 = (unsigned long) isr_timer0 ; // activation de l'interruption du timer 0
T0TCR = 1 ;   // activation du timer + début du comptage
}

int main()
{
PINSEL1 = 1 << 21 ;
init_timer ();
    while(1);     
}






//unsigned int son1K[37] = {511,611,707,795,872,936,983,1012,1022,1012,983,936,872,799,795,707,611,511,411,315,227,150,
                         // 86,39,10,0,10,39,86,150,227,315,411,511};
													
													
//unsigned int son1K[37] = {512, 569, 626, 683, 740, 797, 854,911, 968, 1024, 968, 911, 854, 797, 740, 683, 626, 569, 512,
 


//unsigned int son1K[37] = {511,611,707,795,872,936,983,1012,1022,1012,983,936,872,799,795,707,611,511,411,315,227,150,
                         // 86,39,10,0,10,39,86,150,227,315,411,511};
													
													
//unsigned int son1K[37] = {512, 569, 626, 683, 740, 797, 854,911, 968, 1024, 968, 911, 854, 797, 740, 683, 626, 569, 512,
                         // 455, 398, 341, 284, 227, 170, 113, 56, 0, 56, 113, 170, 227 ,284 , 314, 398, 455, 512};
	