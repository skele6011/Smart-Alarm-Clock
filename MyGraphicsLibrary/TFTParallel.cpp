#include "TFTParallel.h"

// --- Basic ILI commands used (common) ---
#define ILI_CMD_NOP     0x00
#define ILI_CMD_SWRESET 0x01
#define ILI_CMD_SLPOUT  0x11
#define ILI_CMD_DISPON  0x29
#define ILI_CMD_CASET   0x2A
#define ILI_CMD_PASET   0x2B
#define ILI_CMD_RAMWR   0x2C
#define ILI_CMD_COLMOD  0x3A
#define ILI_CMD_MADCTL  0x36
#define ILI_CMD_INVON   0x21
#define ILI_CMD_INVOFF  0x20

// -----------------------------------------------------------------------------
// Full 5x7 font, ASCII 32..126: 95 chars * 5 bytes
// Source: Adafruit GLCD 5x7 style (common)
const uint8_t TFTParallel::font5x7[][5] = {
  {0x00,0x00,0x00,0x00,0x00}, // ' ' 32
  {0x00,0x00,0x5F,0x00,0x00}, // '!' 33
  {0x00,0x07,0x00,0x07,0x00}, // '"' 34
  {0x14,0x7F,0x14,0x7F,0x14}, // '#' 35
  {0x24,0x2A,0x7F,0x2A,0x12}, // '$' 36
  {0x23,0x13,0x08,0x64,0x62}, // '%' 37
  {0x36,0x49,0x55,0x22,0x50}, // '&' 38
  {0x00,0x05,0x03,0x00,0x00}, // ''' 39
  {0x00,0x1C,0x22,0x41,0x00}, // '(' 40
  {0x00,0x41,0x22,0x1C,0x00}, // ')' 41
  {0x14,0x08,0x3E,0x08,0x14}, // '*' 42
  {0x08,0x08,0x3E,0x08,0x08}, // '+' 43
  {0x00,0x50,0x30,0x00,0x00}, // ',' 44
  {0x08,0x08,0x08,0x08,0x08}, // '-' 45
  {0x00,0x60,0x60,0x00,0x00}, // '.' 46
  {0x20,0x10,0x08,0x04,0x02}, // '/' 47
  {0x3E,0x51,0x49,0x45,0x3E}, // '0' 48
  {0x00,0x42,0x7F,0x40,0x00}, // '1' 49
  {0x42,0x61,0x51,0x49,0x46}, // '2' 50
  {0x21,0x41,0x45,0x4B,0x31}, // '3' 51
  {0x18,0x14,0x12,0x7F,0x10}, // '4' 52
  {0x27,0x45,0x45,0x45,0x39}, // '5' 53
  {0x3C,0x4A,0x49,0x49,0x30}, // '6' 54
  {0x01,0x71,0x09,0x05,0x03}, // '7' 55
  {0x36,0x49,0x49,0x49,0x36}, // '8' 56
  {0x06,0x49,0x49,0x29,0x1E}, // '9' 57
  {0x00,0x36,0x36,0x00,0x00}, // ':' 58
  {0x00,0x56,0x36,0x00,0x00}, // ';' 59
  {0x08,0x14,0x22,0x41,0x00}, // '<' 60
  {0x14,0x14,0x14,0x14,0x14}, // '=' 61
  {0x00,0x41,0x22,0x14,0x08}, // '>' 62
  {0x02,0x01,0x51,0x09,0x06}, // '?' 63
  {0x32,0x49,0x79,0x41,0x3E}, // '@' 64
  {0x7E,0x11,0x11,0x11,0x7E}, // 'A' 65
  {0x7F,0x49,0x49,0x49,0x36}, // 'B' 66
  {0x3E,0x41,0x41,0x41,0x22}, // 'C' 67
  {0x7F,0x41,0x41,0x22,0x1C}, // 'D' 68
  {0x7F,0x49,0x49,0x49,0x41}, // 'E' 69
  {0x7F,0x09,0x09,0x09,0x01}, // 'F' 70
  {0x3E,0x41,0x49,0x49,0x7A}, // 'G' 71
  {0x7F,0x08,0x08,0x08,0x7F}, // 'H' 72
  {0x00,0x41,0x7F,0x41,0x00}, // 'I' 73
  {0x20,0x40,0x41,0x3F,0x01}, // 'J' 74
  {0x7F,0x08,0x14,0x22,0x41}, // 'K' 75
  {0x7F,0x40,0x40,0x40,0x40}, // 'L' 76
  {0x7F,0x02,0x0C,0x02,0x7F}, // 'M' 77
  {0x7F,0x04,0x08,0x10,0x7F}, // 'N' 78
  {0x3E,0x41,0x41,0x41,0x3E}, // 'O' 79
  {0x7F,0x09,0x09,0x09,0x06}, // 'P' 80
  {0x3E,0x41,0x51,0x21,0x5E}, // 'Q' 81
  {0x7F,0x09,0x19,0x29,0x46}, // 'R' 82
  {0x46,0x49,0x49,0x49,0x31}, // 'S' 83
  {0x01,0x01,0x7F,0x01,0x01}, // 'T' 84
  {0x3F,0x40,0x40,0x40,0x3F}, // 'U' 85
  {0x1F,0x20,0x40,0x20,0x1F}, // 'V' 86
  {0x3F,0x40,0x38,0x40,0x3F}, // 'W' 87
  {0x63,0x14,0x08,0x14,0x63}, // 'X' 88
  {0x07,0x08,0x70,0x08,0x07}, // 'Y' 89
  {0x61,0x51,0x49,0x45,0x43}, // 'Z' 90
  {0x00,0x7F,0x41,0x41,0x00}, // '[' 91
  {0x02,0x04,0x08,0x10,0x20}, // '\' 92
  {0x00,0x41,0x41,0x7F,0x00}, // ']' 93
  {0x04,0x02,0x01,0x02,0x04}, // '^' 94
  {0x40,0x40,0x40,0x40,0x40}, // '_' 95
  {0x00,0x01,0x02,0x04,0x00}, // '`' 96
  {0x20,0x54,0x54,0x54,0x78}, // 'a' 97
  {0x7F,0x48,0x44,0x44,0x38}, // 'b' 98
  {0x38,0x44,0x44,0x44,0x20}, // 'c' 99
  {0x38,0x44,0x44,0x48,0x7F}, // 'd' 100
  {0x38,0x54,0x54,0x54,0x18}, // 'e' 101
  {0x08,0x7E,0x09,0x01,0x02}, // 'f' 102
  {0x0C,0x52,0x52,0x52,0x3E}, // 'g' 103
  {0x7F,0x08,0x04,0x04,0x78}, // 'h' 104
  {0x00,0x44,0x7D,0x40,0x00}, // 'i' 105
  {0x20,0x40,0x44,0x3D,0x00}, // 'j' 106
  {0x7F,0x10,0x28,0x44,0x00}, // 'k' 107
  {0x00,0x41,0x7F,0x40,0x00}, // 'l' 108
  {0x7C,0x04,0x18,0x04,0x78}, // 'm' 109
  {0x7C,0x08,0x04,0x04,0x78}, // 'n' 110
  {0x38,0x44,0x44,0x44,0x38}, // 'o' 111
  {0x7C,0x14,0x14,0x14,0x08}, // 'p' 112
  {0x08,0x14,0x14,0x18,0x7C}, // 'q' 113
  {0x7C,0x08,0x04,0x04,0x08}, // 'r' 114
  {0x48,0x54,0x54,0x54,0x20}, // 's' 115
  {0x04,0x3F,0x44,0x40,0x20}, // 't' 116
  {0x3C,0x40,0x40,0x20,0x7C}, // 'u' 117
  {0x1C,0x20,0x40,0x20,0x1C}, // 'v' 118
  {0x3C,0x40,0x30,0x40,0x3C}, // 'w' 119
  {0x44,0x28,0x10,0x28,0x44}, // 'x' 120
  {0x0C,0x50,0x50,0x50,0x3C}, // 'y' 121
  {0x44,0x64,0x54,0x4C,0x44}, // 'z' 122
  {0x00,0x08,0x36,0x41,0x00}, // '{' 123
  {0x00,0x00,0x7F,0x00,0x00}, // '|' 124
  {0x00,0x41,0x36,0x08,0x00}, // '}' 125
  {0x02,0x01,0x02,0x04,0x02}  // '~' 126
};

