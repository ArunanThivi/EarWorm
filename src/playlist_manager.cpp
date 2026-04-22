#include <Arduino.h>
#include <ArduinoJson.h>

#include <SdFat.h>
#include <playlist_manager.h>

struct SpiRamAllocator : ArduinoJson::Allocator {
  void* allocate(size_t size) override {
    return heap_caps_malloc(size, MALLOC_CAP_SPIRAM);
  }

  void deallocate(void* pointer) override {
    heap_caps_free(pointer);
  }

  void* reallocate(void* ptr, size_t new_size) override {
    return heap_caps_realloc(ptr, new_size, MALLOC_CAP_SPIRAM);
  }
};
SpiRamAllocator allocator;

String _genre = "";

SdFile file;

track* playlist;
const int PLAYLIST_SIZE = 500;
uint currentIndex = 0;

JsonDocument filter;

void _shuffle_playlist(track* playlist, int n);

int init_playlist_manager() {
    if (!file.open("library.json", FILE_READ)) {
        Serial.println("Failed to open library.json");
        return -1;
    }

    playlist = (track*) heap_caps_malloc(PLAYLIST_SIZE*sizeof(track), MALLOC_CAP_SPIRAM);
    if (playlist == NULL) {
        Serial.println("Could not allocate space for playlist in PSRAM");
        return -1;
    }

    filter["id"] = true;
    filter["duration_ms"] = true;
    filter["filepath"] = true;
    return 0;
}

int build_playlist() {
    int count = 0;
    JsonDocument song(&allocator);
    if (_genre == "") {
        filter["genre"] = false;
    } else {
        filter["genre"] = true;
    }

    int c;
    file.seekSet(0); //Reset library.json cursor
    while ((c = file.read()) != '{');
    file.seekCur(-1);
    while (count < PLAYLIST_SIZE) {
        DeserializationError error = deserializeJson(song, file, DeserializationOption::Filter(filter));
        if (error) {
            Serial.print("deserializeJson() failed: ");
            Serial.println(error.c_str());
            return -1;
        }

        if (_genre == "" || _genre == song["genre"]) {
            strncpy(playlist[count].id, song["id"].as<const char*>(), 24);
            strncpy(playlist[count].filepath, song["filepath"].as<const char*>(), 128);
            playlist[count].duration = song["duration_ms"].as<uint32_t>();
            count++;
        }

        //Read until next object
        bool done = false;
        while ((c = file.read())  != -1) {
            if (c == '{') {
                file.seekCur(-1);
                break;
            }
            if (c == ']') {
                done = true;
                break;
            }
        }
        if (done) break;
    }

    _shuffle_playlist(playlist, count);
    currentIndex = 0;
    return count;
}

String get_active_genre() {
    return _genre;
}

void set_active_genre(String genre) {
    _genre = genre;
}

void _shuffle_playlist(track* playlist, int n) {
    for (int i = n - 1; i > 0; i--) {
        int j = rand() % (i + 1);
        track tmp = playlist[j];
        playlist[j] = playlist[i];
        playlist[i] = tmp;
    }
}