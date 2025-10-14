#include <Arduino.h>
#include "TFTParallel.h"

// Create TFT object
TFTParallel tft(480, 320);  // width, height

#define rgb(r,g,b) tft.color565(r,g,b)

void setup() {
  Serial.begin(115200);
  tft.begin();

  tft.fillScreen(rgb(0,0,0)); // black background
  delay(500);

  drawColorBars();
  delay(1000);
  drawStaticShapes();
  delay(1000);
}

void loop() {
  animate();
  delay(20);
}

// --------------------- DEMO FUNCTIONS ---------------------

void drawColorBars() {
  uint16_t bars[] = {
    rgb(255,0,0), rgb(0,255,0), rgb(0,0,255),
    rgb(255,255,0), rgb(0,255,255), rgb(255,0,255)
  };
  int n = sizeof(bars) / sizeof(bars[0]);
  int barH = tft.getHeight() / n;
  for (int i = 0; i < n; i++) {
    int y = i * barH;
    tft.fillRect(0, y, tft.getWidth(), barH, bars[i]);
  }
}

void drawStaticShapes() {
  uint16_t white = rgb(255,255,255);
  tft.drawRect(10, 10, 100, 60, white);
  tft.fillRect(20, 20, 80, 40, rgb(100,100,255));

  tft.drawCircle(tft.getWidth() - 50, 40, 30, white);
  tft.fillCircle(tft.getWidth() - 50, 40, 18, rgb(200,50,50));

  tft.drawLine(0, tft.getHeight()/2, tft.getWidth()-1, tft.getHeight()/2, white);
}

void animate() {
  static int rx = 20, ry = 100;
  static int vx = 3, vy = 2;
  int rw = 50, rh = 50;

  // Clear previous rectangle
  tft.fillRect(rx, ry, rw, rh, rgb(0,0,0));

  rx += vx;
  ry += vy;

  if (rx < 0) { rx = 0; vx = -vx; }
  if (rx + rw > tft.getWidth()) { rx = tft.getWidth() - rw; vx = -vx; }
  if (ry < 0) { ry = 0; vy = -vy; }
  if (ry + rh > tft.getHeight()) { ry = tft.getHeight() - rh; vy = -vy; }

  tft.fillRect(rx, ry, rw, rh, rgb(50,200,50));
}
