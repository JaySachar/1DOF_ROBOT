#ifndef CTRL_LOOP_H
#define CTRL_LOOP_H

#include <stdint.h>
#include <tm4c123gh6pm.h>

// Function declarations
void ctrl_loop(float desired_position, float current_position);
float PID_control(float error);
#endif // CTRL_LOOP_H
