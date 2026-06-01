#include <config.h>

void setup()
{
    Wire.begin(21, 22);
    delay(500);

    Serial.begin(UART_BAUD);
    Serial.setTimeout(UART_TIMEOUT_MS);
    delay(2000);

    Serial.println("╔══════════════════════════════════╗");
    Serial.println("║     DriveSense  |  ESP32 FW      ║");
    Serial.println("╚══════════════════════════════════╝");

    SENSOR.initialize();
    Serial.println("[MPU6050] Initialized");
    Serial.println("[UART] Sending windows to PC over Serial USB");
}

void loop()
{
    unsigned long now = millis();
    if (now - lastSampleMs < SAMPLE_RATE_MS)
        return;
    lastSampleMs = now;

    SensorSample s;
    mpu_read(s);
    window[windowIdx] = s;
    windowIdx = (windowIdx + 1) % WINDOW_SIZE;
    sampleCount++;

    Serial.printf("[%6lu] Acc(g) X:%7.3f Y:%7.3f Z:%7.3f  |  Gyro(°/s) X:%7.2f Y:%7.2f Z:%7.2f\n",
                  now / 1000,
                  s.accX / G, s.accY / G, s.accZ / G,
                  s.gyroX, s.gyroY, s.gyroZ);

    if (sampleCount >= WINDOW_SIZE && (sampleCount % STEP_SIZE == 0))
    {
        Serial.println("[Window] Full — sending to PC over UART...");
        post_window();
    }
}