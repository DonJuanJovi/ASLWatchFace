#include <pebble.h>
#include "settings.h"
#include "messaging.h"

#define TIME_STR_LEN 6
#define DATE_STR_LEN 12

static Window *s_window;
static TextLayer *s_time_layer;
static TextLayer *s_date_layer;
static GFont s_time_font;

static char s_time_buf[TIME_STR_LEN];
static char s_date_buf[DATE_STR_LEN];

static void update_display() {
  time_t now = time(NULL);
  struct tm *t = localtime(&now);

  if (clock_is_24h_style()) {
    strftime(s_time_buf, sizeof(s_time_buf), "%H:%M", t);
  } else {
    strftime(s_time_buf, sizeof(s_time_buf), "%I:%M", t);
    if (s_time_buf[0] == '0') {
      memmove(s_time_buf, s_time_buf + 1, strlen(s_time_buf));
    }
  }

  strftime(s_date_buf, sizeof(s_date_buf), "%a %e", t);

  text_layer_set_text(s_time_layer, s_time_buf);
  text_layer_set_text(s_date_layer, s_date_buf);
}

static void apply_settings() {
  window_set_background_color(s_window, globalSettings.bgColor);
  text_layer_set_text_color(s_time_layer, globalSettings.timeColor);
  text_layer_set_text_color(s_date_layer, globalSettings.infoColor);
}

static void on_settings_changed() {
  apply_settings();
  update_display();
}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
  update_display();
}

static void window_load(Window *window) {
  Layer *root = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(root);

  // Time layer — custom ASL font, centered on screen
  s_time_font = fonts_load_custom_font(
      resource_get_handle(RESOURCE_ID_FONT_ASL_68));
  int time_y = (bounds.size.h / 2) - 40;
  s_time_layer = text_layer_create(GRect(0, time_y, bounds.size.w, 76));
  text_layer_set_font(s_time_layer, s_time_font);
  text_layer_set_text_alignment(s_time_layer, GTextAlignmentCenter);
  text_layer_set_background_color(s_time_layer, GColorClear);
  layer_add_child(root, text_layer_get_layer(s_time_layer));

  // Date layer — day-of-week + day-of-month, top right
  #ifdef PBL_ROUND
    GRect date_rect = GRect(bounds.size.w / 2 + 2, 26,
                            bounds.size.w / 2 - 12, 24);
  #else
    GRect date_rect = GRect(bounds.size.w - 74, 4, 70, 24);
  #endif
  s_date_layer = text_layer_create(date_rect);
  text_layer_set_font(s_date_layer,
                      fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD));
  text_layer_set_text_alignment(s_date_layer, GTextAlignmentRight);
  text_layer_set_background_color(s_date_layer, GColorClear);
  layer_add_child(root, text_layer_get_layer(s_date_layer));

  apply_settings();
  update_display();
}

static void window_unload(Window *window) {
  text_layer_destroy(s_time_layer);
  text_layer_destroy(s_date_layer);
  fonts_unload_custom_font(s_time_font);
}

static void init() {
  Settings_init();
  messaging_init(on_settings_changed);

  s_window = window_create();
  window_set_window_handlers(s_window, (WindowHandlers) {
    .load = window_load,
    .unload = window_unload
  });
  window_stack_push(s_window, true);

  tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);
}

static void deinit() {
  window_destroy(s_window);
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}
