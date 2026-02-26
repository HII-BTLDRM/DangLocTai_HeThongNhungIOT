#include "stm32f401re_rcc.h"
#include "stm32f401re_gpio.h"
#include "stm32f401re_spi.h"
#include "stm32f401re_exti.h"
#include "misc.h"

// ==========================================
// ĐỊNH NGHĨA MACRO CHÂN CẮM VÀ DỮ LIỆU
// ==========================================
#define GPIO_PIN_SET             1
#define GPIO_PIN_RESET           0

// Macro cho Nút nhấn và LED
#define LED_GPIO_PORT            GPIOA
#define LED_GPIO_PIN             GPIO_Pin_5
#define BUTTON_GPIO_PORT         GPIOC
#define BUTTON_GPIO_PIN          GPIO_Pin_13

// Macro cho SPI Master (SPI2)
#define SPI_Master               SPI2
#define SPI_Master_Clock         RCC_APB1Periph_SPI2
#define SPI_Master_GPIO          GPIOB
#define SPI_Master_GPIO_Clock    RCC_AHB1Periph_GPIOB
#define NSS_Master               GPIO_Pin_12
#define SCK_Master               GPIO_Pin_13
#define MISO_Master              GPIO_Pin_14
#define MOSI_Master              GPIO_Pin_15

// Macro cho SPI Slave (SPI1)
#define SPI_Slave                SPI1
#define SPI_Slave_Clock          RCC_APB2Periph_SPI1
#define SPI_Slave_GPIO_A         GPIOA
#define SPI_Slave_GPIO_B         GPIOB
#define SPI_Slave_GPIO_Clock_A   RCC_AHB1Periph_GPIOA
#define SPI_Slave_GPIO_Clock_B   RCC_AHB1Periph_GPIOB
#define SCK_Slave                GPIO_Pin_3  // Nằm ở Port B
#define NSS_Slave                GPIO_Pin_4  // Nằm ở Port A
#define MISO_Slave               GPIO_Pin_6  // Nằm ở Port A
#define MOSI_Slave               GPIO_Pin_7  // Nằm ở Port A

// Dữ liệu kiểm tra
#define Check_DataSlave          0xB1

// Biến toàn cục lưu dữ liệu nhận được từ ngắt
volatile uint8_t Recive_Data = 0;

// ==========================================
// HÀM ÉP XUNG VÀ DELAY
// ==========================================
void SystemClock_Config_8MHz(void) {
    RCC_HCLKConfig(RCC_SYSCLK_Div2);
}

void delay_ms(volatile uint32_t ms) {
    for (volatile uint32_t i = 0; i < (ms * 1000); i++);
}

// ==========================================
// KHỞI TẠO NGOẠI VI CƠ BẢN (LED & NÚT)
// ==========================================
static void Led_Init(void) {
    GPIO_InitTypeDef GPIO_InitStructure;
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);

    GPIO_InitStructure.GPIO_Pin = LED_GPIO_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;
    GPIO_Init(LED_GPIO_PORT, &GPIO_InitStructure);
}

static void Button_Init(void) {
    GPIO_InitTypeDef GPIO_InitStructure;
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);

    GPIO_InitStructure.GPIO_Pin = BUTTON_GPIO_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_Init(BUTTON_GPIO_PORT, &GPIO_InitStructure);
}

// ==========================================
// KHỞI TẠO SPI MASTER (SPI2)
// ==========================================
static void SPI_Master_Init(void) {
    GPIO_InitTypeDef GPIO_InitStructure;
    SPI_InitTypeDef SPI_InitStructure;

    // 1. Cấp clock
    RCC_AHB1PeriphClockCmd(SPI_Master_GPIO_Clock, ENABLE);
    RCC_APB1PeriphClockCmd(SPI_Master_Clock, ENABLE);

    // 2. Cấu hình SCK, MISO, MOSI làm Alternate Function
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_InitStructure.GPIO_Pin = SCK_Master | MISO_Master | MOSI_Master;
    GPIO_Init(SPI_Master_GPIO, &GPIO_InitStructure);

    GPIO_PinAFConfig(SPI_Master_GPIO, GPIO_PinSource13, GPIO_AF_SPI2);
    GPIO_PinAFConfig(SPI_Master_GPIO, GPIO_PinSource14, GPIO_AF_SPI2);
    GPIO_PinAFConfig(SPI_Master_GPIO, GPIO_PinSource15, GPIO_AF_SPI2);

    // 3. Cấu hình chân NSS (PB12) làm Output thường để điều khiển bằng phần mềm
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_Pin = NSS_Master;
    GPIO_Init(SPI_Master_GPIO, &GPIO_InitStructure);
    GPIO_SetBits(SPI_Master_GPIO, NSS_Master); // Mặc định kéo lên CAO

    // 4. Cấu hình bộ SPI2
    SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
    SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
    SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
    SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
    SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;
    SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
    SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_4;
    SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_LSB; // Theo tài liệu PDF
    SPI_Init(SPI_Master, &SPI_InitStructure);

    // 5. Bật SPI2
    SPI_Cmd(SPI_Master, ENABLE);
}

