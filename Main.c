#include <WiFi.h>
#include <HTTPClient.h>
#include <Wire.h>

// Pin Definitions
#define TRIGGER_PIN 7
#define ECHO_PIN 6
#define LED_PIN 13

// Wi-Fi Credentials
const char *ssid = "Your_SSID";
const char *password = "Your_PASSWORD";

// GoPro IP Address and YOLO Server Endpoint
const char *goproIP = "192.168.1.100";   // Replace with GoPro IP
const char *yoloServer = "http://<YOLO_SERVER_IP>:5000/detect";

// Constants
#define DISTANCE_THRESHOLD 50 // Distance threshold in cm for an action
#define SENSOR_READING_DELAY 100
#define MAX_QUEUE_SIZE 20     // Circular buffer size for distance readings

// ESP32-specific variables
WiFiClient espClient;
HTTPClient http;

// Circular Queue for Averaging Distance
volatile uint32_t distanceQueue[MAX_QUEUE_SIZE] = {0};
volatile uint8_t queueStart = 0;
volatile uint8_t queueEnd = 0;
volatile uint8_t queueSize = 0;

// Function Prototypes
void HCSR04_Init();
void HCSR04_Trigger();
uint32_t HCSR04_GetDistance();
void QueueAdd(uint32_t distance);
uint32_t QueueAverage();
void setupWiFi();
void connectToGoPro();
void sendToYOLO(uint32_t distance);
void handleThresholdAction(uint32_t distance);
void blinkLED(int times);

// Setup Function
void setup() {
    Serial.begin(115200);
    pinMode(TRIGGER_PIN, OUTPUT);
    pinMode(ECHO_PIN, INPUT);
    pinMode(LED_PIN, OUTPUT);

    Serial.println("Initializing HCSR04...");
    HCSR04_Init();

    Serial.println("Setting up Wi-Fi...");
    setupWiFi();

    Serial.println("Connecting to GoPro...");
    connectToGoPro();

    Serial.println("System Initialization Complete!");
}

// Main Loop
void loop() {
    // Trigger Ultrasonic Sensor
    HCSR04_Trigger();
    delay(SENSOR_READING_DELAY);

    // Get Distance
    uint32_t distance = HCSR04_GetDistance();
    QueueAdd(distance);

    // Calculate Average Distance
    uint32_t avgDistance = QueueAverage();
    Serial.printf("Current Distance: %d cm, Average Distance: %d cm\n", distance, avgDistance);

    // Handle Threshold Actions
    handleThresholdAction(avgDistance);

    // Send Data to YOLOv5 Server
    sendToYOLO(avgDistance);

    delay(500); // Main loop delay
}

// Function Definitions

// Initialize Ultrasonic Sensor
void HCSR04_Init() {
    digitalWrite(TRIGGER_PIN, LOW);
    delay(100);
}

// Trigger Ultrasonic Sensor
void HCSR04_Trigger() {
    digitalWrite(TRIGGER_PIN, HIGH);
    delayMicroseconds(10);
    digitalWrite(TRIGGER_PIN, LOW);
}

// Get Distance from Ultrasonic Sensor
uint32_t HCSR04_GetDistance() {
    long duration = pulseIn(ECHO_PIN, HIGH);
    return (duration * 0.034) / 2;
}

// Add Distance to Circular Queue
void QueueAdd(uint32_t distance) {
    if (queueSize < MAX_QUEUE_SIZE) {
        distanceQueue[queueEnd] = distance;
        queueEnd = (queueEnd + 1) % MAX_QUEUE_SIZE;
        queueSize++;
    } else {
        distanceQueue[queueStart] = distance;
        queueStart = (queueStart + 1) % MAX_QUEUE_SIZE;
        queueEnd = (queueEnd + 1) % MAX_QUEUE_SIZE;
    }
}

// Calculate Average Distance from Queue
uint32_t QueueAverage() {
    uint32_t sum = 0;
    for (uint8_t i = 0; i < queueSize; i++) {
        sum += distanceQueue[i];
    }
    return queueSize > 0 ? (sum / queueSize) : 0;
}

// Setup Wi-Fi Connection
void setupWiFi() {
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("\nWi-Fi Connected!");
    Serial.printf("IP Address: %s\n", WiFi.localIP().toString().c_str());
}

// Connect to GoPro
void connectToGoPro() {
    if (http.begin(espClient, goproIP)) {
        int httpCode = http.GET();
        if (httpCode > 0) {
            Serial.printf("GoPro Connected, Response: %d\n", httpCode);
        } else {
            Serial.printf("GoPro Connection Failed: %s\n", http.errorToString(httpCode).c_str());
        }
        http.end();
    } else {
        Serial.println("Failed to Initialize GoPro HTTP Connection");
    }
}

// Send Data to YOLOv5 Server
void sendToYOLO(uint32_t distance) {
    if (WiFi.status() == WL_CONNECTED) {
        http.begin(espClient, String(yoloServer));
        http.addHeader("Content-Type", "application/json");

        String payload = "{\"distance_cm\":" + String(distance) + "}";
        int httpCode = http.POST(payload);

        if (httpCode > 0) {
            String response = http.getString();
            Serial.printf("YOLO Response: %s\n", response.c_str());
        } else {
            Serial.printf("YOLO Server Error: %s\n", http.errorToString(httpCode).c_str());
        }
        http.end();
    } else {
        Serial.println("Wi-Fi Not Connected!");
    }
}

// Handle Actions Based on Distance Threshold
void handleThresholdAction(uint32_t distance) {
    if (distance < DISTANCE_THRESHOLD) {
        Serial.println("Threshold Reached: Activating LED Blink");
        blinkLED(3);
    }
}

// Blink LED N Times
void blinkLED(int times) {
    for (int i = 0; i < times; i++) {
        digitalWrite(LED_PIN, HIGH);
        delay(200);
        digitalWrite(LED_PIN, LOW);
        delay(200);
    }
}
