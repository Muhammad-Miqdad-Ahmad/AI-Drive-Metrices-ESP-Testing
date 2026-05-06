# DriveSense | ESP32 Firmware

DriveSense is an ESP32-based motion analysis system designed for real-time driving behavior detection. It leverages an MPU6050 IMU to collect high-precision accelerometer and gyroscope data, which is then processed into sliding windows and transmitted to an AI-powered backend for event classification.

## 🚀 Key Features

- **Captive Portal Provisioning**: Automatic Access Point mode for seamless Wi-Fi configuration via a modern, mobile-responsive web interface.
- **High-Precision Sampling**: Real-time acquisition of 6-axis motion data (3-axis Accel + 3-axis Gyro).
- **Intelligent Windowing**: Data is buffered into sliding windows (configurable size and step) to match AI model training parameters.
- **Backend Integration**: Synchronous JSON transmission to a REST API for real-time inference and feedback.
- **Persistent Storage**: Uses ESP32 NVS (Non-Volatile Storage) to remember Wi-Fi credentials across reboots.
- **Visual Feedback**: Serial-based telemetry and classification results display.

## 🛠 Hardware Requirements

- **Microcontroller**: ESP32 (WROOM-32 recommended)
- **Sensor**: MPU6050 (Accelerometer & Gyroscope)
- **Connections**:
  - `VCC` -> `3.3V`
  - `GND` -> `GND`
  - `SDA` -> `GPIO 21`
  - `SCL` -> `GPIO 22`

## 💻 Software & Libraries

This project is built using **PlatformIO** and requires the following libraries:

- `electroniccats/MPU6050` @ ^1.4.1
- `bblanchon/ArduinoJson` @ ^7.0.0
- Built-in ESP32 libraries: `WiFi`, `WebServer`, `DNSServer`, `HTTPClient`, `Preferences`, `Wire`

## ⚙️ Configuration

System parameters are defined in `lib/Config/include/config.h`:

| Constant | Description | Default |
|----------|-------------|---------|
| `BACKEND_URL` | The endpoint for AI inference | `http://172.16.21.59:8000/predict` |
| `SAMPLE_RATE_MS` | Delay between sensor reads | `500` (2 Hz) |
| `WINDOW_SIZE` | Total samples per inference window | `28` |
| `STEP_SIZE` | Overlap increment for windows | `14` |
| `AP_SSID` | SSID for initial configuration | `Drive Metrices AI` |

## 🚀 Getting Started

1. **Clone the repository**:
   ```bash
   git clone <repository-url>
   cd Metrices-AI-prototype
   ```

2. **Open in PlatformIO**:
   Import the folder into VS Code with the PlatformIO extension installed.

3. **Configure Backend**:
   Update `BACKEND_URL` in `lib/Config/include/config.h` to point to your inference server.

4. **Flash the ESP32**:
   Connect your board and run:
   ```bash
   pio run --target upload
   ```

5. **Initial Setup**:
   - On first boot, the ESP32 will start an Access Point named **"Drive Metrices AI"**.
   - Connect to it with your phone/PC.
   - A portal should open automatically (or visit `http://192.168.4.1`).
   - Enter your local Wi-Fi credentials and save.
   - The device will reboot and begin transmitting data.

## 📊 Data Transmission

The device sends a JSON payload to the backend:

```json
{
  "window": [
    {"AccX": 0.12, "AccY": -0.05, "AccZ": 9.81, "GyroX": 0.0, "GyroY": 0.2, "GyroZ": -0.1},
    ...
  ]
}
```

The backend is expected to return:
```json
{
  "class_name": "Normal Driving",
  "class_id": 0,
  "confidence": 98.5
}
```

## 📝 License

This project is part of a Final Year Project (FYP). See the main documentation for licensing details.
