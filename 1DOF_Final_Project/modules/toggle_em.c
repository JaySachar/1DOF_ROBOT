#include "toggle_em.h"
#include <stdint.h>
#include "tm4c123gh6pm.h"

// Define the port and pin for the electromagnet control
#define EM_PORT SYSCTL_RCGCGPIO_R3
#define EM_PIN 0x01  // PD0

// Function to initialize the GPIO for the electromagnet control
void ElectromagnetInit(void) {
    // Enable the GPIO port D
    SYSCTL_RCGCGPIO_R |= EM_PORT;

    // Wait for the GPIO port to be ready
    while((SYSCTL_PRGPIO_R & EM_PORT) == 0) {
    }

    // Unlock PD0 if necessary
    GPIO_PORTD_LOCK_R = GPIO_LOCK_KEY;
    GPIO_PORTD_CR_R |= EM_PIN;

    // Configure PD0 as output
    GPIO_PORTD_DIR_R |= EM_PIN;
    GPIO_PORTD_DEN_R |= EM_PIN;
}

// Function to control the electromagnet
void ElectromagnetControl(int state) {
    if(state) {
        GPIO_PORTD_DATA_R |= EM_PIN;  // Set the pin high
    } else {
        GPIO_PORTD_DATA_R &= ~EM_PIN;  // Set the pin low
    }
}
