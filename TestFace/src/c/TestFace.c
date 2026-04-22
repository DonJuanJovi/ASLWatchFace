#include <pebble.h>
#include "settings.h"
#include "messaging.h"

#define HOUR_STR_LEN 3
#define MIN_STR_LEN 3
#define DATE_STR_LEN 12

static Window *s_window;
static TextLayer *s_hour_layer;
static TextLayer *s_min_layer;
static TextLayer *s_date_layer;
static GFont s_font_asl;
static GFont s_font_dissaramas;

static char s_hour_buf[HOUR_STR_LEN];
static char s_min_buf[MIN_STR_LEN];
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

  strftime(s_date_buf, sizeof(s_date_buf), "%a %e", t);

  text_layer_set_text(s_hour_layer, s_hour_buf);
  text_layer_set_text(s_min_layer, s_min_buf);
  text_layer_set_text(s_date_layer, s_date_buf);
}

static GFont get_selected_font() {
  return globalSettings.fontChoice == 1 ? s_font_dissaramas : s_font_asl;
}

static void apply_settings() {
  window_set_background_color(s_window, globalSettings.bgColor);
  text_layer_set_text_color(s_hour_layer, globalSettings.timeColor);
  text_layer_set_text_color(s_min_layer, globalSettings.timeColor);
  text_layer_set_text_color(s_date_layer, globalSettings.infoColor);
  GFont font = get_selected_font();
  text_layer_set_font(s_hour_layer, font);
  text_layer_set_font(s_min_layer, font);
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

  // Load both fonts
  s_font_asl = fonts_load_custom_font(
      resource_get_handle(RESOURCE_ID_FONT_ASL_55));
  s_font_dissaramas = fonts_load_custom_font(
      resource_get_handle(RESOURCE_ID_FONT_DISSARAMAS_48));
  GFont font = get_selected_font();
  int row_h = 85;
  int gap = -6;
  int total_h = row_h * 2 + gap;
  int start_y = (bounds.size.h - total_h) / 2 - 4;

  s_hour_layer = text_layer_create(GRect(0, start_y, bounds.size.w, row_h));
  text_layer_set_font(s_hour_layer, font);
  text_layer_set_text_alignment(s_hour_layer, GTextAlignmentCenter);
  text_layer_set_background_color(s_hour_layer, GColorClear);
  layer_add_child(root, text_layer_get_layer(s_hour_layer));

  // Minutes layer — bottom half, centered
  s_min_layer = text_layer_create(GRect(0, start_y + row_h + gap, bounds.size.w, row_h));
  text_layer_set_font(s_min_layer, font);
  text_layer_set_text_alignment(s_min_layer, GTextAlignmentCenter);
  text_layer_set_background_color(s_min_layer, GColorClear);
  layer_add_child(root, text_layer_get_layer(s_min_layer));

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
  text_layer_destroy(s_hour_layer);
  text_layer_destroy(s_min_layer);
  text_layer_destroy(s_date_layer);
  fonts_unload_custom_font(s_font_asl);
  fonts_unload_custom_font(s_font_dissaramas);
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
