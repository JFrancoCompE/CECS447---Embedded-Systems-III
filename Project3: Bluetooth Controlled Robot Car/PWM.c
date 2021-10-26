// Name: Jesus Franco
// Student ID: 014046368
// Course Number: CECS 347 Section 7317
// Assignment: Project 1: Wall Follower Robot
// PWM.c
// Description: Use PWM0/PB6 and PWM1/PB7 to generate pulse-width modulated outputs. 

#include <stdint.h>
#include "tm4c123gh6pm.h"

#define PWM_0_GENA_ACTCMPAD_ONE 0x000000C0  // Set the output signal to 1
#define PWM_0_GENA_ACTLOAD_ZERO 0x00000008  // Set the output signal to 0
#define PWM_0_GENB_ACTCMPBD_ONE 0x00000C00  // Set the output signal to 1
#define PWM_0_GENB_ACTLOAD_ZERO 0x00000008  // Set the output signal to 0

#define SYSCTL_RCC_USEPWMDIV    0x00100000  // Enable PWM Clock Divisor
#define SYSCTL_RCC_PWMDIV_M     0x000E0000  // PWM Unit Clock Divisor
#define SYSCTL_RCC_PWMDIV_2     0x00000000  // /2


// period is 16-bit number of PWM clock cycles in one period (3<=period)
// period for PB6 and PB7 must be the same
// duty is number of PWM clock cycles output is high  (2<=duty<=period-1)
// PWM clock rate = processor clock rate/SYSCTL_RCC_PWMDIV
//                = BusClock/2 
//                = 80 MHz/2 = 40 MHz (in this example)
// Output on PB6/M0PWM0
void PWM0A_Init(uint16_t period, uint16_t duty){
  SYSCTL_RCGCPWM_R |= 0x01;             // 1) activate PWM0
  SYSCTL_RCGCGPIO_R |= 0x02;            // 2) activate port B
  while((SYSCTL_PRGPIO_R&0x02) == 0){};
  GPIO_PORTB_CR_R |= 0x40;
	GPIO_PORTB_AFSEL_R |= 0x40;           // enable alt funct on PB6
  GPIO_PORTB_PCTL_R &= ~0x0F000000;     // configure PB6 as PWM0
  GPIO_PORTB_PCTL_R |= 0x04000000;
  GPIO_PORTB_AMSEL_R &= ~0x40;          // disable analog functionality on PB6
  GPIO_PORTB_DEN_R |= 0x40;             // enable digital I/O on PB6
  SYSCTL_RCC_R = 0x00100000 |           // 3) use PWM divider
      (SYSCTL_RCC_R & (~0x000E0000));   //    configure for /2 divider
  PWM0_0_CTL_R = 0;                     // 4) re-loading down-counting mode
  PWM0_0_GENA_R = 0xC8;                 // low on LOAD, high on CMPA down
  // PB6 goes low on LOAD
  // PB6 goes high on CMPA down
  PWM0_0_LOAD_R = period - 1;           // 5) cycles needed to count down to 0
  PWM0_0_CMPA_R = duty - 1;             // 6) count value when output rises
  PWM0_0_CTL_R |= 0x00000001;           // 7) start PWM0
  PWM0_ENABLE_R |= 0x00000001;          // enable PB6/M0PWM0
}
// change duty cycle of PB6
// duty is number of PWM clock cycles output is high  (2<=duty<=period-1)
void PWM0A_Duty(uint16_t duty){
  PWM0_0_CMPA_R = duty - 1;             // 6) count value when output rises
}
// period is 16-bit number of PWM clock cycles in one period (3<=period)
// period for PB6 and PB7 must be the same
// duty is number of PWM clock cycles output is high  (2<=duty<=period-1)
// PWM clock rate = processor clock rate/SYSCTL_RCC_PWMDIV
//                = BusClock/2 
//                = 80 MHz/2 = 40 MHz (in this example)
// Output on PB7/M0PWM1
void PWM0B_Init(uint16_t period, uint16_t duty){
  volatile unsigned long delay;
  SYSCTL_RCGCPWM_R |= 0x01;             // 1) activate PWM0
  SYSCTL_RCGCGPIO_R |= 0x02;            // 2) activate port B
  delay = SYSCTL_RCGCGPIO_R;            // allow time to finish activating
  GPIO_PORTB_CR_R |= 0x80;
  GPIO_PORTB_AFSEL_R |= 0x80;           // enable alt funct on PB7
  GPIO_PORTB_PCTL_R &= ~0xF0000000;     // configure PB7 as M0PWM1
  GPIO_PORTB_PCTL_R |= 0x40000000;
  GPIO_PORTB_AMSEL_R &= ~0x80;          // disable analog functionality on PB7
  GPIO_PORTB_DEN_R |= 0x80;             // enable digital I/O on PB7
  SYSCTL_RCC_R |= SYSCTL_RCC_USEPWMDIV; // 3) use PWM divider
  SYSCTL_RCC_R &= ~SYSCTL_RCC_PWMDIV_M; //    clear PWM divider field
  SYSCTL_RCC_R += SYSCTL_RCC_PWMDIV_2;  //    configure for /2 divider
  PWM0_0_CTL_R = 0;                     // 4) re-loading down-counting mode
  PWM0_0_GENB_R = (PWM_0_GENB_ACTCMPBD_ONE|PWM_0_GENB_ACTLOAD_ZERO);
  // PB7 goes low on LOAD
  // PB7 goes high on CMPB down
  PWM0_0_LOAD_R = period - 1;           // 5) cycles needed to count down to 0
  PWM0_0_CMPB_R = duty - 1;             // 6) count value when output rises
  PWM0_0_CTL_R |= 0x00000001;           // 7) start PWM0
  PWM0_ENABLE_R |= 0x00000002;          // enable PB7/M0PWM1
}
// change duty cycle of PB7
// duty is number of PWM clock cycles output is high  (2<=duty<=period-1)
void PWM0B_Duty(uint16_t duty){
  PWM0_0_CMPB_R = duty - 1;             // 6) count value when output rises
}

