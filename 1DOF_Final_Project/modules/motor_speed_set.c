#include "motor_speed_set.h"

// 2 PWM Modules, each with 4 PWM generator blocks, which can generate 2 PWM outputs, totally 16 total PWM outputs
// Each block generates 2 signals that share the same frequency and timer
// The outputs pwmA and pwmB are known as MnPWM0, MnPWM1, MnPWM2, etc.
// Each generator has one 16 bit counter, runs in down or up/down mode.
// Output frequency controller by a 16-bit load value
// Load value updates can be synchronized
// Has a Dead-band generator, producing 2 PWM signals with programmable dead-band delays for the Half-H-Bridge
// HB1 is PE5
// HB2 is PE4
// PE4 and PE5 are Module 0, PWM 4 and PWM 5. Controlled by: Module 0 PWM Generator 2
// In PLL_Init.c, we set the clock's crystal to be 16MHz.

void gen_PWM(HBridgePin pin, float dc){
    // My idea is that we call this for generating the PWM for HB1 and HB2. So if we want
    // a 25% duty cycle for HB1, we'd do gen_PWM('1', 0.25), similarly
    // a 75% duty cycle will be set for the other
    uint32_t load_value = PWM0_2_LOAD_R - 1;

    switch(pin){
        case HB2: // Case where HB1 gets main PWM while HB2 gets comp
            // Set PWM for M0PWM4
            PWM0_2_CMPA_R = dc*load_value;
            PWM0_2_CMPB_R = (1-dc)*load_value;
            break;

        case HB1:
            PWM0_2_CMPA_R = (1-dc)*load_value;
            PWM0_2_CMPB_R = dc*load_value;
            break;

        default:
            break;
    }
}

void PWM_Init(void){
    SYSCTL_RCGC0_R     |= SYSCTL_RCC_USEPWMDIV; // Enable the PWM clock
    SYSCTL_RCGC2_R     |= SYSCTL_RCGC2_GPIOE; // Enable Port E Clock Gating
    while((SYSCTL_PRGPIO_R & SYSCTL_PRGPIO_R4) == 0) {}; // Wait for Port E to be ready

    GPIO_PORTE_AFSEL_R |= 0x30; // Set pins 4 and 5 as alternate functions (0011.0000)
    GPIO_PORTE_PCTL_R  &= ~(0xF << 20); // Clear bits in PE5
    GPIO_PORTE_PCTL_R  &= ~(0xF << 16); // Clear Bits in PE4
    GPIO_PORTE_PCTL_R  |= (4 << 20) | (4 << 16); // Set bits 20-23 (PE5) and 16-19 (PE4) to 4 which is M0PWM4 and M0PWM5 in Table 23-5
    GPIO_PORTE_DEN_R |= 0x30; // Enable digital function on PE4 and PE5
    SYSCTL_RCC_R = (SYSCTL_RCC_R & ~SYSCTL_RCC_USEPWMDIV) | SYSCTL_RCC_USEPWMDIV | SYSCTL_RCC_PWMDIV_2; // Configure PWM clock divider
    // SYSCTL_RCGC0_R     |= SYSCTL_RCC_PWMDIV_2;

    // Use PWM2CTL because we are M0PWM4 and M0PWM5
    PWM0_2_CTL_R |= 0x00000000;
    PWM0_2_GENA_R |= 0x0000008C;
    PWM0_2_GENB_R |= 0x0000080C;
    // The clock is set to 16Mhz, we divide that by 2 . So we're left with 8Mhz
    // The PWM clock source is the system clock divided by 2 that we do in line in line 21
    // The period thus is 1/8000, or 0.000125 [s] or 0.125[ms].
    // If the desired frequency is lets say 32kHz, so we can't hear it
    // That is a period of 1/32000 = 0.00003125.
    // There are 8 clock ticks per period
    // So we calculate the load value as: desired period * Clock Source Frequency = 1/32000 * 8*10^6 = 250
    uint32_t load_value = 5000; // 0x000000F9;
    PWM0_2_LOAD_R |= load_value - 1; // Set the Load Register equal to 250 - 1
    // PWM0_2_CMPA_R = (0.75)*(load_value-1);
    // PWM0_2_CMPB_R = (0.25)*(load_value-1);
    // Configure and enable deadband for PWM0 Generator 2
    PWM0_2_DBCTL_R = PWM_0_DBCTL_ENABLE;
    PWM0_2_DBRISE_R = 10;
    PWM0_2_DBFALL_R = 10;
    PWM0_2_CTL_R |= PWM_0_CTL_ENABLE;
    PWM0_ENABLE_R |= PWM_ENABLE_PWM4EN | PWM_ENABLE_PWM5EN;

}
