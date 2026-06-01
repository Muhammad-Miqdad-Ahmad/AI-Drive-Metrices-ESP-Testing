#ifndef CONFIG_H
#define CONFIG_H

#include <Wire.h>
#include <MPU6050.h>
#include <Arduino.h>
#include <ArduinoJson.h>

extern MPU6050 SENSOR;

constexpr float G = 9.81f;

// ─── USER CONFIG ─────────────────────────────────────────────────────────────
#define UART_BAUD       115200
#define UART_TIMEOUT_MS 5000   // ms to wait for PC response
#define SAMPLE_RATE_MS  500    // 2 Hz — must match model training rate
#define WINDOW_SIZE     28     // must match notebook WINDOW_SIZE
#define STEP_SIZE       14     // must match notebook STEP_SIZE
#define MPU_ADDR        0x68

// Conversion factors (±2g, ±250°/s defaults)
#define ACCEL_SCALE (9.81f / 16384.0f) // LSB → m/s²
#define GYRO_SCALE  (1.0f  / 131.0f)  // LSB → °/s

// ─── TYPES ───────────────────────────────────────────────────────────────────
struct SensorSample
{
    float accX, accY, accZ;
    float gyroX, gyroY, gyroZ;
};

// ─── GLOBALS (defined in config.cpp) ─────────────────────────────────────────
extern int           windowIdx;
extern int           sampleCount;
extern unsigned long lastSampleMs;
extern SensorSample  window[WINDOW_SIZE];

// ─── FUNCTIONS ───────────────────────────────────────────────────────────────
bool mpu_read(SensorSample &s);
void post_window();

#endif