// ==========================================
// KHỞI TẠO SPI SLAVE (SPI1)
// ==========================================
static void SPI_Slave_Init(void) {
    GPIO_InitTypeDef GPIO_InitStructure;
    SPI_InitTypeDef SPI_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    // 1. Cấp clock
    RCC_AHB1PeriphClockCmd(SPI_Slave_GPIO_Clock_A, ENABLE);
    RCC_AHB1PeriphClockCmd(SPI_Slave_GPIO_Clock_B, ENABLE);
    RCC_APB2PeriphClockCmd(SPI_Slave_Clock, ENABLE);

    // 2. Cấu hình MISO, MOSI (Port A)
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_InitStructure.GPIO_Pin = MISO_Slave | MOSI_Slave;
    GPIO_Init(SPI_Slave_GPIO_A, &GPIO_InitStructure);

    // Cấu hình SCK (Port B)
    GPIO_InitStructure.GPIO_Pin = SCK_Slave;
    GPIO_Init(SPI_Slave_GPIO_B, &GPIO_InitStructure);

    GPIO_PinAFConfig(SPI_Slave_GPIO_B, GPIO_PinSource3, GPIO_AF_SPI1);
    GPIO_PinAFConfig(SPI_Slave_GPIO_A, GPIO_PinSource6, GPIO_AF_SPI1);
    GPIO_PinAFConfig(SPI_Slave_GPIO_A, GPIO_PinSource7, GPIO_AF_SPI1);

    // Cấu hình NSS (PA4) làm Input
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_InitStructure.GPIO_Pin = NSS_Slave;
    GPIO_Init(SPI_Slave_GPIO_A, &GPIO_InitStructure);

    // 3. Cấu hình bộ SPI1
    SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
    SPI_InitStructure.SPI_Mode = SPI_Mode_Slave;
    SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
    SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
    SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;
    SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
    SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_4;
    SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_LSB;
    SPI_Init(SPI_Slave, &SPI_InitStructure);

    // 4. Cấu hình Ngắt cho SPI1
    NVIC_InitStructure.NVIC_IRQChannel = SPI1_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    SPI_ITConfig(SPI_Slave, SPI_I2S_IT_RXNE, ENABLE);

    // 5. Bật SPI1
    SPI_Cmd(SPI_Slave, ENABLE);
}

// ==========================================
// HÀM TRUYỀN DỮ LIỆU
// ==========================================
static void Send_Data(SPI_TypeDef * SPIx, uint8_t data) {
    // 1. Kéo chân NSS xuống GND cho phép truyền dữ liệu. [cite: 1795]
    GPIO_ResetBits(SPI_Master_GPIO, NSS_Master);

    // 2. Sử dụng hàm SPI_I2S_SendData để truyền dữ liệu. [cite: 1796]
    SPI_I2S_SendData(SPIx, data);

    // 3. Sử dụng hàm SPI_I2S_GetFlagStatus chờ cho đến khi dữ liệu truyền xong. [cite: 1797]
    while(SPI_I2S_GetFlagStatus(SPIx, SPI_I2S_FLAG_BSY) == SET) {;}

    // 4. Kéo chân NSS lên mức cao khi đã truyền xong dữ liệu. [cite: 1798]
    GPIO_SetBits(SPI_Master_GPIO, NSS_Master);
}

// ==========================================
// TRÌNH PHỤC VỤ NGẮT SPI1 (SLAVE NHẬN DATA)
// ==========================================
void SPI1_IRQHandler(void) {
    // Kiểm tra cờ ngắt nhận dữ liệu
    if(SPI_I2S_GetITStatus(SPI1, SPI_I2S_IT_RXNE) == SET) {

        // Gán biến nhận dữ liệu với hàm ReceiveData để thực hiện nhiệm vụ bật tắt Led. [cite: 1826]
        Recive_Data = SPI_I2S_ReceiveData(SPI1);
    }
    SPI_I2S_ClearITPendingBit(SPI1, SPI_I2S_IT_RXNE);
}

// ==========================================
// CHƯƠNG TRÌNH CHÍNH
// ==========================================
int main(void) {
    SystemClock_Config_8MHz();
    Button_Init();
    Led_Init();
    SPI_Master_Init();
    SPI_Slave_Init();

    // Tắt LED ban đầu
    GPIO_ResetBits(LED_GPIO_PORT, LED_GPIO_PIN);

    while(1) {
        // Nhấn nút PC13 để gửi dữ liệu từ SPI2 => SPI1. [cite: 1494]
        if(GPIO_ReadInputDataBit(BUTTON_GPIO_PORT, BUTTON_GPIO_PIN) == 0) {

            delay_ms(20); // Chống rung cơ học
            if(GPIO_ReadInputDataBit(BUTTON_GPIO_PORT, BUTTON_GPIO_PIN) == 0) {

                // Viết chương trình truyền dữ liệu có giá trị là 0xB1 từ SPI Master [cite: 1493]
                Send_Data(SPI_Master, Check_DataSlave);

                // Chờ nhả nút
                while(GPIO_ReadInputDataBit(BUTTON_GPIO_PORT, BUTTON_GPIO_PIN) == 0);
            }
        }

        // Nếu dữ liệu nhận được từ SPI Slave trùng với dữ liệu được truyền đi từ SPI Master [cite: 1494]
        if (Recive_Data == Check_DataSlave) {

            // Led Green trên Kit (PA5) sẽ bật tắt 5 lần rồi trở lại trạng thái ban đầu. [cite: 1494]
            for (int i = 0; i < 5; i++) {
                GPIO_SetBits(LED_GPIO_PORT, LED_GPIO_PIN);
                delay_ms(300); // Rút ngắn delay để mô phỏng Proteus đỡ bị treo
                GPIO_ResetBits(LED_GPIO_PORT, LED_GPIO_PIN);
                delay_ms(300);
            }
            // Reset biến để chờ lần bấm tiếp theo
            Recive_Data = 0;
        }
    }
    return 0;
}
