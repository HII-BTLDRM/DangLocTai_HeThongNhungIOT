#include <stdint.h>
#include <stm32f401re_gpio.h>
#include <stm32f401re_rcc.h>

// --- ĐỊNH NGHĨA CHÂN CẮM ---
#define BUTTON_PRESS             0

#define BUTTON_GPIO_PORT         GPIOB
#define BUTTON_GPIO_PIN          GPIO_Pin_3

#define LED_GPIO_PORT            GPIOA
#define LED_GPIO_PIN             GPIO_Pin_11

#define BUZZER_GPIO_PORT         GPIOC
#define BUZZER_GPIO_PIN          GPIO_Pin_9

// --- ÉP XUNG NHỊP 8MHz ---
void SystemClock_Config_8MHz(void) {
    RCC_HCLKConfig(RCC_SYSCLK_Div2);
}

// --- KHỞI TẠO NGOẠI VI TÁCH RỜI (An toàn 100%) ---
void mainInit(void) {
    GPIO_InitTypeDef GPIO_InitStruct;

    // 1. Cấp clock TÁCH RỜI cho từng Port (Tránh lỗi thư viện)
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);

    // 2. Cấu hình Nút nhấn (PB3)
    GPIO_InitStruct.GPIO_Pin = BUTTON_GPIO_PIN;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_Init(BUTTON_GPIO_PORT, &GPIO_InitStruct);

    // 3. Cấu hình LED (PA11)
    GPIO_InitStruct.GPIO_Pin = LED_GPIO_PIN;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_DOWN;
    GPIO_Init(LED_GPIO_PORT, &GPIO_InitStruct);

    // 4. Cấu hình Còi (PC9)
    GPIO_InitStruct.GPIO_Pin = BUZZER_GPIO_PIN;
    GPIO_Init(BUZZER_GPIO_PORT, &GPIO_InitStruct);
}

// ==========================================
// CHƯƠNG TRÌNH CHÍNH
// ==========================================
int main(void) {
    SystemClock_Config_8MHz();
    mainInit();

    // Tắt LED và Còi lúc mới bật
    GPIO_ResetBits(LED_GPIO_PORT, LED_GPIO_PIN);
    GPIO_ResetBits(BUZZER_GPIO_PORT, BUZZER_GPIO_PIN);

    while (1) {
        // Đọc trạng thái nút
        if (GPIO_ReadInputDataBit(BUTTON_GPIO_PORT, BUTTON_GPIO_PIN) == BUTTON_PRESS) {
            // Nút bị nhấn -> Bật
            GPIO_SetBits(LED_GPIO_PORT, LED_GPIO_PIN);
            GPIO_SetBits(BUZZER_GPIO_PORT, BUZZER_GPIO_PIN);
        } else {
            // Nút nhả ra -> Tắt
            GPIO_ResetBits(LED_GPIO_PORT, LED_GPIO_PIN);
            GPIO_ResetBits(BUZZER_GPIO_PORT, BUZZER_GPIO_PIN);
        }
    }
    return 0;
}
