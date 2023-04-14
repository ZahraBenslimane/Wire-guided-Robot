# include "LPC23xx.h"

// P2.0 : Sortie DTMF 

#define sortir_sur_DAC(val_son)   DACR = (int) val_son <<6;

#define FREQ_0_H 342
#define FREQ_0_B 241 
#define FREQ_1_H 310
#define FREQ_1_B 178 
#define FREQ_2_H 342
#define FREQ_2_B 178
#define FREQ_3_H 378
#define FREQ_3_B 178
#define FREQ_4_H 310
#define FREQ_4_B 197
#define FREQ_5_H 342
#define FREQ_5_B 197
#define FREQ_6_H 378
#define FREQ_6_B 197
#define FREQ_7_H 310
#define FREQ_7_B 218
#define FREQ_8_H 342
#define FREQ_8_B 218
#define FREQ_9_H 378
#define FREQ_9_B 218
#define FREQ_A_H 418
#define FREQ_A_B 178
#define FREQ_B_H 418
#define FREQ_B_B 197
#define FREQ_C_H 418
#define FREQ_C_B 218
#define FREQ_D_H 418
#define FREQ_D_B 241

#define FREQ_ASTERIST_H 310
#define FREQ_ASTERIST_B 241
#define FREQ_SHARP_H  378
#define FREQ_SHARP_B 241


float Vref = 3.3;
unsigned int flag_dtmf_emit;
unsigned int son1K[37] = {511,599,685,766,839,902,953,991,1014,1022,1014,991,953,902,839,766,685,599,511,422,336,255,
                          182,119,68,30,7,0,7,30,68,119,182,255,336,422,511};

unsigned int index_a_virgule_H;
unsigned int index_a_virgule_B;
unsigned int inc_freqH = 378;
unsigned int inc_freqB = 241;
unsigned int ech_avantH;
unsigned int ech_avantB;
unsigned int ech_apresH;
unsigned int ech_apresB;; 
unsigned int virguleH;
unsigned int virguleB;
unsigned int indexH;
unsigned int indexB;
unsigned int val_sonH;
unsigned int val_sonB;

unsigned int val_son = 0;
unsigned int dcp_100ms;
unsigned int index_son;
													
unsigned int seq_freq_haute[12];
unsigned int seq_freq_basse[12];


float out_voltage;

