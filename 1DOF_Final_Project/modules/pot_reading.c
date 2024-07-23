#include "pot_reading.h"
// Notes:

/* 12-bit precision ADC
 * 12 shared Analog input channels
 * 1 Million samples / second (1Mhz)
 * ENSURE PLL_INIT() IS RUN BEFORE THIS!!!!
 * Pin PE3 is Used for Potenttiometer
 */

void ADC0_Init(void){
    SYSCTL_RCGCADC_R   |= 0x01; // Enable ADC Module 0 Run Mode CLock Gating Control (pg. 352)
    SYSCTL_RCGCGPIO_R  |= SYSCTL_RCGCGPIO_R4; // Enable Port E in Run Mode
    while ((SYSCTL_PRGPIO_R & 0x10) == 0) {}; // Wait for Port E to be ready
    GPIO_PORTE_AFSEL_R |= 0x08; // 0000.1000 Turn on PE3 for Alternate Function Selection
    GPIO_PORTE_DEN_R   &= ~0x08; // Turn port PE3 to 0. ~0x08 == ~0000.1000 == 1111.01111
    GPIO_PORTE_AMSEL_R |= 0x08; // Disable the analog isolation circuit for ADC input pin
}

void sample_sequencer_init(void){
    ADC0_PC_R = 0x1; // Configure for 125K samples/sec
    ADC0_ACTSS_R &= ~0x00000001; // Disable Sample Sequencer 0 which has a FIFO depth of 8
    ADC0_EMUX_R = (ADC0_EMUX_R & ~0xF) | 0xF; // Set trigger to always be sampling
    ADC0_SSMUX0_R = 0; // Set channel (AIN0);
    ADC0_SSCTL0_R = 0x6; // Set END0, IE0
    ADC0_ACTSS_R |= 0x00000001; // Enable Sample Sequencer 0 which has a FIFO depth of 8
}

void pot_read(uint32_t *data)
{
    ADC0_PSSI_R = 0x1;
    while((ADC0_RIS_R & 0x1) == 0){} // wait for conversion
    int i = 0;
    for (i; i < 8; i++){
        data[i] = ADC0_SSFIFO0_R & 0x0FFF; // Read the 12-bit result
    }
    ADC0_ISC_R = 0x1; // Clear Completion Flag
}
