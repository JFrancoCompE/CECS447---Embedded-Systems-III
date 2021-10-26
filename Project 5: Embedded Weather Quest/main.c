// Name: Jesus Franco
// Student ID: 014046368
// Course Number: CECS 447
// Assignment: Project 5: An Embedded Weather Quest
// Description: This code will allow the use Johnathan Valvano's CC3100GetWeather_4C123 project
//							to be tweaked to create the Embedded Weather Quest. This code will allow the
//							user to manually input the city name, city ID, geographic coordinates, or zip
//							code to get location information such as temperatures, humidity, and weather
//							states. The information will be displayed neatly on the ST7335R LCD display for
//							the top half of the display and will display the current weather state animation
//							for the bottom half of the display. If either onboard launchpad switches are
//							pressed, the program will cycle all over again and await new manual inputs.

// ST7735R LCD connections
//
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

/* CREDIT IS GIVEN BELOW TO ORIGINAL CREATORS */

/*
 * main.c - Example project for UT.6.02x Embedded Systems - Shape the World
 * Jonathan Valvano and Ramesh Yerraballi
 * October 27, 2015
 * Hardware requirements 
     TM4C123 LaunchPad, optional Nokia5110
     CC3100 wifi booster and 
     an internet access point with OPEN, WPA, or WEP security
 
 * derived from TI's getweather example
 * Copyright (C) 2014 Texas Instruments Incorporated - http://www.ti.com/
 *
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *
 *    Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 *    Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the
 *    distribution.
 *
 *    Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 *  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 *  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 *  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 *  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 *  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 *  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
*/

/*
 * Application Name     -   Get weather
 * Application Overview -   This is a sample application demonstrating how to
                            connect to openweathermap.org server and request for
              weather details of a city.
 * Application Details  -   http://processors.wiki.ti.com/index.php/CC31xx_SLS_Get_Weather_Application
 *                          doc\examples\sls_get_weather.pdf
 */
 /* CC3100 booster pack connections (unused pins can be used by user application)
Pin  Signal        Direction      Pin   Signal     Direction
P1.1  3.3 VCC         IN          P2.1  Gnd   GND      IN
P1.2  PB5 UNUSED      NA          P2.2  PB2   IRQ      OUT
P1.3  PB0 UART1_TX    OUT         P2.3  PE0   SSI2_CS  IN
P1.4  PB1 UART1_RX    IN          P2.4  PF0   UNUSED   NA
P1.5  PE4 nHIB        IN          P2.5  Reset nRESET   IN
P1.6  PE5 UNUSED      NA          P2.6  PB7  SSI2_MOSI IN
P1.7  PB4 SSI2_CLK    IN          P2.7  PB6  SSI2_MISO OUT
P1.8  PA5 UNUSED      NA          P2.8  PA4   UNUSED   NA
P1.9  PA6 UNUSED      NA          P2.9  PA3   UNUSED   NA
P1.10 PA7 UNUSED      NA          P2.10 PA2   UNUSED   NA

Pin  Signal        Direction      Pin   Signal      Direction
P3.1  +5  +5 V       IN           P4.1  PF2 UNUSED      OUT
P3.2  Gnd GND        IN           P4.2  PF3 UNUSED      OUT
P3.3  PD0 UNUSED     NA           P4.3  PB3 UNUSED      NA
P3.4  PD1 UNUSED     NA           P4.4  PC4 UART1_CTS   IN
P3.5  PD2 UNUSED     NA           P4.5  PC5 UART1_RTS   OUT
P3.6  PD3 UNUSED     NA           P4.6  PC6 UNUSED      NA
P3.7  PE1 UNUSED     NA           P4.7  PC7 NWP_LOG_TX  OUT
P3.8  PE2 UNUSED     NA           P4.8  PD6 WLAN_LOG_TX OUT
P3.9  PE3 UNUSED     NA           P4.9  PD7 UNUSED      IN (see R74)
P3.10 PF1 UNUSED     NA           P4.10 PF4 UNUSED      OUT(see R75)

UART0 (PA1, PA0) sends data to the PC via the USB debug cable, 115200 baud rate
Port A, SSI0 (PA2, PA3, PA5, PA6, PA7) sends data to Nokia5110 LCD

*/
#include "cc3100\simplelink\include\simplelink.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/debug.h"
#include "driverlib/fpu.h"
#include "driverlib/gpio.h"
#include "driverlib/pin_map.h"
#include "driverlib/rom.h"
#include "driverlib/sysctl.h"
#include "driverlib/uart.h"
#include "utils/uartstdio.h"
#include "utils/cmdline.h"
#include "application_commands.h"
#include "LED.h"
#include <string.h>
#include "tm4c123gh6pm.h"
#include "ST7735.h"

// To Do: replace the following three lines with your access point information
#define SSID_NAME  "BringMeFood" /* Access point name to connect to */
#define SEC_TYPE   SL_SEC_TYPE_WPA
#define PASSKEY    "ILOVEJESUSFRANCO"  /* Password in case of secure AP */ 
#define BAUD_RATE   115200

