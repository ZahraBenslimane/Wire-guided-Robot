#include "LPC23xx.h"

// P0.0 : LED BOBINE AVANT 
// P0.1 : LED BOBINE ARRIERE

// P2.0  : OUTPUT PWM1.1  
// P2.1  : OUTPUT PWM1.2
// P2.2  : Timer only for visualisation
// P2.3  : OUTPUT PWM1.4 IR 
// P2.4 -- P2.7 : INPUT DIP SWITCHES NUMERO ROBOT
// P2.10: ARRET COUP DE POING
// P2.11: INPUT etat BOBINE AVANT    : 0: Bob avant à droite | 0: Bob avant à gauche
// P2.12: INPUT etat BOBINE ARRIERE  : 0: Bob arrière à droite | 0: Bob arrière à gauche




/*****************************************************************************************************************************************/

#define G_D_Bob_av  ((FIO2PIN&(1<<11)))   // if == 0 : //la bobine avant est à droite    else //la bobine avant est à gauche
#define G_D_Bob_ar  ((FIO2PIN&(1<<12)))   // if == 0 : //la bobine arrière est à droite  else //la bobine arrière est à gauche

#define SET_RESET_LED_AV(etat)   if(etat==1){FIO0PIN0 |= 0x01 ;} else {FIO0PIN0 &= 0xFE ;}   // P2.0
#define SET_RESET_LED_AR(etat)   if(etat==1){FIO0PIN0 |= 0x02 ;} else {FIO0PIN0 &= 0xFD ;}   // P2.1

#define ARRET_COUP_DE_POING(etat)   if(etat==1){ save_PWM1 = PWM1MR1; save_PWM2 = PWM1MR2;  PWM1MR1 = 0; PWM1MR2= 0; PWM1LER |= 0x7;} \
                                    else { PWM1MR1 = save_PWM1; PWM1MR2 = save_PWM2; PWM1LER |= 0x7;}

																		
unsigned long etat0_Bob_av;
unsigned int stock;
int is_first_Timer_interruptio = 1;

unsigned int save_PWM1;
unsigned int save_PWM2;	
int arret_reprise_CoupDePoing = 0;  		
																		
/************************************************/
#define PCLK 12e6
#define TAILLE_TRAME 52
#define GET_NUM_ROBOT    (FIO2PIN &0xF0)  >> 4
#define GET_SPEED_ROBOT   4  // (FIO2PIN &0xF00) >> 8
//#define GET_STATUS_ROBOT

int nb_trames=0;
int nb_entetes;
int etat=1;
int new_trame=0;
int pulse_IR = 1;
unsigned int index_IR = 51;
unsigned int trame[TAILLE_TRAME]; 
unsigned int cpt_IR = 15;
/***************************************************************************************************/

unsigned char checksum ;
unsigned char status,vitesse,num;

