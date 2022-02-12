#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <math.h>

char str[2000];
int led1 = 5;
int led2 = 22;
int led3 = 23;
int touch_pin = 13;
int sw = 12;
int ldr = 34;
int sw_state = 0;

int toilet_begin1 = 0;
int toilet_begin2 = 0;
int toilet_begin3 = 0;

int toilet_state1 = 0;
int toilet_state2 = 0;
int toilet_state3 = 0;

const char* ssid = "Pang";
const char* password = "kururu966";
const char* url_post = "https://ecourse.cpe.ku.ac.th/exceed06/api/toilet/update/";
const int _size = 2 * JSON_OBJECT_SIZE(3);

StaticJsonDocument<_size> JSONPost;
StaticJsonDocument<_size> JSONGet;

void WiFi_Connect() {
  WiFi.disconnect();
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to the WiFi network");
  Serial.print("IP Address : ");
  Serial.println(WiFi.localIP());
}

void _post() {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;

    http.begin(url_post);
    http.addHeader("Content-Type", "application/json");

    JSONPost["toilet_state1"] = toilet_state1;
    JSONPost["toilet_state2"] = toilet_state2;
    JSONPost["toilet_state3"] = toilet_state3;
    serializeJson(JSONPost, str);
    int httpCode = http.POST(str);

    if (httpCode == HTTP_CODE_OK) {
      String payload = http.getString();
      Serial.println(httpCode);
      Serial.println("POST result");
      Serial.println(payload);
    } else {
      Serial.println(httpCode);
      Serial.println("ERROR on HTTP Request");
    }
  } else {
    WiFi_Connect();
  }
  delay(100);
}

void setup() {
  pinMode(touch_pin, INPUT);
  pinMode(sw, INPUT);
  pinMode(ldr, INPUT);

  pinMode(led1, OUTPUT);
  pinMode(led2, OUTPUT);
  pinMode(led3, OUTPUT);

  digitalWrite(led1, HIGH);
  digitalWrite(led2, LOW);
  digitalWrite(led3, LOW);

  Serial.begin(115200);
  WiFi_Connect();
}

void loop() {
  if (touchRead(touch_pin) < 30) {
    delay(500);
    if (toilet_state1 == 1) {
      toilet_state1 = 0;
    }
    else {
      toilet_state1 = 1;
    }
    delay(200);
    digitalWrite(led1, !digitalRead(led1));
  }
  if (analogRead(ldr) > 2000) {
    if (toilet_state2 == 1) {
      toilet_state2 = 0;
    }
    else {
      toilet_state2 = 1;
    }
    digitalWrite(led2, !digitalRead(led2));
    delay(200);

  }

  if (sw_state == digitalRead(sw)) {
    delay(200);
    if (sw_state != digitalRead(sw)) {
      if (toilet_state3 == 1) {
        toilet_state3 = 0;
      }
      else {
        toilet_state3 = 1;
      }
      delay(200);
      digitalWrite(led3, !digitalRead(led3));
    }
  }
  if (toilet_state1 != toilet_begin1 || toilet_state2 != toilet_begin2 || toilet_state3 != toilet_begin3) {
    _post();
    toilet_begin1 = toilet_state1;
    toilet_begin2 = toilet_state2;
    toilet_begin3 = toilet_state3;
  }
}
