# include "LPC23xx.h"

float Vref = 3.3;
unsigned int son1K[37] = {511,599,685,766,839,902,953,991,1014,1022,1014,991,953,902,839,766,685,599,511,422,336,255,
                          182,119,68,30,7,0,7,30,68,119,182,255,336,422,511};

unsigned int index_son = 0;
float out_son;

void isr_PWM (void)__irq
{
	
	index_son++;
DACR = (int)son1K[index_son]<<6; 
out_son = ( (DACR & 0xFFC0)>>6 );
	
if(index_son == 35){index_son=0;}

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
int main (){

    FIO2DIR |= 0x02 | 1<<2;         //p2.1 GPIO
	  FIO2CLR |= 0x02 | 1<<2;         //p2.1 GPIO
    PINSEL1 |= 1 << 21;  // p0.26 AOUT
    init_PWM();

    while (1){ 

    };
}