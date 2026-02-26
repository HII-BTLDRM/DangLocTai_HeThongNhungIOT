#include "stm32f401re_rcc.h"
#include "stm32f401re_gpio.h"

#define LED_PORT         GPIOA
#define LED_PIN          GPIO_Pin_5

#define BUTTON_PORT      GPIOC
#define BUTTON_PIN       GPIO_Pin_13

void delay_ms(volatile uint32_t ms) {
    for (volatile uint32_t i = 0; i < (ms * 1000); i++);
}

void SystemClock_8MHz(void) {
    RCC_HCLKConfig(RCC_SYSCLK_Div2);
}

void Led_Init(void) {
    GPIO_InitTypeDef GPIO_InitStruct;
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);

    GPIO_InitStruct.GPIO_Pin = LED_PIN;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;

    GPIO_Init(LED_PORT, &GPIO_InitStruct);
}

void Button_Init(void) {
    GPIO_InitTypeDef GPIO_InitStruct;
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);

    GPIO_InitStruct.GPIO_Pin = BUTTON_PIN;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;

    GPIO_Init(BUTTON_PORT, &GPIO_InitStruct);
}

int main(void) {
    uint8_t led_state = 0;

    SystemClock_8MHz();

    Led_Init();
    Button_Init();

    GPIO_ResetBits(LED_PORT, LED_PIN);

    while (1) {
        if (GPIO_ReadInputDataBit(BUTTON_PORT, BUTTON_PIN) == Bit_RESET) {

            delay_ms(20);

            if (GPIO_ReadInputDataBit(BUTTON_PORT, BUTTON_PIN) == Bit_RESET) {

                led_state = !led_state;

                if (led_state == 1) {
                    GPIO_SetBits(LED_PORT, LED_PIN);
                } else {
                    GPIO_ResetBits(LED_PORT, LED_PIN);
                }

                while (GPIO_ReadInputDataBit(BUTTON_PORT, BUTTON_PIN) == Bit_RESET) {

                }

                delay_ms(20);
            }
        }
    }
}
