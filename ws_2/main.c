#include "main.h"

#define RED GPIO_Pin_8
#define GREEN GPIO_Pin_9
#define BLUE GPIO_Pin_10
#define LEDS RED | GREEN | BLUE

#define LEFT_BUTTON GPIO_Pin_0

#define PERIOD 1000000
#define PRESCALER 84

int leds[] = { RED, GREEN, BLUE };

void initLedsAndButton(void);
void initTIM(void);

int main(void)
{
  initLedsAndButton();
  initTIM();

  int8_t order = 0,
       i = 0;

  GPIO_SetBits(GPIOA, RED | GREEN | BLUE);

  while (1)
  {
	  char level = GPIO_ReadInputDataBit(GPIOE, LEFT_BUTTON);

	  if (level == Bit_RESET) {
          order ^= 1;
	  }

      if (TIM_GetCounter(TIM2) == 0) {
          GPIO_SetBits(GPIOA, leds[i]); 
          if (order) {
              i++;
              i = i % 3;
          } else {
              i--;
              if (i < 0) {
                  i = 2;
              }
          }
          GPIO_ResetBits(GPIOA, leds[i]);
      }
  }
}

void initLedsAndButton(void) {
    GPIO_InitTypeDef gpio_struct;
    
    // Init leds
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
    gpio_struct.GPIO_Pin = LEDS;
    gpio_struct.GPIO_Mode = GPIO_Mode_OUT;
    gpio_struct.GPIO_OType = GPIO_OType_PP;
    gpio_struct.GPIO_Speed = GPIO_Speed_100MHz;
    gpio_struct.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Init(GPIOA, &gpio_struct);

    // Init a button
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);
    gpio_struct.GPIO_Pin = LEFT_BUTTON;
    gpio_struct.GPIO_Mode = GPIO_Mode_IN;
    gpio_struct.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_Init(GPIOE, &gpio_struct);
}

void initTIM(void) {
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
    TIM_TimeBaseInitTypeDef tim_struct;
    tim_struct.TIM_Period = PERIOD - 1;
    tim_struct.TIM_Prescaler = PRESCALER - 1;
    tim_struct.TIM_ClockDivision = 0;
    tim_struct.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM2, &tim_struct);
    TIM_Cmd(TIM2, ENABLE);
}
