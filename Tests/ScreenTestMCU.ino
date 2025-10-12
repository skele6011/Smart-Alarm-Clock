#include <MCUFRIEND_kbv.h>
#include <Adafruit_GFX.h> // Core graphics library

// -------------------- PIN DEFINITIONS --------------------
#define TFT_RD   2    // Read strobe (can be -1 if not used)
#define TFT_WR   4    // Write strobe
#define TFT_RS   15   // DC (Data/Command)
#define TFT_CS   33   // Chip select
#define TFT_RST  32   // Reset

#define TFT_D0   12
#define TFT_D1   13
#define TFT_D2   26
#define TFT_D3   25
#define TFT_D4   17
#define TFT_D5   16
#define TFT_D6   27
#define TFT_D7   14

// -------------------- TFT OBJECT --------------------
MCUFRIEND_kbv tft;

// -------------------- SETUP --------------------
void setup() {
  Serial.begin(115200);
  Serial.println("TFT Test Starting...");

  // Detect TFT ID
  uint16_t ID = tft.readID();
  Serial.print("TFT ID = 0x");
  Serial.println(ID, HEX);

  if (ID == 0xD3D3) ID = 0x9486; // Force ID if unknown

  // Initialize TFT
  tft.begin(ID);
  tft.setRotation(1);          // Landscape mode
  tft.fillScreen(0x0000);       // Clear screen

  // Draw initial text
  tft.setTextColor(0xFFFF);
  tft.setTextSize(2);
  tft.setCursor(10, 10);
  tft.println("MCUFRIEND Test!");
}

// -------------------- LOOP --------------------
void loop() {
  // Color cycling test
  tft.fillScreen(0xF800); delay(500); // RED
  tft.fillScreen(0x07E0); delay(500); // GREEN
  tft.fillScreen(0x001F); delay(500); // BLUE
  tft.fillScreen(0xFFFF); delay(500); // WHITE
  tft.fillScreen(0x0000); delay(500); // BLACK
}

