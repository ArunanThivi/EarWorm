#include <Arduino.h>
#include <HTTPClient.h>

bool run_sync(HTTPClient& client);

void handle_wifi_connection(String SSID, String Password);