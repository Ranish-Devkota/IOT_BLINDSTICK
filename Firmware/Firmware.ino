#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <LittleFS.h>

#define trigPin 5
#define echoPin 16
#define buzzerPin 4
#define greenLedPin 0
#define redLedPin 12
#define yellowLedPin 14
#define sw1Pin 13
#define sw2Pin 15

const char* ssid = "Assistance";  // Replace with your network SSID
const char* password = "1234567890";  // Replace with your network password

AsyncWebServer server(80);

int prevDistance = -1; // Initialize to an invalid value
bool sw1Pressed = false;
bool sw2Pressed = false;
bool emergencyAlert = false;
bool webPageOpen = false;

unsigned long previousMillis = 0;
const long interval = 400;  // Fetch data interval (400ms)

void setup() {
  Serial.begin(9600);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(buzzerPin, OUTPUT);
  pinMode(greenLedPin, OUTPUT);
  pinMode(redLedPin, OUTPUT);
  pinMode(yellowLedPin, OUTPUT);
  pinMode(sw1Pin, INPUT_PULLUP);
  pinMode(sw2Pin, INPUT_PULLUP);

  if (!LittleFS.begin()) {
    Serial.println("An Error has occurred while mounting LittleFS");
    return;
  }

  WiFi.softAP(ssid, password);
  Serial.print("AP IP address: ");
  Serial.println(WiFi.softAPIP());

  server.on("/", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(LittleFS, "/index.html");
    webPageOpen = true;
  });
  server.on("/distance", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send_P(200, "text/plain", String(getDistance()).c_str());
  });
  server.on("/status", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send_P(200, "text/plain", getStatus().c_str());
  });
    server.on("/emergency", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send_P(200, "text/plain", emergencyStatus().c_str());
  });
  
  server.on("/alert", HTTP_GET, [](AsyncWebServerRequest * request) {
    emergencyAlert = true;
  });

  server.begin();
}

void loop() {
  unsigned long currentMillis = millis();
  
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;
    
    int distance = getDistance();
    handleBuzzer(distance);
    sw1Pressed = digitalRead(sw1Pin);
    handleAlert();
  }
}

int getDistance() {
  long duration, distance;
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  duration = pulseIn(echoPin, HIGH);
  distance = duration * 0.017;
  
  if (distance > 0) {
    prevDistance = distance;
    Serial.println(distance);
    return distance;
  }
  
  return prevDistance;
}

String getStatus() {
  int distance = getDistance();
  if (distance < 20 && distance > 0) {
    return "Too Near";
  } else if (distance > 20 && distance < 35) {
    return "Be Careful";
  } else {
    return "Clear";
  }
}
String emergencyStatus() {
  
  if (digitalRead(sw1Pin)) {
    return "Emergency Alert";
  }
else
return "No Alert";
}
void handleAlert() {
  if (emergencyAlert) {
    digitalWrite(buzzerPin, HIGH);
    digitalWrite(redLedPin, HIGH);
    emergencyAlert = false;
  } else {
    digitalWrite(buzzerPin, LOW);
    digitalWrite(redLedPin, LOW);
  }

  
}

void handleBuzzer(int distance) {
  if (distance < 20 && distance > 0) {
    digitalWrite(yellowLedPin, LOW);
    digitalWrite(redLedPin, HIGH);
    digitalWrite(greenLedPin, LOW);
    for(int i = 0 ; i<5 ; i++){
      digitalWrite(buzzerPin, HIGH);
      delay(10);
      digitalWrite(buzzerPin, LOW);
      delay(10);
    }
  } else if (distance > 20 && distance < 35) {
    digitalWrite(yellowLedPin, HIGH);
    digitalWrite(redLedPin, LOW);
    digitalWrite(greenLedPin, LOW);
    for(int i = 0 ; i<5 ; i++){
      digitalWrite(buzzerPin, HIGH);
      delay(10);
      digitalWrite(buzzerPin, LOW);
      delay(10);
    }
  } else  {
    digitalWrite(yellowLedPin, LOW);
    digitalWrite(redLedPin, LOW);
    digitalWrite(greenLedPin, HIGH);
    digitalWrite(buzzerPin, LOW);
  }
}
