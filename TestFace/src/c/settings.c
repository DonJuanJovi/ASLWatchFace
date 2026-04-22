#include "settings.h"
#include <pebble.h>

Settings globalSettings;

void Settings_init() {
  globalSettings.bgColor       = DEFAULT_BG_COLOR;
  globalSettings.timeColor     = DEFAULT_TIME_COLOR;
  globalSettings.timeFillColor = DEFAULT_TIME_FILL_COLOR;
  globalSettings.infoColor     = DEFAULT_INFO_COLOR;

  if (persist_exists(SETTINGS_PERSIST_KEY)) {
    persist_read_data(SETTINGS_PERSIST_KEY, &globalSettings, sizeof(Settings));
  }
}

void Settings_saveToStorage() {
  persist_write_data(SETTINGS_PERSIST_KEY, &globalSettings, sizeof(Settings));
}