// ---------------------------- Internal helpers / constructor ----------------
TFTParallel::TFTParallel(uint16_t w, uint16_t h)
  : width(w), height(h), rotation(0), textColor(0xFFFF), textBg(0x0000), textSize(1),
    inTransaction(false), dataMaskLow(0), dataMaskHigh(0), fastPathAvailable(false)
{
  // zero-initialize mask tables (they will be filled if prepareFastPath runs)
  memset(dataPinMaskPerValueLow, 0, sizeof(dataPinMaskPerValueLow));
  memset(dataPinMaskPerValueHigh, 0, sizeof(dataPinMaskPerValueHigh));
}

// Platform-specific includes for fast ESP32 register access
#if defined(ESP32)
  #include "soc/gpio_struct.h"
  #include "driver/gpio.h"
#endif

void TFTParallel::initPins() {
  // Data pins
  pinMode(TFT_D0, OUTPUT);
  pinMode(TFT_D1, OUTPUT);
  pinMode(TFT_D2, OUTPUT);
  pinMode(TFT_D3, OUTPUT);
  pinMode(TFT_D4, OUTPUT);
  pinMode(TFT_D5, OUTPUT);
  pinMode(TFT_D6, OUTPUT);
  pinMode(TFT_D7, OUTPUT);

  // Control pins
  pinMode(TFT_CS, OUTPUT);
  pinMode(TFT_DC, OUTPUT);
  pinMode(TFT_RST, OUTPUT);
  pinMode(TFT_WR, OUTPUT);
  pinMode(TFT_RD, OUTPUT);

  // Default states
  digitalWrite(TFT_CS, HIGH);
  digitalWrite(TFT_DC, HIGH);
  digitalWrite(TFT_WR, HIGH);
  digitalWrite(TFT_RD, HIGH);
  digitalWrite(TFT_RST, HIGH);

  // Try to prepare fast path (ESP32 specific). If it fails, fastPathAvailable remains false.
  prepareFastPath();
}

