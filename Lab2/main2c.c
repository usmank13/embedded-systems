// Usman Khan
// Lab 2 Task 2c


#ifndef LAB2HEADER_H
#define LAB2HEADER_H
#include "lab2header.h"
#endif
#include <stdint.h>

// main method that runs the program
int main(void)
{
  // configuring registers
  sys_init();
  timer_init();
  
  // setting the initial state of the machine
  GPIO_PORTE_DATA_R = 0x00;
  LState = STOP; // start in the STOP state
   
  
   while (1) {
    TrafficLight();
   }
   
   return 0;
}

void timer_init(void) {
  volatile unsigned short delay = 0;
  SYSCTL_RCGCTIMER_R |= 0x01; // enable timer 0
  delay++;
  delay++; // putting this delay here before accessing
  
  GPTMCTL_0 = 0x00; 
  GPTMCFG_0 = 0x00;
  GPTMTAMR_0 = 0x02; // puts timer A in periodic timer mode, TACDIR bit is 0 so counts down
  GPTMTAILR_0 = 16000000; //0xF42400; // 16 mil. 
  GPTMCTL_0 = 0x01; // enables timer A 
  
  
  GPTMCTL_1 = 0x00; 
  GPTMCFG_1 = 0x00;
  GPTMTAMR_1 = 0x02; // puts timer 1A in periodic timer mode, TACDIR bit is 0 so counts down
  GPTMTAILR_1 = 16000000*2;  // timer 2 counts 2 seconds
  GPTMCTL_1 = 0x01; // enables timer 1A 
  
  
  // we don't want to use timer interrupts
  GPTMIMR_0 = 0x00; 
  GPTMICR_0 = 0x00;
  

}

// checks if the button is pressed, pin E1 is the pedestrian input
// returns an unsigned int (either a 0 or 1) representing presed vs not pressed
unsigned int ped_input(void) {
  return ((GPIO_PORTE_DATA_R & 0x02) == 0x02); 
}

// pin E0 is the start/stop button. 
// returns an unsigned int (either a 0 or 1) representing pressed vs not pressed
unsigned int start_input(void) {
  return ((GPIO_PORTE_DATA_R & 0x01) == 0x01);
}

void timer_delay(int seconds) {
   // set timer length to be 5 secs or 2 seconds
   GPTMTAILR_0 = 16000000*seconds;
   while ((GPTMRIS_0 & 0x01) != 0x01) { 
   } 
   GPTMICR_0 = 0x01; 
}


// initializes the system and configures registers
// to enable reading and writing
void sys_init(void) {
 
  volatile unsigned short delay = 0;
  SYSCTL_RCGCGPIO_R |= 0x1010; // activate clock for relevant registers
  delay++;
  delay++;
  
  // Setting up the registers for controlling and reading from
  // the LEDs and the switches
  GPIO_PORTE_AMSEL_R &= ~0xFF; // disable analog function
  GPIO_PORTE_DIR_R = 0xFC;  // set input/output, bits 1 and 0 are inputs 
  GPIO_PORTE_AFSEL_R &= ~0xFF;  // regular port function (?)
  GPIO_PORTE_DEN_R |= 0xFF; // enable digital i/o
  
  // Note: the least significant two bits of port E are for the switches 
  // Green LED is connected to PE5, R is PE2, an Y is PE3
  // These values are all set by GPIO_PORTE_DATA_R
  
  // change EN0 to enable an interrupt from port E
  NVIC_EN0_R = 0x01;  // We want to enable bit 4
  
  GPIO_PORTE_IM_R = 0x0;
  GPIO_PORTE_IS_R = 0x0;
  GPIO_PORTE_IBE_R = 0x0;
  GPIOICR_E = 0x03;
  GPIO_PORTE_IEV_R = 0x02;
  GPIOICR_E = 0x3;
  GPIO_PORTE_IM_R = 0x3; 
  
  
}



// define states of the traffic light controller
enum LStates { GO, WARN, STOP } LState;

// implements the TrafficLight FSM
// Consists of the three states defined above, their transition conditions
// and the outputs at each state

unsigned char ped_held = 0;
unsigned char start_held = 0;

void TrafficLight() {
  // declaring the input variables
  // these inputs control FSM behavior
  unsigned int PED = ped_input();  // put delays inside these functions?
  unsigned int START = start_input();
  
  unsigned char mask = 0x2C; // mask for changing bit values

  
  // Defining state transitions based on inputs
  // we want to go to the next state when we get the interrupt 
  switch (LState) {
  case GO:

    if (START && ped_held) {
      LState = WARN;
      ped_held = 0;
    } else if (START && ((GPTMRIS_0 & 0x01) == 0x01)) {
      GPTMICR_0 = 0x01;
      LState = STOP;
      
    } else {
      if (((GPTMRIS_0 & 0x01) == 0x01)) {
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


// interrupt service routine activated when a button is pressed
// implements functinoality of having to hold for two seconds for a button press 
// to be registered
void GPIO_SW_Handler(void) {
  
  if ((GPIOMIS_E == 0x02)) { // if we got the interrupt from PED and we're in GO state   
      ped_held = 1;
      GPTMTAILR_1 = 16000000*2; // start timer2
      unsigned int PED;
      while ((GPTMRIS_1 & 0x01) != 0x01) { // while timer 2 is going
        PED = ped_input();
        if (PED == 0) {  // if ped button is released
          ped_held = 0;
          break;
        }
      } // if ped doesn't get released, it is true here after the loop
      GPTMICR_1 = 0x01; 

      unsigned int start = start_input();
      if (ped_held && start) {
        LState = WARN;
      }
  } else { // interrupt was triggered by the other switch 
    GPIOICR_E = 0x03; 
    start_held = 1;
    GPTMTAILR_1 = 16000000*2;
    unsigned int START;
    while ((GPTMRIS_1 & 0x01) != 0x01) { // while timer 2 is going
      START = start_input();
      if (START == 0) {  // if start button is released
        start_held = 0;
        break;
      }
    }
      GPTMICR_1 = 0x01; 

  }
  GPIOICR_E = 0x02; // clears interrupt flag

}