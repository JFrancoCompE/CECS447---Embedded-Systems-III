// Lab1.c
// Runs on TM4C123
// Uses ST7735.c LCD.
// Jonathan Valvano
// August 5, 2015
// Possible main program to test the lab
// Feel free to edit this to match your specifications

// Backlight (pin 10) connected to +3.3 V
// MISO (pin 9) unconnected 
// SCK (pin 8) connected to PA2 (SSI0Clk)
// MOSI (pin 7) connected to PA5 (SSI0Tx)
// TFT_CS (pin 6) connected to PA3 (SSI0Fss)
// CARD_CS (pin 5) unconnected
// Data/Command (pin 4) connected to PA6 (GPIO)
// RESET (pin 3) connected to PA7 (GPIO)
// VCC (pin 2) connected to +3.3 V
// Gnd (pin 1) connected to ground

// Min: My LCD pinouts
// LED: Backlight (pin 8) connected to +3.3 V
// SCK (pin 7) connected to PA2 (SSI0Clk)
// SDA: MOSI (pin 6) connected to PA5 (SSI0Tx)
// A0:Data/Command (pin 5) connected to PA6 (GPIO)
// RESET (pin 4) connected to PA7 (GPIO)
// CS (pin 3) connected to PA3 (SSI0Fss)
// Gnd (pin 2) connected to ground
// VCC (pin 1) connected to +3.3 V

#include <stdio.h>
#include <stdint.h>
#include "string.h"
#include "ST7735.h"
#include "PLL.h"
#include "walking.h"
#include "tm4c123gh6pm.h"
void DelayWait10ms(uint32_t n);
// const will place these structures in ROM


int main(void){
	uint32_t x = 40, ht = 0;	
	int32_t l = 15, h = 15;
	
  PLL_Init(Bus40MHz);
//  PLL_Init(Bus80MHz);
	
  ST7735_InitR(INITR_REDTAB);
	
	// DRAW THE GROUND
	ST7735_FillScreen(ST7735_CYAN);
	ST7735_FillRect(0, 150,128, 30, ST7735_GREEN);
	  
	// DRAW THE SUN
	ST7735_FillCircle(100, 30, 15,ST7735_YELLOW);
	
  while(1){
		
  // DRAW THE MAN
	ST7735_XYPlotMan( x, l, h, ST7735_RED);
	
  DelayWait10ms(10);
		
	// REMOVE THE MAN
	ST7735_XYPlotMan( x, l, h, ST7735_CYAN);
 
	// MOVE THE COORDINATES OF THE MAN
	x++;
 
	// CODE for MOVING the LEG
	l--;
	if (l == -15)
	    l = 15;
 
	// CODE for MOVING the HAND    
	if (ht == 1)
	    h++;
	else
	    h--;
 
	if (h == 15)
	   ht = 0;
	else if (h == -15)   
		ht = 1;
 
	//Reset parameters
	  if(x>170){
			x = 40, ht = 0,l = 15, h = 15;
    }
	}

  
} 


// Subroutine to wait 10 msec
// Inputs: None
// Outputs: None
// Notes: ...
void DelayWait10ms(uint32_t n){uint32_t volatile time;
  while(n){
//    time = 727240*2/91;  // 10msec for launchpad
    time = 2724/91;  // for simulation: 27240*2
    while(time){
	  	time--;
    }
    n--;
  }
}
