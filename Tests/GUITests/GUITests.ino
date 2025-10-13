// Libraries
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include "secrets.h"
#include <set>
#include <TFT_eSPI.h>

// const char* SSID = "DOEGuest";
// const char* PASSWORD = "NYC$itevent";
const char* TIME_API = "http://worldclockapi.com/api/json/est/now";
const char* WEATHER_API = "https://api.open-meteo.com/v1/forecast?latitude=40.6501&longitude=-73.9496&hourly=temperature_2m,rain,showers&timezone=America%2FNew_York&forecast_days=3";
TFT_eSPI tft = TFT_eSPI();
String screenStartText = "Hi, I am Screen!";

String getCurrentTime() {
  // Create HTTP object and set URL
  HTTPClient http;
  http.begin(TIME_API);
  // Get HTTP code to later verify output (200 = success, 429 = too many requests, etc)
  int httpCode = http.GET();
  // Serial.println(httpCode);
  if (httpCode == HTTP_CODE_OK) {
    // Set all JSON into string
    String stringDataTime = http.getString();
    // Make dict variable 'organizedTimeData' with allocated 1024 bytes
    StaticJsonDocument<1024> organizedTimeData;
    // Parse JSON string into dict variable
    deserializeJson(organizedTimeData, stringDataTime);

    // "2025-09-20T11:59-04:00"
    String dateAndTime = organizedTimeData["currentDateTime"];

    // Find the spot of 'T" and make a substring of everything from after it
    int tIndex = dateAndTime.indexOf('T');
    String onlyTime = dateAndTime.substring(tIndex + 1);

    // Find the spot of '+' in the previous substring, if doesn't exist, look for spot of '-'
    // This rules out the timezone difference part
    int tzIndex = onlyTime.indexOf('+');
    if (tzIndex == -1) {
      tzIndex = onlyTime.indexOf('-');
    }
    // Turn 'onlyTime' back to another substring of itself, but lowered from the beginning till right before timezone difference part
    onlyTime = onlyTime.substring(0, tzIndex);
    Serial.println(onlyTime);

    return onlyTime;
  } else {
    Serial.println("Issue with Time HTTP");
    Serial.println(httpCode);
  }
  http.end();
  // Return function incase of error
  return "";
}

class Weather {
  // Private variables only
  private:
    DynamicJsonDocument organizedWeatherData;

  // Everything accessible from outside
  public:
    // Constructor + initilization list
    Weather() : organizedWeatherData(9000) {

    } 

    void fetchWeatherData() {
      HTTPClient http;
      http.begin(WEATHER_API);
      int httpCode = http.GET();
      if (httpCode == HTTP_CODE_OK){
        String stringDataWeather = http.getString();
        deserializeJson(organizedWeatherData, stringDataWeather);
      } else {  
        Serial.println("Issue with Weather HTTP");
      }
      http.end();
    }
    
    void printNext24Hours() {
      for (int i = 0; i < 24; i++) {
        String hourTime = organizedWeatherData["hourly"]["time"][i];
        float temperature = organizedWeatherData["hourly"]["temperature_2m"][i];
        float rain = organizedWeatherData["hourly"]["rain"][i];
        float showers = organizedWeatherData["hourly"]["showers"][i];

        Serial.println(hourTime);
        Serial.print(" Temp: "); Serial.print(temperature);
        Serial.print(" Rain%: "); Serial.println((rain + showers) * 100);
      }
    }
    
};

class Alarms {
  private:
    std::set<String> alarmSet;
    // int alarmCount = 0;

  public: 
    Alarms() {    }

    void addAlarm(String time) {
      auto result = alarmSet.insert(time);
      if (result.second) {
        Serial.println("Successfuly added alarm at " + time);
        // alarmCount++;
      } else {
        Serial.println("Already exists");
      }
    }

    void removeAlarm(String time) {
      auto erased = alarmSet.erase(time);
      if (erased == 1) {
        Serial.println("Alarm erased");
      } else {
        Serial.println("Alarm not found!");
      }
    }

    bool checkAlarm(String time) {
      for (const auto& alarmTime : alarmSet) {
        if (alarmTime == time) {
          return true;
        }
      }
      return false;
    }
};

class SpecificTimeTest {
  private: 
    String _time; 
  
  public:
    SpecificTimeTest() {
      _time = "12:50";
      
    }

    // Getter
    String time() {
      return _time;
    }