// ----------------------------- Fast path preparation ------------------------
void TFTParallel::prepareFastPath() {
#if defined(ESP32)
  // Build masks for pins <32 and >=32
  uint32_t maskAllLow = 0;
  uint32_t maskAllHigh = 0;

  // map each data pin to a bit in either low or high mask
  const int datPins[8] = {TFT_D0, TFT_D1, TFT_D2, TFT_D3, TFT_D4, TFT_D5, TFT_D6, TFT_D7};
  for (int bit = 0; bit < 8; ++bit) {
    int p = datPins[bit];
    if (p < 0) { // invalid pin
      continue;
    }
    if (p < 32) {
      maskAllLow |= (1u << p);
    } else {
      // For out1 registers, bits correspond to pin-32 positions
      maskAllHigh |= (1u << (p - 32));
    }
  }

  dataMaskLow = maskAllLow;
  dataMaskHigh = maskAllHigh;

  // Precompute per-value masks:
  // For a byte value v, compute which GPIO pins must be set (out1/out)
  for (int v = 0; v < 256; ++v) {
    uint32_t setLow = 0;
    uint32_t setHigh = 0;
    for (int bit = 0; bit < 8; ++bit) {
      if (v & (1 << bit)) {
        int p = datPins[bit];
        if (p < 32) setLow |= (1u << p);
        else setHigh |= (1u << (p - 32));
      }
    }
    dataPinMaskPerValueLow[v] = setLow;
    dataPinMaskPerValueHigh[v] = setHigh;
  }

  // Mark fast path available — we will use direct register writes using GPIO struct
  fastPathAvailable = true;
#else
  fastPathAvailable = false;
#endif
}