void create_trame_IR(int num_robot,int vitesse_robot, int status_robot){
	
	index_IR = 51;
	//etat = 1;
	
	//CHECKSUM   checksum = ~((a + b + c + d) & 0x0F) + 1 ;
	 status=  0x00 | (  (status_robot&(0x01))  | (status_robot&(0x02)) | (status_robot&(0x04)) | (status_robot&(0x08))  );
	 vitesse= 0x00 | (  (vitesse_robot&0x01)   | (vitesse_robot&0x02)  | (vitesse_robot&0x04)  | (vitesse_robot&0x08)   );
	 num=     0x00 | (  (num_robot&0x01)       | (num_robot&0x02)      | (num_robot&0x04)      | (num_robot&0x08)       );
	//num=num3+num2*2+num1*4+num0*8
	 checksum = 0x00 | (~((status + vitesse + num ) & 0x0F) + 1 );
	//checksum = 0xE;
  //checksum = checksum&0x01;tab_checksum[2]=(checksum&0x02)>>1;tab_checksum[1]=(checksum&0x04)>>2;tab_checksum[0]=(checksum&0x08)>>3;
	
  // Entete
	trame[0]=  0;
	trame[1]=  1;
	//CHECKSUM
	if ((checksum&0x08)){trame[13]= 1;trame[12]= 1;trame[11]= 0;}else{trame[13]= 1;trame[12]= 0;trame[11]= 0;}
	if ((checksum&0x04)){trame[10]= 1;trame[9]= 1;trame[8]= 0;}else{trame[10]= 1;trame[9]= 0;trame[8]= 0;}
	if ((checksum&0x02)){trame[7]= 1;trame[6]= 1;trame[5]= 0;}else{trame[7]= 1;trame[6]= 0;trame[5]= 0;}
  if ((checksum&0x01)){trame[4]= 1;trame[3]= 1;trame[2]= 0;}else{trame[4]= 1;trame[3]= 0;trame[2]= 0;}
	//Status ROBOT		
	if ((status_robot&0x08)){trame[25]= 1;trame[24]= 1;trame[23]= 0;}else{trame[25]= 1;trame[24]= 0;trame[23]= 0;}
	if ((status_robot&0x04)){trame[22]= 1;trame[21]= 1;trame[20]= 0;}else{trame[22]= 1;trame[21]= 0;trame[20]= 0;}
	if ((status_robot&0x02)){trame[19]= 1;trame[18]= 1;trame[17]= 0;}else{trame[19]= 1;trame[18]= 0;trame[17]= 0;}
  if ((status_robot&0x01)){trame[16]= 1;trame[15]= 1;trame[14]= 0;}else{trame[16]= 1;trame[15]= 0;trame[14]= 0;}
	//Vitesse ROBOT
	if ((vitesse_robot&0x08)){trame[37]= 1;trame[36]= 1;trame[35]= 0;}else{trame[37]= 1;trame[36]= 0;trame[35]= 0;}
	if ((vitesse_robot&0x04)){trame[34]= 1;trame[33]= 1;trame[32]= 0;}else{trame[34]= 1;trame[33]= 0;trame[32]= 0;}
	if ((vitesse_robot&0x02)){trame[31]= 1;trame[30]= 1;trame[29]= 0;}else{trame[31]= 1;trame[30]= 0;trame[29]= 0;}
  if ((vitesse_robot&0x01)){trame[28]= 1;trame[27]= 1;trame[26]= 0;}else{trame[28]= 1;trame[27]= 0;trame[26]= 0;}			
	//Numéro ROBOT
	if ((num_robot&0x08)){trame[49]= 1;trame[48]= 1;trame[47]= 0;}else{trame[49]= 1;trame[48]= 0;trame[47]= 0;}
	if ((num_robot&0x04)){trame[46]= 1;trame[45]= 1;trame[44]= 0;}else{trame[46]= 1;trame[45]= 0;trame[44]= 0;}
	if ((num_robot&0x02)){trame[43]= 1;trame[42]= 1;trame[41]= 0;}else{trame[43]= 1;trame[42]= 0;trame[41]= 0;}
  if ((num_robot&0x01)){trame[40]= 1;trame[39]= 1;trame[38]= 0;}else{trame[40]= 1;trame[39]= 0;trame[38]= 0;}
  // Entete 
	trame[50]=  0;
	trame[51]=  1;	
	
}

void isr_PWM(void)__irq
{	
	cpt_IR--;
	if (pulse_IR == 1 & etat==1){PWM1MR4 = 24;}else{PWM1MR4 = 0;}
	PWM1LER |= 0x17; 
	if (cpt_IR == 0){
		 if(index_IR>0) {index_IR--;}else{index_IR=1;nb_entetes++;}
		 if(nb_entetes==3){index_IR=51;nb_entetes=0; 
                       if(etat==1){nb_trames++;}else{nb_trames--;}
		                   if(nb_trames==3){etat=0;}
		                   if(nb_trames==0){new_trame=1;}}
     pulse_IR = trame[index_IR];
		 cpt_IR = 15;
 }
	VICVectAddr = 0;
	PWM1IR = 1;  // Aquitemment PWM1MR0
}


