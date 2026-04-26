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


track* playlist;
const uint MAX_SIZE = 500;
uint currentIndex = 0;

uint playlist_size = 0;

JsonDocument filter;

void _shuffle_playlist(track* playlist, int n);

int init_playlist_manager() {


    playlist = (track*) heap_caps_malloc(MAX_SIZE*sizeof(track), MALLOC_CAP_SPIRAM);
    if (playlist == NULL) {
        Serial.println("Could not allocate space for playlist in PSRAM");
        return -1;
    }

    filter["id"] = true;
    filter["duration_ms"] = true;
    filter["filepath"] = true;
    return 0;
}

int build_playlist(SdFat& sd) {
    int count = 0;
    JsonDocument song(&allocator);
    if (_genre == "") {
        filter["genre"] = false;
    } else {
        filter["genre"] = true;
    }
    File32 file = sd.open("/library.json", FILE_READ);
    if (!file) {
        Serial.println("Failed to open library.json");
        return -1;
    }

    if (!file.find("[")) {
        Serial.println("/library.json is missing array start '['");
        return -1;
    }
    do {
        DeserializationError error = deserializeJson(song, file, DeserializationOption::Filter(filter));
        if (error) {
            Serial.print("deserializeJson() failed: ");
            Serial.println(error.c_str());
            return -1;
        }
        if (_genre == "" || _genre == song["genre"]) {
            strncpy(playlist[count].id, song["id"].as<const char*>(), 24);
            playlist[count].id[sizeof(playlist[count].id) - 1] = '\0';
            strncpy(playlist[count].filepath, song["filepath"].as<const char*>(), 128);
            playlist[count].filepath[sizeof(playlist[count].filepath) - 1] = '\0';
            playlist[count].duration = song["duration_ms"].as<uint32_t>();
            count++;
        }

    } while (count < MAX_SIZE && file.findUntil(",","]"));

    playlist_size = count;
    _shuffle_playlist(playlist, count);
    currentIndex = 0;
    return count;
}


track* get_next_track() {
    currentIndex += 1;
    if (currentIndex == playlist_size) {
        _shuffle_playlist(playlist, playlist_size);
        currentIndex = 0;
        return get_current_track();
    }
    return get_current_track();
}

track* get_prev_track() {
    if (currentIndex == 0) {
        return get_current_track();
    }
    currentIndex -= 1;
    return get_current_track();
}

track* get_current_track() {
    return &playlist[currentIndex];
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