// Usman Khan
// ECE 474 Lab 4 task 1a: getting LCD display working

#include "Lab_4_Header.h"
#include <stdint.h>
#include <stdio.h>
#include "SSD2119_Touch.h"
#include "SSD2119_Display.h"
// macro defined for checking bits easily
#define CHECK_BIT(var,pos) ((var) & (1<<(pos)))



int main(void) {
  
  LCD_Init();
  LCD_ColorFill(Color4[0]);
  float test = 26.13;
  LCD_PrintFloat(test);
  LCD_PrintFloat(test + 1);
  while (1) {
  }
  
  return 0;
}
