#include <stdint.h>
#include <stm32f401re_gpio.h>
#include <stm32f401re_rcc.h>

// ==========================================
// ĐỊNH NGHĨA MACRO CHÂN CẮM (Theo Kit Lumi)
// ==========================================
#define BUTTON_PRESS             0   // Mức 0 do dùng trở kéo lên (Pull-up)

// Nút nhấn B2
#define BUTTON_GPIO_PORT         GPIOB
#define BUTTON_GPIO_PIN          GPIO_Pin_3
#define BUTTON_CLOCK             RCC_AHB1Periph_GPIOB

// Đèn LED GREEN
#define LED_GPIO_PORT            GPIOA
#define LED_GPIO_PIN             GPIO_Pin_11
#define LED_CLOCK                RCC_AHB1Periph_GPIOA

// Còi BUZZER
#define BUZZER_GPIO_PORT         GPIOC
#define BUZZER_GPIO_PIN          GPIO_Pin_9
#define BUZZER_CLOCK             RCC_AHB1Periph_GPIOC

// ==========================================
// HÀM KHỞI TẠO HỆ THỐNG
// ==========================================
// Cấu hình xung nhịp 8MHz để chạy mượt trên Proteus
void SystemClock_Config_8MHz(void) {
    RCC_HCLKConfig(RCC_SYSCLK_Div2);
}

// Hàm khởi tạo toàn bộ ngoại vi (Gộp chung để code gọn gàng)
void mainInit(void) {
    GPIO_InitTypeDef GPIO_InitStructure;

    // 1. Cấp xung nhịp (Clock) cho cả 3 bến cảng (Port A, B, C) cùng lúc
    RCC_AHB1PeriphClockCmd(BUTTON_CLOCK | LED_CLOCK | BUZZER_CLOCK, ENABLE);

    // 2. Cấu hình Nút nhấn B2 (Input, có trở kéo lên Pull-up)
    GPIO_InitStructure.GPIO_Pin = BUTTON_GPIO_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_Init(BUTTON_GPIO_PORT, &GPIO_InitStructure);

    // 3. Cấu hình LED GREEN (Output, Kéo xuống GND mặc định)
    GPIO_InitStructure.GPIO_Pin = LED_GPIO_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;
    GPIO_Init(LED_GPIO_PORT, &GPIO_InitStructure);

    // 4. Cấu hình Còi BUZZER (Output, Kéo xuống GND mặc định)
    GPIO_InitStructure.GPIO_Pin = BUZZER_GPIO_PIN;
    // Tái sử dụng lại các thông số Mode, OType, PuPd đã cài ở trên cho lẹ
    GPIO_Init(BUZZER_GPIO_PORT, &GPIO_InitStructure);
}

// ==========================================
// CHƯƠNG TRÌNH CHÍNH
// ==========================================
int main(void) {
    // 1. Khởi tạo xung nhịp và ngoại vi
    SystemClock_Config_8MHz();
    mainInit();

    // 2. Trạng thái an toàn: Tắt LED và Còi khi vừa cấp điện
    GPIO_ResetBits(LED_GPIO_PORT, LED_GPIO_PIN);
    GPIO_ResetBits(BUZZER_GPIO_PORT, BUZZER_GPIO_PIN);

    // 3. Vòng lặp vô tận (Trái tim hệ thống)
    while (1) {
        // Đọc trạng thái nút nhấn B2 bằng hàm SPL chuẩn
        if (GPIO_ReadInputDataBit(BUTTON_GPIO_PORT, BUTTON_GPIO_PIN) == BUTTON_PRESS) {

            // Nếu nút ĐANG BỊ NHẤN -> Bật LED và Bật Còi
            GPIO_SetBits(LED_GPIO_PORT, LED_GPIO_PIN);
            GPIO_SetBits(BUZZER_GPIO_PORT, BUZZER_GPIO_PIN);

        } else {

            // Nếu nút ĐƯỢC NHẢ RA -> Tắt LED và Tắt Còi
            GPIO_ResetBits(LED_GPIO_PORT, LED_GPIO_PIN);
            GPIO_ResetBits(BUZZER_GPIO_PORT, BUZZER_GPIO_PIN);

        }
    }

    return 0;
}
