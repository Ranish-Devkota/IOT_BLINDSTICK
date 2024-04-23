#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <LittleFS.h>
#include <TinyGPS++.h>
#include <SoftwareSerial.h>

#define trigPin 5
#define echoPin 16
#define buzzerPin 4
#define greenLedPin 0
#define redLedPin 12
#define yellowLedPin 14
#define vibratorPin 10

const char* ssid = "Assistance";  
const char* password = "1234567890";  

AsyncWebServer server(80);

int prevDistance = -1; 
bool emergencyAlert = false;
bool webPageOpen = false;
double lat_val, lng_val;

TinyGPSPlus gps;
SoftwareSerial GPS_SoftSerial(13, 15);  

unsigned long previousMillis = 0;
const long interval = 400;

void setup() {
Serial.begin(115200);
  GPS_SoftSerial.begin(9600); 
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(buzzerPin, OUTPUT);
  pinMode(vibratorPin, OUTPUT);
  pinMode(greenLedPin, OUTPUT);
  pinMode(redLedPin, OUTPUT);
  pinMode(yellowLedPin, OUTPUT);


  while (!Serial);
  Serial.println("Reading Data from GPS!");
  
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
  server.on("/location", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send_P(200, "text/plain", getFormattedLocation().c_str());
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
//    Serial.println(distance);
    return distance;
  }
  
  return prevDistance;
}

String getStatus() {
  int distance = getDistance();
  if (distance <= 20 && distance > 0) {
    return "Too Near";
  } else if (distance <= 35) {
    return "Be Careful";
  } else {
    return "Clear";
  }
}

String emergencyStatus() {
  
  if (emergencyAlert) {
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
  if (distance <= 20 && distance > 0) {
    digitalWrite(yellowLedPin, LOW);
    digitalWrite(redLedPin, HIGH);
    digitalWrite(greenLedPin, LOW);
    for(int i = 0 ; i<50 ; i++){
     
      analogWrite(vibratorPin,255);
    }
    for(int i = 0 ; i<5 ; i++){
      digitalWrite(buzzerPin, HIGH);
   
      delay(5);
      digitalWrite(buzzerPin, LOW);
      
      delay(5);
    }


  } else if (distance <= 35) {
    digitalWrite(yellowLedPin, HIGH);
    digitalWrite(redLedPin, LOW);
    digitalWrite(greenLedPin, LOW);
    for(int i = 0 ; i<25 ; i++){
     
      analogWrite(vibratorPin,180);
    }
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
    analogWrite(vibratorPin,0);
  }
}


void readgps() {
  smartDelay(1000);
  bool loc_valid;
  loc_valid = gps.location.isValid();
  if (loc_valid) {
    lat_val = gps.location.lat();
    lng_val = gps.location.lng();
  } else {
    lat_val = -1.00;
    lng_val = -1.00;
  }
  Serial.print(lat_val);
  Serial.println(lng_val);
}

String getFormattedLocation() {
  if (lat_val == -1.00 || lng_val == -1.00) {
    return "Location Not Available";
  } else {
    String lat_dir = lat_val < 0 ? "S" : "N";
    String lon_dir = lng_val < 0 ? "W" : "E";
    return String(abs(lat_val), 6) + " " + lat_dir + ", " + String(abs(lng_val), 6) + " " + lon_dir;
  }
}

void smartDelay(unsigned long ms) {
  unsigned long start = millis();
  do {
    while (GPS_SoftSerial.available()) 
      gps.encode(GPS_SoftSerial.read());
  } while (millis() - start < ms);
}
