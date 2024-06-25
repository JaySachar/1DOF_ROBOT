#include <stdint.h>
#include <stdio.h>
#include "tm4c123gh6pm.h"

void SysTickInit(void);
void SysTickHandler(void);
void PortFInit(void);
void SW1_Handler(void);

int main(void)
{
    PortFInit();
    PWM_Control();
    while(1){};
}


void PortFInit(void) {
    SYSCTL_RCGC2_R |= 0x00000020;
    while ((SYSCTL_PRGPIO_R & 0x00000020) == 0) {};
    GPIO_PORTF_LOCK_R = 0x4C4F434B;
    GPIO_PORTF_CR_R = 0x1F;
    GPIO_PORTF_AMSEL_R = 0x00;
    GPIO_PORTF_PCTL_R = 0x00000000;
    GPIO_PORTF_DIR_R = 0x0E;
    GPIO_PORTF_AFSEL_R = 0x00;
    GPIO_PORTF_PUR_R = 0x11;
    GPIO_PORTF_DEN_R = 0x1F;

    // Enable the Interrupt for SW1
    // Interrupts are enabled/disabled via GPIO_IM (GPIOIM Register pg. 667)
    GPIO_PORTF_IS_R &= ~0x10; // Set PF4 to be edge-sensitive (~0001.0000 -> 1110.1111)
    GPIO_PORTF_IBE_R &= ~0x10; // Set PF4 to not be triggered by both edges
    GPIO_PORTF_IEV_R |= 0x10; // Set PF4 to be rising edge base, so it is normally high level, button pressed, low level, button is not longer depressed, high level, ISR called
    GPIO_PORTF_ICR_R |= 0x10; // Clear any Interrupts on PF4
    GPIO_PORTF_IM_R |= 0x10; // Port 4 (SW1 Interrupt Enabled)
    // Port F is Vector Number 46, Interrupt Number 30
    // Interrupt Number 30, so Register 36 (PRI7)
    NVIC_PRI7_R =(NVIC_PRI7_R & 0xFF1EFFFF)|0x00A00000;



    // Bits 23:21 are set to 101 (5, middle priority),and are associated with 4n+2 (4*7+2 = 30) so interrupt 30 is set to a priority of 5
 /*   000 XXXXX 000 XXXXX 000 XXXXX 000 XXXXX // NVIOC_PRI7_R initially. We want to target bits 23:21 while preserving the reserved bits values (XXXXX)
   &111 11111 000 11111 111 11111 111 11111 // Preserves all other bits, while setting bits 23:21 to 0
   _________________________________
    000 XXXXX 000 XXXXX 000 XXXXX 000 XXXXX
   |000 00000 101 00000 000 00000 000 00000
   _________________________________
    000 XXXXX 101 XXXXX 000 XXXXX 000 XXXXX
*/
    // Enable interrupt for interrupt 30 (EN0)
    NVIC_EN0_R = (1 << 30); // Enable interrupts for Port F (Interrupt 30)
                            // NVIC_EN0_R = 0100 0000 0000 0000 0000 0000 0000 0000 == 0x4000.0000
}

void PWM_Control(void){
    // GPTMTnILR and GPTMTnPR registers specify the start value for the 24 bit or 48 bit down counter for PWM Mode (pg 716)
    // PWM mode is enabled with GPTMTnMR register by setting TnAMS bit to 0x1, the TnCMR bit to 0x0, and the TnMR field to 0x2
    // Each PWM Generator block makes 2 PWM signals, that share the same timer and frequency, that can be programmed with independent actions, or as a single pair of complementary signals with dead-band delays inserted (what we want, the latter)
    // 2 PWM Modules, with 4 PWM generator blocks, for a total of 16 possible PWM outputs (wowie...)
    // for PWM at PF2 and PF3, it is M1PWM6 and M1PWM7 respectively. This is Module 1 PWM Generator 3.
    SYSCTL_RCGCPWM_R |= SYSCTL_RCGCPWM_R1;;// Enable to PWM0 Clock gating control so the PWM module receives a clock and functions. THis is at bit 20 of 31.
    SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_R5;  // Enable clock for Port F
    while ((SYSCTL_PRPWM_R & SYSCTL_PRPWM_R1) == 0) {}; // Wait for PWM1 module to be ready

    GPIO_PORTF_AFSEL_R |= 0x0000000C;       // C == 1100 so PF3 and PF2 are selected? hopefully?
    GPIO_PORTF_PCTL_R &~ ((0xF << 8) | (0xF << 12)); // Clear bits for PF2 and PF3
    GPIO_PORTF_PCTL_R  |= (5 << 8) | (5 << 12);  // PF3 and PF2, so We're looking at PMC2 and PMC3, setting it to 5? i think.
                                                 // PCTL register is 32 bits wide. 4 bits for each pin. we're looking for:
                                                 // 0000.0000.0000.0000.0101.0101.0000.0000
                                                 // 0101 == 5, so its 0x00005500

    // Set the Run-Mode Clock Configuration (RCC) register to use PWM Divide with a divide of 2
    SYSCTL_RCC_R |= SYSCTL_RCC_USEPWMDIV; // Enable PWM Divide
    SYSCTL_RCC_R &= ~SYSCTL_RCC_PWMDIV_M;// Clear SysCTL_RCC_R (Divided field)
    SYSCTL_RCC_R |= SYSCTL_RCC_PWMDIV_2; // Set PWM Divide value to 2

    // Configure PWM Generator for Countdown mode with immediate updates to the parameters.
    PWM1_3_CTL_R |= 0x00000000;      // Write PWM3CTL register with a value of 0x0000.0000, PWM0GENA register with 0x0000.008C, and PWM0GENB register with 0x0000.080C
    PWM1_3_GENA_R |= 0x0000008C;
    PWM1_3_GENB_R |= 0x0000080C;

    PWM1_3_LOAD_R |= 400000 - 1; // Set the PWM Frequency to 100Hz (80 MHz / 2 / 100)

    PWM1_3_CMPA_R = 0; // Set PF2 to be a 25% duty cycle
    PWM1_3_CMPB_R = 0; // Set PF3 to be complimentary with a 75% duty cycle

    // Enable the deadband geneartor with a delay of 10 cycles
    PWM1_3_DBCTL_R = PWM_1_DBCTL_ENABLE | PWM_1_DBCTL_MODE_GENA | PWM_1_DBCTL_MODE_GENB;
    PWM1_3_DBRISE_R = 10; // Set deadband rising edge delay
    PWM1_3_DBFALL_R = 10; // Set deadband falling edge delay

    PWM1_3_CTL_R  |= 0x00000001; // Start the timers in PWM module 1 generator 3
    PWM1_ENABLE_R |= PWM_ENABLE_PWM6EN | PWM_ENABLE_PWM7EN; // Enable PWM outputs
}

void SW1_Handler(void){
    static int count = 1;
    if (count != 11){
        PWM1_3_CMPA_R += 40000;
        PWM1_3_CMPB_R = 400000 - PWM1_3_CMPA_R;
    }
    else {
        PWM1_3_CMPA_R = 0;
        PWM1_3_CMPB_R = 400000;
        count = 1;
    }

    count++;
    
    // Increase PWM duty cycle by 10%, unless at 100%, in which case set Duty Cycle to 0%.
    GPIO_PORTF_ICR_R |= 0x10; // Clear any Interrupts on PF4
}
