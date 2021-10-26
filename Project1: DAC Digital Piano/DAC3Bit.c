// Name: Jesus Franco
// Student ID: 014046368
// Course Number: CECS 447
// Assignment: Project 1: Digital Piano (part 2)
// DAC3Bit.c
// Description: This code allows the user to interchange between a piano mode
//  						or a auto-player mode. SW1 will turn ON/OFF the program and SW2
//							will change the mode from intially a piano mode to a auto-play
//							mode. PortsB will output a 6 bit DAC to ouput the sound to an audio
//							amplifier which is connected to some apple earphones. PortC and PortD
//							will serve as piano keys inputs to allow the user to do sound.

//	 PD3-0 & PC6-4 -> Piano buttons

//	 PF4 -> SW1 to turn ON/OFF music
//   PF0 -> SW2 to change mode

//   PB5-0 -> DAC outputs

//   Red LED	 -> Music is OFF
//	 Green LED -> Music is ON

#include "tm4c123gh6pm.h"
#include "Sound.h"
#include "SwitchLed.h"
#include "PLL.h"

// 2. Declarations Section

// define music note data structure 
struct Note {
  unsigned long tone_index;
  unsigned char delay;
};

typedef const struct Note NTyp;

// Constants
#define TONE_DURATION 2 // each tone uses the same duration 
#define NUM_VALs  64    // Assume 6-bit DAC is used, that will give 64 values for one period.

// initial values for piano major tones.
// Assume SysTick clock frequency is 50MHz.
const unsigned long tonetab[] =
// C, D, E, F, G, A, B
// 1, 2, 3, 4, 5, 6, 7
{95556, 85131, 75843, 71586, 63776, 56818, 50619, // C5 Major
 47778, 42566, 37921, 35793, 31888, 28409, 25309, // C6 Major
 23889, 21283, 18961, 17897, 15944, 14205, 12655};// C7 Major

// index definition for tones used in happy birthday.
#define G6 4+7
#define A6 5+7
#define B6 6+7
#define C7 0+14
#define D7 1+14
#define E7 2+14
#define F7 3+14
#define G7 4+14
 
// note table for Happy Birthday
// doe ray mi fa so la ti 
// C   D   E  F  G  A  B
NTyp happybirthday[] = 
{
// so   so   la   so   doe' ti
   G6,2,G6,2,A6,4,G6,4,C7,4,B6,4,
// pause so   so   la   so   ray' doe'
   0,4,  G6,2,G6,2,A6,4,G6,4,D7,4,C7,4,
// pause so   so   so'  mi'  doe' ti   la
   0, 4, G6,2,G6,2,G7,4,E7,4,C7,4,B6,4,A6,8, 
// pause fa'  fa'   mi'  doe' ray' doe'  stop
	 0,4,  F7,2,F7,2, E7,4,C7,4,D7,4,C7,8, 0,0
};

void play_a_song(NTyp notetab[]);
void Delay(void);
void PortF_Init(void);

#define C4_K 95420 // 262 Hz
#define D4_K 85034 // 294 Hz
#define E4_K 75758 // 330 Hz
#define F4_K 71633 // 349 Hz
#define G4_K 63776 // 392 Hz
#define A4_K 56818 // 440 Hz
#define B4_K 50607 // 494 Hz

// basic functions defined at end of startup.s
extern void DisableInterrupts(void); // Disable interrupts
extern void EnableInterrupts(void);  // Enable interrupts
extern void WaitForInterrupt(void);  // low power mode

unsigned long input_D, input_C, previous, previous2; // Inputs for PortD and PortC inputs
unsigned char music_flag = 0;												 // music ON/OFF 
unsigned char next_flag =0;													 // play next mode keyboard/auto-play
unsigned char keyboard_flag = 0;										 // keyboard flag to be in keyboard mode 
unsigned char play_flag = 0;												 // play flag to be in auto-play mode

