#ifndef CONFIG_H
#define CONFIG_H

#include <Wire.h>
#include <WiFi.h>
#include <MPU6050.h>
#include <Arduino.h>
#include <WebServer.h>
#include <DNSServer.h>
#include <HTTPClient.h>
#include <Preferences.h>
#include <ArduinoJson.h>

extern MPU6050 SENSOR;

// ─── USER CONFIG ─────────────────────────────────────────────────────────────
#define BACKEND_URL "http://172.16.21.59:8000/predict"
#define SAMPLE_RATE_MS 500 // 2 Hz — must match model training rate
#define WINDOW_SIZE 28     // must match notebook WINDOW_SIZE
#define STEP_SIZE 14       // must match notebook STEP_SIZE
#define MPU_ADDR 0x68

// Conversion factors (±2g, ±250°/s defaults)
#define ACCEL_SCALE (9.81f / 16384.0f) // LSB → m/s²
#define GYRO_SCALE (1.0f / 131.0f)     // LSB → °/s

// ─── AP PORTAL ───────────────────────────────────────────────────────────────
#define AP_SSID "Drive Metrices AI"
#define AP_PASS "" // open AP
#define DNS_PORT 53

// ─── TYPES ───────────────────────────────────────────────────────────────────
enum State
{
    STATE_AP,
    STATE_CONNECTING,
    STATE_RUNNING
};

struct SensorSample
{
    float accX, accY, accZ;
    float gyroX, gyroY, gyroZ;
};

// ─── GLOBALS (defined in functions.cpp) ──────────────────────────────────────
extern DNSServer dns;
extern WebServer server;
extern Preferences prefs;
extern State currentState;
extern int windowIdx;
extern int sampleCount;
extern unsigned long lastSampleMs;
extern SensorSample window[WINDOW_SIZE];
extern const char PORTAL_HTML[] PROGMEM;

// ─── FUNCTIONS ───────────────────────────────────────────────────────────────
bool mpu_read(SensorSample &s);
void start_ap();
void post_window();
bool connect_wifi(const String &ssid, const String &pass);

#endif