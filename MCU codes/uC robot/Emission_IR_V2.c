#include "LPC23xx.h"

// P2.0 : PWM 

#define PCLK 12e6
#define TAILLE_TRAME 52

int nb_trames=0;
int nb_entetes;
int etat=1;
unsigned int index_IR = 51;
unsigned int trame[TAILLE_TRAME]; 
unsigned int cpt_IR = 15;
int new_trame;
int pulse_IR = 1;

void isr_PWM(void)__irq
{	
	cpt_IR--;
	if (pulse_IR == 1 & etat==1){PWM1MR1 = 24;}else{PWM1MR1 = 0;} PWM1LER |= 0x2;
	if (cpt_IR == 0){
		 if(index_IR>0) {index_IR--;}else{index_IR=1;nb_entetes++;}
		 if(nb_entetes==3){index_IR=51;nb_entetes=0; if(etat==1){nb_trames++;}else{nb_trames--;}
		                   if(nb_trames==3){etat=0;}
		                   if(nb_trames==0){etat=1;}}
     pulse_IR = trame[index_IR];
		 cpt_IR = 15;
 }
	VICVectAddr = 0;
	PWM1IR = 1;
}

unsigned char checksum ;
unsigned long tab_checksum[4];
unsigned char status,vitesse,num;

void create_trame_IR(int num_robot[],int vitesse_robot[], int status_robot[] ){
	// Entete
	trame[0]=  0;
	trame[1]=  1;
	//CHECKSUM   checksum = ~((a + b + c + d) & 0x0F) + 1 ;
	 status=  0x00 | (  (status_robot[3]&(0x01))  | (status_robot[2]&(0x02)) | (status_robot[1]&(0x04)) | (status_robot[0]&(0x08))  );
	 vitesse= 0x00 | (  (vitesse_robot[3]&0x01)   | (vitesse_robot[2]&0x02)  | (vitesse_robot[1]&0x04)  | (vitesse_robot[0]&0x08)   );
	 num=     0x00 | (  (num_robot[3]&0x01)       | (num_robot[2]&0x02)      | (num_robot[1]&0x04)      | (num_robot[0]&0x08)       );
	
	 checksum = 0x00 | (~((status + vitesse + num ) & 0x0F) + 1 );
	//checksum = 0xE;
	
   tab_checksum[3]=checksum&0x01;tab_checksum[2]=(checksum&0x02)>>1;tab_checksum[1]=(checksum&0x04)>>2;tab_checksum[0]=(checksum&0x08)>>3;
   	
	//CHECKSUM
	if ((tab_checksum[0])==1){trame[13]= 1;trame[12]= 1;trame[11]= 0;}else{trame[13]= 1;trame[12]= 0;trame[11]= 0;}
	if ((tab_checksum[1])==1){trame[10]= 1;trame[9]= 1;trame[8]= 0;}else{trame[10]= 1;trame[9]= 0;trame[8]= 0;}
	if ((tab_checksum[2])==1){trame[7]= 1;trame[6]= 1;trame[5]= 0;}else{trame[7]= 1;trame[6]= 0;trame[5]= 0;}
  if ((tab_checksum[3])==1){trame[4]= 1;trame[3]= 1;trame[2]= 0;}else{trame[4]= 1;trame[3]= 0;trame[2]= 0;}
	//Status ROBOT		
	if ((status_robot[0])==1){trame[25]= 1;trame[24]= 1;trame[23]= 0;}else{trame[25]= 1;trame[24]= 0;trame[23]= 0;}
	if ((status_robot[1])==1){trame[22]= 1;trame[21]= 1;trame[20]= 0;}else{trame[22]= 1;trame[21]= 0;trame[20]= 0;}
	if ((status_robot[2])==1){trame[19]= 1;trame[18]= 1;trame[17]= 0;}else{trame[19]= 1;trame[18]= 0;trame[17]= 0;}
  if ((status_robot[3])==1){trame[16]= 1;trame[15]= 1;trame[14]= 0;}else{trame[16]= 1;trame[15]= 0;trame[14]= 0;}
	//Vitesse ROBOT
	if ((vitesse_robot[0])==1){trame[37]= 1;trame[36]= 1;trame[35]= 0;}else{trame[37]= 1;trame[36]= 0;trame[35]= 0;}
	if ((vitesse_robot[1])==1){trame[34]= 1;trame[33]= 1;trame[32]= 0;}else{trame[34]= 1;trame[33]= 0;trame[32]= 0;}
	if ((vitesse_robot[2])==1){trame[31]= 1;trame[30]= 1;trame[29]= 0;}else{trame[31]= 1;trame[30]= 0;trame[29]= 0;}
  if ((vitesse_robot[3])==1){trame[28]= 1;trame[27]= 1;trame[26]= 0;}else{trame[28]= 1;trame[27]= 0;trame[26]= 0;}			
	//Numéro ROBOT
	if ((num_robot[0])==1){trame[49]= 1;trame[48]= 1;trame[47]= 0;}else{trame[49]= 1;trame[48]= 0;trame[47]= 0;}
	if ((num_robot[1])==1){trame[46]= 1;trame[45]= 1;trame[44]= 0;}else{trame[46]= 1;trame[45]= 0;trame[44]= 0;}
	if ((num_robot[2])==1){trame[43]= 1;trame[42]= 1;trame[41]= 0;}else{trame[43]= 1;trame[42]= 0;trame[41]= 0;}
  if ((num_robot[3])==1){trame[40]= 1;trame[39]= 1;trame[38]= 0;}else{trame[40]= 1;trame[39]= 0;trame[38]= 0;}
	
  // Entete 
	trame[50]=  0;
	trame[51]=  1;	
	
}

void init_PWM(void)
{
    PINSEL4 |= 0x00000001;  // P2.0   en PWM   01 
    PWM1PCR = (1<<9);       // PMW1.1 Output Enable + Single edge
    PWM1CTCR = 0x00;
	  PWM1MR0 = 336 ;         // 28uS* PCLK
	  PWM1MR1 = 24;           // 2uS max : 28uS/14
    PWM1TCR = 0x9;     // Counter Enable + PWM enable
    PWM1MCR = 0x3;     // INTERUPTION + RESET si TC = MR0
    PWM1LER |= 0x7;    // Enable PWM Match 0 and 1 2 and  Latches   111
	  VICVectAddr8 = (unsigned long) isr_PWM ;
	  VICIntEnable =1<<8 ;  
}

int main (){
	int num_robot[]= {0,1,1,1}; int vitesse_robot[] = {0,0,1,1}; int status_robot[]={1,0,0,0};
	create_trame_IR(num_robot,vitesse_robot,status_robot);
	
	init_PWM();
	
	while (1);
}
