#include <TFT_eSPI.h>  // Make sure User_Setup.h is configured for 8-bit parallel

TFT_eSPI tft = TFT_eSPI();  // Create TFT object

void setup() {
  Serial.begin(115200);
  Serial.println("Waiting 3 seconds before starting...");
  delay(3000); // Wait 3 full seconds

  Serial.println("Initializing TFT...");

  // Manual reset (optional)
  pinMode(TFT_RST, OUTPUT);
  digitalWrite(TFT_RST, LOW);
  delay(50);
  digitalWrite(TFT_RST, HIGH);
  delay(50);

  tft.init();
  tft.setRotation(1); // Landscape orientation
  tft.fillScreen(TFT_BLACK);

  tft.setTextColor(TFT_WHITE);
  tft.setTextSize(2);
  tft.setCursor(10, 10);
  tft.println("TFT Test Started!");
}

void loop() {
  // Nothing else, just keep screen showing text
}
