#include "messaging.h"
#include "settings.h"
#include <pebble.h>

static void (*s_settings_callback)(void);

static void inbox_received_handler(DictionaryIterator *iter, void *context) {
  Tuple *bg_t   = dict_find(iter, MESSAGE_KEY_SETTING_BG_COLOR);
  Tuple *tm_t   = dict_find(iter, MESSAGE_KEY_SETTING_TIME_COLOR);
  Tuple *info_t = dict_find(iter, MESSAGE_KEY_SETTING_INFO_COLOR);

  if (bg_t) {
    globalSettings.bgColor = GColorFromHEX(bg_t->value->int32);
  }
  if (tm_t) {
    globalSettings.timeColor = GColorFromHEX(tm_t->value->int32);
  }
  if (info_t) {
    globalSettings.infoColor = GColorFromHEX(info_t->value->int32);
  }

  Settings_saveToStorage();

  if (s_settings_callback) {
    s_settings_callback();
  }
}

void messaging_init(void (*settings_changed_callback)(void)) {
  s_settings_callback = settings_changed_callback;
  app_message_register_inbox_received(inbox_received_handler);
  app_message_open(256, 8);
}
