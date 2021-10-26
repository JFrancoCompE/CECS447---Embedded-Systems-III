// DAC_Basictones.c
// Runs on TM4C123
// Use SysTick interrupts to implement a sine wave for each C Major notes
// Min He
// January 11, 2021

// This example shows how to generate music tones from a 3-bit DAC outputs.
// The program will first play 8 C Major notes automatically, 
// it will then allow user to play the same 8 C mojor notes manually using 
// the onboard push button SW1: press and hold SW1 will play one note;
// release SW1 will stop playing the note. The same operation can be repeated
// to play more notes in C major table in rising order and then repeat.
// System configuration: system clock is 16MHz, Systick timer is 
// used to control output rate. A Tone_Tab is used to represent 
// number of clocks for each tone frequency
// A 3-bit DAC is used to convert output from digital to analog.
// There are 16 values for each tone period.
// Port B bits 2-0 have the 3-bit DAC.
// Port F is onboard LaunchPad switches and LED.
// Port F bit 4 is negative logic switch to play sound, SW1.
// SysTick ISR: PF3 ISR heartbeat.


#include "tm4c123gh6pm.h"
#include "Sound.h"
#include "SwitchLed.h"

// initial values for piano major tones: 
// Assume system clock is 16MHz
const unsigned long Tone_Tab[] = 
// Note name: C, D, E, F, G, A, B, C'
// Offset:0, 1, 2, 3, 4, 5, 6, 7
//{30534,27211,24242,22923,20408,18182,16194,15289}; // C4 Major notes
{15289,13621,12135,11454,10204,9091,8099,7645}; // C5 Major notes
//{7645,6810,6067,5727,5102,4545,4050,3822}; // C6 Major notes

#define NUM_VALs  16  // Assume 3-bit DAC is used, that will give 16 values for one period.
#define NUM_NOTEs 8   // number of notes to be played repeatedly

// basic functions defined at end of startup.s
extern void DisableInterrupts(void); // Disable interrupts
extern void EnableInterrupts(void);  // Enable interrupts
extern void WaitForInterrupt(void);  // low power mode

int main(void){ 
	unsigned char i,j,input,previous; 
  unsigned char tone_index = 0;	
	
  DisableInterrupts();    
  SwitchLed_Init();       // use Port F for onboard switches and LEDs
  DAC_Init();          // Port B is DAC
  
	// Automatic play for all 8 notes
  for(i=0;i<8;i++){
		EnableInterrupts();
    Sound_Init(Tone_Tab[i]/NUM_VALs);      // Play 8 major notes
		for (j=0;j<50;j++) 
      Delay10ms();  // delay for tempo
    DisableInterrupts();		
  }
  
	// Manual play: push SW1 for one note, release and push again moves to the next note.
  previous = Switch_In()&0x10;
  while(1){ 
    input = Switch_In()&0x10; // bit 4 means SW1 pressed
    if(input&&(previous==0)){ // just pressed     
      EnableInterrupts();
      Sound_Init(Tone_Tab[tone_index]/NUM_VALs);      // Play 8 major notes
			tone_index = (tone_index+1)%NUM_NOTEs;					// move to the next note
    }
    if(previous&&(input==0)){ // just released     
      DisableInterrupts();    // stop sound
    }
    previous = input; 
    Delay10ms();  // remove switch bounce    
  }  
}