void UART_Init(void){
  SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);
  SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
  GPIOPinConfigure(GPIO_PA0_U0RX);
  GPIOPinConfigure(GPIO_PA1_U0TX);
  GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);
  UARTStdioConfig(0,BAUD_RATE,50000000);
}

#define MAX_RECV_BUFF_SIZE  1024
#define MAX_SEND_BUFF_SIZE  512
#define MAX_HOSTNAME_SIZE   40
#define MAX_PASSKEY_SIZE    32
#define MAX_SSID_SIZE       32
#define SUCCESS             0
#define CONNECTION_STATUS_BIT   0
#define IP_AQUIRED_STATUS_BIT   1

/* Application specific status/error codes */
typedef enum{
    DEVICE_NOT_IN_STATION_MODE = -0x7D0,/* Choosing this number to avoid overlap w/ host-driver's error codes */

    STATUS_CODE_MAX = -0xBB8
}e_AppStatusCodes;

/* Status bits - These are used to set/reset the corresponding bits in 'g_Status' */
typedef enum{
    STATUS_BIT_CONNECTION =  0, /* If this bit is:
                                 *      1 in 'g_Status', the device is connected to the AP
                                 *      0 in 'g_Status', the device is not connected to the AP
                                 */

    STATUS_BIT_IP_AQUIRED,       /* If this bit is:
                                 *      1 in 'g_Status', the device has acquired an IP
                                 *      0 in 'g_Status', the device has not acquired an IP
                                 */

}e_StatusBits;

#define SET_STATUS_BIT(status_variable, bit)    status_variable |= (1<<(bit))
#define CLR_STATUS_BIT(status_variable, bit)    status_variable &= ~(1<<(bit))
#define GET_STATUS_BIT(status_variable, bit)    (0 != (status_variable & (1<<(bit))))
#define IS_CONNECTED(status_variable)           GET_STATUS_BIT(status_variable, \
                                                               STATUS_BIT_CONNECTION)
#define IS_IP_AQUIRED(status_variable)          GET_STATUS_BIT(status_variable, \
                                                               STATUS_BIT_IP_AQUIRED)
typedef struct{
    UINT8 SSID[MAX_SSID_SIZE];
    INT32 encryption;
    UINT8 password[MAX_PASSKEY_SIZE];
}UserInfo;

/*
 * GLOBAL VARIABLES -- Start
 */
char Recvbuff[MAX_RECV_BUFF_SIZE];
char SendBuff[MAX_SEND_BUFF_SIZE];
char HostName[MAX_HOSTNAME_SIZE];
unsigned long DestinationIP;
int SockID;

typedef enum{
    CONNECTED = 0x01,
    IP_AQUIRED = 0x02,
    IP_LEASED = 0x04,
    PING_DONE = 0x08
}e_Status;
UINT32  g_Status = 0;
/*
 * GLOBAL VARIABLES -- End
 */


 /*
 * STATIC FUNCTION DEFINITIONS  -- Start
 */

static int32_t configureSimpleLinkToDefaultState(char *);


/*
 * STATIC FUNCTION DEFINITIONS -- End
 */


void Crash(uint32_t time){
  while(1){
    for(int i=time;i;i--){};
    LED_RedToggle();
  }
}

void SysTick_Init(void);	// initialization for SysTick
void SysTick_Wait10ms(unsigned long delay); // 10ms delay assume 50MHz clock

#define MAXLEN 100		// Max length for parsed string arrays
char City[MAXLEN];		// Array for city name
char Temp[MAXLEN];		// Array for current temperature
char MinTemp[MAXLEN]; // Array for minimum temperature
char MaxTemp[MAXLEN]; // Array for maximum temperature
char Humidity[MAXLEN];// Array for humidity
char Weather[MAXLEN]; // Array for main weather

char cloud_flag = 0; // Flag for LCD animated cloud image
char clear_flag = 0; // Flag for LCD animated clear sky image
char rain_flag = 0;  // Flag for LCD animated rain image

unsigned long counter_clear = 1; // Counter for clear sky image
unsigned long counter_cloud = 1; // Counter for cloud image
unsigned long counter_rain = 1;  // Counter for rain image

void ST7735_Main_Plot(); // Main LCD static plot

