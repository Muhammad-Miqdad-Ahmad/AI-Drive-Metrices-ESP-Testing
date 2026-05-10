#include <config.h>

void setup()
{
    Wire.begin(21, 22);
    delay(500);

    Serial.begin(115200);
    delay(2000);

    Serial.println("╔══════════════════════════════════╗");
    Serial.println("║     DriveSense  |  ESP32 FW      ║");
    Serial.println("╚══════════════════════════════════╝");

    SENSOR.initialize();
    Serial.println("[MPU6050] Initialized");

    prefs.begin("wifi", true);
    String ssid = prefs.getString("ssid", "");
    String pass = prefs.getString("pass", "");
    prefs.end();

    if (ssid.isEmpty())
    {
        Serial.println("[Boot] No credentials → starting AP portal");
        currentState = STATE_AP;
        start_ap();
    }
    else
    {
        currentState = STATE_CONNECTING;
        if (connect_wifi(ssid, pass))
        {
            currentState = STATE_RUNNING;
            Serial.println("[Boot] WiFi connected → sensor loop starting");
            Serial.printf("[Boot] Backend: %s\n", BACKEND_URL);
        }
        else
        {
            Serial.println("[Boot] WiFi failed → clearing creds, starting AP");
            prefs.begin("wifi", false);
            prefs.clear();
            prefs.end();
            currentState = STATE_AP;
            start_ap();
        }
    }
}

void loop()
{
    if (currentState == STATE_AP)
    {
        dns.processNextRequest();
        server.handleClient();
        return;
    }

    if (currentState != STATE_RUNNING)
        return;

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
        Serial.println("[Window] Full — sending to backend...");
        post_window();
    }
}