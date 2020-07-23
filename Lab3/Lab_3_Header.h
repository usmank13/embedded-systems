
#ifndef _Lab_3_HEADER_
#define _Lab_3_HEADER_

#include <stdint.h>

// ******************** CLOCK CONTROL REGISTERS ********************** //
#define MOSCCTL                 (*(volatile uint32_t *) 0x400FE07C)
#define RIS                     (*(volatile uint32_t *) 0x400FE050)
#define RSCLKCFG                (*(volatile uint32_t *) 0x400FE0B0)
#define PLLFREQ0                (*(volatile uint32_t *) 0x400FE160)
#define PLLFREQ1                (*(volatile uint32_t *) 0x400FE164)
#define PLLSTAT                 (*(volatile uint32_t *) 0x400FE168)
#define MEMTIM0                 (*(volatile uint32_t *) 0x400FE0C0)

void Timer0_init(void);
void Timer1_init(void);
void LED_init(void);
void SW_init(void);
void ADC_init(void);
void tempLight(void);
void UART_init(void);
void timer_delay();
void UART2_init(void);
void send_blue(void);
void send(void);

// using Timer 0
#define SYSCTL_RCGCTIMER_R      (*((volatile uint32_t *)0x400FE604))

#define GPTMCFG_0   (*((volatile uint32_t *)0x40030000)) 
#define GPTMCTL_0   (*((volatile uint32_t *)0x4003000C))
#define GPTMTAMR_0  (*((volatile uint32_t *)0x40030004)) 
#define GPTMTAILR_0 (*((volatile uint32_t *)0x40030028))
#define GPTMRIS_0   (*((volatile uint32_t *)0x4003001C))
#define GPTMICR_0   (*((volatile uint32_t *)0x40030024))
#define GPTMIMR_0   (*((volatile uint32_t *)0x40030018)) 
#define NVIC_EN0    (*((volatile uint32_t *)0xE000E100))
#define NVIC_EN0_R  (*((volatile uint32_t *)0xE000E100))
#define TIMER0_ADCEV (*((volatile uint32_t *)0x40030070))


//Timer 1
#define GPTMCFG_1          (*((volatile uint32_t *)0x40031000))
#define GPTMCTL_1            (*((volatile uint32_t *)0x4003100C))
#define GPTMTAMR_1          (*((volatile uint32_t *)0x40031004))
#define GPTMTAILR_1          (*((volatile uint32_t *)0x40031028))
#define GPTMRIS_1            (*((volatile uint32_t *)0x4003101C))
#define GPTMICR_1            (*((volatile uint32_t *)0x40031024))


// ADC0 Registers
#define SYSCTL_RCGCADC_R        (*((volatile uint32_t *)0x400FE638))
#define ADC0_ACTSS_R            (*((volatile uint32_t *)0x40038000))
#define ADC0_EMUX_R             (*((volatile uint32_t *)0x40038014))
#define ADC0_CC_R               (*((volatile uint32_t *)0x40038FC8))
#define ADC0_SSCTL3_R           (*((volatile uint32_t *)0x400380A4))
#define ADC0_SSFIFO3_R          (*((volatile uint32_t *)0x400380A8))
#define ADC0_IM_R               (*((volatile uint32_t *)0x40038008))
#define ADC0_SSTSH3_R           (*((volatile uint32_t *)0x400380BC))
#define ADC0_SSFIFO3_R          (*((volatile uint32_t *)0x400380A8))
#define ADC0_ISC_R              (*((volatile uint32_t *)0x4003800C))


// GPIO definitions
#define SYSCTL_RCGCGPIO_R     (*((volatile uint32_t *)0x400FE608))
#define GPIOIM_PJ (*((volatile uint32_t *)0x40060410))
#define NVIC_EN1  (*((volatile uint32_t *)0xE000E104))
#define GPIOICR_J (*((volatile uint32_t *)0x4006041C))
#define GPIOMIS_J (*((volatile uint32_t *)0x40060418))

#define GPIO_PJ_DIR       (*((volatile uint32_t *)0x40060400))
#define GPIO_PJ_DEN       (*((volatile uint32_t *)0x4006051C))

#define GPIO_PJ_PUR       (*((volatile uint32_t *)0x40060510))
#define GPIO_PORTJ_LOCK_R       (*((volatile uint32_t *)0x40060520))
#define GPIO_PORTJ_CR_R         (*((volatile uint32_t *)0x40060524))
#define GPIO_PJ_DATA       (*((volatile uint32_t *)0x400603FC))

// GPIO PORT A
#define GPIO_PORTA_AFSEL_R      (*((volatile uint32_t *)0x40058420))
#define GPIO_PORTA_DEN_R        (*((volatile uint32_t *)0x4005851C))
#define GPIO_PORTA_PCTL_R       (*((volatile uint32_t *)0x4005852C))


// LED GPIO registers
#define GPIO_PF_DIR        (*((volatile uint32_t *)0x4005D400))
#define GPIO_PF_DEN        (*((volatile uint32_t *)0x4005D51C))

#define GPIO_PN_DIR        (*((volatile uint32_t *)0x40064400))
#define GPIO_PN_DEN        (*((volatile uint32_t *)0x4006451C))

#define GPIO_PN_DATA      (*((volatile uint32_t *)0x400643FC))
#define GPIO_PF_DATA      (*((volatile uint32_t *)0x4005D3FC))

// UART Registers
#define SYSCTL_RCGCUART_R       (*((volatile uint32_t *)0x400FE618))

// UART 0
#define UART0_DR_R              (*((volatile uint32_t *)0x4000C000)) // data
#define UART0_FR_R              (*((volatile uint32_t *)0x4000C018)) // flag
#define UART0_CTL_R             (*((volatile uint32_t *)0x4000C030))
#define UART0_CC_R              (*((volatile uint32_t *)0x4000CFC8))
#define SYSCTL_ALTCLKCFG_R      (*((volatile uint32_t *)0x400FE138))
#define UART0_LCRH_R            (*((volatile uint32_t *)0x4000C02C))
#define UART0_IBRD_R            (*((volatile uint32_t *)0x4000C024))
#define UART0_FBRD_R            (*((volatile uint32_t *)0x4000C028))

// UART 1 
#define UART2_DR_R              (*((volatile uint32_t *)0x4000E000))
#define UART2_FR_R              (*((volatile uint32_t *)0x4000E018))
#define UART2_IBRD_R            (*((volatile uint32_t *)0x4000E024))
#define UART2_FBRD_R            (*((volatile uint32_t *)0x4000E028))
#define UART2_LCRH_R            (*((volatile uint32_t *)0x4000E02C))
#define UART2_CTL_R             (*((volatile uint32_t *)0x4000E030))
#define UART2_CC_R              (*((volatile uint32_t *)0x4000EFC8))
#define UART2_RIS_R             (*((volatile uint32_t *)0x4000E03C))


#endif /* _Lab_3_HEADER_ */
