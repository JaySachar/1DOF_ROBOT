#ifndef ARM_CALIBRATION_H
#define ARM_CALIBRATION_H

#include <stdint.h>
#include <tm4c123gh6pm.h>
#include "pot_reading.h"
// Function Declarations
void calibrate_arm_position(float ADCVal); // This function will take the ADC reading from the potentiometer
                                           // Determine which angle it is relatively from the lef

#endif // ARM_CALIBRATION_H
