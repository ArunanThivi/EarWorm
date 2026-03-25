
#include <Arduino.h>
#include <WiFi.h>
#include <MD5Builder.h>
#include <ArduinoJson.h>
#include <SdFat.h>
#include <HTTPClient.h>
#include <subsonic.h>


// //Define IP of navidrome server 
const String APIVersion = "1.16.1";
const String appName = "EarWorm";
const int PORT = 4533;

String serverURL;
String username;
String password;

String _generateSalt(int length);
Auth _generateAuth(String password);


bool authenticate(HTTPClient& client, String _serverURL, String _username, String _password) {
    Auth auth = _generateAuth(_password); 
    String url = "http://"+_serverURL+":"+String(PORT)
    + "/rest/ping?u="+ _username
    + "&t=" + auth.token
    + "&s=" + auth.salt
    + "&v=" + APIVersion
    + "&c=" + appName
    + "&f=json";
    //Debug for printing the request
    Serial.println(url.c_str());
    client.begin(url);
    client.setTimeout(5000);
    int statusCode = client.GET();
    String response = client.getString();
    client.end();
    if (statusCode == 200 && response.indexOf("\"status\":\"ok\"") != -1) {
        Serial.println("Successfully authenticated");
        username = _username;
        password = _password;
        serverURL = _serverURL;
        return true;
    } else {
        Serial.println("Subsonic auth FAILED");
        return false;
    }   
}

int get_indexes(HTTPClient& client, int modified_since = 0) {
    /*
    TODO: Call the Subsonic getIndexes endpoint.
    If modified_since is 0, fetches the full artist index (used for first boot and reconciliation syncs).
    Otherwise passes modified_since as the ifModifiedSince parameter to fetch only changed artists.
    Returns number of artists retrieved, or -1 on error.
    */
    return 0;
}

int get_albums(HTTPClient& client, String artist_id, Album* album_array, int max_count) {
    Auth auth = _generateAuth(password);
    String url = "http://"+serverURL+":"+String(PORT)
        + "/rest/Artist?"
        + "id=" + artist_id //Change argument to Song*
        + "&u="+ username
        + "&t=" + auth.token
        + "&s=" + auth.salt
        + "&v=" + APIVersion
        + "&c=" + appName
        + "&f=json";
    //Debug for printing the request
    Serial.println(url.c_str());
    client.begin(url);
    client.setTimeout(5000);
    int statusCode = client.GET();
    String response = client.getString();
    client.end();
    if (statusCode != 200) {
        Serial.println("HTTP Response not ok :'(");
        return -1;
    }
    //TODO: Extract response to albums.
    return 0;
}

int get_songs(HTTPClient& client, String album_id, Song* song_array, int max_count) {
    Auth auth = _generateAuth(password);
    String url = "http://"+serverURL+":"+String(PORT)
        + "/rest/getAlbum?"
        + "id=" + album_id //Change argument to Song*
        + "&u="+ username
        + "&t=" + auth.token
        + "&s=" + auth.salt
        + "&v=" + APIVersion
        + "&c=" + appName
        + "&f=json";
    //Debug for printing the request
    Serial.println(url.c_str());
    client.begin(url);
    client.setTimeout(5000);
    int statusCode = client.GET();
    String response = client.getString();
    client.end();
    if (statusCode != 200) {
        Serial.println("HTTP Response not ok :'(");
        return -1;
    }
    //TODO: Extract response into Songs.
    return 0;
}

bool download_track(HTTPClient& client, String title, String filepath) {
    Auth auth = _generateAuth(password);
    String url = "http://"+serverURL+":"+String(PORT)
        + "/rest/stream?"
        + "id=" + title //Change argument to Song*
        + "&maxBitRate=192"
        + "&format=mp3"
        + "&estimateContentLength=true"
        + "&u="+ username
        + "&t=" + auth.token
        + "&s=" + auth.salt
        + "&v=" + APIVersion
        + "&c=" + appName
        + "&f=json";
    //Debug for printing the request
    Serial.println(url.c_str());
    client.begin(url);
    client.setTimeout(5000);
    int statusCode = client.GET();
    String response = client.getString();
    client.end();
    if (statusCode != 200) {
        Serial.println("HTTP Response not ok :'(");
        return false;
    }
    // Serial.println(response); 4MB file too big to print, assume it works.
    //TODO: write response to file.
    return true;
}

bool download_cover_art(HTTPClient& client, String album_id, String filepath) {
    Auth auth = _generateAuth(password);
    String url = "http://"+serverURL+":"+String(PORT)
        + "/rest/getCoverArt?"
        + "id=" + album_id //TODO: Change argument to Album*
        + "&size=200"
        + "&u="+ username
        + "&t=" + auth.token
        + "&s=" + auth.salt
        + "&v=" + APIVersion
        + "&c=" + appName
        + "&f=json";
    //Debug for printing the request
    Serial.println(url.c_str());
    client.begin(url);
    client.setTimeout(5000);
    int statusCode = client.GET();
    String response = client.getString();
    client.end();
    if (statusCode != 200) {
        Serial.println("HTTP Response not ok :'(");
        return false;
    }
    Serial.println(response);
    //TODO: write response to file.
    return true;

}


//Private functions
String _generateSalt(int length) {
  const char charset[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789";
  String res = "";
  for (int i = 0; i < length; i++) {
    res +=  charset[esp_random() % (sizeof(charset) - 1)];
  }
  return res;
}

Auth _generateAuth(String password) {
  MD5Builder md5;
  Auth res;
  String salt = _generateSalt(16);
  char saltedPass[128];
  snprintf(saltedPass, 128, "%s%s", password.c_str(), salt.c_str());
  md5.begin();
  md5.add(saltedPass);
  md5.calculate();
  res.salt = salt;
  res.token = md5.toString();
  return res;
}