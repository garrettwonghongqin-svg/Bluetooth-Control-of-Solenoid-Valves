#include "WiFi.h"

void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println();
  Serial.println("🔍 Scanning for available WiFi networks...");

  // Set WiFi mode to station (client)
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();  // Disconnect from any previous connection
  delay(100);

  int networkCount = WiFi.scanNetworks();
  Serial.println("Scan complete.");

  if (networkCount == 0) {
    Serial.println("❌ No networks found.");
  } else {
    Serial.printf("✅ %d networks found:\n", networkCount);
    for (int i = 0; i < networkCount; ++i) {
      Serial.printf("%d: %s (%d dBm) %s\n",
                    i + 1,
                    WiFi.SSID(i).c_str(),
                    WiFi.RSSI(i),
                    (WiFi.encryptionType(i) == WIFI_AUTH_OPEN) ? "OPEN" : "SECURED");
      delay(10);
    }
  }

  Serial.println("\nScan done!");
}

void loop() {
  // Nothing here — single scan done in setup()
}