void isr_PWM (void)__irq
{
	
dcp_100ms --;
FIO2PIN ^= 0x02; 
	
		if(dcp_100ms == 0){ dcp_100ms = 3600; FIO2PIN ^= 1<<2; 
		                  if (index_son>0) {index_son--;} 
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
	
  sortir_sur_DAC(val_son);
	
out_voltage =  ( (DACR & 0xFFC0)>>6 );


VICVectAddr = 0;
PWM1IR = 1 ;
}

void init_PWM(void)
{
    PINSEL4 |= 0x00000001;  // P2.0   en PWM   01 
    PWM1PCR = (1<<9);       // PMW1.1 Output Enable + Single edge
    PWM1CTCR = 0x00;
    PWM1MR0 = 336;         // 28uS* PCLK
    PWM1MR1 = PWM1MR0*50/100;
    PWM1TCR = 0x9;     // Counter Enable + PWM enable
    PWM1MCR = 0x3;     // INTERUPTION + RESET si TC = MR0
    PWM1LER |= 0x3;    // Enable PWM Match 0 and 1 2 and  Latches   111
      VICVectAddr8 = (unsigned long) isr_PWM ;
      VICIntEnable =1<<8 ;
}

void load_DTMF_code(unsigned char X, unsigned char Y){
	
	index_son = 11;
  dcp_100ms = 3600;
	
	seq_freq_haute[11] = FREQ_ASTERIST_H;  seq_freq_basse[11] = FREQ_ASTERIST_B;
	seq_freq_haute[10] = FREQ_ASTERIST_H;  seq_freq_basse[10] = FREQ_ASTERIST_B;
	seq_freq_haute[9] = 0;  seq_freq_basse[9] = 0;
	
	    switch(X)
    {
				case 0:
            seq_freq_haute[8] = FREQ_0_H; seq_freq_basse[8] = FREQ_0_B;
        		seq_freq_haute[7] = FREQ_0_H; seq_freq_basse[7] = FREQ_0_B;		break;
        case 1:
            seq_freq_haute[8] = FREQ_1_H; seq_freq_basse[8] = FREQ_1_B;
        		seq_freq_haute[7] = FREQ_1_H; seq_freq_basse[7] = FREQ_1_B;		break;
        case 2:
            seq_freq_haute[8] = FREQ_2_H; seq_freq_basse[8] = FREQ_2_B;
        		seq_freq_haute[7] = FREQ_2_H; seq_freq_basse[7] = FREQ_2_B;		break;
        case 3:
            seq_freq_haute[8] = FREQ_3_H; seq_freq_basse[8] = FREQ_3_B;
        		seq_freq_haute[7] = FREQ_3_H; seq_freq_basse[7] = FREQ_3_B;		break;
        case 4:
            seq_freq_haute[8] = FREQ_4_H; seq_freq_basse[8] = FREQ_4_B;
         		seq_freq_haute[7] = FREQ_4_H; seq_freq_basse[7] = FREQ_4_B;		break;
			  case 5:
            seq_freq_haute[8] = FREQ_5_H; seq_freq_basse[8] = FREQ_5_B;
        		seq_freq_haute[7] = FREQ_5_H; seq_freq_basse[7] = FREQ_5_B;		break;
			  case 6:
            seq_freq_haute[8] = FREQ_6_H; seq_freq_basse[8] = FREQ_6_B;
        		seq_freq_haute[7] = FREQ_6_H; seq_freq_basse[7] = FREQ_6_B;		break;
				case 7:
            seq_freq_haute[8] = FREQ_7_H; seq_freq_basse[8] = FREQ_7_B;
        		seq_freq_haute[7] = FREQ_7_H; seq_freq_basse[7] = FREQ_7_B;		break;
			  case 8:
            seq_freq_haute[8] = FREQ_8_H; seq_freq_basse[8] = FREQ_8_B;
        		seq_freq_haute[7] = FREQ_8_H; seq_freq_basse[7] = FREQ_8_B;		break;
				case 9:
            seq_freq_haute[8] = FREQ_9_H; seq_freq_basse[8] = FREQ_9_B;
        		seq_freq_haute[7] = FREQ_9_H; seq_freq_basse[7] = FREQ_9_B;		break;
				case 'A':
            seq_freq_haute[8] = FREQ_A_H; seq_freq_basse[8] = FREQ_A_B;
        		seq_freq_haute[7] = FREQ_A_H; seq_freq_basse[7] = FREQ_A_B;		break;
				case 'B':
            seq_freq_haute[8] = FREQ_B_H; seq_freq_basse[8] = FREQ_B_B;
        		seq_freq_haute[7] = FREQ_B_H; seq_freq_basse[7] = FREQ_B_B;		break;
				case 'C':
            seq_freq_haute[8] = FREQ_C_H; seq_freq_basse[8] = FREQ_C_B;
        		seq_freq_haute[7] = FREQ_C_H; seq_freq_basse[7] = FREQ_C_B;		break;
				case 'D':
            seq_freq_haute[8] = FREQ_D_H; seq_freq_basse[8] = FREQ_D_B;
        		seq_freq_haute[7] = FREQ_D_H; seq_freq_basse[7] = FREQ_D_B;		break;			
        // operator doesn't match any case constantµ
        default:
					  seq_freq_haute[8] = 0; seq_freq_basse[8] = 0;
        		seq_freq_haute[7] = 0; seq_freq_basse[7] = 0;		         
    }
		
					  seq_freq_haute[6] = 0; seq_freq_basse[6] = 0;		
		
	    switch(Y)
    {
				case 0:
            seq_freq_haute[5] = FREQ_0_H; seq_freq_basse[5] = FREQ_0_B;
        		seq_freq_haute[4] = FREQ_0_H; seq_freq_basse[4] = FREQ_0_B;		break;
        case 1:
            seq_freq_haute[5] = FREQ_1_H; seq_freq_basse[5] = FREQ_1_B;
        		seq_freq_haute[4] = FREQ_1_H; seq_freq_basse[4] = FREQ_1_B;		break;
        case 2:
            seq_freq_haute[5] = FREQ_2_H; seq_freq_basse[5] = FREQ_2_B;
        		seq_freq_haute[4] = FREQ_2_H; seq_freq_basse[4] = FREQ_2_B;		break;
        case 3:
            seq_freq_haute[5] = FREQ_3_H; seq_freq_basse[5] = FREQ_3_B;
        		seq_freq_haute[4] = FREQ_3_H; seq_freq_basse[4] = FREQ_3_B;		break;
        case 4:
            seq_freq_haute[5] = FREQ_4_H; seq_freq_basse[5] = FREQ_4_B;
         		seq_freq_haute[4] = FREQ_4_H; seq_freq_basse[4] = FREQ_4_B;		break;
			  case 5:
            seq_freq_haute[5] = FREQ_5_H; seq_freq_basse[5] = FREQ_5_B;
        		seq_freq_haute[4] = FREQ_5_H; seq_freq_basse[4] = FREQ_5_B;		break;
			  case 6:
            seq_freq_haute[5] = FREQ_6_H; seq_freq_basse[5] = FREQ_6_B;
        		seq_freq_haute[4] = FREQ_6_H; seq_freq_basse[4] = FREQ_6_B;		break;
				case 7:
            seq_freq_haute[5] = FREQ_7_H; seq_freq_basse[5] = FREQ_7_B;
        		seq_freq_haute[4] = FREQ_7_H; seq_freq_basse[4] = FREQ_7_B;		break;
			  case 8:
            seq_freq_haute[5] = FREQ_8_H; seq_freq_basse[5] = FREQ_8_B;
        		seq_freq_haute[4] = FREQ_8_H; seq_freq_basse[4] = FREQ_8_B;		break;
				case 9:
            seq_freq_haute[5] = FREQ_9_H; seq_freq_basse[5] = FREQ_9_B;
        		seq_freq_haute[4] = FREQ_9_H; seq_freq_basse[4] = FREQ_9_B;		break;
				case 'A':
            seq_freq_haute[5] = FREQ_A_H; seq_freq_basse[5] = FREQ_A_B;
        		seq_freq_haute[4] = FREQ_A_H; seq_freq_basse[4] = FREQ_A_B;		break;
				case 'B':
            seq_freq_haute[5] = FREQ_B_H; seq_freq_basse[5] = FREQ_B_B;
        		seq_freq_haute[4] = FREQ_B_H; seq_freq_basse[4] = FREQ_B_B;		break;
				case 'C':
            seq_freq_haute[5] = FREQ_C_H; seq_freq_basse[5] = FREQ_C_B;
        		seq_freq_haute[4] = FREQ_C_H; seq_freq_basse[4] = FREQ_C_B;		break;
				case 'D':
            seq_freq_haute[5] = FREQ_D_H; seq_freq_basse[5] = FREQ_D_B;
        		seq_freq_haute[4] = FREQ_D_H; seq_freq_basse[4] = FREQ_D_B;		break;			
        // operator doesn't match any case constantµ
        default:
					  seq_freq_haute[5] = 0; seq_freq_basse[5] = 0;
        		seq_freq_haute[4] = 0; seq_freq_basse[4] = 0;		         
    }
		
			seq_freq_haute[3] = 0; seq_freq_basse[3] = 0;
			seq_freq_haute[2] = FREQ_SHARP_H;  seq_freq_basse[2] = FREQ_SHARP_B;
	    seq_freq_haute[1] = FREQ_SHARP_H;  seq_freq_basse[1] = FREQ_SHARP_B;
      seq_freq_haute[0] = 0; seq_freq_basse[0] = 0;

	
}
int main (){
	
	  load_DTMF_code(1,1);

    FIO2DIR |= 0x02 | 1<<2; FIO2CLR |= 0x02 | 1<<2;     //p2.1 GPIO
    PINSEL1 |= 1 << 21;  // p0.26 AOUT
    init_PWM();

    while (1){ 

    };
}
