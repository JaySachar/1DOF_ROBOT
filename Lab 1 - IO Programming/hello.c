#include <stdint.h>
#include <stdio.h>
#include "../tm4c123gh6pm.h"

/**
 * hello.c
 */

/* if PF4 closed, PF4 is LOW, and PF2 should be HIGH, while PF3 and PF1 stay LOW
 *
 * PF4 is HIGH unless SW1 is CLOSED
 */
void PortF_Init(void) {
    SYSCTL_RCGC2_R |= 0x00000020; // Enable the clock for Port F
    while ((SYSCTL_PRGPIO_R & 0x00000020) == 0) {};  // Wait until Port F Clock is ready
    GPIO_PORTF_LOCK_R = 0x4C4F434B; // Unlocks GPIO Port F, allowing changes to the GPIO_CR Register
    GPIO_PORTF_CR_R = 0x1F; // Allows changes to PF4-0
    GPIO_PORTF_AMSEL_R = 0x00;
    GPIO_PORTF_PCTL_R = 0x00000000;
    GPIO_PORTF_DIR_R = 0x0E;
    GPIO_PORTF_AFSEL_R = 0x00;
    GPIO_PORTF_PUR_R = 0x11;
    GPIO_PORTF_DEN_R = 0x1F;
}

int main(void)
{

    PortF_Init();
	return 0;
}
