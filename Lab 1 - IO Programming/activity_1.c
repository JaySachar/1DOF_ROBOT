// Standard Libraries
#include <stdint.h>
#include <stdio.h>
#include "../tm4c123gh6pm.h"

// Bit Mask Declaration
#define PF4 0x10 // Bit mask for PF4 (Input)  (0001 0000)
#define PF3 0x08  // Bit mask for PF3 (output)(0000 1000)
#define PF2 0x04  // Bit mask for PF2 (output)(0000 0100)
#define PF1 0x02  // Bit mask for PF1 (output)(0000 0010)
#define PF0 0x01  // Bit mask for PF0 (input) (0000 0001)

// Function Declaration
void PortF_Init(void);

/* if PF4 closed, PF4 is LOW, and PF2 should be HIGH, while PF3 and PF1 stay LOW
 *
 * PF4 is HIGH unless SW1 is CLOSED
 */


int main(void)
{
    PortF_Init(); // Initialize Port F
    while(1){
        if ((GPIO_PORTF_DATA_R & PF4) == 0x00){ // Check if PF4 is LOW (SW1 is pressed)
            GPIO_PORTF_DATA_R |= P2; // Set PF2 HIGH
        }
        else {
        GPIO_PORTF_DATA_R &= ~PF2; // Set PF2 LOW
        }

        GPIO_PORTF_DATA_R &= ~(PF3 | PF1); // Ensure PF3 and PF1 are LOW
    }
    return 0;
}

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
