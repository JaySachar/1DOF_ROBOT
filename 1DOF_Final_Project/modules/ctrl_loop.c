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

    float scaled_control_signal = control_signal / max_voltage;

    if(error >= 2) // Run CW
    { // Run CW
        gen_PWM(HB2, scaled_control_signal);
        gen_PWM(HB1, 0.0);

    } else if (error <= -2)// Run CCW
    { // Run CCW
        gen_PWM(HB2, -scaled_control_signal);
        gen_PWM(HB1, 0);
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

    float Kp = 1;
    float Ki = 0;
    float Kd = 0;

    integral += error;
    derivative = error - previous_error;
    control_signal = Kp*error + Ki*integral + Kd * derivative;

    previous_error = error;

    return control_signal;
}
void SysTickInit(void){
    // Disable timer
    NVIC_ST_CTRL_R = 0;
    // Set reload value to be exactly 0.2 [ms]
    NVIC_ST_RELOAD_R = 0x00F42400;
    // Clear the current value register. This register contains the current value at the time the register is accessed
    NVIC_ST_CURRENT_R = 0;
    NVIC_SYS_PRI3_R = (NVIC_SYS_PRI3_R&0x00FFFFFF)|0x40000000;
    // 0x00000007 -> for the last 4 bits: 0111, which sets CLK_SRC to system clock
    // Enables ITERN (interrupts generated to the NVIC when SysTick counts to 0)
    // Enables the counter. Here, it loads in the reload value, and begins counting down. When reaching 0, the count bit is set, and an interrupt is generated
    NVIC_ST_CTRL_R = 0x00000007;
}

