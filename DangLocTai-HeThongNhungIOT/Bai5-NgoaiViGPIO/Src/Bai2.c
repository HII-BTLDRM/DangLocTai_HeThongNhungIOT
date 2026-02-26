#include <stdint.h>
#include <stm32f401re_gpio.h>
#include <stm32f401re_rcc.h>

// --- ĐỊNH NGHĨA CHÂN CẮM (Theo chuẩn Kit mở rộng Lumi) ---
#define BUZZER_PORT      GPIOC
#define BUZZER_PIN       GPIO_Pin_9

#define BUTTON_PORT      GPIOB
#define BUTTON_PIN       GPIO_Pin_5

// --- HÀM ÉP XUNG NHỊP VỀ 8MHz ---
void SystemClock_Config_8MHz(void) {
    // STM32F4 mặc định dùng dao động nội HSI 16MHz.
    // Lệnh này cấu hình bộ chia AHB chia đôi (16 / 2 = 8MHz) cho toàn hệ thống.
    RCC_HCLKConfig(RCC_SYSCLK_Div2);
}

// --- HÀM DELAY CHUẨN CHO 8MHz ---
void delay_ms(volatile uint32_t ms) {
    // Vòng lặp tính toán xấp xỉ 1 mili-giây ở xung nhịp 8MHz
    for (volatile uint32_t i = 0; i < (ms * 1000); i++);
}

// --- HÀM KHỞI TẠO CÒI (BUZZER) ---
void Buzzer_Init(void) {
    GPIO_InitTypeDef GPIO_InitStruct;
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE); // Cấp xung nhịp Port C

    GPIO_InitStruct.GPIO_Pin = BUZZER_PIN;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_DOWN;

    GPIO_Init(BUZZER_PORT, &GPIO_InitStruct);
}

// --- HÀM KHỞI TẠO NÚT NHẤN (BUTTON) ---
void Button_Init(void) {
    GPIO_InitTypeDef GPIO_InitStruct;
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE); // Cấp xung nhịp Port B

    GPIO_InitStruct.GPIO_Pin = BUTTON_PIN;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP; // Kéo lên mức cao

    GPIO_Init(BUTTON_PORT, &GPIO_InitStruct);
}

// ==========================================
// CHƯƠNG TRÌNH CHÍNH
// ==========================================
int main(void) {
    uint8_t buzzer_state = 0; // Biến lưu trạng thái còi (0: Tắt, 1: Kêu)

    // 1. Ép xung nhịp hệ thống về 8MHz ngay lập tức
    SystemClock_Config_8MHz();

    // 2. Khởi tạo phần cứng
    Buzzer_Init();
    Button_Init();

    // 3. Tắt còi khi vừa cấp điện (Dùng hàm SPL chuẩn)
    GPIO_ResetBits(BUZZER_PORT, BUZZER_PIN);

    // 4. Vòng lặp vô tận
    while (1) {
        // Đọc trạng thái chân PB5. Có trở kéo lên nên bình thường là 1, bấm vào là 0 (Bit_RESET)
        if (GPIO_ReadInputDataBit(BUTTON_PORT, BUTTON_PIN) == Bit_RESET) {

            delay_ms(20); // Chống rung lần 1 (20ms)

            // Kiểm tra lại xem nút còn đang bị bấm không
            if (GPIO_ReadInputDataBit(BUTTON_PORT, BUTTON_PIN) == Bit_RESET) {

                // Đảo trạng thái còi
                buzzer_state = !buzzer_state;

                // Xuất tín hiệu ra chân PC9 bằng hàm chuẩn SPL
                if (buzzer_state == 1) {
                    GPIO_SetBits(BUZZER_PORT, BUZZER_PIN);   // Bật còi
                } else {
                    GPIO_ResetBits(BUZZER_PORT, BUZZER_PIN); // Tắt còi
                }

                // Chờ người dùng nhả nút ra
                while (GPIO_ReadInputDataBit(BUTTON_PORT, BUTTON_PIN) == Bit_RESET) {
                    // Đứng im tại đây
                }

                delay_ms(20); // Chống rung lúc nhả nút
            }
        }
    }

    return 0;
}
