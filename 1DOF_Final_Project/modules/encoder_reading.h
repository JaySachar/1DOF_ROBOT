#ifndef ENCODER_READING_H
#define ENCODER_READING_H

#include <stdint.h>
#include <tm4c123gh6pm.h>

// Function declarations
void QEI1_Init(void);
float QEI1_GetMotorPosition(void);

#endif // ENCODER_READING_H
