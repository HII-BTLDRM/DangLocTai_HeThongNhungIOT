#include <stdint.h>
#include <stm32f401re_gpio.h>
#include <stm32f401re_rcc.h>

// --- Bước 3: Định nghĩa các macro ---
#define LOW                   0
#define BTN_PRESS             LOW

#define GPIO_PIN_SET          1
#define GPIO_PIN_RESET        0
#define GPIO_PIN_LOW          0
#define GPIO_PIN_HIGH         1

// Define chân LED RED (Kit mở rộng)
#define LEDRED_GPIO_PORT      GPIOB
#define LEDRED_GPIO_PIN       GPIO_Pin_13
#define LEDREDControl_SetClock RCC_AHB1Periph_GPIOB

// Define chân Nút nhấn B2 (Kit mở rộng)
#define BUTTON_GPIO_PORT      GPIOB
#define BUTTON_GPIO_PIN       GPIO_Pin_3
#define BUTTONControl_SetClock RCC_AHB1Periph_GPIOB

// Hàm ép xung nhịp 8MHz để Proteus chạy mượt (vẫn nên giữ)
static void SystemClock_Config_8MHz(void) {
    RCC_HCLKConfig(RCC_SYSCLK_Div2);
}

// Hàm delay cơ bản
static void delay(void) {
    for (volatile uint32_t i = 0; i < 250000; i++);
}

// --- Bước 4 & 5: Khởi tạo phần cứng ---
static void LEDRED_init(void) {
    GPIO_InitTypeDef GPIO_InitStructure;
    RCC_AHB1PeriphClockCmd(LEDREDControl_SetClock, ENABLE);

    GPIO_InitStructure.GPIO_Pin = LEDRED_GPIO_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;

    GPIO_Init(LEDRED_GPIO_PORT, &GPIO_InitStructure);
}

static void Button_Init(void) {
    GPIO_InitTypeDef GPIO_InitStructure;
    RCC_AHB1PeriphClockCmd(BUTTONControl_SetClock, ENABLE);

    GPIO_InitStructure.GPIO_Pin = BUTTON_GPIO_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;

    GPIO_Init(BUTTON_GPIO_PORT, &GPIO_InitStructure);
}

// --- Bước 6: Xây dựng hàm điều khiển LED bằng thanh ghi BSRR ---
static void LedControl_SetStatus(GPIO_TypeDef *GPIOx, uint16_t GPIO_PIN, uint8_t Status) {
    // Ghi vào nửa dưới thanh ghi BSRR để đẩy lên mức 1
    if (Status == GPIO_PIN_SET) {
        GPIOx->BSRRL = GPIO_PIN;
    }
    // Ghi vào nửa trên thanh ghi BSRR để kéo xuống mức 0
    if (Status == GPIO_PIN_RESET) {
        GPIOx->BSRRH = GPIO_PIN;
    }
}

// --- Bước 7: Xây dựng hàm đọc Nút nhấn bằng thanh ghi IDR ---
static uint8_t ButtonRead_Status(GPIO_TypeDef *GPIOx, uint16_t GPIO_PIN) {
    uint8_t Read_Pin = 0x00;
    // Dùng phép AND bit (&) để trích xuất trạng thái của riêng chân đó trong thanh ghi Input Data Register (IDR)
    if ((GPIOx->IDR & GPIO_PIN) != (uint32_t) Bit_RESET) {
        Read_Pin = (uint8_t) Bit_SET;
    } else {
        Read_Pin = (uint8_t) Bit_RESET;
    }
    return Read_Pin;
}

// --- Bước 8: Hàm Main xử lý logic ---
int main(void) {
    uint8_t led_status = 0;

    SystemClock_Config_8MHz();

    LEDRED_init();
    Button_Init();

    // Đảm bảo LED tắt ban đầu
    LedControl_SetStatus(LEDRED_GPIO_PORT, LEDRED_GPIO_PIN, GPIO_PIN_RESET);

    while (1) {
        // Kiểm tra nút nhấn
        if (ButtonRead_Status(BUTTON_GPIO_PORT, BUTTON_GPIO_PIN) == BTN_PRESS) {
            delay(); // Chống rung

            // Xác nhận lại
            if (ButtonRead_Status(BUTTON_GPIO_PORT, BUTTON_GPIO_PIN) == BTN_PRESS) {

                led_status = !led_status;

                // Xuất trạng thái
                if (led_status == 1) {
                    LedControl_SetStatus(LEDRED_GPIO_PORT, LEDRED_GPIO_PIN, GPIO_PIN_SET);
                } else {
                    LedControl_SetStatus(LEDRED_GPIO_PORT, LEDRED_GPIO_PIN, GPIO_PIN_RESET);
                }

                // Chờ nhả nút
                while (ButtonRead_Status(BUTTON_GPIO_PORT, BUTTON_GPIO_PIN) == BTN_PRESS);
                delay(); // Chống rung nhả nút
            }
        }
    }
    return 0;
}