// This function will plot a clear sky animation for when the 
// weather is in a clear sky state. This animation consists of
// of flares coming from the sun to be moving continously.
void ST7735_Clear_Plot(){	
	switch(counter_clear){
		case 1: // Background picture with sun
			ST7735_FillRect(0, 60,128, 100, ST7735_CYAN);
			ST7735_FillCircle(64, 100, 25, ST7735_YELLOW);
			ST7735_DrawCircle(64, 100, 25, ST7735_BLACK);
		case 2: // Starting here until below will be yellow lines representing sun rays
			ST7735_DrawLine(40, 80, 90, 120, ST7735_YELLOW);
			SysTick_Wait10ms(10); // 10ms * 10 = 100ms delay
			counter_clear++;
			break;
		case 3: 
			ST7735_DrawLine(40, 120, 90, 80, ST7735_YELLOW);
			SysTick_Wait10ms(10);
			counter_clear++;
			break;
		case 4:	
			ST7735_DrawLine(35, 100,95 ,100, ST7735_YELLOW);
			SysTick_Wait10ms(10);
			counter_clear++;
			break;
		case 5: 
			ST7735_DrawLine(65, 70,65 ,130, ST7735_YELLOW);
			SysTick_Wait10ms(10);
			counter_clear++;
			break;
		case 6:
			ST7735_DrawLine(50, 73 ,80, 130, ST7735_YELLOW);
			SysTick_Wait10ms(10);
			counter_clear++;
			break;
		case 7:
			ST7735_DrawLine(50, 130 ,80, 73, ST7735_YELLOW);
			SysTick_Wait10ms(10);
			counter_clear++;
			break;
		case 8:
			ST7735_DrawLine(30, 110 ,100, 90, ST7735_YELLOW);
			SysTick_Wait10ms(10);
			counter_clear++;
			break;
		case 9:
			ST7735_DrawLine(30, 90 ,100, 110, ST7735_YELLOW);
			SysTick_Wait10ms(10);
			counter_clear++;
			break;
		case 10: // Reset the counter
			counter_clear = 1;
			break;
	}// end switch
}// end function

// This function will plot a cloudy sky animation for when the 
// weather is in the cloudy state. This animation consists of
// of multiple circles which are expanding to slowly create a bigger cloud
void ST7735_Cloud_Plot(){
	switch(counter_cloud){
		case 1: // Random circles to create a bigger cloud over time
			ST7735_FillCircle(65, 100, 15, ST7735_WHITE);
			SysTick_Wait10ms(25);
			counter_cloud++;
			break;
		case 2:
			ST7735_FillCircle(55, 105, 15, ST7735_WHITE);
			SysTick_Wait10ms(25);
			counter_cloud++;
			break;
		case 3:
			ST7735_FillCircle(75, 105, 15, ST7735_WHITE);
			SysTick_Wait10ms(25);
			counter_cloud++;
			break;
		case 4:
			ST7735_FillCircle(45, 110, 15, ST7735_WHITE);
			SysTick_Wait10ms(25);
			counter_cloud++;
			break;
		case 5:
			ST7735_FillCircle(85, 95, 15, ST7735_WHITE);
			SysTick_Wait10ms(25);
			counter_cloud++;
			break;
		case 6:
			ST7735_FillCircle(35, 95, 15, ST7735_WHITE);
			SysTick_Wait10ms(25);
			counter_cloud++;
			break;
		case 7:
			ST7735_FillCircle(95, 110, 15, ST7735_WHITE);
			SysTick_Wait10ms(25);
			counter_cloud++;
			break;
		case 8:	
			ST7735_FillCircle(65, 85, 20, ST7735_WHITE);
			SysTick_Wait10ms(25);
			counter_cloud++;
			break;
		case 9:		
			ST7735_FillCircle(30, 115, 15, ST7735_WHITE);
			SysTick_Wait10ms(25);
			counter_cloud++;
			break;
		case 10:
			ST7735_FillCircle(65, 115, 20, ST7735_WHITE);
			SysTick_Wait10ms(25);
			counter_cloud++;
			break;
		case 11:
			ST7735_FillCircle(100, 95, 15, ST7735_WHITE);
			SysTick_Wait10ms(25);
			counter_cloud++;
			break;
		case 12: // Reset the counter
			ST7735_FillRect(0, 60,128, 100, ST7735_CYAN);
			counter_cloud = 1;
			break;
	}// end switch
}// end funciton

