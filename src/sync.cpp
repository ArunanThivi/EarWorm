
#include <Arduino.h>
#include <WiFi.h>
#include <Preferences.h>
#include <HTTPClient.h>
#include <subsonic.h>

int status = WL_IDLE_STATUS;
void _printWiFiStatus();
Preferences prefs;
bool run_sync(HTTPClient& client) {
    prefs.begin("earworm", true);
    uint32_t lastIncremental = prefs.getULong("lastSync");
    uint32_t lastFull = prefs.getULong("lastFull");
    prefs.end();
    if (lastFull == 0) {
        return _full_sync(client);
    }
    if (!_incremental_sync(client)) {
        Serial.println("Incremental Sync failed");
        return false;
    }
    if ((millis() - lastFull) > (30 * 24 * 60 * 60 * 1000) && !_reconciliation_sync(client)) {
        Serial.println("Reconciliation Sync failed");
        return false;
    }
    return true;
}

bool _full_sync(HTTPClient& client) {
    //TODO: Handle download list
    if (!get_indexes(client)) {
        Serial.println("Full Sync failed");
        return false;
    }
    uint32_t now = millis();

    //TODO: get_albums for each artist
    //TODO: Update library.json
    prefs.begin("earworm", false);
    prefs.putULong("lastSync", now);
    prefs.putULong("lastFull", now);
    prefs.end();
    return true;
}
/**
 * 
 */
bool _incremental_sync(HTTPClient& client, uint32_t lastSync) {
    //TODO: Handle download list
    if (!get_indexes(client, lastSync)) {
        Serial.println("incremental sync failed");
        return false;
    }
    //TODO: Update library.json
    uint32_t now = millis();
    prefs.begin("earworm", false);
    prefs.putULong("lastSync", now);
    prefs.end();
    return true;
}

bool _reconciliation_sync(HTTPClient& client) {
    /*
    Fetch the full artist index via get_indexes(0) and diff against library.json.
    Removes any local tracks, albums, or artists no longer present on the server.
    Updates last_full_sync timestamp on completion.
    Returns true if completed without errors.
    */
}

int _process_download_queue(HTTPClient& client) {
    //TODO: Download new content
}

void handle_wifi_connection(String SSID, String Password) {
    Serial.print("Attempting to connect to SSID: ");
    Serial.print(SSID);
    WiFi.begin(SSID, Password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }

    Serial.println("");
    Serial.println("Connected to WiFi");
    _printWiFiStatus();
}
// put function definitions here:
void _printWiFiStatus() {
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI): ");
  Serial.print(rssi);
  Serial.println(" dBm");
}