/********************************************************************************************************/
void ISR_T0()__irq{
	
	// Détection de coté 
	if( !G_D_Bob_av ){ SET_RESET_LED_AV(1);} else {SET_RESET_LED_AV(0);}
	if( !G_D_Bob_ar ){ SET_RESET_LED_AR(1);} else {SET_RESET_LED_AR(0);}
	// Permutation de vitesse
	if(is_first_Timer_interruptio){etat0_Bob_av = ((FIO2PIN&(1<<11))); is_first_Timer_interruptio=0; } 
	if( G_D_Bob_av ^ etat0_Bob_av ){  
		  stock   = PWM1MR2;
      PWM1MR2 = PWM1MR1;
		  PWM1MR1 = stock; 
		  PWM1LER |= 0x17;  
		  etat0_Bob_av = G_D_Bob_av;
	}  
	
	FIO2PIN0  ^= 0x04;
	VICVectAddr =0;
	T0IR = 1;
}

void ISR_EXTINT0(void) __irq{ // INTERUPTION COUP DE POING
	     arret_reprise_CoupDePoing ^= 1;
       ARRET_COUP_DE_POING(arret_reprise_CoupDePoing)
       EXTINT = 1;     // Acquittement périf int.externe
       VICVectAddr = 0; // Acquittement VIC
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
	FIO2DIR1 &= ~( (1<<11) | (1<<12)) ;  // P2.11  et P2.12 : INPUT
	//FIO2CLR = (1<<10) | (1<<11);  // P2.10  et P2.11 eteindre
	// ARRET COUP DE POING
	
}

void init_PWM(void)
{
    PINSEL4 |= 0x00000045;    // P2.0 et P2.1  et P2.3 en PWM     0100 0101 
    PWM1PCR = (1<<9) | (1<<10) | (1<<12);    // PMW1.1 et PWM.2 PWM.4 Output Enable + Single edge
    PWM1CTCR = 0x00;
    PWM1MR0 = 336 ;         // 28uS* PCLK
    PWM1MR1 = 20* PWM1MR0 / 100;
    PWM1MR2 = 80* PWM1MR0 / 100;
	  PWM1MR4 = 24;       // 2uS max : 28uS/14
    PWM1TCR = 0x9;      // Counter Enable + PWM enable
    PWM1MCR = 0x3;      // INTERUPTION + RESET si TC = MR0
    PWM1LER|= 0x17;     // Enable PWM Match 0 and 1 2 et 4 and  Latches    01 0111
	  VICVectAddr8 = (unsigned long) isr_PWM ;
    VICIntEnable =1<<8 ; 
}


void init_EXINT0(){
    PINSEL4  |= 1<<20;   // P2.10 en EINT0
    EXTMODE = 1;             // Front
    EXTPOLAR = 0;             // Descendant 
    VICIntEnable = (1 << 14);                     // Activation du vecteur d'interruption
    VICVectAddr14 = (unsigned long) ISR_EXTINT0;
}

int main (){
	
	int num_robot= GET_NUM_ROBOT; int vitesse_robot = 4; int status_robot=8;
	create_trame_IR(num_robot,vitesse_robot,status_robot);
	
	init_GPIO();
	init_EXINT0();
	init_T0();
	init_PWM();

	
	while (1){ 
        if (new_trame==1){etat =1; 
				                 if (GET_NUM_ROBOT != num_robot || GET_SPEED_ROBOT != vitesse_robot) 
													   num_robot = GET_NUM_ROBOT; vitesse_robot = GET_SPEED_ROBOT;
				                    {create_trame_IR( num_robot,vitesse_robot,status_robot); 
				                      ;new_trame=0;}}
	};
}