void ST7735_Rain_Plot(){
	switch(counter_rain){
		case 1:// Background picture with giant cloud
			ST7735_FillCircle(65, 87, 20, ST7735_WHITE);
			ST7735_FillCircle(45, 100, 20, ST7735_WHITE);
			ST7735_FillCircle(85, 100, 20, ST7735_WHITE);
			ST7735_FillCircle(65, 105, 20, ST7735_WHITE);
			counter_rain++;
			break;
		case 2:// Onward below are small circles to animate raindrops
			ST7735_FillCircle(65, 135, 3, ST7735_BLUE); // middle 1 B
			ST7735_FillCircle(40, 125, 3, ST7735_BLUE); // far left 1 B
			ST7735_FillCircle(90, 125, 3, ST7735_BLUE); // far right 1 B
			counter_rain++;
			SysTick_Wait10ms(5);
			break;
		case 3:
			ST7735_FillCircle(65, 135, 3, ST7735_CYAN); // middle 1 C
			ST7735_FillCircle(40, 125, 3, ST7735_CYAN); // far left 1 C
			ST7735_FillCircle(90, 125, 3, ST7735_CYAN); // far right 1 C
			ST7735_FillCircle(52, 130, 3, ST7735_BLUE); // left 1 B
			ST7735_FillCircle(78, 130, 3, ST7735_BLUE); // right 1 B
			counter_rain++;
			SysTick_Wait10ms(5);
			break;
		case 4:
			ST7735_FillCircle(52, 130, 3, ST7735_CYAN); // left 1 C
			ST7735_FillCircle(78, 130, 3, ST7735_CYAN); // right 1 C
			ST7735_FillCircle(65, 145, 3, ST7735_BLUE); // middle 2 B
			ST7735_FillCircle(40, 135, 3, ST7735_BLUE); // far left 2 B
			ST7735_FillCircle(90, 135, 3, ST7735_BLUE); // far right 2 B
			counter_rain++;
			SysTick_Wait10ms(5);
			break;
		case 5:
			ST7735_FillCircle(65, 145, 3, ST7735_CYAN); // middle 2 C
			ST7735_FillCircle(40, 135, 3, ST7735_CYAN); // far left 2 C
			ST7735_FillCircle(90, 135, 3, ST7735_CYAN); // far right 2 C
			ST7735_FillCircle(62, 130, 3, ST7735_BLUE); // left 2 B
			ST7735_FillCircle(88, 130, 3, ST7735_BLUE); // right 2 B
			counter_rain++;
			SysTick_Wait10ms(5);
			break;
		case 6:
			ST7735_FillCircle(62, 130, 3, ST7735_CYAN); // left 2 C
			ST7735_FillCircle(88, 130, 3, ST7735_CYAN); // right 2 C
			ST7735_FillCircle(65, 155, 3, ST7735_BLUE); // middle 3 B
			ST7735_FillCircle(40, 145, 3, ST7735_BLUE); // far left 3 B
			ST7735_FillCircle(90, 145, 3, ST7735_BLUE); // far right 3 B
			counter_rain++;
			SysTick_Wait10ms(5);
			break;
		case 7:
			ST7735_FillCircle(65, 155, 3, ST7735_CYAN); // middle 3 C
			ST7735_FillCircle(40, 145, 3, ST7735_CYAN); // far left 3 C
			ST7735_FillCircle(90, 145, 3, ST7735_CYAN); // far right 3 C
			ST7735_FillCircle(72, 130, 3, ST7735_BLUE); // left 3 B
			ST7735_FillCircle(98, 130, 3, ST7735_BLUE); // right 3 B
			counter_rain++;
			SysTick_Wait10ms(5);
			break;
		case 8:
			ST7735_FillCircle(72, 130, 3, ST7735_CYAN); // left 3 C
			ST7735_FillCircle(98, 130, 3, ST7735_CYAN); // right 3 C
			counter_rain++;
			SysTick_Wait10ms(5);
			break;
		case 9:// Reset counter
			counter_rain = 1;
			SysTick_Wait10ms(1);
			break;
	}// end switch
}// end function

// This function will output all the necessary requirements onto the
// serial terminal and onto the LCD
void Outputs(){
	uint32_t i;
  char *pt = NULL;
	
/* find ticker name in response*/
	pt = strstr(Recvbuff, "\"name\"");
	i = 0; 
	if( NULL != pt ){
		pt = pt + 8; // skip over "name":"
		while((i<MAXLEN)&&(*pt)&&(*pt!='\"')){
			City[i] = *pt; // copy into City string
			pt++; i++;    
		}
	}
	City[i] = 0;
	UARTprintf("City: ");
	UARTprintf(City);
	UARTprintf("\n");
	ST7735_DrawString(5, 0, City ,ST7735_YELLOW);
	
/* find Temperature Value in response */
	pt = strstr(Recvbuff, "\"temp\"");
	i = 0; 
	if( NULL != pt ){
		pt = pt + 7; // skip over "temp":
		while((i<MAXLEN)&&(*pt)&&(*pt!='\"')){
			Temp[i] = *pt; // copy into Temp string
			pt++; i++;    
		}
	}
	Temp[i] = 0;
	Temp[strlen(Temp)-1] = '\0';
	UARTprintf("Temp: ");
	UARTprintf(Temp);
	UARTprintf(" F\n");
	ST7735_DrawString(13, 1, Temp ,ST7735_YELLOW);
	ST7735_DrawString(13 + strlen(Temp), 1, " F",ST7735_YELLOW);
	
/* find Min Temperature Value in response */
	pt = strstr(Recvbuff, "\"temp_min\"");
	i = 0; 
	if( NULL != pt ){
		pt = pt + 11; // skip over "temp_min":
		while((i<MAXLEN)&&(*pt)&&(*pt!='\"')){
			MinTemp[i] = *pt; // copy into MinTemp string
			pt++; i++;    
		}
	}
	MinTemp[i] = 0;	
	MinTemp[strlen(MinTemp)-1] = '\0';
	UARTprintf("Min Temp: ");
	UARTprintf(MinTemp);
	UARTprintf(" F\n");
	ST7735_DrawString(9, 2, MinTemp ,ST7735_YELLOW);
	ST7735_DrawString(9 + strlen(MinTemp), 2, " F",ST7735_YELLOW);

/* find Max Temperature Value in response */
	pt = strstr(Recvbuff, "\"temp_max\"");
	i = 0; 
	if( NULL != pt ){
		pt = pt + 11; // skip over "temp_max":
		while((i<MAXLEN)&&(*pt)&&(*pt!='\"')){
			MaxTemp[i] = *pt; // copy into MaxTemp string
			pt++; i++;    
		}
	}
	MaxTemp[i] = 0;	
	MaxTemp[strlen(MaxTemp)-1] = '\0';
	UARTprintf("Max Temp: ");
	UARTprintf(MaxTemp);
	UARTprintf(" F\n");
	ST7735_DrawString(9, 3, MaxTemp ,ST7735_YELLOW);
	ST7735_DrawString(9 + strlen(MaxTemp), 3, " F",ST7735_YELLOW);	
	
/* find Humidity Temperature Value in response */
	pt = strstr(Recvbuff, "\"humidity\"");
	i = 0; 
	if( NULL != pt ){
		pt = pt + 11; // skip over "humidity":
		while((i<MAXLEN)&&(*pt)&&(*pt!='\"')){
			Humidity[i] = *pt; // copy into Humidity string
			pt++; i++;    
		}
	}
	Humidity[i] = 0;
	Humidity[strlen(Humidity)-1] = '\0';
	Humidity[strlen(Humidity)-1] = '\0';
	UARTprintf("Humidity: ");
	UARTprintf(Humidity);
	UARTprintf("\n");
	ST7735_DrawString(9, 4, Humidity ,ST7735_YELLOW);	
	
/* find weather in response */
	pt = strstr(Recvbuff, "\"main\"");
	i = 0; 
	if( NULL != pt ){
		pt = pt + 8; // skip over "main":"
		while((i<MAXLEN)&&(*pt)&&(*pt!='\"')){
			Weather[i] = *pt; // copy into Weather string
			pt++; i++;    
		}
	}
	Weather[i] = 0;
	UARTprintf("Weather: ");
	UARTprintf(Weather);
	UARTprintf("\n");
	ST7735_DrawString(8, 5, Weather ,ST7735_YELLOW);
	
	// Set up flags to determine if the weather is in the
	// clear sky state, the cloud state, or the rain state
	if(Weather[0] == 'C'){
		if(Weather[1] == 'l'){
			if(Weather[2] == 'o'){
				cloud_flag = 1;
			}
			else if(Weather[2] == 'e'){
				clear_flag = 1;
			}
		}
	}
	else if(Weather[0] == 'R'){
		rain_flag = 1;
	}
}


