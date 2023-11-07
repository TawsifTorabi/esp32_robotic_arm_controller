//AsyncTCP, WebServer library from me-no-dev
//ESP32Servo from kevin harrington, jhon k. bennett
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <ESP32Servo.h> 

const char *ssid = "inferno";
const char *password = "dAN_BR0WNinferno";

int servo1pin = 13;
int servo2pin = 12;
int servo3pin = 14;
int servo4pin = 27;
int servo5pin = 26;
int servo6pin = 25;

Servo servo1, servo2, servo3, servo4, servo5, servo6;

int servo1pos = 90;
int servo2pos = 90;
int servo3pos = 90;
int servo4pos = 90;
int servo5pos = 90;
int servo6pos = 90;

AsyncWebServer server(80);
AsyncWebSocket ws("/remote");

unsigned long lastMessageTime = 0;
const unsigned long messageTimeout = 500; // 2 seconds

void notifyClients() {
  ws.textAll(String("Notification"));
}

void handleWebSocketMessage(void *arg, uint8_t *data, size_t len) {
    lastMessageTime = millis(); // Reset the timeout timer
    AwsFrameInfo *info = (AwsFrameInfo *)arg;
    if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT) {
        data[len] = 0;
        const char *message = (char *)data;
        Serial.println(message);

        if (strncmp(message, "servo:1,value:", 14) == 0) {
            int servoValue = atoi(message + 14);
            servo1.write(servoValue);
            notifyClients();
        } else if (strncmp(message, "servo:2,value:", 14) == 0) {
            int servoValue = atoi(message + 14);
            servo2.write(servoValue);
            notifyClients();
        } else if (strncmp(message, "servo:3,value:", 14) == 0) {
            int servoValue = atoi(message + 14);
            servo3.write(servoValue);
            notifyClients();
        } else if (strncmp(message, "servo:4,value:", 14) == 0) {
            int servoValue = atoi(message + 14);
            servo4.write(servoValue);
            notifyClients();
        } else if (strncmp(message, "servo:5,value:", 14) == 0) {
            int servoValue = atoi(message + 14);
            servo5.write(servoValue);
            notifyClients();
        } else if (strncmp(message, "servo:6,value:", 14) == 0) {
            int servoValue = atoi(message + 14);
            servo6.write(servoValue);
            notifyClients();
        }
    }
}

void onEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type,
             void *arg, uint8_t *data, size_t len) {
  switch (type) {
    case WS_EVT_CONNECT:
      Serial.printf("WebSocket client #%u connected from %s\n", client->id(), client->remoteIP().toString().c_str());
      break;
    case WS_EVT_DISCONNECT:
      Serial.printf("WebSocket client #%u disconnected\n", client->id());
      break;
    case WS_EVT_DATA:
      handleWebSocketMessage(arg, data, len);
      break;
    case WS_EVT_PONG:
    case WS_EVT_ERROR:
      break;
  }
}

void initWebSocket() {
  ws.onEvent(onEvent);
  server.addHandler(&ws);
}

void setup() {
  Serial.begin(115200);

  // Allow allocation of all timers
  ESP32PWM::allocateTimer(0);
  ESP32PWM::allocateTimer(1);
  ESP32PWM::allocateTimer(2);
  ESP32PWM::allocateTimer(3);

  servo1.setPeriodHertz(50);
  servo2.setPeriodHertz(50);
  servo3.setPeriodHertz(50);
  servo4.setPeriodHertz(50);
  servo5.setPeriodHertz(50);
  servo6.setPeriodHertz(50);

  servo1.attach(servo1pin, 500, 2400);
  servo2.attach(servo2pin, 500, 2400);
  servo3.attach(servo3pin, 500, 2400);
  servo4.attach(servo4pin, 1000, 2000);
  servo5.attach(servo5pin, 1000, 2000);
  servo6.attach(servo6pin, 1000, 2000);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }

  Serial.println("Connected to WiFi");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  initWebSocket();

  server.begin();

  Serial.println("Configuring Watch Dog Timer...");
}

void loop() {
  ws.cleanupClients();
}
