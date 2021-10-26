// Name: Jesus Franco
// Student ID: 014046368
// Course Number: CECS 347 Section 7317
// Assignment: Project 1: Wall Follower Robot
// PWM.c
// Description: Use PWM0/PB6 and PWM1/PB7 to generate pulse-width modulated outputs. 

#include <stdint.h>

// period is 16-bit number of PWM clock cycles in one period (3<=period)
// period for PB6 and PB7 must be the same
// duty is number of PWM clock cycles output is high  (2<=duty<=period-1)
// PWM clock rate = processor clock rate/SYSCTL_RCC_PWMDIV
//                = BusClock/2 
//                = 80 MHz/2 = 40 MHz (in this example)
// Output on PB6/M0PWM0
void PWM0A_Init(uint16_t period, uint16_t duty);

// change duty cycle of PB6
// duty is number of PWM clock cycles output is high  (2<=duty<=period-1)
void PWM0A_Duty(uint16_t duty);

// period is 16-bit number of PWM clock cycles in one period (3<=period)
// period for PB6 and PB7 must be the same
// duty is number of PWM clock cycles output is high  (2<=duty<=period-1)
// PWM clock rate = processor clock rate/SYSCTL_RCC_PWMDIV
//                = BusClock/2 
//                = 80 MHz/2 = 40 MHz (in this example)
// Output on PB7/M0PWM1
void PWM0B_Init(uint16_t period, uint16_t duty);

// change duty cycle of PB7
// duty is number of PWM clock cycles output is high  (2<=duty<=period-1)
void PWM0B_Duty(uint16_t duty);

void PWM1_PF2_Init(uint16_t period, uint16_t duty);

void PWM1_PF2_Duty(uint16_t duty);


void PWM_Motors_Init(uint16_t period, uint16_t duty);

void PWM0_RightMotor_Duty(uint16_t duty);
void PWM1_LeftMotor_Duty(uint16_t duty);