// ----------------------------- low-level: write bus -------------------------
inline void TFTParallel::writeBus(uint8_t val) {
  // Two code paths:
  // - fastPathAvailable (ESP32) uses direct GPIO registers to clear/set the exact bits for the data pins.
  // - fallback uses digitalWrite on each data pin.

  if (fastPathAvailable) {
#if defined(ESP32)
    // Clear all data bits and then set the ones needed for this value.
    // Clearing: write 1s to out_w1tc for pins that we will clear (i.e., those in dataMask but not in new set)
    uint32_t setLow  = dataPinMaskPerValueLow[val];
    uint32_t setHigh = dataPinMaskPerValueHigh[val];

    // Compute clear masks for out and out1 registers
    uint32_t clearLow  = dataMaskLow & (~setLow);
    uint32_t clearHigh = dataMaskHigh & (~setHigh);

    // Clear first
    if (clearLow) {
      GPIO.out_w1tc = clearLow;
    }
    if (clearHigh) {
      // out1 registers sometimes expose .val; many cores accept direct assignment. Use .val to be safe.
      GPIO.out1_w1tc.val = clearHigh;
    }

    // Set desired bits
    if (setLow) {
      GPIO.out_w1ts = setLow;
    }
    if (setHigh) {
      GPIO.out1_w1ts.val = setHigh;
    }
#endif
  } else {
    // Portable fallback — individual pin writes (slower)
    digitalWrite(TFT_D0, (val >> 0) & 1);
    digitalWrite(TFT_D1, (val >> 1) & 1);
    digitalWrite(TFT_D2, (val >> 2) & 1);
    digitalWrite(TFT_D3, (val >> 3) & 1);
    digitalWrite(TFT_D4, (val >> 4) & 1);
    digitalWrite(TFT_D5, (val >> 5) & 1);
    digitalWrite(TFT_D6, (val >> 6) & 1);
    digitalWrite(TFT_D7, (val >> 7) & 1);
  }
}

// Pulse WR (active low). Keep it short — controller needs a small pulse to latch data
inline void TFTParallel::pulseWR() {
  // active low write strobe
  digitalWrite(TFT_WR, LOW);
  // a few nops to meet timing
  asm volatile("nop\n\tnop\n\tnop\n\t");
  digitalWrite(TFT_WR, HIGH);
}

// ----------------------------- Commands & Data -----------------------------
void TFTParallel::writeCommand(uint8_t cmd) {
  if (!inTransaction) {
    // single command mode: toggle CS around the byte
    digitalWrite(TFT_DC, LOW);
    digitalWrite(TFT_CS, LOW);
    writeBus(cmd);
    pulseWR();
    digitalWrite(TFT_CS, HIGH);
  } else {
    // in a transaction: assume CS low and caller controls DC
    digitalWrite(TFT_DC, LOW);
    writeBus(cmd);
    pulseWR();
    digitalWrite(TFT_DC, HIGH); // restore data mode by default
  }
}

void TFTParallel::writeData(uint8_t data) {
  if (!inTransaction) {
    digitalWrite(TFT_DC, HIGH);
    digitalWrite(TFT_CS, LOW);
    writeBus(data);
    pulseWR();
    digitalWrite(TFT_CS, HIGH);
  } else {
    // caller has CS low
    digitalWrite(TFT_DC, HIGH);
    writeBus(data);
    pulseWR();
  }
}

void TFTParallel::writeData16(uint16_t data) {
  writeData((uint8_t)(data >> 8));
  writeData((uint8_t)(data & 0xFF));
}

void TFTParallel::pushColor(uint16_t color) {
  // simple two writeData calls (respects inTransaction)
  writeData((uint8_t)(color >> 8));
  writeData((uint8_t)(color & 0xFF));
}

// ----------------------------- Address window -------------------------------
// setAddrWindow writes CASET/PASET/RAMWR in the simple (non-rotated) coordinate space.
// Use setAddrWindowRotated() for logical coords (it maps to physical coords depending on rotation).
void TFTParallel::setAddrWindow(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1) {
  writeCommand(ILI_CMD_CASET);
  writeData((uint8_t)(x0 >> 8));
  writeData((uint8_t)(x0 & 0xFF));
  writeData((uint8_t)(x1 >> 8));
  writeData((uint8_t)(x1 & 0xFF));

  writeCommand(ILI_CMD_PASET);
  writeData((uint8_t)(y0 >> 8));
  writeData((uint8_t)(y0 & 0xFF));
  writeData((uint8_t)(y1 >> 8));
  writeData((uint8_t)(y1 & 0xFF));

  writeCommand(ILI_CMD_RAMWR);
}

