#ifndef MOTOR_SPEED_SET_H
#define MOTOR_SPEED_SET_H

#include "../tm4c123gh6pm.h"
#include <stdint.h>

// create an enum housing the types HB1 and HB2
typedef enum{
    HB1,
    HB2
} HBridgePin;

void gen_PWM(HBridgePin pin, float dc); // Generates a PWM for the two generators used (Pin 1 and Pin 2.
void PWM_Init(void);

#endif // MOTOR_SPEED_SET.H
