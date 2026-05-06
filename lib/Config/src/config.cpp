#include <config.h>

// ─── GLOBALS ─────────────────────────────────────────────────────────────────
MPU6050 SENSOR;
Preferences prefs;
WebServer server(80);
DNSServer dns;
State currentState = STATE_AP;

SensorSample window[WINDOW_SIZE];
int windowIdx = 0;
int sampleCount = 0;
unsigned long lastSampleMs = 0;

// ─── MPU6050 ─────────────────────────────────────────────────────────────────
bool mpu_read(SensorSample &s)
{
  int16_t ax, ay, az, gx, gy, gz;
  SENSOR.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);

  s.accX = ax * ACCEL_SCALE;
  s.accY = ay * ACCEL_SCALE;
  s.accZ = az * ACCEL_SCALE;
  s.gyroX = gx * GYRO_SCALE;
  s.gyroY = gy * GYRO_SCALE;
  s.gyroZ = gz * GYRO_SCALE;
  return true;
}

// ─── AP & CAPTIVE PORTAL ─────────────────────────────────────────────────────

void start_ap()
{
  WiFi.mode(WIFI_AP);
  WiFi.softAP(AP_SSID, AP_PASS);
  Serial.printf("[AP] Started. SSID: %s  IP: %s\n",
                AP_SSID, WiFi.softAPIP().toString().c_str());

  dns.start(DNS_PORT, "*", WiFi.softAPIP());

  server.on("/", HTTP_GET, []()
            { server.send_P(200, "text/html", PORTAL_HTML); });

  auto redirect = []()
  { server.sendHeader("Location", "/"); server.send(302); };
  server.on("/generate_204", HTTP_GET, redirect);
  server.on("/hotspot-detect.html", HTTP_GET, redirect);
  server.on("/connecttest.txt", HTTP_GET, redirect);
  server.on("/ncsi.txt", HTTP_GET, redirect);
  server.on("/redirect", HTTP_GET, redirect);

  server.on("/scan", HTTP_GET, []()
            {
        int n = WiFi.scanNetworks(false, false);
        JsonDocument doc;
        JsonArray arr = doc.to<JsonArray>();
        for (int i = 0; i < n; i++) {
            JsonObject o = arr.add<JsonObject>();
            o["ssid"] = WiFi.SSID(i);
            o["rssi"] = WiFi.RSSI(i);
        }
        String out;
        serializeJson(doc, out);
        server.send(200, "application/json", out);
        WiFi.scanDelete(); });

  server.on("/save", HTTP_POST, []()
            {
        String ssid = server.arg("ssid");
        String pass = server.arg("pass");
        if (ssid.isEmpty()) {
            server.send(200, "application/json", "{\"ok\":false}");
            return;
        }
        prefs.begin("wifi", false);
        prefs.putString("ssid", ssid);
        prefs.putString("pass", pass);
        prefs.end();
        server.send(200, "application/json", "{\"ok\":true}");
        Serial.printf("[AP] Credentials saved: SSID=%s\n", ssid.c_str());
        delay(800);
        ESP.restart(); });

  server.onNotFound(redirect);
  server.begin();
  Serial.println("[AP] Web server running.");
}

// ─── WIFI CONNECT ────────────────────────────────────────────────────────────

bool connect_wifi(const String &ssid, const String &pass)
{
  Serial.printf("[WiFi] Connecting to \"%s\"...\n", ssid.c_str());
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid.c_str(), pass.c_str());
  unsigned long t = millis();
  while (WiFi.status() != WL_CONNECTED)
  {
    if (millis() - t > 15000)
    {
      Serial.println("[WiFi] Timed out.");
      return false;
    }
    delay(300);
    Serial.print('.');
  }
  Serial.printf("\n[WiFi] Connected! IP: %s\n", WiFi.localIP().toString().c_str());
  return true;
}

// ─── BACKEND POST ────────────────────────────────────────────────────────────

void post_window()
{
  if (WiFi.status() != WL_CONNECTED)
  {
    Serial.println("[HTTP] WiFi disconnected — skipping POST.");
    return;
  }

  JsonDocument doc;
  JsonArray arr = doc["window"].to<JsonArray>();
  for (int i = 0; i < WINDOW_SIZE; i++)
  {
    int idx = (windowIdx + i) % WINDOW_SIZE;
    JsonObject o = arr.add<JsonObject>();
    o["AccX"] = window[idx].accX;
    o["AccY"] = window[idx].accY;
    o["AccZ"] = window[idx].accZ;
    o["GyroX"] = window[idx].gyroX;
    o["GyroY"] = window[idx].gyroY;
    o["GyroZ"] = window[idx].gyroZ;
  }

  String body;
  serializeJson(doc, body);

  HTTPClient http;
  http.begin(BACKEND_URL);
  http.addHeader("Content-Type", "application/json");
  http.setTimeout(5000);

  int code = http.POST(body);
  if (code == 200)
  {
    String resp = http.getString();
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
  else
  {
    Serial.printf("[HTTP] POST failed: %d\n", code);
  }
  http.end();
}