// Rotation-aware address mapping. This will leave CS low and DC set to DATA (caller should stream pixels then call endWrite()).
void TFTParallel::setAddrWindowRotated(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1) {
  // Map logical (x0,y0,x1,y1) to physical controller coords according to rotation.
  // rawWidth/rawHeight are the physical display dims in portrait mode.
  const uint16_t rawW = TFT_WIDTH_DEFAULT;
  const uint16_t rawH = TFT_HEIGHT_DEFAULT;

  uint16_t tx0 = x0, ty0 = y0, tx1 = x1, ty1 = y1;

  switch (rotation) {
    case 0:
      // nothing
      break;
    case 1:
      // (x,y) -> (y, rawW - 1 - x)
      tx0 = y0;
      ty0 = (uint16_t)(rawW - 1 - x1);
      tx1 = y1;
      ty1 = (uint16_t)(rawW - 1 - x0);
      break;
    case 2:
      // (x,y) -> (rawW -1 - x, rawH -1 - y)
      tx0 = (uint16_t)(rawW - 1 - x1);
      ty0 = (uint16_t)(rawH - 1 - y1);
      tx1 = (uint16_t)(rawW - 1 - x0);
      ty1 = (uint16_t)(rawH - 1 - y0);
      break;
    case 3:
      // (x,y) -> (rawH -1 - y, x)
      tx0 = (uint16_t)(rawH - 1 - y1);
      ty0 = x0;
      tx1 = (uint16_t)(rawH - 1 - y0);
      ty1 = x1;
      break;
    default:
      break;
  }

  // Write CASET/PASET/RAMWR while keeping CS low for streaming.
  digitalWrite(TFT_DC, LOW);
  digitalWrite(TFT_CS, LOW);
  writeBus(ILI_CMD_CASET);
  pulseWR();

  digitalWrite(TFT_DC, HIGH);
  writeBus((uint8_t)(tx0 >> 8)); pulseWR();
  writeBus((uint8_t)(tx0 & 0xFF)); pulseWR();
  writeBus((uint8_t)(tx1 >> 8)); pulseWR();
  writeBus((uint8_t)(tx1 & 0xFF)); pulseWR();

  digitalWrite(TFT_DC, LOW);
  writeBus(ILI_CMD_PASET);
  pulseWR();

  digitalWrite(TFT_DC, HIGH);
  writeBus((uint8_t)(ty0 >> 8)); pulseWR();
  writeBus((uint8_t)(ty0 & 0xFF)); pulseWR();
  writeBus((uint8_t)(ty1 >> 8)); pulseWR();
  writeBus((uint8_t)(ty1 & 0xFF)); pulseWR();

  digitalWrite(TFT_DC, LOW);
  writeBus(ILI_CMD_RAMWR);
  pulseWR();

  // Leave DC = HIGH (data) and CS LOW for subsequent writes
  digitalWrite(TFT_DC, HIGH);
}

// ----------------------------- Transactions & streaming --------------------
void TFTParallel::startWrite() {
  inTransaction = true;
  digitalWrite(TFT_CS, LOW);
}

void TFTParallel::endWrite() {
  digitalWrite(TFT_CS, HIGH);
  inTransaction = false;
}

// Write single byte while in transaction; DC should be set by caller (or can be set right before).
void TFTParallel::write8(uint8_t v) {
  // assume CS already low
  writeBus(v);
  pulseWR();
}

// Write 16-bit value MSB then LSB while in transaction.
void TFTParallel::write16(uint16_t v) {
  writeBus((uint8_t)(v >> 8));
  pulseWR();
  writeBus((uint8_t)(v & 0xFF));
  pulseWR();
}

// Push a repeated color value len times while CS is low (fast)
void TFTParallel::pushColorN(uint16_t color, uint32_t len) {
  // caller should have DC HIGH and CS low (startWrite called)
  uint8_t hi = (uint8_t)(color >> 8);
  uint8_t lo = (uint8_t)(color & 0xFF);

  // small optimization: write both bytes per loop
  for (uint32_t i = 0; i < len; ++i) {
    writeBus(hi); pulseWR();
    writeBus(lo); pulseWR();
  }
}

// ----------------------------- Public drawing API ---------------------------
void TFTParallel::begin() {
  initPins();
  reset();
  ili9486_init();
  setRotation(rotation);
  fillScreen(0x0000);
}

