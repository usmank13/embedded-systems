// Usman Khan
// ECE 474 
// Lab 4 task 1c: displaying temp and clock freq on the LCD with virtual
// buttons to toggle frequency

#include "Lab_4_Header.h"
#include "Lab_3_Driver.h"
#include <stdint.h>
#include <stdio.h>
#include "SSD2119_Touch.h"
#include "SSD2119_Display.h"
#include "Lab_3_Header.h"


// macro defined for checking bits easily
#define CHECK_BIT(var,pos) ((var) & (1<<(pos)))

enum frequency freq = PRESET2;
enum LStates {Init, one, two, three, four} LState;
float tempC;
float tempF;

// green button x: 800 to 1100
// green button y: same thing 

// blue button x: 1400 to 1600
// blue button y: same

unsigned long xmin1 = 800;
unsigned long xmax1 = 1100;
unsigned long ymin1 = 800;
unsigned long ymax1 = 1100;

unsigned long xmin2 = 1400;
unsigned long xmax2 = 1600;
unsigned long ymin2 = 800;
unsigned long ymax2 = 1100;


int main(void) {
  
  PLL_Init(freq);             // PLL must be configured before everthing else
  SW_init();
  Timer0_init();
  ADC_init();
  LCD_Init();
  LCD_ColorFill(Color4[0]);
 

  while (1) {
     message();
     if ((touchx > xmin1) && (touchx < xmax1) && (touchy > ymin1) && (touchy < ymax1)) {
      freq = PRESET3;
      PLL_Init(freq);
      Timer0_init(); 
     } else if ((touchx > xmin2) && (touchx < xmax2) && (touchy > ymin2) && (touchy < ymax2)) {
      freq = PRESET1;
      PLL_Init(freq);
      Timer0_init();
     } else {
     }
  }
  
  return 0;
}

// prints the current temperature in F and C, as well as the current 
// clock frequency
void message(void) {
  LCD_PrintString("The current temperature is ");
  LCD_PrintFloat(tempC);
  LCD_PrintString(" C, ");
  LCD_PrintFloat(tempF);
  LCD_PrintString(" F.");
  LCD_PrintChar('\n');
  LCD_PrintString("The current clock frequency is ");
  LCD_PrintInteger(freq);
  LCD_PrintString(" MHz.");
  LCD_SetCursor(0, 0);
}

// enable system timers and initialize timer 0
// this timer is for triggering the ADC interrupt
void Timer0_init(void) {
  volatile unsigned short delay = 0;
  SYSCTL_RCGCTIMER_R |= 0xFF; // enable timer 0 and tiemr 1
  delay++;
  delay++; // putting this delay here before accessing

  GPTMICR_0 = 0x01;
  GPTMCTL_0 = 0x00; 
  GPTMCFG_0 = 0; 
  GPTMTAMR_0 = 0x02; // puts timer A in periodic timer mode, TACDIR bit is 0 so counts down
  GPTMTAILR_0 = freq*1000000; 

  TIMER0_ADCEV = 0x01; // set to 1 to enable ADC to trigger on time out events 
  GPTMCTL_0 = 0x21; // enables timer A and the trigger for timer A to ADC (bit fields 5 and 0)
  
  
}

// Initialize and configure switches
void SW_init() {
   volatile unsigned short delay = 0;
   SYSCTL_RCGCGPIO_R |= 0x1121; // Enable PortF, PortJ, PortN GPIO. we need bit 9
   delay++; // Delay 2 more cycles before access Timer registers
   delay++; // Refer to Page. 756 of Datasheet for info
   
   GPIO_PJ_DIR = 0x0; // set port J to inputs
   GPIO_PJ_DEN = 0xFF; // set port J to digital
   
   // For enabling the use of user switches
   GPIO_PORTJ_LOCK_R = 0x4C4F434B;
   GPIO_PORTJ_CR_R = 0x03;
   GPIO_PJ_PUR = 0x03;
   
     // enable interrupt for port J
   NVIC_EN1 = 0x80000; 
   GPIOIM_PJ = 0x03; // to get interrupts from both the user switches
}

// Initialize ADC and ADC interrupts
void ADC_init() {
  volatile unsigned short delay = 0;
  SYSCTL_RCGCADC_R |= 0x01; 
  delay++;
  delay++;
  
  ADC0_CC_R = 0x01;
  
  ADC0_ACTSS_R = 0x00; // initially disable sample sequencers
  
  // set adcemux to trigger by timer. Timer is 0x5 for bits 15:12
  ADC0_EMUX_R = 0x5000;
  
    // hold width to be at least 16 ADC clocks
  ADC0_SSTSH3_R = 0x4;
  ADC0_SSCTL3_R = 0x0E;

  
  NVIC_EN0 = 0x20000;
  // set ADCIM for interrupts
  ADC0_IM_R = 0x08; // interrupts from sample sequencer 3 are sent to the interrupt controller
  
  ADC0_ACTSS_R = 0x08;

}



// read temperature, set appropriate LED pattern state
// LEDs flash at 2 Hz. 
void ADC0_Handler() {
  // clear timer flag here too?
  GPTMICR_0 = 0x01;
  // clear the flag
  ADC0_ISC_R = 0x8;
  tempC = (147.5 - (247.5 * ADC0_SSFIFO3_R) / 4096);
  tempF = (tempC * 1.8) + 32;
  GPTMICR_0 = 0x01;
  
}

// interrupt service routine for a GPIO interrupt from the user switches
// based on which switch was pressed, different behavior occurs
// SW1 changes clock to 12 MHz, SW2 changes clock to 120 MHz
void GPIO_PJ_Handler(void) {
  if (GPIOMIS_J == 0x01) { // switch 1 was pressed 
    GPIOICR_J = 0x03; // clears flag
    // change system clock to 12 MHz 
    freq = PRESET3;    
    
  } else {  // switch 2 was pressed
    GPIOICR_J = 0x03; 
    // change the system clock to 120 MHz
    freq = PRESET1; 
  }
  PLL_Init(freq);
  Timer0_init(); 

}