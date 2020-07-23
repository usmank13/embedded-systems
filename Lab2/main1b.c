// Usman Khan
// ECE 474 task 1b
// Implements traffic light system with timer-based constraints.
// 5 seconds to cycle through different states, holding button for 2 seconds
// to get a response from the system

#ifndef LAB2HEADER_H
#define LAB2HEADER_H
#include "lab2header.h"
#endif
#include <stdint.h>

// program main 
int main(void)
{
//  // configuring registers
  sys_init();
  timer_init();
  
  // setting the initial state of the machine
  GPIO_PORTE_DATA_R = 0x00;
  LState = STOP;
  
   while (1) {
   TrafficLight();
   
   }
   
   return 0;
}


// puts in a delay of a specified number of seconds
void timer_delay(int seconds) {
   GPTMTAILR_0 = 16000000*seconds;
   while ((GPTMRIS_0 & 0x01) != 0x01) { 
   } 
   GPTMICR_0 = 0x01; 
}


unsigned int ped_input(void) {
  return ((GPIO_PORTE_DATA_R & 0x02) == 0x02); 
}

unsigned int start_input(void) {
  return ((GPIO_PORTE_DATA_R & 0x01) == 0x01);
}


void timer_init(void) {
  volatile unsigned short delay = 0;
  SYSCTL_RCGCTIMER_R |= 0xFF; // enable timer 0 and tiemr 1
  delay++;
  delay++; // putting this delay here before accessing
  
  GPTMCTL_0 |= 0x00; 
  
  GPTMCFG_0 |= 0; 
  
  GPTMTAMR_0 = 0x02; // puts timer A in periodic timer mode, TACDIR bit is 0 so counts down
  GPTMTAILR_0 = 16000000;
  
  GPTMCTL_0 = 0x01; // enables timer A 
  
  GPTMCTL_1 = 0x00; 
  
  GPTMCFG_1 = 0x00;
  GPTMTAMR_1 = 0x02; // puts timer 1A in periodic timer mode, TACDIR bit is 0 so counts down
  GPTMTAILR_1 = 16000000*2;  // timer 2 counts 2 seconds
  
  GPTMCTL_1 = 0x01; // enables timer 1A 
  
}


// initializes the system and configures registers
// to enable reading and writing
void sys_init(void) {
 
  volatile unsigned short delay = 0;
  SYSCTL_RCGCGPIO_R = 0x1010; // activate clock for relevant registers
  delay++;
  delay++;
  
  // Setting up the registers for controlling and reading from
  // the LEDs and the switches
  GPIO_PORTE_AMSEL_R &= ~0xFF; // disable analog function
  GPIO_PORTE_DIR_R |= 0xFC;  // set input/output, bits 1 and 0 are inputs 
  GPIO_PORTE_AFSEL_R &= ~0xFF;  // regular port function (?)
  GPIO_PORTE_DEN_R |= 0xFF; // enable digital i/o
  
  // Note: the least significant two bits of port E are for the switches 
  // Green LED is connected to PE5, R is PE2, an Y is PE3
  // These values are all set by GPIO_PORTE_DATA_R
  
    
}

// define states of the traffic light controller
enum LStates { GO, WARN, STOP } LState;

// implements the TrafficLight FSM
// Consists of the three states defined above, their transition conditions
// and the outputs at each state
void TrafficLight() {
  unsigned int PED = ped_input();  // put delays inside these functions?
  unsigned int START = start_input();
  
  unsigned char mask = 0x2C; // mask for changing bit values
  unsigned char held = 1;
  GPTMICR_1 = 0x01; 

  
  // Defining state transitions based on inputs
  switch (LState) {
  case GO:
    if (START && PED) {
      GPTMTAILR_1 = 16000000*2; 
      while ((GPTMRIS_1 & 0x01) != 0x01) { // for checking if the button has been held for 2 secs
        PED = ped_input(); 
        if (PED == 0) { 
          held = 0;
          break;
        }
      } 
    }
      if (held && PED) {
        LState = WARN; 
        GPTMICR_1 = 0x01;
        
    }
      else if (START && ((GPTMRIS_0 & 0x01) == 0x01)) { // start and timer flag thrown
        
      LState = STOP;
      GPTMICR_0 = 0x01; // clear the flag
    } else { 
      if (((GPTMRIS_0 & 0x01) == 0x01)) { // 5 second timer flag
        GPTMICR_0 = 0x01;
      }
        LState = GO; 
    }
    break;
    
  case WARN:
    timer_delay(5); 
    if (START) { 
      LState = STOP;
    } else {
      LState = GO;
    }
    
    break;
  
  case STOP:
    timer_delay(5);
    LState = GO;
    
    break;
  }
  
  // Defining the outputs to the LEDs at each state
  switch (LState) { 
  case GO:
    GPIO_PORTE_DATA_R = (GPIO_PORTE_DATA_R &  ~mask) | (0x20 & mask); // change LEDs without affecting switch values
    break;
    
  case WARN:
    GPIO_PORTE_DATA_R = (GPIO_PORTE_DATA_R & ~mask) | (0x08 & mask);
    break;
  
  case STOP:
    GPIO_PORTE_DATA_R = (GPIO_PORTE_DATA_R & ~mask) | (0x04 & mask);
    break;
  }
  
}