#include "LPC23xx.h"

#define PCLK 12e6
#define TAILLE_MSG 16
#define MOTIF_E 150
#define MOTIF_1 75
#define MOTIF_0 50

unsigned int message[TAILLE_MSG]; 
unsigned int new_message; unsigned int index_message = TAILLE_MSG; unsigned int cpt = MOTIF_E;

void init_message(int type_Message, int num_Robot, int LL_ABCD, int num_Poste){
	int index = TAILLE_MSG-2;
	message[TAILLE_MSG-1] = MOTIF_E; 
  message[index]=( type_Message&0x04)? MOTIF_1: MOTIF_0; index--;
	message[index]=( type_Message&0x02)? MOTIF_1: MOTIF_0; index--;
	message[index]=( type_Message&0x01)? MOTIF_1: MOTIF_0; index--;
	message[index]=( num_Robot&0x08)? MOTIF_1: MOTIF_0; index--;
	message[index]=( num_Robot&0x04)? MOTIF_1: MOTIF_0; index--;
	message[index]=( num_Robot&0x02)? MOTIF_1: MOTIF_0; index--;
	message[index]=( num_Robot&0x01)? MOTIF_1: MOTIF_0; index--;
	message[index]=( LL_ABCD&0x02)? MOTIF_1: MOTIF_0; index--;
	message[index]=( LL_ABCD&0x01)? MOTIF_1: MOTIF_0; index--;
	message[index]=( num_Poste&0x10)? MOTIF_1: MOTIF_0; index--;
	message[index]=( num_Poste&0x08)? MOTIF_1: MOTIF_0; index--;
	message[index]=( num_Poste&0x04)? MOTIF_1: MOTIF_0; index--;
	message[index]=( num_Poste&0x02)? MOTIF_1: MOTIF_0; index--;
	message[index]=( num_Poste&0x01)? MOTIF_1: MOTIF_0; index--;
	message[index] = MOTIF_E;

}


void isr_PWM(void)__irq
{	
	cpt--;//nombre de periode à 50Khz restantes pour le motif
	//decider si on doit emettre ou pas (début motif ou pause intermotif)
	if (cpt <= 25){	PWM1MR1 = 0;}else{	PWM1MR1 = 120;}//0% ou 50%
		PWM1LER |= 0x2;
	if (cpt == 0){//motif fini, il faut charger le motif_suivant
		 if (index_message>0) {index_message--;new_message=1;} //changer de motif sauf si on a fini
     cpt = message[index_message];  
	}
	VICVectAddr = 0;
	PWM1IR = 1;
}

void init_PWM(void)
{
    PINSEL4 |= 0x00000001;  // P2.0   en PWM   01 
    PWM1PCR = (1<<9);       // PMW1.1 Output Enable + Single edge
    PWM1CTCR = 0x00;
	  PWM1MR0 = 240 ;         // 20*10e-6 * PCLK
    PWM1MR1 = 120;          // 50* PWM1MR0 / 100
    PWM1TCR = 0x9;     // Counter Enable + PWM enable
    PWM1MCR = 0x3;     // INTERUPTION + RESET si TC = MR0
    PWM1LER |= 0x7;    // Enable PWM Match 0 and 1 2 and  Latches   111
	  VICVectAddr8 = (unsigned long) isr_PWM ;
	  VICIntEnable =1<<8 ;  
}

int main (){
	unsigned int type_msg = 1, robot = 0,  LL = 3,  poste = 7;
  init_message(type_msg,robot,LL,poste);
	init_PWM();
	while (1);
}

