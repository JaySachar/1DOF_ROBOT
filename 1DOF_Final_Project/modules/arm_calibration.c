#include "arm_calibration.h"

void calibrate_arm_position(float ADCVal)
{
    // General idea: I only have voltages rn, so after calibrating it with the ADCVal calculated:
    // If greater than 3V we are in the right quadrant, go CCW until voltage == 1.186
    // One thing about that though, is we will be using PWM signals. So maybe we just go at like 55% 45% CCW, so very slow
    // Since the ADC Values update relatively slowly.
    // Another idea is we take the ADC value it reads, map it to an known motor encoder value, and then do the
    // opposite CCW. I think the first one's faster since we just fucking send it CCW
}
