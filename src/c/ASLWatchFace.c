#include <pebble.h>
#include "settings.h"
#include "messaging.h"

#define HOUR_STR_LEN 3
#define MIN_STR_LEN 3
#define TIME_STR_LEN 5
#define DOW_STR_LEN 10
#define FULL_DATE_STR_LEN 16
#define INFO_LAYER_HEIGHT 18
#define LAYER_GAP 5
#define SMALL_CLOCK_LAYER_HEIGHT 35
#define LARGE_CLOCK_LAYER_HEIGHT 50
#define SMALL_SCREEN_HEIGHT 200

static Window *s_window;
static TextLayer *s_dow_layer;
static TextLayer *s_time_layer;
static TextLayer *s_date_layer;
static GFont s_font_asl;
static GFont s_font_asl_sm;
static bool s_use_small_font;

static char s_hour_buf[HOUR_STR_LEN];
static char s_min_buf[MIN_STR_LEN];
static char s_time_buf[TIME_STR_LEN];
static char s_dow_buf[DOW_STR_LEN];
static char s_full_date_buf[FULL_DATE_STR_LEN];

static TextLayer *create_text_layer(Layer *parent, GRect frame, GFont font) {
  TextLayer *layer = text_layer_create(frame);
  text_layer_set_font(layer, font);
  text_layer_set_text_alignment(layer, GTextAlignmentCenter);
  text_layer_set_background_color(layer, GColorClear);
  layer_add_child(parent, text_layer_get_layer(layer));
  return layer;
}

static int get_clock_layer_height() {
  if (s_use_small_font) {
    return SMALL_CLOCK_LAYER_HEIGHT;
  }
  return LARGE_CLOCK_LAYER_HEIGHT;
}

static int get_clock_top_gap() {
  if (s_use_small_font) {
    return LAYER_GAP;
  }
  return LAYER_GAP + (LARGE_CLOCK_LAYER_HEIGHT - 45);
}

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

  strftime(s_dow_buf, sizeof(s_dow_buf), "%A", t);
  strftime(s_full_date_buf, sizeof(s_full_date_buf), "%b %e, %Y", t);

  text_layer_set_text(s_dow_layer, s_dow_buf);
  text_layer_set_text(s_time_layer, s_time_buf);
  text_layer_set_text(s_date_layer, s_full_date_buf);
}

static GFont get_selected_font() {
  if (s_use_small_font) {
    return s_font_asl_sm;
  }
  return s_font_asl;
}

static void apply_settings() {
  window_set_background_color(s_window, globalSettings.bgColor);
  text_layer_set_text_color(s_dow_layer, globalSettings.infoColor);
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
  GFont info_font = fonts_get_system_font(FONT_KEY_GOTHIC_14);

  // Load fonts (large and small variants)
  s_font_asl = fonts_load_custom_font(
      resource_get_handle(RESOURCE_ID_FONT_ASL_64));
  s_font_asl_sm = fonts_load_custom_font(
      resource_get_handle(RESOURCE_ID_FONT_ASL_42));

  // Use small fonts on smaller screens (Flint, etc.), large on Emery
  s_use_small_font = bounds.size.h < SMALL_SCREEN_HEIGHT;
  GFont font = get_selected_font();
  int clock_top_gap = get_clock_top_gap();
  int clock_h = get_clock_layer_height();
  int stack_h = INFO_LAYER_HEIGHT + clock_top_gap + clock_h + LAYER_GAP + INFO_LAYER_HEIGHT;
  int stack_top = (bounds.size.h - stack_h) / 2;
  int dow_y = stack_top;
  int time_y = dow_y + INFO_LAYER_HEIGHT + clock_top_gap;
  int date_y = time_y + clock_h + LAYER_GAP;

  s_dow_layer = create_text_layer(
      root, GRect(0, dow_y, bounds.size.w, INFO_LAYER_HEIGHT), info_font);
  s_time_layer = create_text_layer(
      root, GRect(0, time_y, bounds.size.w, clock_h), font);
  s_date_layer = create_text_layer(
      root, GRect(0, date_y, bounds.size.w, INFO_LAYER_HEIGHT), info_font);

  apply_settings();
  update_display();
}

static void window_unload(Window *window) {
  text_layer_destroy(s_dow_layer);
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
