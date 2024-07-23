// Standard Libraries
#include <stdint.h>
#include <stdio.h>
#include "../tm4c123gh6pm.h"

// Bit Mask Declaration
#define PF4 0x10 // Bit mask for PF4 (Input)  (0001 0000) (Switch 1)
#define PF3 0x08  // Bit mask for PF3 (output)(0000 1000) (Green LED)
#define PF2 0x04  // Bit mask for PF2 (output)(0000 0100) (Blue LED)
#define PF1 0x02  // Bit mask for PF1 (output)(0000 0010) (Red LED)
#define PF0 0x01  // Bit mask for PF0 (input) (0000 0001) (Switch 2)

// Function Declaration
void PortF_Init(void);
void delay(void);

int main(void)
{
    PortF_Init(); // Initialize Port F
    while(1){
        // Continuously check if PF4 (SW1) is pressed (it is 0 when pressed)
        if((GPIO_PORTF_DATA_R & PF4) == 0x00){ // Mask the Port with PF4 (0001 0000) (if not pressed GPIO_PORTF_DATA_R == 0001 0001)
            while((GPIO_PORTF_DATA_R & PF0) != 0x00)
            {
                GPIO_PORTF_DATA_R |= PF1; // Set PF1 (Red LED) to HIGH (0001 0001)
                delay();
/*              0001 0001 == GPIO_PORTF_DATA_R
               |0000 0010 == PF2
                0001 0011 == NEW GPIO_PORTF_DATA_R
*/
                GPIO_PORTF_DATA_R &= ~PF1; // Set PF1 (Red LED) LOW
/*              0001 0011 == GPIO_PORTF_DATA_R
 *             &1111 1101 == ~PF1
 *              00001 0001 == NEW GPIO_PORTF_DATA_R
 */
                GPIO_PORTF_DATA_R |= PF2; // Set PF2   (Blue LED) to HIGH
                delay();
                GPIO_PORTF_DATA_R &= ~PF2; // Set PF2 (Blue LED) to LOW

                GPIO_PORTF_DATA_R |= PF3; // Set PF3 (Green LED) to HIGH
                delay();
                GPIO_PORTF_DATA_R &= ~PF3; // Set PF3 (Green LED) to LOW
            }
    }
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

void delay(void){
    volatile uint32_t time;
    for (time = 0; time <200000; time++);
}
