# include "LPC23xx.h"

#define Vref 3.3
#define sortir_sur_DAC(val_son)   DACR = (int) val_son <<6;


unsigned int son1K[37] = {511,599,685,766,839,902,953,991,1014,1022,1014,991,953,902,839,766,685,599,511,422,336,255,
                          182,119,68,30,7,0,7,30,68,119,182,255,336,422,511};


unsigned int High_Freq[] = {1209,1336,1477,1029,1336,1447,1209,1336,1447,1336,1209,1447,1633,1633,1633,1633};
unsigned int Low_Freq[]	 = {697,697,697,770,770,770,852,852,852,941,941,941,697,770,852,941};
			

unsigned int seq_freq_haute[12] = {310, 310, 310, 310, 310, 310} ;
unsigned int seq_freq_basse[12] = {310, 310, 310, 0, 241, 241};

unsigned int index_son = 0;	
unsigned int index_a_virgule_H = 0x0000;
unsigned int index_a_virgule_B = 0x0000;;	

unsigned int inc_freqH, inc_freqB;
unsigned int ech_avantH, ech_avantB;
unsigned int ech_apresH, ech_apresB;
unsigned int virguleH, virguleB;	
unsigned int val_sonH, val_sonB;
unsigned int val_son = 0;
unsigned int indexH, indexB;
unsigned int dcp_100ms = 3600;


// ech_avantH = son1K[indexH]; ech_apresH = son1K[indexH+1]
// ech_avantB = son1K[indexB]; ech_apresB = son1K[indexB+1]	

// indexH = index_a_virgule_H >> 8; virguleH  = index_a_virgule_H &0xFF;
// indexB = index_a_virgule_B >> 8; virguleB  = index_a_virgule_B &0xFF;

// val_sonH = (ech_avantH * (256-virguleH) + ech_apresH * virguleH) >>8;
// val_sonB = (ech_avantB * (256-virguleB) + ech_apresB * virguleB) >>8;
// val_son = (val_sonH + val_sonB)>>1;
// sortir_sur_le_DAC(val_son)

float out_voltage;

void isr_PWM (void)__irq
{
FIO2PIN ^= 0x02; 
dcp_100ms --;
//	index_son=0;
	
	
	if(dcp_100ms == 0){ dcp_100ms = 3600;
		                  if (index_son<5) {index_son++;}
		                   inc_freqH = seq_freq_haute[index_son];
											 inc_freqB = seq_freq_basse[index_son];
										}
		
	index_a_virgule_H +=inc_freqH;
	if ( index_a_virgule_H > 36*256) {index_a_virgule_H -= 36*256;}									
	index_a_virgule_B +=inc_freqB;
	if ( index_a_virgule_B > 36*256) {index_a_virgule_B -= 36*256;}										
	
	
	indexH = index_a_virgule_H >> 8; virguleH  = index_a_virgule_H &0xFF;
  indexB = index_a_virgule_B >> 8; virguleB  = index_a_virgule_B &0xFF;
	
	ech_avantH = son1K[indexH]; ech_apresH = son1K[indexH+1];
	ech_avantB = son1K[indexB]; ech_apresB = son1K[indexB+1];
	
 val_sonH = (ech_avantH * (256-virguleH) + ech_apresH * virguleH) >>8;
 val_sonB = (ech_avantB * (256-virguleB) + ech_apresB * virguleB) >>8;
 val_son = (val_sonH + val_sonB)>>1;
 sortir_sur_DAC(val_sonH);
	 
out_voltage =  ( (DACR & 0xFFC0)>>6 )* Vref /1024;
	
VICVectAddr = 0;
PWM1IR = 1;
}

void init_PWM(void)
{
    PINSEL4 |= 0x00000001;  // P2.0   en PWM   01 
    PWM1PCR = (1<<9);       // PMW1.1 Output Enable + Single edge
    PWM1CTCR = 0x00;
	  PWM1MR0 = 333;         // 28uS* PCLK
	  PWM1MR1 = PWM1MR0*50/100;         
    PWM1TCR = 0x9;     // Counter Enable + PWM enable
    PWM1MCR = 0x3;     // INTERUPTION + RESET si TC = MR0
    PWM1LER |= 0x3;    // Enable PWM Match 0 and 1 2 and  Latches   111
	  VICVectAddr8 = (unsigned long) isr_PWM ;
	  VICIntEnable =1<<8 ;  
}
int main (){

	FIO2DIR |= 1<<1;         //p2.1 GPIO
	PINSEL1 |= 1 << 21;  // p0.26 AOUT
	init_PWM();
	
	while (1){ 

	};
}