void PWM1_PF2_Init(uint16_t period, uint16_t duty){
  SYSCTL_RCGCPWM_R |= 0x02;             // 1) activate PWM1
  SYSCTL_RCGCGPIO_R |= 0x20;            // 2) activate port F
  while((SYSCTL_PRGPIO_R&0x20) == 0){};
	GPIO_PORTF_AFSEL_R |= 0x04;           // enable alt funct on PB6
  GPIO_PORTF_PCTL_R &= ~0x00000F00;     // configure PB6 as PWM0
  GPIO_PORTF_PCTL_R |= 0x00000500;
		
		
  GPIO_PORTF_AMSEL_R &= ~0x04;          // disable analog functionality on PF2
  GPIO_PORTF_DEN_R |= 0x04;             // enable digital I/O on PF2
		
		
  SYSCTL_RCC_R = 0x00100000 |           // 3) use PWM divider
		(SYSCTL_RCC_R & (~0x000E0000));   //    configure for /2 divider	
  PWM1_3_CTL_R = 0;                     // 4) re-loading down-counting mode
  PWM1_3_GENA_R = 0xC8;                 // low on LOAD, high on CMPA down

  PWM1_3_LOAD_R = period - 1;           // 5) cycles needed to count down to 0
  PWM1_3_CMPA_R = duty - 1;             // 6) count value when output rises
  PWM1_3_CTL_R |= 0x00000001;           // 7) start PWM3
  PWM1_ENABLE_R |= 0x00000040;          // enable PF2/M1PWM3
}

void PWM1_PF2_Duty(uint16_t duty){
  PWM1_3_CMPA_R = duty - 1;        																			// count value when output rises     
}

/////////////////////////////////////////////////////////////////////
//// PWM PORT E																										 //
/////////////////////////////////////////////////////////////////////

//void PWM_Motors_Init(uint16_t period, uint16_t duty){
//	
//  SYSCTL_RCGCPWM_R			|= 0x01;																				// enable PWM0				          
//  SYSCTL_RCGCGPIO_R 		|= 0x10; 																				// Port E clock
//	
//  while((SYSCTL_PRGPIO_R&0x10) == 0){};
//		
//  GPIO_PORTE_AFSEL_R 		|= 0x30;																				// activate alt function of PE4 and PE5          
//  GPIO_PORTE_PCTL_R = (GPIO_PORTF_PCTL_R&0xFF00FFFF) + 0x00440000;			// M0PWM0 (control module) PE4 and PE5
//  GPIO_PORTE_AMSEL_R 		&= ~0x30;          															// disable analog funct on PE4 and PE5
//  GPIO_PORTE_DEN_R 			|= 0x30; 																				// enable I/O on PB6
//		 
//	SYSCTL_RCC_R |= SYSCTL_RCC_USEPWMDIV; 																// use PWM divider
//  SYSCTL_RCC_R &= ~SYSCTL_RCC_PWMDIV_M; 																// clear PWM divider field
//  SYSCTL_RCC_R += SYSCTL_RCC_PWMDIV_2;  																// configure for /2 divider
//	//SYSCTL_RCC_R = 0x00100000 | (SYSCTL_RCC_R & (~0x000E0000));	
//		
//		
//  PWM0_2_CTL_R 					 = 0;  																					// re-loading down-counting mode                   
//  PWM0_2_GENA_R 				 = 0xC8;               													// low on LOAD, high on CMPA down PE4
//	PWM0_2_GENB_R 				 = 0xC08;               												// low on LOAD, high on CMPA down PE5
//		
//  PWM0_2_LOAD_R					 = period - 1;																	// cycles needed to count down to 0
//  PWM0_2_CMPA_R 			   = duty - 1;  																	// count value when output rises 
//	PWM0_2_CMPB_R					 = duty - 1;
//  PWM0_2_CTL_R 					|= 0x00000001;           												// update PWM Generator 0
//  PWM0_ENABLE_R 			  |= 0x30;          															// enable PE4/M0PWM4 and PE5/M0PWM5
//}


//void PWM0_RightMotor_Duty(uint16_t duty){
//  PWM0_2_CMPA_R = duty - 1;        																			// count value when output rises     
//}

//void PWM1_LeftMotor_Duty(uint16_t duty){
//  PWM0_2_CMPB_R = duty - 1;        																			// count value when output rises     
//}

