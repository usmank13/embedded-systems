// Usman Khan
// ECE 474, Lab 2: Timers and Interrupts 
// task 2b: blinking LED at 1 Hz with user switches 1 and 2 interrupting

#ifndef LAB2HEADER_H
#define LAB2HEADER_H
#include "lab2header.h"
#endif
#include <stdint.h>

int main(void) {
  GPIO_init();
  timer_init();
  
  while (1) {
    led_blink();
  }
  return 0;
}

void timer_init(void) {
  volatile unsigned short delay = 0;
  RCGCTIMER |= 0x01; // enable timer 0
  delay++;
  delay++; // putting this delay here before accessing
  
  GPTMCTL_0 = 0x00; // should this be all 0's here? 
  
  GPTMCFG_0 = 0x00;
  GPTMTAMR_0 = 0x02; // puts timer A in periodic timer mode, TACDIR bit is 0 so counts down
  GPTMTAILR_0 = 16000000; 
  
  GPTMCTL_0 = 0x01; // enables timer A 
  
  // in 2b we are not using the timer interrupt
  NVIC_EN0 = 0x00000;
  GPTMIMR_0 = 0x00; 
  
  GPTMICR_0 = 0x00;
}

void GPIO_init(void) {
   volatile unsigned short delay = 0;
   RCGCGPIO |= 0x1120; // Enable PortF, PortJ, PortN GPIO. we need bit 9
   delay++; // Delay 2 more cycles before access Timer registers
   delay++; // Refer to Page. 756 of Datasheet for info
   
   GPIO_PF_DIR = 0xFF; // Set port F to output
   GPIO_PF_DEN = 0xFF; // Set port F to digital port
   
   GPIO_PN_DIR = 0xFF; // Set port N to outputs
   GPIO_PN_DEN = 0xFF; // set port N to digital 
   
   GPIO_PF_DATA = 0x0F; // turn on the last 4 bits of port F (turns LED4 on)
   GPIO_PN_DATA = 0x0; // set Port N to 0 
   
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

void led_blink(void) {
    while ((GPTMRIS_0 & 0x01) != 0x01) { 
    }
    GPTMICR_0 = 0x01; 
    
    GPIO_PF_DATA = 0x01; 
    
    while ((GPTMRIS_0 & 0x01) != 0x01) { 
      
    }
    GPTMICR_0 = 0x01; 
    
    GPIO_PF_DATA = 0x00;
  
}


// interrupt service routine for a GPIO interrupt from the user switches
// based on which switch was pressed, different behavior occurs
void GPIO_PJ_Handler(void) {
  if (GPIOMIS_J == 0x01) { // switch 1 was pressed
    GPIOICR_J = 0x03;
    GPTMCTL_0 = 0x00; // disables timer
    GPIO_PN_DATA = 0x01; // led 2 on 
  } else {  
    GPIOICR_J = 0x03; 
    GPTMTAILR_0 = 16000000;
    GPTMCTL_0 = 0x01;
    GPIO_PN_DATA = 0x00;

  }
  GPIOICR_J = 0x03; // clear bits 0 and 1
    
}
