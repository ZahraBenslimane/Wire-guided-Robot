#include "LPC23xx.h"

#define PCLK 12e6
#define PERIODE 10e-3 

#define G_D_Bob_av  ((FIO2PIN&(1<<10)))   // if == 0 : //la bobine avant est à droite    else //la bobine avant est à gauche
#define G_D_Bob_ar  ((FIO2PIN&(1<<11)))   // if == 0 : //la bobine arrière est à droite  else //la bobine arrière est à gauche

#define SET_RESET_LED_AV(etat)   if(etat==1){FIO0PIN0 |= 0x01 ;} else {FIO0PIN0 &= 0xFE ;}   // P2.0
#define SET_RESET_LED_AR(etat)   if(etat==1){FIO0PIN0 |= 0x02 ;} else {FIO0PIN0 &= 0xFD ;}   // P2.1


unsigned long etat0_Bob_av;
int is_first_Timer_interruptio = 1;
unsigned int stock;

void ISR_T0()__irq{
	
	// Détection de coté 
	if( !G_D_Bob_av ){ SET_RESET_LED_AV(1);} else {SET_RESET_LED_AV(0);}
	if( !G_D_Bob_ar ){ SET_RESET_LED_AR(1);} else {SET_RESET_LED_AR(0);}
	// Permutation de vitesse
	if(is_first_Timer_interruptio){etat0_Bob_av = ((FIO2PIN&(1<<10))); is_first_Timer_interruptio=0; } 
	if( G_D_Bob_av ^ etat0_Bob_av ){  
		  stock   = PWM1MR2;
      PWM1MR2 = PWM1MR1;
		  PWM1MR1 = stock; 
		  PWM1LER |= 0x7;
		  etat0_Bob_av = G_D_Bob_av;
	}  
	
	FIO2PIN0  ^= 0x04;
	VICVectAddr =0;
	T0IR = 1;
}

	
void init_T0(){
	
	T0MR0 = 0.01 * 12e+6;    // I quote Mr bras : "je n'est pas besoin de lire ça très souvent .. on a juste besoin de lire
	                        // quand on fait la mesure de distance ... on pas besoin de la lire plus que 100 fois par seconde"
	T0MCR = 3;              // Interuption + Reset
	VICIntEnable = (1 << 4);					// Activation du vecteur d'interruption 4
	VICVectAddr4 = (unsigned long)ISR_T0;	// Activation de l'interrupion du Timer 0
	T0TCR = 1;
}


void init_GPIO(){
	SCS|=1;
	// TIMER
	PINSEL4 = 0;        // P2.2 en GPIO
	FIO2DIR0 = 0x04;    // P2.2  en sortie
	// LED
	FIO0DIR = 0x03 ;    // P0.0 et P0.1 en sortie  
	FIO0PIN = 0x00 ;    
	// INPUTS
	FIO2DIR1 |= 0x00 ;  // P2.10  et P2.11 : INPUT
	//FIO2CLR = (1<<10) | (1<<11);  // P2.10  et P2.11 eteindre
	// ARRET COUP DE POING
	
}

void init_PWM(void)
{
    PINSEL4 |= 0x00000005;    // P2.0 et P2.1  en PWM   0101 
    PWM1PCR = (1<<9) | (1<<10);    // PMW1.1 et PWM.2 Output Enable + Single edge
    PWM1CTCR = 0x00;
    PWM1MR0 = PERIODE * PCLK;
    PWM1MR1 = 20* PWM1MR0 / 100;
    PWM1MR2 = 80* PWM1MR0 / 100;
    PWM1TCR = 0x9;     // Counter Enable + PWM enable
    PWM1MCR = 0x2;     // RESET si TC = MR0
    PWM1LER |= 0x7;     // Enable PWM Match 0 and 1 2 and  Latches   111
}

int main (){
	init_GPIO();
	init_T0();
	init_PWM();
	while (1);
}

