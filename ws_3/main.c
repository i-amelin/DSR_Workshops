#include "main.h"

#define RED GPIO_Pin_8
#define GREEN GPIO_Pin_9
#define BLUE GPIO_Pin_10
#define LEDS RED | GREEN | BLUE

#define LEFT_BUTTON GPIO_Pin_0

#define PERIOD 1000000
#define PRESCALER 84

int8_t order, i;
int leds[] = { RED, GREEN, BLUE };

void initLedsAndButton(void);
void initTIM(void);
void initNVIC(void);
void initEXTI(void);
void TIM2_IRQHandler(void);
void EXTI0_IRQHandler(void);

int main(void)
{
  order = 1;
  i = 0;

  initLedsAndButton();
  initTIM();
  initEXTI();
  initNVIC();

  GPIO_SetBits(GPIOA, RED | GREEN | BLUE);

  while (1) {
      // Nothing here
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
    TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);
    TIM_Cmd(TIM2, ENABLE);
}

void initNVIC(void) {
    NVIC_InitTypeDef nvic_struct;
    // Init NVIC for a Timer interrupt
    nvic_struct.NVIC_IRQChannel = TIM2_IRQn;
    nvic_struct.NVIC_IRQChannelPreemptionPriority = 0;
    nvic_struct.NVIC_IRQChannelSubPriority = 1;
    nvic_struct.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&nvic_struct);

    // Init NVIC for a external interrupt
    nvic_struct.NVIC_IRQChannel = EXTI0_IRQn;
    nvic_struct.NVIC_IRQChannelPreemptionPriority = 0;
    nvic_struct.NVIC_IRQChannelSubPriority = 0;
    NVIC_Init(&nvic_struct);
}

void initEXTI(void) {
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
    SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOE, EXTI_PinSource0);
    
    EXTI_InitTypeDef exti_struct;
    exti_struct.EXTI_Line = EXTI_Line0;
    exti_struct.EXTI_LineCmd = ENABLE;
    exti_struct.EXTI_Mode = EXTI_Mode_Interrupt;
    exti_struct.EXTI_Trigger = EXTI_Trigger_Rising;
    EXTI_Init(&exti_struct);
}

void TIM2_IRQHandler(void) {
    if (TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET) {
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
        TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
    }
}

void EXTI0_IRQHandler(void) {
    if (EXTI_GetITStatus(EXTI_Line0) != RESET) {
        order ^= 1;
        EXTI_ClearITPendingBit(EXTI_Line0);
    }
}
