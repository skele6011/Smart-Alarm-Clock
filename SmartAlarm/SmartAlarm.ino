// Libraries
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include "secrets.h"

// const char* SSID = "___";
// const char* PASSWORD = "___";
const char* TIME_API = "http://worldclockapi.com/api/json/est/now";
const char* WEATHER_API = "https://api.open-meteo.com/v1/forecast?latitude=40.6501&longitude=-73.9496&hourly=temperature_2m,rain,showers&timezone=America%2FNew_York&forecast_days=3";

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
    // New
    static const int MAX_ALARMS = 10;
    String alarms[MAX_ALARMS];
    int alarmCount = 0;

  public: 
    Alarms() {
      
    }

    void addAlarm(String time) {
      if (alarmCount >= MAX_ALARMS) {
        Serial.println("Failed to add alarm: Too many alarms already");
      } else {
        alarms[alarmCount] = time;
        Serial.println("Alarm added");
        alarmCount++;
      }
    }

    void removeAlarm(String time) {
      bool found = false;
      
      for (int i = 0; i < alarmCount; i++) {
        if (alarms[i] == time) {
          found = true;

        }
      }

      if (!found) {
        Serial.println("Alarm not found.");
      }
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

const int LED_PIN = 2;
SpecificTimeTest specificTimeTest;
Weather weather;

void setup() {
  // Baud #
  Serial.begin(115200);

  pinMode(LED_PIN, OUTPUT);

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

  // Get started (time takes only 5 seconds so no need)
  weather.fetchWeatherData();
  weather.printNext24Hours();
}

// Weather flag
unsigned long lastWeatherUpdate = 0;
const unsigned long weatherInterval = 3600000; // 1 hour in milliseconds

void loop() {
  String currentTime = getCurrentTime();
  Serial.println(currentTime);

  if (specificTimeTest.isTimeYet(currentTime)) {
    digitalWrite(LED_PIN, HIGH);
  } else {
    digitalWrite(LED_PIN, LOW);
  }

  if ((millis() - lastWeatherUpdate) > weatherInterval) {
    weather.fetchWeatherData();
    weather.printNext24Hours();
    lastWeatherUpdate = millis();
  }


delay(5000);
}