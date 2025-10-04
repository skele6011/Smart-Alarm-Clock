// Libraries
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include "secrets.h"
#include <set>

// const char* SSID = "DOEGuest";
// const char* PASSWORD = "NYC$itevent";
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

const int LED_PIN = 2;
const int BUTTON_PIN = 13;
SpecificTimeTest specificTimeTest;
Weather weather;
Alarms alarms;

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

  pinMode(BUTTON_PIN, INPUT_PULLUP);
}

// Weather flag
unsigned long lastWeatherUpdate = 0;
const unsigned long weatherInterval = 3600000; // 1 hour in milliseconds
// Time flag
const unsigned int timeInterval = 5000;
unsigned long lastTimeUpdate = 0;

void loop() {
  String currentTime;
  if (millis() - lastTimeUpdate > timeInterval) {
  currentTime = getCurrentTime();
  Serial.println(currentTime);
  lastTimeUpdate = millis();
  }

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
  
  if (alarms.checkAlarm(currentTime)) {
    digitalWrite(LED_PIN, HIGH);
  }

  int buttonState = digitalRead(BUTTON_PIN);
  if (buttonState == LOW) {
    Serial.println("Button Pressed!");
  }
  
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

