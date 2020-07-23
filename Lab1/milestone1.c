#include <stdint.h>
#include "lab1header.h"

#define LED4 0x1

#define LED3 0x1
#define LED2 0x1
#define LED1 0x1

int main(void)
{
   volatile unsigned short delay = 0;
   RCGCGPIO |= 0x1120; // Enable PortF, PortJ, PortN GPIO. we need bit 9
   delay++; // Delay 2 more cycles before access Timer registers
   delay++; // Refer to Page. 756 of Datasheet for info
   
   GPIO_PF_DIR = 0xFF; // Set port F to output
   GPIO_PF_DEN = 0xFF; // Set port F to digital port
   
   GPIO_PN_DIR = 0xFF; // Set port N to outputs
   GPIO_PN_DEN = 0xFF; // set port N to digital 
   
   GPIO_PF_DATA = 0x0F; // turn on the last 4 bits of port F 
   GPIO_PN_DATA = 0x0; // set Port N to 0 
   
   GPIO_PJ_DIR = 0x0; // set port J to inputs
   GPIO_PJ_DEN = 0xFF; // set port J to digital
   
   // For enabling the use of user switches
   GPIO_PORTJ_LOCK_R = 0x4C4F434B;
   GPIO_PORTJ_CR_R = 0x03;
   GPIO_PJ_PUR = 0x03;
   

   
   while (1) {
     
     /* This code block is for controlling the LEDS via switches.
        Commented out when running the pattern-cycling program
     GPIO_PN_DATA = 0x0;
     
     if (GPIO_PJ_DATA == 0x01) {
        GPIO_PN_DATA = 0x01;
     } else if (GPIO_PJ_DATA == 0X02) {
        GPIO_PN_DATA = 0x02;
     } */
   
     
    // This code block sequentially turns on and off the LEDs
    GPIO_PF_DATA = 0x0F;
    for (j = 0; j < 1000000; j++) {}
    for (j = 0; j < 1000000; j++) {}

    GPIO_PF_DATA = 0xFF;
    for (j = 0; j < 1000000; j++) {}
    
    GPIO_PN_DATA = 0x01;
    for (j = 0; j < 1000000; j++) {}
    
    GPIO_PN_DATA = 0x03;
    for (j = 0; j < 1000000; j++) {}
    
    for (j = 0; j < 1000000; j++) {}
    GPIO_PN_DATA = 0x0;
    GPIO_PF_DATA = 0x0;
    
    
   }
   
   return 0;
}





