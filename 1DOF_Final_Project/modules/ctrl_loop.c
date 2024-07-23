#include "ctrl_loop.h"
#include "encoder_reading.h"
#include "motor_speed_set.h"


void ctrl_loop(float desired_position, float current_position){
    //float current_position = QEI1_GetMotorPosition();
    float error = desired_position - current_position;
    float max_voltage = 12.0; // Max voltage is 12 volts to the motor

    float control_signal = PID_control(error);

    if (control_signal > max_voltage)
    {
        control_signal = max_voltage;
    }
    else if (control_signal < -max_voltage)
    {
        control_signal = -max_voltage;
    }

    float scaled_control_signal = control_signal / (1.5*max_voltage);

    if(error >= 2) // Run CW
    {
        gen_PWM(HB1, scaled_control_signal);
    } else if (error <= -2)// Run CCW
    {
        gen_PWM(HB2, -scaled_control_signal);
    }
    else
    {
        PWM0_2_CMPA_R = 0; // Set the comparator to load value, effectively making duty cycle 0
        PWM0_2_CMPB_R = 0; // Set the comparator to load value, effectively making duty cycle 0
    }
}

float PID_control(float error){
    static float integral = 0.0;
    static float previous_error = 0.0;
    float derivative;
    float control_signal;

    float Kp = 2;
    float Ki = 0;
    float Kd = 0;

    integral += error;
    derivative = error - previous_error;
    control_signal = Kp*error + Ki*integral + Kd * derivative;

    previous_error = error;

    return control_signal;
}
