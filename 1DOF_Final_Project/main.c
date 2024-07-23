#include <stdint.h>
#include "tm4c123gh6pm.h"
#include "modules/encoder_reading.h"
#include "modules/motor_speed_set.h"
#include "modules/PLL_Init.h"
#include "modules/pot_reading.h"
#include "modules/ctrl_loop.h"


void GPIOF_Init(void);
int main(void) {
    QEI1_Init();   // Initialize QEI1
    GPIOF_Init();   // Initialize GPIO for debugging
    PLL_Init();
    ADC0_Init();
    sample_sequencer_init();
    PWM_Init();

    float desired_position = 90.0; // degrees
    float current_position;
    while(1){
        current_position = QEI1_GetMotorPosition();
        ctrl_loop(desired_position, current_position);
        //gen_PWM(HB1, 0.5);
    }
}

void GPIOF_Init(void) {
    SYSCTL_RCGCGPIO_R |= 0x20;       // Enable clock for Port F
    while ((SYSCTL_PRGPIO_R & 0x20) == 0) {}; // Wait for Port F to be ready

    GPIO_PORTF_LOCK_R = 0x4C4F434B;  // Unlock Port F
    GPIO_PORTF_CR_R |= 0x0E;         // Allow changes to PF1, PF2, PF3
    GPIO_PORTF_DIR_R |= 0x0E;        // Set PF1, PF2, PF3 as output
    GPIO_PORTF_DEN_R |= 0x0E;        // Enable digital function on PF1, PF2, PF3
}