/*
 * Application's entry point
 */
// 1) change Austin Texas to your city
// 2) metric(for celsius), imperial(for fahrenheit)
// api.openweathermap.org/data/2.5/weather?q={city name},{state code}&appid={API key}
//#define REQUEST "GET /data/2.5/weather?q=Long Beach&APPID=7907b2abac2053aed180a74b9310b119&units=metric HTTP/1.1\r\nUser-Agent: Keil\r\nHost:api.openweathermap.org\r\nAccept: */*\r\n\r\n"

// API commands to find City, Zip Code, City ID, and Geographic Coordinates
char REQUEST_CITY [200] = "GET /data/2.5/weather?q=";
char REQUEST_ZIP [200] = "GET /data/2.5/weather?zip=";
char REQUEST_CITY_ID [200] = "GET /data/2.5/weather?id=";
char REQUEST_GEO [200] = "GET /data/2.5/weather?lat=";
// My APPID 
char APPID [200] = "&APPID=861009ad5a8d2979ccb31161a0004860&units=imperial HTTP/1.1\r\nUser-Agent: Keil\r\nHost:api.openweathermap.org\r\nAccept: */*\r\n\r\n";
// Tempory string to use to combine the full REQUEST command
char TEMP [200];

// 1) go to http://openweathermap.org/appid#use 
// 2) Register on the Sign up page
// 3) get an API key (APPID) replace the 7907b2abac2053aed180a74b9310b119 with your APPID
int main(void){int32_t retVal;  SlSecParams_t secParams;
	// String arrays for prompted manual input
	char string_city[20], string_city_id[20], string_lat[20], string_lon[20], string_zip[20];
	
  char *pConfig = NULL; INT32 ASize = 0; SlSockAddrIn_t  Addr;
  initClk();        // PLL 50 MHz
  UART_Init();      // Send data to PC, 115200 bps
  LED_Init();       // Initialize LaunchPad I/O 
	ST7735_InitR(INITR_REDTAB); // Initialization for ST7735R LCD 
	SysTick_Init(); // Initialization for SysTick
	
  UARTprintf("Weather App\n");
  retVal = configureSimpleLinkToDefaultState(pConfig); // set policies
  if(retVal < 0)Crash(4000000);
  retVal = sl_Start(0, pConfig, 0);
  if((retVal < 0) || (ROLE_STA != retVal) ) Crash(8000000);
  secParams.Key = PASSKEY;
  secParams.KeyLen = strlen(PASSKEY);
  secParams.Type = SEC_TYPE; // OPEN, WPA, or WEP
  sl_WlanConnect(SSID_NAME, strlen(SSID_NAME), 0, &secParams, 0);
  while((0 == (g_Status&CONNECTED)) || (0 == (g_Status&IP_AQUIRED))){
    _SlNonOsMainLoopTask();
  }
  UARTprintf("Connected\n");
  while(1){
		ST7735_Main_Plot(); // Main static LCD Plot
		clear_flag = 0; // initially set clear sky flag off
		cloud_flag = 0; // intiially set cloud sky flag off
		rain_flag = 0;  // initially set rainy sky flag off
		
		// Menu options for the Serial Terminal
		UARTprintf("Welcome to my Embedded Weather Quester!\n");
		UARTprintf("Please choose your query criteria:\n");
		UARTprintf("	a. City Name\n");
		UARTprintf("	b. City ID\n");
		UARTprintf("	c. Geographic Coordinates\n");
		UARTprintf("	d. Zip Code\n");\
		// Awaiting for manual Char value from user
		switch(UARTgetc()){
			case 'a': // Set up API command for City Name
				UARTprintf("\nCity Name: ");
				UARTgets(string_city,20);
				strcat(TEMP, REQUEST_CITY);
				strcat(TEMP, string_city);
				strcat(TEMP, APPID);	
				break;
			case 'b':// Set up API command for City ID
				UARTprintf("\nCity ID: ");
				UARTgets(string_city_id,20);
				strcat(TEMP, REQUEST_CITY_ID);
				strcat(TEMP, string_city_id);
				strcat(TEMP, APPID);
				break;	
			case 'c':// Set up API command for Geographical Coordinates
				UARTprintf("\nLatitude: ");
				UARTgets(string_lat,20);
				UARTprintf("\nLongitude: ");
				UARTgets(string_lon,20);
				strcat(TEMP, REQUEST_GEO);
				strcat(TEMP, string_lat); //lat
				strcat(TEMP, "&lon=");
				strcat(TEMP, string_lon); //lon
				strcat(TEMP, APPID);
				UARTprintf("\n");
				UARTprintf(TEMP);
				UARTprintf("\n");
				break;			
			case 'd':// Set up API command for Zip Code
				UARTprintf("\nZIP Code: ");
				UARTgets(string_zip,20);
				strcat(TEMP, REQUEST_ZIP);
				strcat(TEMP, string_zip);
				strcat(TEMP, APPID);
				UARTprintf("\n");
				UARTprintf(TEMP);
				UARTprintf("\n");
				break;
		}// end switch
		
    strcpy(HostName,"api.openweathermap.org");
    retVal = sl_NetAppDnsGetHostByName(HostName,
             strlen(HostName),&DestinationIP, SL_AF_INET);
    if(retVal == 0){
      Addr.sin_family = SL_AF_INET;
      Addr.sin_port = sl_Htons(80);
      Addr.sin_addr.s_addr = sl_Htonl(DestinationIP);// IP to big endian 
      ASize = sizeof(SlSockAddrIn_t);
      SockID = sl_Socket(SL_AF_INET,SL_SOCK_STREAM, 0);
      if( SockID >= 0 ){
        retVal = sl_Connect(SockID, ( SlSockAddr_t *)&Addr, ASize);
      }
      if((SockID >= 0)&&(retVal >= 0)){
				// Set requested API command
        strcpy(SendBuff,TEMP);  
        sl_Send(SockID, SendBuff, strlen(SendBuff), 0);// Send the HTTP GET 
        sl_Recv(SockID, Recvbuff, MAX_RECV_BUFF_SIZE, 0);// Receive response 
        sl_Close(SockID);
        LED_GreenOn();
        UARTprintf("\r\n\r\n");
        UARTprintf(Recvbuff);  UARTprintf("\r\n");
      }
    }
		// Clear all manual input strings
		for(int j = 0; j<20; j++){
			string_city[j] = '\0';
			string_city_id[j] = '\0';
			string_lat[j] = '\0';
			string_lon[j] = '\0';
			string_zip[j] = '\0';
		}
		// Clear TEMP REQUEST string
		for(int j = 0; j<200; j++){
			TEMP[j] = '\0';
		}
		// Output all information on Serial Terminal and LCD
		Outputs();
		UARTprintf("\n");
		// While loop to be left hanging if SW1 or SW2 is not pressed
		// if SW1 or SW2 is pressed, the program will cycle all over again
		// for new manual inputs
    while(Board_Input()==0){
			if(clear_flag) ST7735_Clear_Plot();			// Display clear sky animation
			else if(cloud_flag) ST7735_Cloud_Plot();// Display cloud sky animation
			else if(rain_flag) ST7735_Rain_Plot();	// Display rain sky animation
		}; // wait for touch
    LED_GreenOff();
  }
}

