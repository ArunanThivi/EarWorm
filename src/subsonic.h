
#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>

struct Auth {
  String salt;
  String token;
};

struct Album {
    String id;
    String name;
    String genre;
    String artist;
};

struct Song {
    String id;
    String title;
    String album;
    String artist;
    unsigned int duration; //ms
};


bool authenticate(HTTPClient& client, String serverURL, String username, String password);

int get_indexes(HTTPClient& client, int modified_since = 0);

int get_albums(HTTPClient& client, String artist_id, Album* album_array, int max_count);

int get_songs(HTTPClient& client, String album_id, Song* song_array, int max_count);

bool download_track(HTTPClient& client, String title, String filepath);

bool download_cover_art(HTTPClient& client, String album_id, String filepath);