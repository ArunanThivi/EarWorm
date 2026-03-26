# Earworm

## Project Overview
This is a plan for building a portable media player that syncs with my Navidrome server via the subsonic API. The device features deep sleep capability for optimal battery life, wired audio output and a display for
album art and track information.
The interaction model is intentionally lean-back. Press play and the music runs indefinitely. A rotary encoder handles volume and genre filtering.
Control is minimal by design. 

**Key Features**
* Navidrome/Subsonic sync over WiFi
* Local music storage on MicroSD for offline playback
* High-quality wired audio output (via 3.5mm jack)
* 2.8" IPS display to show track information (title, album, artist, duration)
* Deep-sleep mode (months of standby)
* ~8hrs of playback time
* Built-in battery charging via USB-C
* Three modes:
    * Shuffle all tracks
    * Shuffle genre-specific tracks
    * Single track repeat

## Hardware Components

| Component | Product # | Cost |
|---|---|---|
| ESP32-S3 Feather | 5400 | $12.50 |
| 2.8" TFT Display | 1770 | $29.95 |
| I2S DAC  |  | $6.95 |
| MicroSD Breakout | 254 | $7.50 |
| 3.7V 2500mAh LiPo Battery | 328 | $14.95 |
| Rotary Encoder |  |  |
| Tactile Buttons |  |  |
| Perma-proto board |  |  |
| Jumper Wires |  | $8.00 |

## Wiring Connections

## Playback Behavior

### Playback Modes

### Track History

### Genre Filtering

### Rotary Encoder Modes


## Software Architecture

### Core Libraries

| Library | Purpose |
|---|---|
| Arduino.h | Main Arduino Library |
| WiFi.h | Built-in Wifi connectivity |
| HTTPClient.h | HTTP Requests for Subsonic API |
| Preferences.h | Managing NVS  for settings and state |
| SdFat.h | MicroSD Card file operations |
| ArduinoJSON.h | JSON Parsing for API responses |


### SD Card Directory Structure

```
/music/
    /artist_name/
        /album_name/
            track_num. track_tile.mp3
            cover.jpg
library.json
```

### library.json structure
```json

```

### Software Modules

**main.cpp**

`void setup()` - Initialize all hardware: display, DAC, SD Card, buttons, encoder, WiFi

`void loop()` - Main state machine. Dispatches to playback, UI, button, and encoder handlers each iteration

`void handle_playback()` - Process the current audio playback tick. Called every loop iteration

`handle_ui_updates()` - Refresh display elements based on current playback and encoder state.

`void check buttons()` - Read all 4 button states, debounce, detect long presses and dispatch actions

`void check_encoder()` - Read encoder rotation and click state. Dispatches to volume adjustment or genre scroll depending on encoder mode.

**audio_player.cpp**

`bool play_track(String filepath)` - Start playback of given filepath. Returns **true** if file found and playback started.

`void pause()` - Pause current playback. Retains position for resume

`void resume()` - Resume from paused position

`bool next_track()` - Skip to next track in shuffled playlist. Returns **false** if playlist is empty

`bool prev_track()` - Go to the previous track. Returns **false** if at the start of the playlist.

`void set_volume(int level)` - Set playback volume. Level is 0-100

`void set_repeat_single(bool enable)` - Enable or disable repeat single mode. When enabled, the current track loops until this is called with **false**.

`bool is_repeat_single()` - Returns **true** if Repeat Single mode is currently active. Used by UI to show/hide the repeat indicator.

`bool is_playing()` - Returns **true** if audio is currently playing. Used by the power manager idle timer and UI state.

`uint32_t get_playback_position()` - Returns elapsed playback time in milliseconds. Required by `draw_progress_bar()` and state persistence.

`uint32_t get_track_duration()` - Returns total track durationi in milliseconds. Returns 0 if unknown. Required for progress bar rendering.

**playlist_manager.cpp**

`int build_playlist(String genre)` - Build a shuffled playlist from `library.json` filtered by genre. If `genre` is empty, all tracks are used in playlist. Returns number of tracks in the new playlist, or 0 if genre pool is empty.

`const char* get_next_track()` - Returns the filepath of the next track in the shuffled order. Automatically rebuilds and reshuffles when the playlist is exhausted. Never returns `null` as long as `build_playlist()` succeeds

`const char* get_prev_track()` - Returns the filepath of the previous track in history. Returns `null` if at the beginning of history.

`const char* get_current_track()` - Returns the filepath of the currently active track.

`int get_available_genres()` - Populates internal genre list from `library.json` and returns the count of unique genres found. Used to populate encoder genre scroll list.

`const char* get_genre_at(int index)` - Returns the genre name at the given index from the genre list build by `get_available_genres()`. Returns `null` if index is out of range.

`const char* get_active_genre()` - Returns the currently active genre filter string. Returns empty string if set to All tracks

**encoder_manager.cpp**