// This function will display on the LCD screen a static image with the
// necessary requirements to display
void ST7735_Main_Plot(){	
	// Strings to display on the LCD
	char LCD_city[20] = "City: ";
	char LCD_temp[20] = "Current Temp: ";
	char LCD_min_temp[20] = "Min Temp: ";
	char LCD_max_temp[20] = "Max Temp: ";
	char LCD_humidity[20] = "Humidity: ";
	char LCD_weather[20] = "Weather: ";
	// Objects to display and place on the LCD
	ST7735_FillScreen(ST7735_BLACK);
	ST7735_FillRect(0, 60,128, 100, ST7735_CYAN);
	ST7735_DrawString(0,0, LCD_city, ST7735_YELLOW);
	ST7735_DrawString(0,1, LCD_temp, ST7735_YELLOW);
	ST7735_DrawString(0,2, LCD_min_temp, ST7735_YELLOW);
	ST7735_DrawString(0,3, LCD_max_temp, ST7735_YELLOW);
	ST7735_DrawString(0,4, LCD_humidity, ST7735_YELLOW);
	ST7735_DrawString(0,5, LCD_weather, ST7735_YELLOW);
}

// Initialize SysTick with busy wait running at bus clock.
void SysTick_Init(void){
  NVIC_ST_CTRL_R = 0;                   // disable SysTick during setup
  NVIC_ST_RELOAD_R = NVIC_ST_RELOAD_M;  // maximum reload value
  NVIC_ST_CURRENT_R = 0;                // any write to current clears it
                                        // enable SysTick with core clock
  NVIC_ST_CTRL_R = NVIC_ST_CTRL_ENABLE+NVIC_ST_CTRL_CLK_SRC;
}

