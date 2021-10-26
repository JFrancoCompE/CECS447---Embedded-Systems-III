// Name: Jesus Franco
// Student ID: 014046368
// Course Number: CECS 447
// Assignment: Project 3: BlueTooth Controlled Car
// Description: This code will allow communication from a serial terminal to the robot car.
//							There will be commands to change the speed and direction of the car through
//							a BlueTooth Terminal. The UART1 configuration is set to a baud rate of 
//							57600 and will be located under the name of SWAG. 

//   PB3-0 -> L298N IN[4:1] respectively
//   PB6   -> L298N ENA
//   PB7   -> L298N ENB
//
//	 PC4 -> UART1 RX
//	 PC5 -> UART1 TX

//	 Green LED    -> Forward
//	 Blue LED     -> Reverse
//   Yellow LED   -> Left Turn
//	 Sky Blue LED	-> Right Turn
//   Blue LED		  -> The car is too close to the RIGHT wall
//   No LED       -> STOP
//   Pink LED 		-> Speed Up
//   Red LED 			-> Slow Down

#include "tm4c123gh6pm.h"
#include <stdint.h>
#include "PLL.h"
#include "PWM.h"
#include "UART.h"
      
// Constants
#define PERIOD 40000		// 3125 Hz, 100% duty cycle

// Function prototypes
void PortF_Init(void);
void PortB_Init(void);

// Global Variables
char keyPressed;									// UART1 InChar variable
unsigned long speed = 50;					// initial speed is set to 50%

int main(void){
	PortB_Init();										// Port B initialization
	PortF_Init();										// Port F initialization
	PLL_Init();											// PLL initialization for 50MHz
	UART1_Init();

	PWM0A_Init(PERIOD, 0);	        // initialize PWM0A, 3125 Hz, 0% duty
  PWM0B_Init(PERIOD, 0);     		  // initialize PWM0B, 3125 Hz, 0% duty
	GPIO_PORTB_DATA_R = 0x05;       // PORTB to be set to move forward initially
	
  while(1){
		keyPressed = UART1_InChar();	// Receive a Char variable
		
		switch(keyPressed){
			case 'W':										// Forward
				GPIO_PORTF_DATA_R = 0x08; // GREEN
				GPIO_PORTB_DATA_R = 0x05; // Forward direction
				PWM0B_Duty(PERIOD * .01 * speed); // Adjust left wheel speed
				PWM0A_Duty(PERIOD * .01 * speed); // Adjust right wheel speed
				break;
			case 'S':										// Reverse
				GPIO_PORTF_DATA_R = 0x04; // BLUE
				GPIO_PORTB_DATA_R = 0x0A; // Reverse direction
				PWM0B_Duty(PERIOD * .01 * speed); // Adjsut left wheel speed
				PWM0A_Duty(PERIOD * .01 * speed); // Adjust right wheel speed
				break;
			case 'A':										// Left Turn
				GPIO_PORTF_DATA_R = 0x0A; // YELLOW
				PWM0B_Duty(PERIOD * .20); // Adjust left wheel speed to be slower
				PWM0A_Duty(PERIOD * .50); // Adjsut right wheel speed to be faster
				break;
			case 'D':										// Right Turn
				GPIO_PORTF_DATA_R = 0x0C; // SKY BLUE
				PWM0B_Duty(PERIOD * .50); // Adjust left wheel speed to be faster
				PWM0A_Duty(PERIOD * .20); // Adjust right wheel speed to be slower
				break;
			case 'T':										// STOP
				GPIO_PORTF_DATA_R = 0x00; // NONE
				PWM0B_Duty(0);						// No speed
				PWM0A_Duty(0);						// No speed
				break;
			case 'U':										// Speed Up
				GPIO_PORTF_DATA_R = 0x06; // PINK
				speed = speed + 10;				// Adjsut speed by increments of 10%
				if (speed >= 100) speed = 100; // Adjust max speed to 100%
				PWM0B_Duty(PERIOD * .01 * speed); // Adjust left wheel speed
				PWM0A_Duty(PERIOD * .01 * speed); // Adjust right wheel speed
				break;
			case 'L':										// Slow Down
				GPIO_PORTF_DATA_R = 0x02; // RED
				speed = speed - 10;				// Adjust speed by decrements of 10%
				if (speed <= 10) speed = 10; // Adjust min speed to be 10%
				PWM0B_Duty(PERIOD * .01 * speed); // Adjust left wheel speed
				PWM0A_Duty(PERIOD * .01 * speed); // Adjust right wheel speed
				break;
		}// end switch
	} // end while loop
} // end main

// PortF_Init for initializing the onboard switches/LEDs and the interrupts
void PortF_Init(void){    
	volatile unsigned long delay;
  SYSCTL_RCGC2_R |= 0x00000020;     // Acativate clock for port F
  delay = SYSCTL_RCGC2_R;           // delay
  GPIO_PORTF_LOCK_R = 0x4C4F434B;   // unlock GPIO PortF PF0  
  GPIO_PORTF_CR_R |= 0x1F;          // allow changes to PF4-0       
	GPIO_PORTF_DIR_R &= ~0x11;   			// PF4 & PF0 as inputs
  GPIO_PORTF_DIR_R |=  0x0E;    		// PF3-0 as outputs
  GPIO_PORTF_AFSEL_R &= ~0x11;  		// disable alt funct on PF4 & PF0
  GPIO_PORTF_DEN_R |= 0x1F;     		// enable digital I/O on PF4 & PF0  
  GPIO_PORTF_PCTL_R &= ~0x000F0000; // configure PF4 as GPIO
  GPIO_PORTF_AMSEL_R &= ~0x11;      // disable analog functionality on PF4 & PF0
  GPIO_PORTF_PUR_R |= 0x11;     		// enable pull-up on PF4 & PF0
	GPIO_PORTF_IS_R &= ~0x11;     		// PF4 & PF0 is edge-sensitive
  GPIO_PORTF_IBE_R |= 0x11;    			// PF4 & PF0 is both edges
  GPIO_PORTF_IEV_R &= ~0x11;    		// PF4 & PF0 falling edge event
  GPIO_PORTF_ICR_R = 0x11;      		// clear flag4
  GPIO_PORTF_IM_R |= 0x11;      		// arm interrupt on PF4 & PF0
  NVIC_PRI7_R |= (NVIC_PRI7_R&0xFF00FFFF)|0x00A00000; // priority 5
  NVIC_EN0_R |= 0x40000000;      		// enable interrupt 30 in NVIC
}

// PortB_Init for initializing PB7-6 & PB3-0 as outputs
void PortB_Init(void){volatile unsigned long delay;
  SYSCTL_RCGC2_R |= 0x00000002;     // Activate clock for port B
  delay = SYSCTL_RCGC2_R;           // delay   
  GPIO_PORTB_DIR_R = 0x0F;          // PB3-0 as outputs 
  GPIO_PORTB_DEN_R = 0x0F;          // enable digital pins PB3-PB0    
}