`void init_encoder()` - Configure encoder GPIO pins

`int8_t get_rotation_delta()` - Returns the net rotation since last call: positive for clockwise, negative for counter-clockwise, 0 for no change. Resets internal delta after read.

`bool was_clicked()` - Returns true if the encoder button was pressed since last call. Resets the click flag after read.

`EncoderMode get_mode()` - Returns the current encoder mode: VOLUME or GENRE_SELECT.

`EncoderMode toggle_mode()` - Switch between VOLUME and GENRE_SELECT modes. Returns the new mode.

**subsonic.cpp**

`bool authenticate()` - Authenticate against the server using credentials preloaded into non-volative storage (NVS). Returns **true** on success. All subsequent API calls assume authentication has passed.

`int get_indexes(long modified_since)` - Call the Subsonic getIndexes endpoint. If `modified_since` is 0, fetches the full artist index. Otherwise, passes the `modified_since` parameter to fetch only updates. Returns number of artists retrieved or -1 on error.

`int get_artist(String artist_id, String[] album_array, int max_count)` - Fetch the album list for a given artist into a caller-provided array. Returns number of albums retrieved or -1 on error.

`int get_album(String album_id, String[] song_array, int max_count)` - Fetch the track list for a given album into a caller-provided array. Returns number of tracks recieved or -1 on error.

`bool download_track(String track_id, String filepath)` - Downlaod a track to the given SD card path using HTTPClient. Returns **true** if download completed successfully.

`bool download_cover_art(String album_id, String filepath)` - Download and save album art to the given SD card path using HTTPClient. Returns **true** on success.

**sync_manager.cpp**

`bool run_sync()` - Entry point for all sync operations. Checks *last_incremental_sync* timestamp in NVS: if absent, runs `full_sync()`; otherwise runs `incremental_sync()`. Also checks *last_full_sync* and triggers `reconciliation_sync()` if more than 30 days have elapsed. Returns **true** if completed without errors.

`bool full_sync()` - Fetch the complete artist index via `get_indexes(0)`, then walk every artist, album, and track. Downloads all missing content. Stores *last_full_sync* and *last_incremental_sync* timestamps in NVS on completion. Returns **true** if completed without errors.

`bool incremental_sync()` - Call `get_indexes(last_incremental_sync)` to fetch only artists modified since the last sync. For each changed artist, fetch albums and tracks and download anything new. Saves a per-artist timestamp to NVS after each artist completes, enabling partial resume on interruption. Updates *last_incremental_sync* on full completion. Returns **true** if completed without errors.

`bool reconciliation_sync()` - Fetch the full artist index via `get_indexes(0)` and diff against `library.json`. Removes any local tracks, albums, or artists no longer present on the server. Updates *last_full_sync* timestamp on completion. Returns **true** if completed without errors.

`int process_download_queue()` - Process the pending download queue one item at a time. Called from a FreeRTOS background task during playback. Returns the number of tracks downloaded in this pass.

`bool handle_wifi_connection()` - Attempt connection to SSIDs stored in NVS. Returns true if connected within the 10-second timeout, false if all attempts failed.

**ui_manager.cpp**

`void draw_now_playing()` - Render the full now-playing screen: album art, track name, artist, progress bar, battery, active genre, encoder mode, and Repeat Single indicator if active.

`bool draw_album_art()` - Render album art from the SD cache. Returns false if not found; shows placeholder graphic.

`void draw_progress_bar()` - Render playback progress bar using get_playback_position() and get_track_duration().

`void draw_battery_status()` - Display battery percentage from check_battery().

`void draw_sync_status()` - Show sync progress on screen.

`void draw_genre_selector†()` - Render the genre scroll list when encoder is in GENRE_SELECT mode. Highlights the currently selected genre. Shows All at the top of the list.

`void draw_encoder_mode_indicator†()` - Display a small icon or label indicating whether the encoder is in volume or genre selection mode.

`void draw_repeat_indicator†()` - Show or hide the Repeat Single indicator based on is_repeat_single().

**power_manager.cpp**

`void enter_deep_sleep()` - (no return)	Save playback state, active genre, encoder mode, and volume to NVS, then enter ESP32 deep sleep. Execution does not return — device wakes via GPIO interrupt on the power button.

`void wake_handler()` - Called during setup() to check wake reason. Restores saved state if waking from deep sleep.

`uint8_t check_battery()` - Read battery voltage via ADC and return estimated charge as a percentage (0–100).

`void auto_sleep_timer()` - Track idle time and call enter_deep_sleep() when the configurable timeout is reached. Resets on any button press or encoder interaction.

### Error States

* SD Card missing on boot - 
* `library.json` missing or corrupt - 
* WiFi unavailable on boot.
* Sync fails (server unreachable) - 
* SD Card full during sync - 
* Track missing or corrupt during playback - 
* Track replaced on server during active sync - 
* Critical battery level - 
* Album art missing - 