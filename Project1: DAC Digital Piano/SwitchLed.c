// SwitchLed.c
// This software configures the on-board switches and LEDs.
//
// For use with the LM4F120 or TM4C123
// September 15, 2013
// Port B bits 5-0 have the 6-bit DAC
// Port F is onboard LaunchPad switches and LED
// Port F bit 4 is negative logic switch to play sound, SW1
// Port D bits 3-0 for piano keys
// Port C bits 6-4 for piano keys
// SysTick ISR: PF3 ISR heartbeat

#include "tm4c123gh6pm.h"

void PortD_Init(void){ volatile unsigned long  delay;
  SYSCTL_RCGC2_R |= 0x00000008;     // 1) activate clock for Port D
  delay = SYSCTL_RCGC2_R;           // allow time for clock to start
  GPIO_PORTD_AMSEL_R = 0x0F;        // 3) disable analog on PD
  GPIO_PORTD_PCTL_R &= ~0x0000FFFF; // 4) PCTL GPIO on PD3-0
  GPIO_PORTD_DIR_R &= ~0x0F;        // 5) PD3-PD0 input
  GPIO_PORTD_AFSEL_R &= ~0x0F;      // 6) disable alt funct on PD3-0
  GPIO_PORTD_PUR_R |= 0x0F;         // enable pull-up on PD3-0
  GPIO_PORTD_DEN_R |= 0x0F;         // 7) enable digital I/O on PD3-0
}

void PortC_Init(void){ volatile unsigned long  delay;
  SYSCTL_RCGC2_R |= 0x00000004;     // 1) activate clock for Port D
  delay = SYSCTL_RCGC2_R;           // allow time for clock to start
  GPIO_PORTC_AMSEL_R &= ~0x70;      // 3) disable analog on PD
  GPIO_PORTC_PCTL_R &= ~0x0FFF0000; // 4) PCTL GPIO on PD6-4
  GPIO_PORTC_DIR_R &= ~0x70;        // 5) PD3-PD0 input
  GPIO_PORTC_AFSEL_R &= ~0x70;      // 6) disable alt funct on PD6-4
  GPIO_PORTC_PUR_R |= 0x70;         // enable pull-up on PFD6-4
  GPIO_PORTC_DEN_R |= 0x70;         // 7) enable digital I/O on PD6-4
}

// read the values of the two switches
// Output: PD3-0 
unsigned long Switch_In_D(void){ 
  return (GPIO_PORTD_DATA_R&0x0F)^0x0F;
}

// read the values of the two switches
// Output: PC6-4 
unsigned long Switch_In_C(void){ 
  return (GPIO_PORTC_DATA_R&0x70)^0x70;
}

//---------------------Delay10ms---------------------
// wait 1ms for switches to stop bouncing
void Delay10ms(void){unsigned long volatile time;
  //time = 14545;  // 1msec
	time = 3.125*14545;  // 1msec
  while(time){
		time--;
  }
}