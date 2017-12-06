#ifndef __LEDS_AND_BUTTON_H
#define __LEDS_AND_BUTTON_H

#define HSE_VALUE 8000000

#include "stm32f4xx.h"

#if defined (USE_STM324xG_EVAL)
  #include "stm324xg_eval.h"

#elif defined (USE_STM32F4DISC_EVAL) 
  #include "stm32f4disc_eval.h"

#else
 #error "Please select first the Evaluation board used in your application (in Project Options)"
#endif

#endif