    // Normal class functions
    bool isTimeYet(String currentTime) {
      return currentTime == time();
    }
  
};

class PrintOnScreen {
  private:
    TFT_eSPI &tftRef;  // Reference to TFT object for easier OOP handling

    // Helper: convert 8-bit RGB to 16-bit color
    inline uint16_t color565(uint8_t r, uint8_t g, uint8_t b) {
      return ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3);
    }

    // Convert 565 -> RGB
    void rgbFrom565(uint16_t c, uint8_t &r, uint8_t &g, uint8_t &b) {
      r = ((c >> 11) & 0x1F) * 255 / 31;
      g = ((c >> 5) & 0x3F) * 255 / 63;
      b = (c & 0x1F) * 255 / 31;
    }

    // Draw vertical gradient
    void drawVerticalGradient(uint16_t topColor, uint16_t bottomColor) {
      int H = tftRef.height();
      uint8_t r0, g0, b0, r1, g1, b1;
      rgbFrom565(topColor, r0, g0, b0);
      rgbFrom565(bottomColor, r1, g1, b1);

      for (int y = 0; y < H; y++) {
        float v = (H == 1) ? 0.0f : (float)y / (H - 1);
        uint8_t r = (uint8_t)((1.0f - v) * r0 + v * r1);
        uint8_t g = (uint8_t)((1.0f - v) * g0 + v * g1);
        uint8_t b = (uint8_t)((1.0f - v) * b0 + v * b1);
        tftRef.drawFastHLine(0, y, tftRef.width(), color565(r, g, b));
      }
    }

  public:
    PrintOnScreen(TFT_eSPI &tft) : tftRef(tft) {}

    void printBackground() {
      // Gradient background
      uint16_t top = color565(8, 30, 90);
      uint16_t bottom = color565(2, 6, 24);
      drawVerticalGradient(top, bottom);

      // Header bar
      int headerH = max(36, tftRef.height() / 12);
      int pad = 8;
      uint16_t headerColor = color565(18, 60, 140);
      tftRef.fillRoundRect(0, 0, tftRef.width(), headerH + 4, 8, headerColor);

      // Tiny accent line under header
      tftRef.fillRect(0, headerH + 4, tftRef.width(), 2, color565(255, 255, 255));

      // Header text
      tftRef.setTextSize(2);
      tftRef.setTextColor(TFT_WHITE);
      tftRef.setCursor(pad, 8);
      tftRef.print("SMART CLOCK");

      // Small label on right
      String rightLabel = "Design Challenge";
      int txtW = rightLabel.length() * 6 * 2; // approximate
      tftRef.setCursor(tftRef.width() - txtW - pad, 8);
      tftRef.print(rightLabel);

      // Bottom decoration
      tftRef.fillRect(0, tftRef.height() - 6, tftRef.width(), 6, color565(0, 0, 0));
    }

    void printTime(String time) {
      int w = tftRef.width() - 40;
      int h = max(80, tftRef.height() / 5);
      int x = (tftRef.width() - w) / 2;
      int y = (tftRef.height() - h) / 2 + 10;
      int r = 14;

      // Shadow
      tftRef.fillRoundRect(x + 6, y + 6, w, h, r, color565(0, 0, 0));

      // Card background
      uint16_t cardColor = color565(10, 32, 80);
      tftRef.fillRoundRect(x, y, w, h, r, cardColor);

      // Inner highlight
      tftRef.fillRoundRect(x + 6, y + 6, w - 12, 18, r / 2, color565(28, 80, 170));

      // Small label
      tftRef.setTextSize(2);
      tftRef.setTextColor(color565(200, 220, 255));
      tftRef.setCursor(x + 16, y + 10);
      tftRef.print("TIME");

      // Large time text
      int textSizeLarge = 5;
      int charW = 6 * textSizeLarge;
      int textW = time.length() * charW;
      int tx = x + (w - textW) / 2;
      int ty = y + (h / 2) - (8 * textSizeLarge / 2) + 6;

      // Shadow
      tftRef.setTextSize(textSizeLarge);
      tftRef.setTextColor(color565(0, 0, 0));
      tftRef.setCursor(tx + 2, ty + 3);
      tftRef.print(time);

      // Foreground
      tftRef.setTextColor(TFT_WHITE);
      tftRef.setCursor(tx, ty);
      tftRef.print(time);

      // Hint
      tftRef.setTextSize(1);
      tftRef.setTextColor(color565(180, 200, 255));
      tftRef.setCursor(x + 16, y + h - 18);
      tftRef.print("Tap to change format");
    }
};


