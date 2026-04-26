
#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <SPI.h>
#include <TFT_eSPI.h>
#include <credentials.h>
#include <MD5Builder.h>
#include <SdFat.h>
#include <sync.h>
#include <subsonic.h>
#include <playlist_manager.h>


SdFat sd;


//Hardware Pin Config
#define SD_CS_PIN SS

TFT_eSPI tft = TFT_eSPI(TFT_CS, TFT_DC);

//Define IP of navidrome server 

WiFiClient wifi;
HTTPClient client;

void setup() {
  Serial.begin(115200);
  while (!Serial) {;}

  pinMode(SD_CS_PIN, OUTPUT);
  digitalWrite(SD_CS_PIN, HIGH);

  pinMode(TFT_CS, OUTPUT);
  digitalWrite(TFT_CS, HIGH);

  delay(50);

  SPI.begin();

  Serial.println("Initializing SD Card");

  if (!sd.begin(SD_CS_PIN)) {
    Serial.println("SD init failed");
    sd.printSdError(&Serial);  // human-readable symbol/text
    exit(0);
  }

  Serial.println("SD init OK!");

  // tft.init();
  // tft.fillScreen(TFT_BLACK);
  // tft.drawRect(0, 0, tft.width(), tft.height(), TFT_GREEN);

  // // Set "cursor" at top left corner of display (0,0) and select font 4
  // tft.setCursor(0, 4, 4);

  // // Set the font colour to be white with a black background
  // tft.setTextColor(TFT_WHITE);

  // // We can now plot text on screen using the "print" class
  // tft.println(" Initialised default\n");
  // tft.println(" White text");
  
  // tft.setTextColor(TFT_RED);
  // tft.println(" Red text");
  
  // tft.setTextColor(TFT_GREEN);
  // tft.println(" Green text");
  
  // tft.setTextColor(TFT_BLUE);
  // tft.println(" Blue text");

  if (init_playlist_manager()) {
    Serial.println("Error initializing Playlist Manager");
    exit(0);
  }
  Serial.println("Playlist Manager initialized");

  if (build_playlist(sd) < 0) {
    Serial.println("Error building playlist");
    exit(0);
  }
  Serial.println("Playlist built");

    
  Credentials creds = loadCredentials();
  handle_wifi_connection(creds.SSID, creds.wifiPassword);
  
  if (!creds.valid) {
    Serial.println("Error retrieving credentials");
  }


  Serial.println("\nStarting connection to server...");
  if (!authenticate(client, creds.serverURL, creds.username, creds.password)) {
    return;
  }
  // download_cover_art(client, "kGZp3vq2SYBTzsxcBVSzZk", "");
  // download_track(client, "kGZp3vq2SYBTzsxcBVSzZk", "");
  // get_songs(client, "5kZsGy7KsvQTgWItyY7Ziy", nullptr, 0);
  // get_albums(client, "6LWaVXnRB2w3vKXnrMr5Dd", nullptr, 0);
}

void loop() {

  // tft.invertDisplay( false ); // Where i is true or false
 
  // tft.fillScreen(TFT_BLACK);
  // tft.drawRect(0, 0, tft.width(), tft.height(), TFT_GREEN);

  // tft.setCursor(0, 4, 4);

  // tft.setTextColor(TFT_WHITE);
  // tft.println(" Invert OFF\n");

  // tft.println(" White text");
  
  // tft.setTextColor(TFT_RED);
  // tft.println(" Red text");
  
  // tft.setTextColor(TFT_GREEN);
  // tft.println(" Green text");
  
  // tft.setTextColor(TFT_BLUE);
  // tft.println(" Blue text");

  delay(5000);


  // Binary inversion of colours
  tft.invertDisplay( true ); // Where i is true or false
 
  tft.fillScreen(TFT_BLACK);
  tft.drawRect(0, 0, tft.width(), tft.height(), TFT_GREEN);

  tft.setCursor(0, 4, 4);

  tft.setTextColor(TFT_WHITE);
  tft.println(" Invert ON\n");

  tft.println(" White text");
  
  tft.setTextColor(TFT_RED);
  tft.println(" Red text");
  
  tft.setTextColor(TFT_GREEN);
  tft.println(" Green text");
  
  tft.setTextColor(TFT_BLUE);
  tft.println(" Blue text");

  delay(5000);
}