// Time delay using busy wait.
// The delay parameter is in units of the core clock.
void SysTick_Wait(unsigned long delay){
  volatile unsigned long elapsedTime;
  unsigned long startTime = NVIC_ST_CURRENT_R;
  do{
    elapsedTime = (startTime-NVIC_ST_CURRENT_R)&0x00FFFFFF;
  }
  while(elapsedTime <= delay);
}

// Time delay using busy wait.
// This assumes 50 MHz system clock.
void SysTick_Wait10ms(unsigned long delay){
  unsigned long i;
  for(i=0; i<delay; i++){
    SysTick_Wait(500000);  // wait 10ms (assumes  MHz clock)
  }
}

/*!
    \brief This function puts the device in its default state. It:
           - Set the mode to STATION
           - Configures connection policy to Auto and AutoSmartConfig
           - Deletes all the stored profiles
           - Enables DHCP
           - Disables Scan policy
           - Sets Tx power to maximum
           - Sets power policy to normal
           - Unregister mDNS services

    \param[in]      none

    \return         On success, zero is returned. On error, negative is returned
*/
static int32_t configureSimpleLinkToDefaultState(char *pConfig){
  SlVersionFull   ver = {0};
  UINT8           val = 1;
  UINT8           configOpt = 0;
  UINT8           configLen = 0;
  UINT8           power = 0;

  INT32           retVal = -1;
  INT32           mode = -1;

  mode = sl_Start(0, pConfig, 0);


    /* If the device is not in station-mode, try putting it in station-mode */
  if (ROLE_STA != mode){
    if (ROLE_AP == mode){
            /* If the device is in AP mode, we need to wait for this event before doing anything */
      while(!IS_IP_AQUIRED(g_Status));
    }

        /* Switch to STA role and restart */
    retVal = sl_WlanSetMode(ROLE_STA);

    retVal = sl_Stop(0xFF);

    retVal = sl_Start(0, pConfig, 0);

        /* Check if the device is in station again */
    if (ROLE_STA != retVal){
            /* We don't want to proceed if the device is not coming up in station-mode */
      return DEVICE_NOT_IN_STATION_MODE;
    }
  }
    /* Get the device's version-information */
  configOpt = SL_DEVICE_GENERAL_VERSION;
  configLen = sizeof(ver);
  retVal = sl_DevGet(SL_DEVICE_GENERAL_CONFIGURATION, &configOpt, &configLen, (unsigned char *)(&ver));

    /* Set connection policy to Auto + SmartConfig (Device's default connection policy) */
  retVal = sl_WlanPolicySet(SL_POLICY_CONNECTION, SL_CONNECTION_POLICY(1, 0, 0, 0, 1), NULL, 0);

    /* Remove all profiles */
  retVal = sl_WlanProfileDel(0xFF);

    /*
     * Device in station-mode. Disconnect previous connection if any
     * The function returns 0 if 'Disconnected done', negative number if already disconnected
     * Wait for 'disconnection' event if 0 is returned, Ignore other return-codes
     */
  retVal = sl_WlanDisconnect();
  if(0 == retVal){
        /* Wait */
     while(IS_CONNECTED(g_Status));
  }

    /* Enable DHCP client*/
  retVal = sl_NetCfgSet(SL_IPV4_STA_P2P_CL_DHCP_ENABLE,1,1,&val);

    /* Disable scan */
  configOpt = SL_SCAN_POLICY(0);
  retVal = sl_WlanPolicySet(SL_POLICY_SCAN , configOpt, NULL, 0);

    /* Set Tx power level for station mode
       Number between 0-15, as dB offset from max power - 0 will set maximum power */
  power = 0;
  retVal = sl_WlanSet(SL_WLAN_CFG_GENERAL_PARAM_ID, WLAN_GENERAL_PARAM_OPT_STA_TX_POWER, 1, (unsigned char *)&power);

    /* Set PM policy to normal */
  retVal = sl_WlanPolicySet(SL_POLICY_PM , SL_NORMAL_POLICY, NULL, 0);

    /* TBD - Unregister mDNS services */
  retVal = sl_NetAppMDNSUnRegisterService(0, 0);


  retVal = sl_Stop(0xFF);


  g_Status = 0;
  memset(&Recvbuff,0,MAX_RECV_BUFF_SIZE);
  memset(&SendBuff,0,MAX_SEND_BUFF_SIZE);
  memset(&HostName,0,MAX_HOSTNAME_SIZE);
  DestinationIP = 0;;
  SockID = 0;


  return retVal; /* Success */
}




