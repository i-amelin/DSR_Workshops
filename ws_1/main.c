#include "main.h"

#define SWITCH_DELAY 5000000
#define LED1 GPIO_Pin_12
#define LED2 GPIO_Pin_13
#define LED3 GPIO_Pin_14
#define LED4 GPIO_Pin_15
#define LEDS LED1 | LED2 | LED3 | LED4

#define RED GPIO_Pin_8
#define GREEN GPIO_Pin_9
#define BLUE GPIO_Pin_10

#define LEFT_BUTTON GPIO_Pin_0
#define RIGHT_BUTTON GPIO_Pin_1

int leds[] = { LED1, LED2, LED3, LED4 };
int leds1[] = { RED, GREEN, BLUE };

int main(void)
{
  GPIO_InitTypeDef  GPIO_InitStructure;

  /* Enable peripheral clock for GPIOD port */
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);

  /* Init leds */
  GPIO_InitStructure.GPIO_Pin = LEDS;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(GPIOD, &GPIO_InitStructure);

  /* Init the led on the extenstion board */
  GPIO_InitStructure.GPIO_Pin = RED | GREEN | BLUE;
  GPIO_Init(GPIOA, &GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Pin = LEFT_BUTTON;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_Init(GPIOE, &GPIO_InitStructure);

  GPIO_SetBits(GPIOA, RED | GREEN | BLUE);
  while (1)
  {
	  char level = GPIO_ReadInputDataBit(GPIOE, LEFT_BUTTON);

	  if (level == Bit_RESET) {
		  GPIO_ResetBits(GPIOA, RED);
	  } else {
		  GPIO_SetBits(GPIOA, RED);
	  }
  }
}
