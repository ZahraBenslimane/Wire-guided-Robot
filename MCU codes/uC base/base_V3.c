#include "LPC23xx.h"

#define PCLK 12e6
#define TAILLE_MSG 16
#define MOTIF_E 150
#define MOTIF_1 75
#define MOTIF_0 50

int i;
unsigned int index_message = TAILLE_MSG;
unsigned int message[TAILLE_MSG];  //  = {MOTIF_E,MOTIF_1, MOTIF_1 ,MOTIF_0,MOTIF_E}
unsigned int cpt = MOTIF_E;


//
void init_message(int type_Message[], int num_Robot[], int LL_ABCD[], int num_Poste[]){
	//cpt = MOTIF_E;
	int index = TAILLE_MSG-2;
	
	message[TAILLE_MSG-1] = MOTIF_E;

	for (i=0;i<3;i++){
		if( type_Message[i] == 1){ type_Message[i] = MOTIF_1;}  else{ type_Message[i] = MOTIF_0;} 
		message[index]= type_Message[i]; index --;
	}
		for (i=0;i<4;i++){
		if( num_Robot[i] == 1){ num_Robot[i] = MOTIF_1;}  else{ num_Robot[i] = MOTIF_0;} 
		message[index]= num_Robot[i];	index --;
	}
		for (i=0;i<2;i++){
		if( LL_ABCD[i] == 1){ LL_ABCD[i] = MOTIF_1;}   else{ LL_ABCD[i] = MOTIF_0;} 
		message[index]= LL_ABCD[i]; index --;
	}
		for (i=0;i<5;i++){
		if( num_Poste[i] == 1){ num_Poste[i] = MOTIF_1;} else{ num_Poste[i] = MOTIF_0;} 
		message[index]= num_Poste[i];  index --;
	}
		message[index] = MOTIF_E;

}


void isr_PWM(void)__irq
{	
	cpt--;//nombre de periode à 50Khz restantes pour le motif
	//decider si on doit emettre ou pas (début motif ou pause intermotif)
	if (cpt <= 25){	PWM1MR1 = 0;}else{	PWM1MR1 = 120;}//0% ou 50%
		PWM1LER |= 0x2;
	if (cpt == 0){//motif fini, il faut charger le motif_suivant
		 if (index_message>0) {index_message--;} //changer de motif sauf si on a fini
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
	int type_msg[] = {0,0,1}, robot[] = {0,0,0,0},  LL[] = {1,1},  poste[] = {0,1,0,1,1};
  init_message(type_msg,robot,LL,poste);
	init_PWM();
	
	while (1);
}

