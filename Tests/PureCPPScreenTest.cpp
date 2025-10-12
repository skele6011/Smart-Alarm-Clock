#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

// Pin definitions (change to match your wiring)
#define TFT_CS   33
#define TFT_DC   15
#define TFT_RST  32
#define TFT_WR   4
#define TFT_RD   2

#define TFT_D0   12
#define TFT_D1   13
#define TFT_D2   26
#define TFT_D3   25
#define TFT_D4   17
#define TFT_D5   16
#define TFT_D6   27
#define TFT_D7   14

// Simple macro to write 8-bit data
inline void writeData(uint8_t data) {
    // Set all data pins
    gpio_set_level(TFT_D0, (data >> 0) & 0x01);
    gpio_set_level(TFT_D1, (data >> 1) & 0x01);
    gpio_set_level(TFT_D2, (data >> 2) & 0x01);
    gpio_set_level(TFT_D3, (data >> 3) & 0x01);
    gpio_set_level(TFT_D4, (data >> 4) & 0x01);
    gpio_set_level(TFT_D5, (data >> 5) & 0x01);
    gpio_set_level(TFT_D6, (data >> 6) & 0x01);
    gpio_set_level(TFT_D7, (data >> 7) & 0x01);

    // Pulse WR low
    gpio_set_level(TFT_WR, 0);
    gpio_set_level(TFT_WR, 1);
}

// Simple reset
void tftReset() {
    gpio_set_level(TFT_RST, 0);
    vTaskDelay(50 / portTICK_PERIOD_MS);
    gpio_set_level(TFT_RST, 1);
    vTaskDelay(50 / portTICK_PERIOD_MS);
}

// Example: Fill screen with a single color (RGB565)
void fillScreen(uint16_t color) {
    // Send command: Memory Write (0x2C for ILI9486/9488)
    gpio_set_level(TFT_DC, 0); // Command
    writeData(0x2C);
    
    gpio_set_level(TFT_DC, 1); // Data
    uint8_t hi = color >> 8;
    uint8_t lo = color & 0xFF;

    for (int i = 0; i < 320*480; ++i) { // Adjust to screen resolution
        writeData(hi);
        writeData(lo);
    }
}

extern "C" void app_main() {
    // Configure all GPIOs
    gpio_config_t io_conf;
    io_conf.mode = GPIO_MODE_OUTPUT;
    io_conf.pin_bit_mask = ((1ULL<<TFT_CS)|(1ULL<<TFT_DC)|(1ULL<<TFT_RST)|(1ULL<<TFT_WR) |
                            (1ULL<<TFT_D0)|(1ULL<<TFT_D1)|(1ULL<<TFT_D2)|(1ULL<<TFT_D3)|
                            (1ULL<<TFT_D4)|(1ULL<<TFT_D5)|(1ULL<<TFT_D6)|(1ULL<<TFT_D7));
    io_conf.pull_down_en = 0;
    io_conf.pull_up_en = 0;
    io_conf.intr_type = GPIO_INTR_DISABLE;
    gpio_config(&io_conf);

    tftReset();
    gpio_set_level(TFT_CS, 0); // CS low

    while(1) {
        fillScreen(0xF800); // Red
        vTaskDelay(1000 / portTICK_PERIOD_MS);
        fillScreen(0x07E0); // Green
        vTaskDelay(1000 / portTICK_PERIOD_MS);
        fillScreen(0x001F); // Blue
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}
