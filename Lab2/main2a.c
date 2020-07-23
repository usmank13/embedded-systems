// Usman Khan
// ECE 474, Lab 2: Timers and Interrupts 
// task 2A, cycling through LED pattern using timer interrupts


#ifndef LAB2HEADER_H
#define LAB2HEADER_H
#include "lab2header.h"
#endif
#include <stdint.h>

int main(void) {
  GPIO_init();
  timer_init();
  
  while (1) {
  }
  return 0;
}


void timer_init(void) {
  volatile unsigned short delay = 0;
  RCGCTIMER |= 0x01; // enable timer 0
  delay++;
  delay++; // putting this delay here before accessing
  
  GPTMCTL_0 = 0x00; 
  
  GPTMCFG_0 = 0x00;
  GPTMTAMR_0 = 0x02; // puts timer A in periodic timer mode, TACDIR bit is 0 so counts down
  GPTMTAILR_0 = 16000000; 
  GPTMCTL_0 = 0x01; // enables timer A
  
  // enable 19th bit to enable interrupt 19
  NVIC_EN0 = 0x80000;
  GPTMIMR_0 = 0x01; // enable TATOIM bit 
  
  GPTMICR_0 = 0x01;
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
}



// timer interrupt that activates every second. Depending on which LED's
// in the pattern are currently on, it turns LEDs on and off to go to the next state 
// in the pattern. 
void Timer0A_Handler(void) {
  // implement
  GPTMICR_0 = 0x01;
  
  if ((GPIO_PF_DATA == 0x0F) && (GPIO_PN_DATA == 0x00)) { // if LED 4 is on 
    // turn on LED3
    GPIO_PF_DATA = 0xFF; 
  } else if ((GPIO_PF_DATA == 0xFF) && (GPIO_PN_DATA == 0x00)) { // check if both 3 and 4 are on
    // turn on LED 2
    GPIO_PN_DATA = 0x01; 
  } else if ((GPIO_PN_DATA == 0x01) && (GPIO_PF_DATA == 0xFF)) {
    // turn on LED 1
    GPIO_PN_DATA = 0x03;
  } else { 
    // turn off LED 3, 2, and 1
    GPIO_PN_DATA = 0x00;
    GPIO_PF_DATA = 0x0F;
  } 
  
  
}
