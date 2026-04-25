#pragma once
#include <pebble.h>

#define SETTINGS_PERSIST_KEY 1

#define DEFAULT_FONT_CHOICE 0

#define DEFAULT_BG_COLOR   GColorBlack
#define DEFAULT_TIME_COLOR GColorWhite
#define DEFAULT_INFO_COLOR GColorLightGray

typedef struct {
  GColor bgColor;
  GColor timeColor;
  GColor infoColor;
  uint8_t fontChoice;  // 0 = ASL, 1 = Dissaramas
} Settings;

extern Settings globalSettings;

void Settings_init();
void Settings_saveToStorage();
