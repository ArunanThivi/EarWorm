#include <Arduino.h>

struct track {
    char id[24];
    char filepath[128];
    uint32_t duration;
};

int init_playlist_manager();

int build_playlist();

String get_active_genre();

void set_active_genre(String genre);