/*
 * * ASYNCHRONOUS EVENT HANDLERS -- Start
 */

/*!
    \brief This function handles WLAN events

    \param[in]      pWlanEvent is the event passed to the handler

    \return         None

    \note

    \warning
*/
void SimpleLinkWlanEventHandler(SlWlanEvent_t *pWlanEvent){
  switch(pWlanEvent->Event){
    case SL_WLAN_CONNECT_EVENT:
    {
      SET_STATUS_BIT(g_Status, STATUS_BIT_CONNECTION);

            /*
             * Information about the connected AP (like name, MAC etc) will be
             * available in 'sl_protocol_wlanConnectAsyncResponse_t' - Applications
             * can use it if required
             *
             * sl_protocol_wlanConnectAsyncResponse_t *pEventData = NULL;
             * pEventData = &pWlanEvent->EventData.STAandP2PModeWlanConnected;
             *
             */
    }
    break;

    case SL_WLAN_DISCONNECT_EVENT:
    {
      sl_protocol_wlanConnectAsyncResponse_t*  pEventData = NULL;

      CLR_STATUS_BIT(g_Status, STATUS_BIT_CONNECTION);
      CLR_STATUS_BIT(g_Status, STATUS_BIT_IP_AQUIRED);

      pEventData = &pWlanEvent->EventData.STAandP2PModeDisconnected;

            /* If the user has initiated 'Disconnect' request, 'reason_code' is SL_USER_INITIATED_DISCONNECTION */
      if(SL_USER_INITIATED_DISCONNECTION == pEventData->reason_code){
        UARTprintf(" Device disconnected from the AP on application's request \r\n");
      }
      else{
        UARTprintf(" Device disconnected from the AP on an ERROR..!! \r\n");
      }
    }
    break;

    default:
    {
      UARTprintf(" [WLAN EVENT] Unexpected event \r\n");
    }
    break;
  }
}

/*!
    \brief This function handles events for IP address acquisition via DHCP
           indication

    \param[in]      pNetAppEvent is the event passed to the handler

    \return         None

    \note

    \warning
*/
void SimpleLinkNetAppEventHandler(SlNetAppEvent_t *pNetAppEvent){
  switch(pNetAppEvent->Event)
  {
    case SL_NETAPP_IPV4_ACQUIRED:
    {

      SET_STATUS_BIT(g_Status, STATUS_BIT_IP_AQUIRED);
        /*
             * Information about the connected AP's ip, gateway, DNS etc
             * will be available in 'SlIpV4AcquiredAsync_t' - Applications
             * can use it if required
             *
             * SlIpV4AcquiredAsync_t *pEventData = NULL;
             * pEventData = &pNetAppEvent->EventData.ipAcquiredV4;
             * <gateway_ip> = pEventData->gateway;
             *
             */

    }
    break;

    default:
    {
            UARTprintf(" [NETAPP EVENT] Unexpected event \r\n");
    }
    break;
  }
}

/*!
    \brief This function handles callback for the HTTP server events

    \param[in]      pServerEvent - Contains the relevant event information
    \param[in]      pServerResponse - Should be filled by the user with the
                    relevant response information

    \return         None

    \note

    \warning
*/
void SimpleLinkHttpServerCallback(SlHttpServerEvent_t *pHttpEvent,
                                  SlHttpServerResponse_t *pHttpResponse){
    /*
     * This application doesn't work with HTTP server - Hence these
     * events are not handled here
     */
  UARTprintf(" [HTTP EVENT] Unexpected event \r\n");
}

/*!
    \brief This function handles general error events indication

    \param[in]      pDevEvent is the event passed to the handler

    \return         None
*/
void SimpleLinkGeneralEventHandler(SlDeviceEvent_t *pDevEvent){
    /*
     * Most of the general errors are not FATAL are are to be handled
     * appropriately by the application
     */
  UARTprintf(" [GENERAL EVENT] \r\n");
}

/*!
    \brief This function handles socket events indication

    \param[in]      pSock is the event passed to the handler

    \return         None
*/
void SimpleLinkSockEventHandler(SlSockEvent_t *pSock){
  switch( pSock->Event )
  {
    case SL_NETAPP_SOCKET_TX_FAILED:
    {
            /*
            * TX Failed
            *
            * Information about the socket descriptor and status will be
            * available in 'SlSockEventData_t' - Applications can use it if
            * required
            *
            * SlSockEventData_t *pEventData = NULL;
            * pEventData = & pSock->EventData;
            */
      switch( pSock->EventData.status )
      {
        case SL_ECLOSE:
          UARTprintf(" [SOCK EVENT] Close socket operation failed to transmit all queued packets\r\n");
          break;


        default:
          UARTprintf(" [SOCK EVENT] Unexpected event \r\n");
          break;
      }
    }
    break;

    default:
      UARTprintf(" [SOCK EVENT] Unexpected event \r\n");
    break;
  }
}
/*
 * * ASYNCHRONOUS EVENT HANDLERS -- End
 */