int main(void){     
	PLL_Init();		// PLL initialization for 50MHz
	PortF_Init(); // Port F initialization for onboard switches and LEDs
  DAC_Init();   // Port B initialization for 6 bit DAC
	PortD_Init();	// Port D initialization for inputs PD3-0
	PortC_Init(); // Port C initialization for inputs PC6-4
	
	// output is music is on/off
	if(music_flag==1) 				GPIO_PORTF_DATA_R = 0x08; // green LED is ON
	else if(music_flag ==0)		GPIO_PORTF_DATA_R = 0x02; // red LED is OFF
  
  while(1){ 
		if(music_flag){ // if ON
		
			if(keyboard_flag){   // if in keyboard mode
				keyboard_flag = 0; // set keyboard_flag back to 0
				next_flag = 0; 		 // set next_flag to 0
				
				input_D = Switch_In_D()&0x0F; // inputs for portD
				input_C = Switch_In_C()&0x70; // inputs for portC
				
				if((input_D&&(previous==0)) || (input_C && (previous2 ==0))){ // just pressed									     
					EnableInterrupts();		
					if(Switch_In_D()&0x01){Sound_Init(C4_K/64);}      // C4
					else if(Switch_In_D()&0x02){Sound_Init(D4_K/64);} // D4
					else if(Switch_In_D()&0x04){Sound_Init(E4_K/64);} // E4
					else if(Switch_In_D()&0x08){Sound_Init(F4_K/64);} // F4
					else if(Switch_In_C()&0x10){Sound_Init(G4_K/64);} // G4
					else if(Switch_In_C()&0x20){Sound_Init(A4_K/64);} // A4
					else if(Switch_In_C()&0x40){Sound_Init(B4_K/64);} // B4
				}
									
				if((previous&&(input_D==0))|| (previous2&&input_C==0)){ // just released 
					DisableInterrupts();    // stop sound
				}
				
				previous = input_D;  // previous state for inputs D
				previous2 = input_C; // previous state for inputs C
				Delay10ms();  			 // remove switch bounce
											
				if(next_flag == 1) play_flag = 1;			// if next_flag is active, goto auto-play mode
				else 						 	 keyboard_flag = 1; // else stay at keyboard mode
			
			} // end keyboard flag
			else if(play_flag){
				EnableInterrupts();
				play_flag = 0; // set play_flag back to 0
				next_flag = 0; // set next_flag back to 0
				play_a_song(happybirthday); // play happy birthday
				
				if(next_flag == 1) keyboard_flag = 1;	// if next_flag is active, goto keyboard mode
				else 							 play_flag = 1;			// else stay at auto-play mode
			} //end play_flag		
		} // end music flag
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
  GPIO_PORTF_IBE_R &= ~0x11;    			// PF4 & PF0 are not both edges
  GPIO_PORTF_IEV_R &= ~0x11;    		// PF4 & PF0 falling edge event
  GPIO_PORTF_ICR_R = 0x11;      		// clear flag4
  GPIO_PORTF_IM_R |= 0x11;      		// arm interrupt on PF4 & PF0
  NVIC_PRI7_R |= (NVIC_PRI7_R&0xFF00FFFF)|0x00A00000; // priority 5
  NVIC_EN0_R |= 0x40000000;      		// enable interrupt 30 in NVIC
}

// PortF Handler for the switches
void GPIOPortF_Handler(void){
// SW1: Turn ON/OFF
// SW2: Switch mode
	
  if(GPIO_PORTF_RIS_R&0x01){          // SW2 touched 
    GPIO_PORTF_ICR_R = 0x01;          // acknowledge flag0
		next_flag = 1;										// set next_flag to 1
	}
  if(GPIO_PORTF_RIS_R&0x10){          // SW1 touched
    GPIO_PORTF_ICR_R = 0x10;          // acknowledge flag4
		music_flag ^= 1;									// toggle music_flag
		keyboard_flag = 1;								// initially keyboard mode should be set
		play_flag= 0;											// auto-play mode should not initially start
	}
	
	// output is music is on/off
	if(music_flag==1) 				GPIO_PORTF_DATA_R = 0x08; // green LED is ON
	else if(music_flag ==0)		GPIO_PORTF_DATA_R = 0x02; // red LED is OFF
}

// This function will play a song
// Inputs: note table for happy birthday song
void play_a_song(NTyp notetab[])
{
	unsigned char i=0, j;

	while (notetab[i].delay) {
		if(!music_flag || next_flag) break;	// escape the function if music_flag == 0 or next_flag == 1
		if (!notetab[i].tone_index) 
			Sound_stop(); // silence tone, turn off SysTick timer
		else {
			Sound_Init(tonetab[notetab[i].tone_index]/NUM_VALs);
		}
		
		// tempo control: play current note for specified duration
		for (j=0;j<notetab[i].delay;j++){
			if(!music_flag || next_flag) break; // escape the function if music_flag == 0 or next_flag == 1
			Delay();
		}
		
		Sound_stop();
		i++;  // move to the next note
	}
	
	// pause after each play
	for (j=0;j<15;j++) 
		Delay();
}

// Subroutine to wait 0.01 sec
// Inputs: None
// Outputs: None
void Delay(void){
	unsigned long volatile time;
	time = 3.125*727240*20/91;  // 0.01sec
  while(time){
		time--;
  }
}