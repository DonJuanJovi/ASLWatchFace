#pragma once
#include <pebble.h>

#define SETTINGS_PERSIST_KEY 1

#ifdef PBL_COLOR
  #define DEFAULT_BG_COLOR   GColorBlack
  #define DEFAULT_TIME_COLOR GColorWhite
  #define DEFAULT_INFO_COLOR GColorLightGray
#else
  #define DEFAULT_BG_COLOR   GColorBlack
  #define DEFAULT_TIME_COLOR GColorWhite
  #define DEFAULT_INFO_COLOR GColorWhite
#endif

typedef struct {
  GColor bgColor;
  GColor timeColor;
  GColor infoColor;
} Settings;

extern Settings globalSettings;

void Settings_init();
void Settings_saveToStorage();
