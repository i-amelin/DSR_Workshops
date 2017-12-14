#include "main.h"

#define RED GPIO_Pin_8
#define GREEN GPIO_Pin_9
#define BLUE GPIO_Pin_10
#define LEDS RED | GREEN | BLUE

#define BOARD_RED GPIO_Pin_14
#define BOARD_GREEN GPIO_Pin_12
#define BOARD_BLUE GPIO_Pin_15
#define BOARD_LEDS BOARD_RED | BOARD_GREEN | BOARD_BLUE

#define LEFT_BUTTON GPIO_Pin_0
#define RIGHT_BUTTON GPIO_Pin_1

#define PERIOD 1600 
#define PRESCALER 1680

#define DEBOUNCE_DELAY 200

const int leds[] = { RED, GREEN, BLUE };
const int brightness[] = { 0, 80, 160, 320, 800, 1200, 1600 };
int currentBrightnesses[] = { 0, 0, 0 };
int currentColor;

int left_button_delay, right_button_delay;

void initLedsAndButton(void);
void initTIM(void);
void initOC(void);
void initNVIC(void);
void initEXTI(void);
void TIM2_IRQHandler(void);
void EXTI0_IRQHandler(void);
void EXTI1_IRQHandler(void);
void SysTick_Handler(void);
void lightLeds(void);

int main(void) {
    currentColor = 0;
    left_button_delay = 0;
    right_button_delay = 0;

    initTIM();
    initOC();
    initEXTI();
    initNVIC();
    initLedsAndButton();

    while (1) {
    }
}

void initLedsAndButton(void) {
    GPIO_InitTypeDef gpio_struct;
    
    // Init leds
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);
    
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource8, GPIO_AF_TIM1);
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource9, GPIO_AF_TIM1);
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource10, GPIO_AF_TIM1);
    gpio_struct.GPIO_Pin = LEDS;
    gpio_struct.GPIO_Mode = GPIO_Mode_AF;
    gpio_struct.GPIO_OType = GPIO_OType_PP;
    gpio_struct.GPIO_Speed = GPIO_Speed_100MHz;
    gpio_struct.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Init(GPIOA, &gpio_struct);

    gpio_struct.GPIO_Pin = BOARD_LEDS;
    gpio_struct.GPIO_Mode = GPIO_Mode_OUT;
    gpio_struct.GPIO_Speed = GPIO_Speed_100MHz;
    gpio_struct.GPIO_OType = GPIO_OType_PP;
    gpio_struct.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Init(GPIOD, &gpio_struct);

    GPIO_ResetBits(GPIOD, BOARD_LEDS);
    GPIO_SetBits(GPIOD, BOARD_RED);

    // Init buttons
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);
    gpio_struct.GPIO_Pin = LEFT_BUTTON | RIGHT_BUTTON;
    gpio_struct.GPIO_Mode = GPIO_Mode_IN;
    gpio_struct.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_Init(GPIOE, &gpio_struct);
}

void initTIM(void) {
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);
    TIM_TimeBaseInitTypeDef tim_struct;
    tim_struct.TIM_Period = (uint16_t)(PERIOD - 1);
    tim_struct.TIM_Prescaler = (uint16_t)(PRESCALER - 1);
    tim_struct.TIM_ClockDivision = 0;
    tim_struct.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM1, &tim_struct);
    TIM_CtrlPWMOutputs(TIM1, ENABLE);
    TIM_Cmd(TIM1, ENABLE);

    SysTick_Config(SystemCoreClock / 1000);
}

void initOC(void) {
    TIM_OCInitTypeDef oc_struct;
    oc_struct.TIM_OCMode = TIM_OCMode_PWM1;
    oc_struct.TIM_OutputState = TIM_OutputState_Enable;
    oc_struct.TIM_OCPolarity = TIM_OCPolarity_Low;
    oc_struct.TIM_Pulse = brightness[0];

    TIM_OC1Init(TIM1, &oc_struct);
    TIM_OC1PreloadConfig(TIM1, TIM_OCPreload_Enable);
    TIM_OC2Init(TIM1, &oc_struct);
    TIM_OC2PreloadConfig(TIM1, TIM_OCPreload_Enable);
    TIM_OC3Init(TIM1, &oc_struct);
    TIM_OC3PreloadConfig(TIM1, TIM_OCPreload_Enable);
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
            uint8_t currentBrightness = currentBrightnesses[currentColor] + 1;
            currentBrightness %= 7;
            currentBrightnesses[currentColor] = currentBrightness;
            lightLeds();
        }
    }
    EXTI_ClearITPendingBit(EXTI_Line0);
}

void EXTI1_IRQHandler(void) {
    if (EXTI_GetITStatus(EXTI_Line1) != RESET) {
        if (right_button_delay > DEBOUNCE_DELAY) {
            right_button_delay = 0;
            currentColor++;
            currentColor %= 3;
            lightLeds();
        }
    }
    EXTI_ClearITPendingBit(EXTI_Line1);
}

void SysTick_Handler(void) {
    left_button_delay++;
    right_button_delay++;
}

void lightLeds(void) {
    uint8_t currentBrightness = currentBrightnesses[currentColor];
    switch (currentColor) {
        case 0:
            TIM_SetCompare1(TIM1, brightness[currentBrightness]);
            GPIO_ResetBits(GPIOD, BOARD_BLUE);
            GPIO_SetBits(GPIOD, BOARD_RED);
            break;
        case 1:
            TIM_SetCompare2(TIM1, brightness[currentBrightness]);
            GPIO_ResetBits(GPIOD, BOARD_RED);
            GPIO_SetBits(GPIOD, BOARD_GREEN);
            break;
        case 2:
            TIM_SetCompare3(TIM1, brightness[currentBrightness]);
            GPIO_ResetBits(GPIOD, BOARD_GREEN);
            GPIO_SetBits(GPIOD, BOARD_BLUE);
            break;
        }
}
