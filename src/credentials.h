#pragma once
#include <Preferences.h>

struct Credentials {
    String SSID;
    String wifiPassword;
    String serverURL;
    String username;
    String password;
    bool valid;
};

Credentials loadCredentials() {
    Preferences prefs;
    Credentials creds;

    prefs.begin("earworm", true);

    creds.SSID = prefs.getString("ssid", "");
    creds.wifiPassword = prefs.getString("wifiPass", "");
    creds.serverURL = prefs.getString("server", "");
    creds.username = prefs.getString("user", "");
    creds.password = prefs.getString("password", "");
    prefs.end();

    creds.valid = creds.SSID.length() > 0 &&
                  creds.wifiPassword.length() > 0 &&
                  creds.serverURL.length() > 0 &&
                  creds.username.length() > 0 &&
                  creds.password.length() > 0;
    
    return creds;
}