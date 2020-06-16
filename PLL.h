/*! \file PLL.h
*/

/*
  Initialization functions also set the system clock be 80 MHz
*/
void SysTick_Init(void);
void PLL_Init(void);

/*
  Enter a delay in microsecond
*/
void SysTick_Wait(unsigned long delay);