void TFTParallel::reset() {
  digitalWrite(TFT_RST, LOW);
  delay(20);
  digitalWrite(TFT_RST, HIGH);
  delay(150);
}

void TFTParallel::ili9486_init() {
  // Minimal init sequence for ILI family displays (may need expansion for some modules)
  writeCommand(ILI_CMD_SWRESET);
  delay(150);

  writeCommand(ILI_CMD_SLPOUT);
  delay(120);

  writeCommand(ILI_CMD_COLMOD); writeData(0x55); // 16-bit color (RGB565)
  delay(10);

  writeCommand(ILI_CMD_MADCTL); writeData(0x48); // default; setRotation will update it
  delay(10);

  writeCommand(ILI_CMD_DISPON);
  delay(100);
}

void TFTParallel::setRotation(uint8_t r) {
  rotation = r & 3;
  uint8_t madctl = 0;

  // these MADCTL values are common starting points; you may need to tweak for BGR/RGB
  switch (rotation) {
    case 0:
      madctl = 0x48; // MX, BGR (example)
      width = TFT_WIDTH_DEFAULT;
      height = TFT_HEIGHT_DEFAULT;
      break;
    case 1:
      madctl = 0x28; // MV, BGR
      width = TFT_HEIGHT_DEFAULT;
      height = TFT_WIDTH_DEFAULT;
      break;
    case 2:
      madctl = 0x88; // MY, BGR
      width = TFT_WIDTH_DEFAULT;
      height = TFT_HEIGHT_DEFAULT;
      break;
    case 3:
      madctl = 0xE8; // MX+MY+MV, BGR
      width = TFT_HEIGHT_DEFAULT;
      height = TFT_WIDTH_DEFAULT;
      break;
  }

  startWrite();
  digitalWrite(TFT_DC, LOW);
  writeBus(ILI_CMD_MADCTL);
  pulseWR();
  digitalWrite(TFT_DC, HIGH);
  writeBus(madctl);
  pulseWR();
  endWrite();
}

void TFTParallel::invertDisplay(bool inv) {
  if (inv) writeCommand(ILI_CMD_INVON);
  else writeCommand(ILI_CMD_INVOFF);
}

void TFTParallel::fillScreen(uint16_t color) {
  fillRect(0, 0, width, height, color);
}

void TFTParallel::drawPixel(int16_t x, int16_t y, uint16_t color) {
  if (x < 0 || y < 0 || x >= (int16_t)width || y >= (int16_t)height) return;

  startWrite();
  setAddrWindowRotated((uint16_t)x, (uint16_t)y, (uint16_t)x, (uint16_t)y);
  // now CS low & DC high
  write16(color);
  endWrite();
}

void TFTParallel::drawLine(int x0, int y0, int x1, int y1, uint16_t color) {
  // Bresenham
  int dx = abs(x1 - x0), sx = x0 < x1 ? 1 : -1;
  int dy = -abs(y1 - y0), sy = y0 < y1 ? 1 : -1;
  int err = dx + dy, e2;

  while (true) {
    drawPixel(x0, y0, color);
    if (x0 == x1 && y0 == y1) break;
    e2 = 2 * err;
    if (e2 >= dy) { err += dy; x0 += sx; }
    if (e2 <= dx) { err += dx; y0 += sy; }
  }
}

void TFTParallel::drawRect(int x, int y, int w, int h, uint16_t color) {
  if (w <= 0 || h <= 0) return;
  drawLine(x, y, x + w - 1, y, color);
  drawLine(x, y + h - 1, x + w - 1, y + h - 1, color);
  drawLine(x, y, x, y + h - 1, color);
  drawLine(x + w - 1, y, x + w - 1, y + h - 1, color);
}

void TFTParallel::fillRect(int x, int y, int w, int h, uint16_t color) {
  if (w <= 0 || h <= 0) return;

  // clip
  if (x < 0) { w += x; x = 0; }
  if (y < 0) { h += y; y = 0; }
  if ((x + w) > (int)width) w = width - x;
  if ((y + h) > (int)height) h = height - y;
  if (w <= 0 || h <= 0) return;

  uint32_t pixels = (uint32_t)w * (uint32_t)h;

  // Use transaction streaming for speed
  startWrite();
  setAddrWindowRotated((uint16_t)x, (uint16_t)y, (uint16_t)(x + w - 1), (uint16_t)(y + h - 1));
  // After setAddrWindowRotated: CS low, DC high
  pushColorN(color, pixels);
  endWrite();
}

