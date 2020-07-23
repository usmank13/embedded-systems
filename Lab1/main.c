#include <stdint.h>
#include "lab1header.h"
//#include "tm4c1294ncpdt.h"

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


// initializes the system and configures registers
// to enable reading and writing
void sys_init(void) {
 
  volatile unsigned short delay = 0;
  RCGCGPIO = 0x1010; // activate clock for relevant registers
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

  
  // Defining state transitions based on inputs
  switch (LState) {
  case GO:

    if (START && PED) { 
      LState = WARN;
    } else if (!PED && START) {
      // delay 5 secs and then go NOTE: THIS IS THE OLD LAB 1 FILE. WORK IN THE LAB 2 FILE!!
      LState = STOP;
    } else {
      LState = GO;
    }
    
    break;
    
  case WARN:
    // delay 5 secs here maybe
    if (START) { 
      LState = STOP;
    } else {
      LState = GO;
    }
    
    break;
  
  case STOP:
    // delay for 5 secs, then go to GO
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




// main method that runs the program
int main(void)
{
//  // configuring registers
  sys_init();
  
  // setting the initial state of the machine
  GPIO_PORTE_DATA_R = 0x00;
  LState = STOP;
  int j; // counter variable to introduce delay
  
  
   
  
   while (1) {
   TrafficLight();
   for (int j = 0; j < 1000000; j++) {} // introduce a delay at each cycle
    
   }
   
   return 0;
}
