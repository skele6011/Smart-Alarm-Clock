#ifndef TFTPARALLEL_H
#define TFTPARALLEL_H

#include <Arduino.h>
#include <stdint.h>

/*
  TFTParallel — simple 8-bit parallel TFT graphics wrapper
  - Optimized for ESP32 parallel 8-bit attachments (fast direct writes)
  - Portable fallback uses digitalWrite()
  - Provides basic primitives, text (5x7), and transaction-based fast writes
*/

// --- Configure these to match your User_Setup settings (already in your paste) ---
#ifndef TFT_CS
  #define TFT_CS   33
#endif
#ifndef TFT_DC
  #define TFT_DC   15
#endif
#ifndef TFT_RST
  #define TFT_RST  32
#endif
#ifndef TFT_WR
  #define TFT_WR   4
#endif
#ifndef TFT_RD
  #define TFT_RD   2
#endif

#ifndef TFT_D0
  #define TFT_D0 12
#endif
#ifndef TFT_D1
  #define TFT_D1 13
#endif
#ifndef TFT_D2
  #define TFT_D2 26
#endif
#ifndef TFT_D3
  #define TFT_D3 25
#endif
#ifndef TFT_D4
  #define TFT_D4 17
#endif
#ifndef TFT_D5
  #define TFT_D5 16
#endif
#ifndef TFT_D6
  #define TFT_D6 27
#endif
#ifndef TFT_D7
  #define TFT_D7 14
#endif

// Display physical (raw) dimensions (portrait defaults). Adjust if your panel differs
#define TFT_WIDTH_DEFAULT  320
#define TFT_HEIGHT_DEFAULT 480

class TFTParallel {
public:
  // constructor (logical width/height default to physical portrait dims)
  TFTParallel(uint16_t w = TFT_WIDTH_DEFAULT, uint16_t h = TFT_HEIGHT_DEFAULT);

  // Initialization
  void begin();                  // initialize pins and display
  void reset();                  // hardware reset
  void setRotation(uint8_t r);   // 0..3
  void invertDisplay(bool inv);

  // Basic drawing primitives
  void fillScreen(uint16_t color);
  void drawPixel(int16_t x, int16_t y, uint16_t color);
  void drawLine(int x0, int y0, int x1, int y1, uint16_t color);
  void drawRect(int x, int y, int w, int h, uint16_t color);
  void fillRect(int x, int y, int w, int h, uint16_t color);
  void drawCircle(int x0, int y0, int r, uint16_t color);
  void fillCircle(int x0, int y0, int r, uint16_t color);

  // Text (small 5x7 font)
  void setTextColor(uint16_t c) { textColor = c; }
  void setTextBg(uint16_t bg) { textBg = bg; }
  void setTextSize(uint8_t s) { textSize = std::max<uint8_t>(1, s); }
  void drawChar(int16_t x, int16_t y, char c);
  void drawString(int16_t x, int16_t y, const char* str);

  // Fast transaction-style I/O (keep CS low while streaming pixel data)
  void startWrite();                 // CS low
  void endWrite();                   // CS high
  void write8(uint8_t v);            // write one byte while in transaction (DC should be set)
  void write16(uint16_t v);          // write 16-bit value while in transaction
  void pushColorN(uint16_t color, uint32_t len); // send 'len' repeated colors while CS low

  // Utility
  static uint16_t color565(uint8_t r, uint8_t g, uint8_t b);

  // Get display dimensions
  uint16_t getWidth() const { return width; }
  uint16_t getHeight() const { return height; }

private:
  uint16_t width, height;   // logical width/height (depends on rotation)
  uint8_t rotation;         // 0..3
  uint16_t textColor, textBg;
  uint8_t textSize;

  // low level
  void initPins();
  inline void writeBus(uint8_t val); // platform-optimized
  inline void pulseWR();
  void writeCommand(uint8_t cmd);
  void writeData(uint8_t data);
  void writeData16(uint16_t data);
  void setAddrWindow(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1); // legacy (no rotation mapping)
  void setAddrWindowRotated(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1); // rotation-aware
  void pushColor(uint16_t color);

  // controller-specific
  void ili9486_init();

  // font (5x7)
  static const uint8_t font5x7[][5];

  // transaction state
  bool inTransaction;

  // fast bus helpers (precomputed masks for ESP32 path)
  uint32_t dataMaskLow;   // bits set where data pins < 32
  uint32_t dataMaskHigh;  // bits set where data pins >= 32 (shifted to bit 0 in .out1 registers)
  uint32_t dataPinMaskPerValueLow[256];   // precomputed masks: for value v, mask bits (low 32)
  uint32_t dataPinMaskPerValueHigh[256];  // precomputed masks: for value v, mask bits for out1
  bool fastPathAvailable; // true when ESP32 direct register path prepared

  // internal helper to configure masks
  void prepareFastPath();
};

#endif // TFTPARALLEL_H