void TFTParallel::drawCircle(int x0, int y0, int r, uint16_t color) {
  int f = 1 - r;
  int ddF_x = 1;
  int ddF_y = -2 * r;
  int x = 0;
  int y = r;

  drawPixel(x0, y0 + r, color);
  drawPixel(x0, y0 - r, color);
  drawPixel(x0 + r, y0, color);
  drawPixel(x0 - r, y0, color);

  while (x < y) {
    if (f >= 0) {
      y--;
      ddF_y += 2;
      f += ddF_y;
    }
    x++;
    ddF_x += 2;
    f += ddF_x;

    drawPixel(x0 + x, y0 + y, color);
    drawPixel(x0 - x, y0 + y, color);
    drawPixel(x0 + x, y0 - y, color);
    drawPixel(x0 - x, y0 - y, color);
    drawPixel(x0 + y, y0 + x, color);
    drawPixel(x0 - y, y0 + x, color);
    drawPixel(x0 + y, y0 - x, color);
    drawPixel(x0 - y, y0 - x, color);
  }
}

void TFTParallel::fillCircle(int x0, int y0, int r, uint16_t color) {
  drawLine(x0, y0 - r, x0, y0 + r, color);
  int f = 1 - r;
  int ddF_x = 1;
  int ddF_y = -2 * r;
  int x = 0;
  int y = r;

  while (x < y) {
    if (f >= 0) {
      y--;
      ddF_y += 2;
      f += ddF_y;
    }
    x++;
    ddF_x += 2;
    f += ddF_x;

    drawLine(x0 - x, y0 - y, x0 - x, y0 + y, color);
    drawLine(x0 + x, y0 - y, x0 + x, y0 + y, color);
    drawLine(x0 - y, y0 - x, x0 - y, y0 + x, color);
    drawLine(x0 + y, y0 - x, x0 + y, y0 + x, color);
  }
}

// ------------------------------ Text (fast streaming) -----------------------
void TFTParallel::drawChar(int16_t x, int16_t y, char c) {
  if (c < 32 || c > 126) c = '?'; // fallback
  const uint8_t* glyph = font5x7[(uint8_t)c - 32];

  // character box: 5 columns + 1 spacing, height 8
  int charW = 5 * textSize;
  int charH = 8 * textSize;

  // prepare buffer by writing address window and streaming pixels
  startWrite();
  setAddrWindowRotated((uint16_t)x, (uint16_t)y, (uint16_t)(x + charW + textSize - 1), (uint16_t)(y + charH - 1));
  // After setAddrWindowRotated: CS low, DC high and ready to receive pixel data

  // For each column (left to right)
  for (uint8_t col = 0; col < 5; ++col) {
    uint8_t colByte = glyph[col];
    for (uint8_t row = 0; row < 8; ++row) {
      bool pixelOn = (colByte & (1 << row)) != 0;
      uint16_t useColor = pixelOn ? textColor : textBg;
      if (textSize == 1) {
        write16(useColor);
      } else {
        // expand each pixel into textSize x textSize block
        for (uint8_t ys = 0; ys < textSize; ++ys) {
          for (uint8_t xs = 0; xs < textSize; ++xs) {
            write16(useColor);
          }
        }
      }
    }
  }

  // spacing column (background)
  for (int r = 0; r < charH; ++r) write16(textBg);
  endWrite();
}

void TFTParallel::drawString(int16_t x, int16_t y, const char* str) {
  int16_t cx = x;
  int16_t cy = y;
  while (*str) {
    if (*str == '\n') {
      cy += 8 * textSize;
      cx = x;
    } else {
      drawChar(cx, cy, *str);
      cx += (6 * textSize); // 5 + 1 spacing
    }
    ++str;
  }
}

// -------------------------------- Utilities ---------------------------------
uint16_t TFTParallel::color565(uint8_t r, uint8_t g, uint8_t b) {
  return ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3);
}
