#include <pebble.h>
#include "settings.h"
#include "messaging.h"

#define HOUR_STR_LEN 3
#define MIN_STR_LEN 3
#define TIME_STR_LEN 5
#define DATE_STR_LEN 12

static Window *s_window;
static TextLayer *s_time_layer;
static TextLayer *s_date_layer;
static GFont s_font_asl;
static GFont s_font_asl_sm;
static bool s_use_small_font;

static char s_hour_buf[HOUR_STR_LEN];
static char s_min_buf[MIN_STR_LEN];
static char s_time_buf[TIME_STR_LEN];
static char s_date_buf[DATE_STR_LEN];

static void update_display() {
  time_t now = time(NULL);
  struct tm *t = localtime(&now);

  if (clock_is_24h_style()) {
    strftime(s_hour_buf, sizeof(s_hour_buf), "%H", t);
  } else {
    strftime(s_hour_buf, sizeof(s_hour_buf), "%I", t);
    if (s_hour_buf[0] == '0') {
      memmove(s_hour_buf, s_hour_buf + 1, strlen(s_hour_buf));
    }
  }
  strftime(s_min_buf, sizeof(s_min_buf), "%M", t);
  snprintf(s_time_buf, sizeof(s_time_buf), "%s%s", s_hour_buf, s_min_buf);

  strftime(s_date_buf, sizeof(s_date_buf), "%a %e", t);

  text_layer_set_text(s_time_layer, s_time_buf);
  text_layer_set_text(s_date_layer, s_date_buf);
}

static GFont get_selected_font() {
  if (s_use_small_font) {
    return s_font_asl_sm;
  }
  return s_font_asl;
}

static void apply_settings() {
  window_set_background_color(s_window, globalSettings.bgColor);
  text_layer_set_text_color(s_time_layer, globalSettings.timeColor);
  text_layer_set_text_color(s_date_layer, globalSettings.infoColor);
  GFont font = get_selected_font();
  text_layer_set_font(s_time_layer, font);
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

  // Load fonts (large and small variants)
  s_font_asl = fonts_load_custom_font(
      resource_get_handle(RESOURCE_ID_FONT_ASL_64));
  s_font_asl_sm = fonts_load_custom_font(
      resource_get_handle(RESOURCE_ID_FONT_ASL_42));

  // Use small fonts on smaller screens (Flint, etc.), large on Emery
  s_use_small_font = bounds.size.h < 200;
  GFont font = get_selected_font();

  int row_h = s_use_small_font ? 65 : 85;
  int start_y = (bounds.size.h - row_h) / 2 - 4;

  s_time_layer = text_layer_create(GRect(0, start_y, bounds.size.w, row_h));
  text_layer_set_font(s_time_layer, font);
  text_layer_set_text_alignment(s_time_layer, GTextAlignmentCenter);
  text_layer_set_background_color(s_time_layer, GColorClear);
  layer_add_child(root, text_layer_get_layer(s_time_layer));

  // Date layer — centered on round displays, top-right on rectangular
  #ifdef PBL_ROUND
    GRect date_rect = GRect(0, 26, bounds.size.w, 24);
    GTextAlignment date_align = GTextAlignmentCenter;
  #else
    GRect date_rect = GRect(bounds.size.w - 74, 4, 70, 24);
    GTextAlignment date_align = GTextAlignmentRight;
  #endif
  s_date_layer = text_layer_create(date_rect);
  text_layer_set_font(s_date_layer,
                      fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD));
  text_layer_set_text_alignment(s_date_layer, date_align);
  text_layer_set_background_color(s_date_layer, GColorClear);
  layer_add_child(root, text_layer_get_layer(s_date_layer));

  apply_settings();
  update_display();
}

static void window_unload(Window *window) {
  text_layer_destroy(s_time_layer);
  text_layer_destroy(s_date_layer);
  fonts_unload_custom_font(s_font_asl);
  fonts_unload_custom_font(s_font_asl_sm);
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
