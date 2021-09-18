#ifndef _GLOBAL_DEF_H_
#define _GLOBAL_DEF_H_

#define ClkFreq 8000000               // Microcontroller system clock
#define ClkFreqMHz (ClkFreq/1000000)

#define TIM1_CLK_DIVIDER1    1
//#define TIM1_CLK_DIVIDER8    8

#ifdef  TIM1_CLK_DIVIDER1
#define TIM1_CLK_DIV 0x01
#define TIM1_DIV 1
#endif

#ifdef  TIM1_CLK_DIVIDER8
#define TIM1_CLK_DIV 0x02
#define TIM1_DIV 8
#endif

#define TIM1_START      TCCR1B |= TIM1_CLK_DIV;
#define TIM1_STOP       TCCR1B &= ~0x07;



#endif