// const int LED_PIN = 2;
// const int BUTTON_PIN = 13;
// const int BUZZER_PIN = 14;
SpecificTimeTest specificTimeTest;
Weather weather;
Alarms alarms;

void setup() {
  // Baud #
  Serial.begin(115200);

  // pinMode(LED_PIN, OUTPUT);

  // Connect to WiFi and wait till connection
  WiFi.begin(SSID, PASSWORD);
  Serial.println("Connecting..");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  // Provide IP address and verify connection
  Serial.println("Connected!");
  Serial.println("IP Address: ");
  Serial.print(WiFi.localIP());

  // Screen stuff
  Serial.println("Waiting 3 seconds before starting (screen)");

  // Manual screen reset
  // pinMode(TFT_RST, OUTPUT);
  // digitalWrite(TFT_RST, LOW);
  // delay(50);
  // digitalWrite(TFT_RST, HIGH);
  // delay(50);

  // Screen initilaization 
  tft.init();
  tft.setRotation(1);
  tft.fillScreen(TFT_BLACK);

  // Screen text popup
  tft.setTextColor(TFT_WHITE);
  tft.setTextSize(2);
  tft.setCursor(10, 10);
  tft.println(screenStartText);
  


  // Get started (time takes only 5 seconds so no need)
  weather.fetchWeatherData();
  weather.printNext24Hours();
  
  // use "button" mode for button
  // pinMode(BUTTON_PIN, INPUT_PULLUP);
  // // Initialize buzzer
  // pinMode(BUZZER_PIN, OUTPUT);
  // digitalWrite(BUZZER_PIN, LOW);
}

// Weather flag
unsigned long lastWeatherUpdate = 0;
const unsigned long weatherInterval = 3600000; // 1 hour in milliseconds
// Time flag
const unsigned int timeInterval = 5000;
unsigned long lastTimeUpdate = 0;

PrintOnScreen screenPrinting(tft); // pass TFT object reference

void loop() {
  String currentTime = "";
  if (millis() - lastTimeUpdate > timeInterval) {
  currentTime = getCurrentTime();
  Serial.println(currentTime);
  lastTimeUpdate = millis();
  
  // Screen Stuff (Time)

  // tft.fillScreen(TFT_BLACK);
  // tft.setCursor(10, 10);
  // tft.print("Time: ");
  // tft.println(currentTime);
  screenPrinting.printBackground();
  screenPrinting.printTime(currentTime);
  }

  // if (specificTimeTest.isTimeYet(currentTime)) {
  //   digitalWrite(LED_PIN, HIGH);
  // } else {
  //   digitalWrite(LED_PIN, LOW);
  // }

  if ((millis() - lastWeatherUpdate) > weatherInterval) {
    weather.fetchWeatherData();
    weather.printNext24Hours();
    lastWeatherUpdate = millis();
  }

  String timeInput = "";
  if (Serial.available() > 0) {
    char c = Serial.read();
    if (c == '|') {
      Serial.println("Please enter time then press enter: ");
      while (true) {
        if (Serial.available() > 0) {
          char t = Serial.read();
          if (t == '\n') break;
          if (t != '\r') timeInput += t;
        }
      }
      alarms.addAlarm(timeInput);
    }
  }
  

  // ALL What's below to be changed !!! 
  
  // if (alarms.checkAlarm(currentTime)) {
  //   digitalWrite(LED_PIN, HIGH);
  // }

  // int buttonState = digitalRead(BUTTON_PIN);
  // if (buttonState == LOW) {
  //   Serial.println("Button Pressed!");
  //   digitalWrite(BUZZER_PIN, LOW);
  // } else {
  //   digitalWrite(BUZZER_PIN, HIGH);
  // }
  
  


}

/*

*/





// For later
/*


bool alarmAcknowledged = false; // global

bool isAlarmTime = alarms.checkAlarmTime();

if (isAlarmTime && !alarmAcknowledged) {
    digitalWrite(buzzerPin, HIGH);
} else {
    digitalWrite(buzzerPin, LOW);
}

if (buttonPressed && isAlarmTime) {
    alarmAcknowledged = true;
    digitalWrite(buzzerPin, LOW); // turn off immediately
}

// Reset acknowledgment when alarm ends
if (!isAlarmTime) {
    alarmAcknowledged = false;
}


*/

