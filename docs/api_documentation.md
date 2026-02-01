# API Documentation

## 🔌 Web API Endpoints

The ESP32 main controller provides a RESTful API for remote monitoring and configuration. The web server runs on the device's WiFi hotspot (`192.168.4.1`).

## 🌐 Base URL
```
http://192.168.4.1
```

## 📋 Available Endpoints

### 1. GPS Location Data
**GET** `/gps`

Retrieves current GPS location and system status.

#### Response Format
```json
{
    "latitude": "12.840600",
    "longitude": "80.153400",
    "fallTime": "14:32:15",
    "batteryLevel": "85",
    "systemUptime": "7200"
}
```

#### Response Parameters
| Parameter | Type | Description |
|-----------|------|-------------|
| `latitude` | string | Current GPS latitude (6 decimal places) |
| `longitude` | string | Current GPS longitude (6 decimal places) |
| `fallTime` | string | Timestamp of last detected fall (HH:MM:SS) |
| `batteryLevel` | string | Battery level percentage (0-100) |
| `systemUptime` | string | System uptime in seconds |

#### Example Request
```bash
curl http://192.168.4.1/gps
```

### 2. System Status
**GET** `/status`

Retrieves comprehensive system health information.

#### Response Format
```json
{
    "status": "operational",
    "sensors": {
        "ultrasonic": "active",
        "mpu6050": "active",
        "gps": "active",
        "wifi": "active"
    },
    "lastUpdate": "2025-02-01T14:32:15Z",
    "memoryUsage": "68%",
    "wifiClients": 2
}
```

#### Response Parameters
| Parameter | Type | Description |
|-----------|------|-------------|
| `status` | string | System status: `operational`, `warning`, `critical` |
| `sensors` | object | Status of all connected sensors |
| `lastUpdate` | string | Last system update timestamp (ISO 8601) |
| `memoryUsage` | string | Current memory usage percentage |
| `wifiClients` | number | Number of connected WiFi clients |

### 3. Configuration
**GET** `/config`

Retrieves current system configuration.

**POST** `/config`

Updates system configuration.

#### GET Response Format
```json
{
    "fallDetection": {
        "lowThreshold": 0.3,
        "highThreshold": 2.8,
        "windowMs": 300
    },
    "hapticFeedback": {
        "thresholds": [100, 80, 60, 40, 20],
        "motorCount": 5
    },
    "wifi": {
        "ssid": "BlindStick_AP",
        "channel": 1
    }
}
```

#### POST Request Format
```json
{
    "fallDetection": {
        "lowThreshold": 0.4,
        "highThreshold": 2.5,
        "windowMs": 250
    },
    "hapticFeedback": {
        "thresholds": [120, 90, 60, 30, 15]
    }
}
```

#### Configuration Parameters
| Section | Parameter | Type | Range | Default |
|---------|----------|------|-------|---------|
| `fallDetection` | `lowThreshold` | float | 0.1-0.5 | 0.3 |
| `fallDetection` | `highThreshold` | float | 2.0-4.0 | 2.8 |
| `fallDetection` | `windowMs` | integer | 100-500 | 300 |
| `hapticFeedback` | `thresholds` | array | 5 values | [100,80,60,40,20] |
| `hapticFeedback` | `motorCount` | integer | 3-8 | 5 |

## 📡 ESP-NOW Protocol

The ESP8266 nodes communicate using the ESP-NOW protocol for low-latency data transmission.

### Data Packet Structure
```cpp
typedef struct {
    int distance;        // Distance in centimeters
    uint32_t timestamp;  // Unix timestamp
    uint8_t node_id;     // Unique node identifier
    uint8_t battery;     // Battery level (0-100%)
    uint8_t rssi;        // Signal strength
} SensorPacket;
```

### Packet Format
| Field | Size | Description |
|-------|------|-------------|
| `distance` | 2 bytes | Measured distance in cm (0-500) |
| `timestamp` | 4 bytes | Unix epoch timestamp |
| `node_id` | 1 byte | Node identifier (1-255) |
| `battery` | 1 byte | Battery level percentage |
| `rssi` | 1 byte | Signal strength indicator |

## 🔧 Communication Protocol Details

### HTTP Methods
| Method | Usage | Description |
|--------|-------|-------------|
| `GET` | Read data | Retrieve sensor data and status |
| `POST` | Update config | Modify system parameters |
| `PUT` | Replace config | Complete configuration update |
| `DELETE` | Reset | Factory reset operations |

### Response Codes
| Code | Meaning | Description |
|------|---------|-------------|
| `200` | OK | Request successful |
| `400` | Bad Request | Invalid parameters |
| `401` | Unauthorized | Authentication required |
| `404` | Not Found | Endpoint doesn't exist |
| `500` | Server Error | Internal system error |

### Rate Limiting
- **GPS Endpoint**: 1 request/second max
- **Status Endpoint**: 5 requests/second max
- **Config Endpoint**: 1 request/minute max

## 📱 Client Integration Examples

### Python Client
```python
import requests
import json

class DrishtiGuideClient:
    def __init__(self, base_url="http://192.168.4.1"):
        self.base_url = base_url
    
    def get_gps_data(self):
        response = requests.get(f"{self.base_url}/gps")
        return response.json()
    
    def get_status(self):
        response = requests.get(f"{self.base_url}/status")
        return response.json()
    
    def update_config(self, config_data):
        response = requests.post(f"{self.base_url}/config", json=config_data)
        return response.json()

# Usage
client = DrishtiGuideClient()
gps_data = client.get_gps_data()
print(f"Location: {gps_data['latitude']}, {gps_data['longitude']}")
```

### JavaScript Client
```javascript
class DrishtiGuideAPI {
    constructor(baseUrl = 'http://192.168.4.1') {
        this.baseUrl = baseUrl;
    }
    
    async getGPSData() {
        const response = await fetch(`${this.baseUrl}/gps`);
        return await response.json();
    }
    
    async getStatus() {
        const response = await fetch(`${this.baseUrl}/status`);
        return await response.json();
    }
    
    async updateConfig(config) {
        const response = await fetch(`${this.baseUrl}/config`, {
            method: 'POST',
            headers: {
                'Content-Type': 'application/json'
            },
            body: JSON.stringify(config)
        });
        return await response.json();
    }
}

// Usage
const api = new DrishtiGuideAPI();
api.getGPSData().then(data => {
    console.log(`Location: ${data.latitude}, ${data.longitude}`);
});
```

## 🔄 Real-time Updates

### WebSocket Support
For real-time monitoring, the system supports WebSocket connections:

```javascript
const ws = new WebSocket('ws://192.168.4.1/ws');

ws.onmessage = function(event) {
    const data = JSON.parse(event.data);
    console.log('Real-time update:', data);
};
```

### Event Types
| Event | Data | Frequency |
|-------|------|-----------|
| `distance_update` | `{distance: 45, timestamp: 1643721135}` | 5 Hz |
| `fall_detected` | `{timestamp: 1643721135, severity: 'high'}` | Event-driven |
| `gps_update` | `{lat: 12.8406, lon: 80.1534}` | 1 Hz |
| `battery_alert` | `{level: 15, status: 'low'}` | Event-driven |

## 🛡️ Security Considerations

### Authentication
Currently uses WPA2-PSK for WiFi security. Future versions will implement:
- API key authentication
- TLS encryption
- Device certificate validation

### Data Privacy
- No personal data stored on device
- GPS data encrypted during transmission
- Local processing only (no cloud dependency)

---

For integration support or API questions, please refer to the [Troubleshooting Guide](troubleshooting.md) or open an issue on GitHub.