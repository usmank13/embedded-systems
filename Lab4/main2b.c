// Usman Khan
// ECE 474 
// Task 2b: return to sender function implemented using bluetooth


#include <stdint.h>
#include "Lab_3_Header.h"
#include "Lab_3_Driver.h"
#include <stdio.h>

// macro defined for checking bits easily
#define CHECK_BIT(var,pos) ((var) & (1<<(pos)))


enum frequency freq = PRESET2;
enum LStates {Init, one, two, three, four} LState;
float tempValue;

int main(void) {
    // Select system clock frequency preset
    //
    // For lab task requirement:
    //   - Select PRESET 2 (60 MHz) to be the initial system clock frequency
    //   - Clock frequency stay at 60 MHz if no user switches are pressed
    //   - Change clock frequency to 12 MHz if SW 1 is pressed
    //   - Change clock frequency to 120 MHz if SW 2 is pressed
    PLL_Init(freq);             // PLL must be configured before everthing else

    SW_init();
    Timer0_init();
    ADC_init();
    UART2_init();
    
    while (1) {
      send_blue();
    }
    
    return 0;
}


// handle sending and receiving data via bluetooth UART
// reads characters sent from the computer, and sends it back 
void send_blue(void) { 
  // if I receive data, then execute return-to-sender
  if (CHECK_BIT(UART2_RIS_R, 4)) {
    
    char readValue = UART2_DR_R; 
    while (CHECK_BIT(UART2_FR_R, 6)) {} // wait while the receiver is full
    
    UART2_DR_R = readValue;
    while (CHECK_BIT(UART2_FR_R, 3)) {}
    
  }
}

// call sw init before this
void LED_init() {
   
   GPIO_PF_DIR = 0xFF; // Set port F to output
   GPIO_PF_DEN = 0xFF; // Set port F to digital port
   
   GPIO_PN_DIR = 0xFF; // Set port N to outputs
   GPIO_PN_DEN = 0xFF; // set port N to digital 
}


// initialize UART2 for bluetooth communication
void UART2_init() {
  volatile unsigned short delay = 0;
  SYSCTL_RCGCUART_R |= 0x0F; 
  delay++;
  delay++; 
  
  UART2_CTL_R = 0x0; 
  
  SYSCTL_ALTCLKCFG_R = 0x0; // this is set to PIOSC. PIOSC is 16 MHz
    
  
  GPIO_PORTA_DEN_R = 0xF3; // enable digital function for PA7 and PA6
  GPIO_PORTA_AFSEL_R = 0xF3; // PA7 and PA6 set to special function
  
  
  GPIO_PORTA_PCTL_R = 0x11000011; // select the right special function for each pin. 
  
  UART2_IBRD_R = 104; // 104
  UART2_FBRD_R = 11; // 11 
  
  // desired serial parameters to UARTLCRH? keep as normal. Just set it to 8 bit, everything else is 0
  UART2_LCRH_R = 0x60; 
  
  // configure UART clock source with UARTCC
  UART2_CC_R = 0x5;
  
  // Enable UART with UARTCTL,
  UART2_CTL_R = 0x301; 
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

// timer problem happening.. why are we getting stuck?
// half second delay
void timer_delay() {
   GPTMICR_1 = 0x01; // clearing the flag here
   GPTMTAILR_1 = (freq*1000000) / 2;
   while ((GPTMRIS_1 & 0x01) != 0x01) {} 
   GPTMICR_1 = 0x01; 
}




// this timer is for the LED-blinking functionality
// max timer value of this will change with clock frequency
void Timer1_init(void) {
  GPTMICR_1 = 0x01; // clear flag
  
  GPTMCTL_1 = 0x00; 
  GPTMCFG_1 = 0x00;
  GPTMTAMR_1 = 0x02; // periodic mode and counts down. Might have to change
  GPTMTAILR_1 = freq / 2; // potentially change this. Times out half a second  
  GPTMCTL_1 = 0x01; // enables timer 1A 
}


// initialize user switches
void SW_init() {
   volatile unsigned short delay = 0;
   SYSCTL_RCGCGPIO_R |= 0x1121; // Enable PortF, PortJ, PortN GPIO. we need bit 9
   // ENABLE PORT A 
   
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

// initialize ADC
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
    
  // do something with ADCSSCTL3 bit 0 must be set to 0
  ADC0_SSCTL3_R = 0x0E;

  
  NVIC_EN0 = 0x20000;
  // set ADCIM for interrupts
  ADC0_IM_R = 0x08; // interrupts from sample sequencer 3 are sent to the interrupt controller
  
  
  // set ADCACTSS to 0x08. Enable sample sequencer 3. Last line of this function
  ADC0_ACTSS_R = 0x08;

}



// read temperature, set appropriate LED pattern state
void ADC0_Handler() {
  GPTMICR_0 = 0x01;
  ADC0_ISC_R = 0x8;
  tempValue = (147.5 - (247.5 * ADC0_SSFIFO3_R) / 4096);
  printf("%f\n", tempValue);
  if ((tempValue > 0) && (tempValue < 20)) {

    LState = one;
  } else if ((tempValue > 20) && (tempValue < 22.5)) { 
    LState = two;
  } else if ((tempValue > 22.5) && (tempValue < 28)) {
    LState = three;
  } else if (tempValue > 28) {
    LState = four;
  } else {
  }
  GPTMICR_0 = 0x01;
  
}

// interrupt service routine for a GPIO interrupt from the user switches
// based on which switch was pressed, different behavior occurs
void GPIO_PJ_Handler(void) {
  if (GPIOMIS_J == 0x01) { // switch 1 was pressed 
    GPIOICR_J = 0x03; // clears flag
    // change system clock to 12 MHz 
    freq = PRESET3;    
    
  } else {  // switch 2 was pressed
    GPIOICR_J = 0x03; 
    // change the system clock to 120 MHz
    freq = PRESET2; 
  }
  PLL_Init(freq);
  Timer0_init(); 

}

