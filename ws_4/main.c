#include "main.h"
#include "led.h"

#define LEFT_BUTTON GPIO_Pin_0
#define RIGHT_BUTTON GPIO_Pin_1

#define DEBOUNCE_DELAY 200

int left_button_delay, right_button_delay;

void initButtons(void);
void initNVIC(void);
void initEXTI(void);
void TIM2_IRQHandler(void);
void EXTI0_IRQHandler(void);
void EXTI1_IRQHandler(void);
void SysTick_Handler(void);

int main(void) {
    left_button_delay = 0;
    right_button_delay = 0;

    InitLeds();

    initEXTI();
    initNVIC();
    initButtons();

    while (1) {
    }
}

void initButtons(void) {
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);

    GPIO_InitTypeDef gpio_struct;
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);
    gpio_struct.GPIO_Pin = LEFT_BUTTON | RIGHT_BUTTON;
    gpio_struct.GPIO_Mode = GPIO_Mode_IN;
    gpio_struct.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_Init(GPIOE, &gpio_struct);

    SysTick_Config(SystemCoreClock / 1000);
}

void initNVIC(void) {
    NVIC_InitTypeDef nvic_struct;

    // Init NVIC for a external interrupt
    nvic_struct.NVIC_IRQChannel = EXTI0_IRQn;
    nvic_struct.NVIC_IRQChannelPreemptionPriority = 0;
    nvic_struct.NVIC_IRQChannelSubPriority = 0;
    nvic_struct.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&nvic_struct);

    nvic_struct.NVIC_IRQChannel = EXTI1_IRQn;
    nvic_struct.NVIC_IRQChannelSubPriority = 1;
    NVIC_Init(&nvic_struct);
}

void initEXTI(void) {
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
    SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOE, EXTI_PinSource0);
    SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOE, EXTI_PinSource1);
    
    EXTI_InitTypeDef exti_struct;
    exti_struct.EXTI_Line = EXTI_Line0;
    exti_struct.EXTI_LineCmd = ENABLE;
    exti_struct.EXTI_Mode = EXTI_Mode_Interrupt;
    exti_struct.EXTI_Trigger = EXTI_Trigger_Rising;
    EXTI_Init(&exti_struct);

    exti_struct.EXTI_Line = EXTI_Line1;
    EXTI_Init(&exti_struct);
}

void EXTI0_IRQHandler(void) {
    if (EXTI_GetITStatus(EXTI_Line0) != RESET) {
        if (left_button_delay > DEBOUNCE_DELAY) {
            left_button_delay = 0;

            IncrementCurrentColor();
            LightLeds();
        }
    }
    EXTI_ClearITPendingBit(EXTI_Line0);
}

void EXTI1_IRQHandler(void) {
    if (EXTI_GetITStatus(EXTI_Line1) != RESET) {
        if (right_button_delay > DEBOUNCE_DELAY) {
            right_button_delay = 0;
            
            SetToNextColor();
            LightLeds();
        }
    }
    EXTI_ClearITPendingBit(EXTI_Line1);
}

void SysTick_Handler(void) {
    left_button_delay++;
    right_button_delay++;
}
