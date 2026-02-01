#ifndef WIFI_MANAGER_H
#define WIFI_MANAGER_H

#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include <ArduinoJson.h>

// WiFi modes
typedef enum {
    WIFI_MODE_AP,           // Access Point mode
    WIFI_MODE_STATION,      // Station mode
    WIFI_MODE_DUAL          // Both AP and Station
} WiFiMode;

// Client connection structure
typedef struct {
    String macAddress;
    String ipAddress;
    uint32_t connectTime;
    uint8_t rssi;
    bool isActive;
} WiFiClient;

// API endpoint structure
typedef struct {
    String path;
    String method;
    void (*handler)();
    uint32_t lastAccess;
    uint32_t accessCount;
} APIEndpoint;

class WiFiManager {
private:
    WebServer* server;
    WiFiMode currentMode;
    
    // Client management
    WiFiClient clients[8];
    uint8_t clientCount;
    
    // API management
    APIEndpoint endpoints[16];
    uint8_t endpointCount;
    
    // Statistics
    uint32_t totalRequests;
    uint32_t startTime;
    uint32_t lastClientActivity;
    
    // Private methods
    void handleNotFound();
    void handleCORS();
    void sendJSON(JsonDocument& doc, int statusCode = 200);
    void logRequest(String method, String path);
    bool checkRateLimit(String clientIP);

public:
    WiFiManager();
    
    // WiFi management
    bool startAP(const char* ssid, const char* password, int channel = 1);
    bool connectToWiFi(const char* ssid, const char* password);
    void setMode(WiFiMode mode);
    WiFiMode getMode();
    
    // Client management
    void updateClientList();
    WiFiClient* getConnectedClients();
    uint8_t getClientCount();
    bool isClientConnected(String macAddress);
    void disconnectClient(String macAddress);
    
    // Server management
    bool startServer(int port = 80);
    void stopServer();
    void handleRequests();
    
    // API endpoint management
    void registerEndpoint(const char* path, const char* method, void (*handler)());
    void unregisterEndpoint(const char* path);
    APIEndpoint* getEndpoint(const char* path);
    
    // Built-in endpoints
    void handleStatus();
    void handleGPS();
    void handleSensors();
    void handleConfig();
    void handleInfo();
    void handleReset();
    
    // Configuration
    void setAPCredentials(const char* ssid, const char* password);
    void setMaxClients(int maxClients);
    void setAuthEnabled(bool enabled);
    void setRateLimitEnabled(bool enabled);
    
    // Diagnostics
    void printWiFiStatus();
    void printConnectedClients();
    void printAPIStats();
    uint32_t getUptime();
    int getSignalStrength();
    float getThroughput();
    
    // Advanced features
    void enableOTA(bool enabled);
    void enableMDNS(bool enabled);
    void setCaptivePortal(bool enabled);
    bool scanNetworks();
    void setupDNS();
    
    // Security
    void setAuthCredentials(const char* username, const char* password);
    bool authenticateClient();
    void blockIP(String ipAddress);
    void unblockIP(String ipAddress);
    
    // Utilities
    String getLocalIP();
    String getAPSSID();
    String getMACAddress();
    bool isConnected();
    int getRSSI();
    
    // JSON helpers
    void createStatusJSON(JsonDocument& doc);
    void createGPSJSON(JsonDocument& doc);
    void createSensorJSON(JsonDocument& doc);
    void createConfigJSON(JsonDocument& doc);
    
    // Callback functions
    void setClientConnectCallback(void (*callback)(WiFiClient));
    void setClientDisconnectCallback(void (*callback)(WiFiClient));
    void setAPIAccessCallback(void (*callback)(String, String));
};

extern WiFiManager wifiManager;

#endif // WIFI_MANAGER_H