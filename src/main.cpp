
#include <Arduino.h>
#include <WiFi.h>
#include <credentials.h>
#include <MD5Builder.h>
#include <SdFat.h>
#include <sync.h>
#include <subsonic.h>
#include <HTTPClient.h>


SdFat sd;


//Hardware Pin Config
#define SD_CS_PIN SS

//Define IP of navidrome server 

WiFiClient wifi;
HTTPClient client;

void setup() {
  Serial.begin(115200);
  while (!Serial) {;}
  
  Credentials creds = loadCredentials();
  handle_wifi_connection(creds.SSID, creds.wifiPassword);
  
  if (!creds.valid) {
    Serial.println("Error retrieving credentials");
  }

  Serial.println("Initializing SD Card");

  if (!sd.begin(SD_CS_PIN)) {
    Serial.println("SD init Failed!");
    Serial.print("Error Code: ");
    Serial.println(sd.sdErrorCode());
      Serial.print("Error data: ");
    Serial.println(sd.sdErrorData());
    return;
  }

  Serial.println("SD init OK!");
  


  Serial.println("\nStarting connection to server...");
  if (!authenticate(client, creds.serverURL, creds.username, creds.password)) {
    return;
  }
  // download_cover_art(client, "kGZp3vq2SYBTzsxcBVSzZk", "");
  // download_track(client, "kGZp3vq2SYBTzsxcBVSzZk", "");
  get_songs(client, "5kZsGy7KsvQTgWItyY7Ziy", nullptr, 0);
  get_albums(client, "6LWaVXnRB2w3vKXnrMr5Dd", nullptr, 0);
  
  return;
  
}

void loop() {

}

// put function definitions here: