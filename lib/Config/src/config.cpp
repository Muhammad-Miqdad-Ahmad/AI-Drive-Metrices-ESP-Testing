#include <config.h>

// ─── GLOBALS ─────────────────────────────────────────────────────────────────
MPU6050 SENSOR;

SensorSample window[WINDOW_SIZE];
int           windowIdx    = 0;
int           sampleCount  = 0;
unsigned long lastSampleMs = 0;

// ─── MPU6050 ─────────────────────────────────────────────────────────────────
bool mpu_read(SensorSample &s)
{
    int16_t ax, ay, az, gx, gy, gz;
    SENSOR.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);

    s.accX  = ax * ACCEL_SCALE;
    s.accY  = ay * ACCEL_SCALE;
    s.accZ  = az * ACCEL_SCALE;
    s.gyroX = gx * GYRO_SCALE;
    s.gyroY = gy * GYRO_SCALE;
    s.gyroZ = gz * GYRO_SCALE;
    return true;
}

// ─── UART WINDOW SEND ────────────────────────────────────────────────────────
void post_window()
{
    // Build JSON window (identical structure the Python backend expects)
    JsonDocument doc;
    JsonArray arr = doc["window"].to<JsonArray>();
    for (int i = 0; i < WINDOW_SIZE; i++)
    {
        int idx = (windowIdx + i) % WINDOW_SIZE;
        JsonObject o = arr.add<JsonObject>();
        o["AccX"]  = window[idx].accX  / G;
        o["AccY"]  = window[idx].accY  / G;
        o["AccZ"]  = window[idx].accZ  / G;
        o["GyroX"] = window[idx].gyroX;
        o["GyroY"] = window[idx].gyroY;
        o["GyroZ"] = window[idx].gyroZ;
    }

    String body;
    serializeJson(doc, body);

    // Prefix "WINDOW:" so Python can distinguish data from debug lines
    Serial.print("WINDOW:");
    Serial.println(body);

    // Block-read one JSON response line from PC
    String resp = Serial.readStringUntil('\n');
    resp.trim();

    if (resp.isEmpty())
    {
        Serial.println("[UART] No response — timeout.");
        return;
    }

    JsonDocument rDoc;
    if (deserializeJson(rDoc, resp) == DeserializationError::Ok)
    {
        Serial.println("┌──────────────────────────────────────┐");
        Serial.printf("│  Event      : %-22s │\n", rDoc["class_name"].as<const char *>());
        Serial.printf("│  Class ID   : %-22d │\n", rDoc["class_id"].as<int>());
        Serial.printf("│  Confidence : %-21.1f%% │\n", rDoc["confidence"].as<float>());
        Serial.println("└──────────────────────────────────────┘");
    }
}