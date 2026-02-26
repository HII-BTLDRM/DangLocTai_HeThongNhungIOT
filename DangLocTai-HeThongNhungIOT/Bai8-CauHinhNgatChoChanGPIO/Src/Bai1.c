#include "stm32f401re_rcc.h"
#include "stm32f401re_gpio.h"
#include "stm32f401re_exti.h"
#include "stm32f401re_syscfg.h"
#include "misc.h"

// --- ĐỊNH NGHĨA CHÂN CẮM ---
#define LED_PORT                 GPIOA
#define LED_PIN                  GPIO_Pin_5

#define BUTTON_PORT              GPIOC
#define BUTTON_PIN               GPIO_Pin_13

// Biến toàn cục nhận tín hiệu từ ngắt
volatile uint8_t Status = 0;

// --- HÀM ÉP XUNG 8MHz (Cho Proteus) ---
void SystemClock_Config_8MHz(void) {
    RCC_HCLKConfig(RCC_SYSCLK_Div2);
}

// --- HÀM DELAY XẤP XỈ MILI-GIÂY ---
void delay_ms(volatile uint32_t ms) {
    for (volatile uint32_t i = 0; i < (ms * 1000); i++);
}

// --- BƯỚC 3: KHỞI TẠO LED (PA5) ---
void Led_Init(void) {
    GPIO_InitTypeDef GPIO_InitStructure;
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);

    GPIO_InitStructure.GPIO_Pin = LED_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;

    GPIO_Init(LED_PORT, &GPIO_InitStructure);
}

// --- BƯỚC 4: KHỞI TẠO NGẮT (PC13) ---
void Interrupt_Init(void) {
    GPIO_InitTypeDef GPIO_InitStructure;
    EXTI_InitTypeDef EXTI_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    // 4.2 Cấp clock và Khởi tạo chân PC13
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
    GPIO_InitStructure.GPIO_Pin = BUTTON_PIN;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(BUTTON_PORT, &GPIO_InitStructure);

    // 4.3 Cấp clock SYSCFG và Kết nối Line 13 với PC13
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
    SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOC, EXTI_PinSource13);

    // 4.4 Cấu hình EXTI Line 13
    EXTI_InitStructure.EXTI_Line = EXTI_Line13;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising_Falling; // Nhận ngắt ở cả 2 sườn (bấm và nhả) theo đúng PDF
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure);

    // 4.4 Cấu hình NVIC
    NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}

// --- BƯỚC 5: TRÌNH PHỤC VỤ NGẮT ---
void EXTI15_10_IRQHandler(void) {
    // Kiểm tra xem ngắt có đúng là từ Line 13 hay không
    if(EXTI_GetITStatus(EXTI_Line13) != RESET) {

        Status = 1; // Đánh dấu là đã có sự kiện ngắt xảy ra

        // QUAN TRỌNG: Lệnh xóa cờ ngắt phải nằm bên TRONG khối lệnh if này
        EXTI_ClearITPendingBit(EXTI_Line13);
    }
}

// ==========================================
// CHƯƠNG TRÌNH CHÍNH
// ==========================================
int main(void) {
    SystemClock_Config_8MHz();
    Led_Init();
    Interrupt_Init();

    // Bước 7: Tắt đèn khi mới cấp điện
    GPIO_ResetBits(LED_PORT, LED_PIN);

    // Bước 8: Xử lý logic trong vòng lặp vô tận
    while(1) {

        // Nếu ngắt xảy ra, biến Status sẽ bị hàm ngắt đổi thành 1
        if (Status == 1) {

            // 1. Bật đèn LED
            GPIO_SetBits(LED_PORT, LED_PIN);

            // 2. Chờ một khoảng thời gian ngắn
            delay_ms(500);

            // 3. Tắt đèn trở lại như ban đầu (Đúng yêu cầu đề bài)
            GPIO_ResetBits(LED_PORT, LED_PIN);

            // 4. Trả lại biến Status về 0 để chờ lần nhấn nút tiếp theo
            Status = 0;
        }
    }

    return 0;
}
