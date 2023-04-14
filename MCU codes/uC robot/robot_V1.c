#include "LPC23xx.h"
#define PCLK 12e6
#define PERIODE 10e-3 

void init_PWM(void){

    PINSEL4 |= 0x05;    // P2.0 et P2.1   en PWM   0101 

    PWM1PCR = (1<<9) | (1<<10);    // PMW1.1 et PWM.2 Output Enable + Single edge
    PWM1CTCR = 0x00;

    PWM1MR0 = PERIODE * PCLK;
    PWM1MR1 = 50* PWM1MR0 / 100;
    PWM1MR2 = 70* PWM1MR0 / 100;

    PWM1TCR = 0x9;     // Counter Enable + PWM enable
    PWM1MCR = 0x2;     // RESET si TC = MR0
    PWM1LER |= 0x7;     // Enable PWM Match 0 and 1 2 and  Latches   111
}


int main(){
    init_PWM();
    while(1);
}