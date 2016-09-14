#include <pebble.h>

static Window *s_main_window;
static GFont s_time_font, s_date_font, s_fsociety_font;
static TextLayer *s_time_layer, *s_date_layer, *s_hashtag_layer;
static Layer *line;

// ////////////////////////
// // update temperature //
// ////////////////////////
// static void line_update_proc(Layer *layer, GContext *ctx) {
//   GRect bounds = layer_get_bounds(layer);
//   graphics_context_set_fill_color(ctx, GColorBlack);
//   graphics_context_set_stroke_width(ctx, 1);
//   graphics_draw_line(ctx, GPoint(0, 40), GPoint(bounds.size.w, 40));
//   graphics_draw_line(ctx, GPoint(0, bounds.size.h/2), GPoint(bounds.size.w, bounds.size.h/2));
//   graphics_draw_line(ctx, GPoint(0, 118), GPoint(bounds.size.w, 118));
// }

//////////////////////
// load main window //
//////////////////////
static void main_window_load(Window *window) {
  
  //////////////////////////
  // set background color //
  //////////////////////////
  window_set_background_color(window, GColorWhite); // default GColorWhite
  
  ///////////////////////
  // get window bounds //
  ///////////////////////
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);
  
  ///////////////////////
  // load custom fonts //
  ///////////////////////
  s_fsociety_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_ULTRALIGHT_10));
  s_date_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_MR_ROBOT_42));
  s_time_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_MR_ROBOT_48));
  
//   // create temp line
//   line = layer_create(bounds);
//   layer_set_update_proc(line, line_update_proc);
//   layer_add_child(window_layer, line);
  
  /////////////////////
  // load time layer //
  /////////////////////
  s_time_layer = text_layer_create(GRect(0, 0+35, bounds.size.w, bounds.size.h/2));
  text_layer_set_background_color(s_time_layer, GColorClear); // default GColorWhite
//   text_layer_set_text_color(s_time_layer, GColorWhite); // default GColorBlack
  text_layer_set_text_alignment(s_time_layer, GTextAlignmentCenter); // default GTextAlignmentLeft
  text_layer_set_font(s_time_layer, s_time_font); // load custom font
//   text_layer_set_text(s_time_layer, "20:00");
  layer_add_child(window_layer, text_layer_get_layer(s_time_layer)); // add layer to window
  
  /////////////////////
  // load date layer //
  /////////////////////
  s_date_layer = text_layer_create(GRect(0, bounds.size.h/2-3, bounds.size.w, bounds.size.h/2));
  text_layer_set_background_color(s_date_layer, GColorClear); // default GColorWhite
//   text_layer_set_text_color(s_date_layer, GColorWhite); // default GColorBlack
  text_layer_set_text_alignment(s_date_layer, GTextAlignmentCenter); // default GTextAlignmentLeft
  text_layer_set_font(s_date_layer, s_date_font); // load custom font
  text_layer_set_text(s_date_layer, "09:55");
  layer_add_child(window_layer, text_layer_get_layer(s_date_layer)); // add layer to window  
  
  ///////////////
  // #fsoceity //
  ///////////////
  s_hashtag_layer = text_layer_create(GRect(0, bounds.size.h-13, bounds.size.w, 13));
  text_layer_set_background_color(s_hashtag_layer, GColorClear); // default GColorWhite
//   text_layer_set_text_color(s_hashtag_layer, GColorWhite); // default GColorBlack
  text_layer_set_text_alignment(s_hashtag_layer, GTextAlignmentCenter); // default GTextAlignmentLeft
  text_layer_set_font(s_hashtag_layer, s_fsociety_font);
  text_layer_set_text(s_hashtag_layer, "#fsociety");
  layer_add_child(window_layer, text_layer_get_layer(s_hashtag_layer)); // add layer to window          
}

//////////////////////////////
// update time every minute //
//////////////////////////////
static void update_time() {
  
  ////////////////////////
  // get a tm strucutre //
  ////////////////////////
  time_t temp = time(NULL);
  struct tm *tick_time = localtime(&temp);
  
  ///////////////////////////////////////////
  // write the current hours into a buffer //
  ///////////////////////////////////////////
  static char s_time_buffer[8];
  strftime(s_time_buffer, sizeof(s_time_buffer), "%I:%M", tick_time);
  // %k = Hour 0..23
  // %l = Hour 0..12
  
  static char s_date_buffer[8];
  strftime(s_date_buffer, sizeof(s_date_buffer), "%a%e", tick_time);
  
  static char s_weekday_buffer[8];
  strftime(s_weekday_buffer, sizeof(s_weekday_buffer), "%a", tick_time);
  
  ////////////////
  // write time //
  ////////////////
  text_layer_set_text(s_time_layer, s_time_buffer+((' ' == s_time_buffer[0])?1:0)); // remove leading space from %l
  
  ////////////////
  // write date //
  ////////////////
  text_layer_set_text(s_date_layer, s_date_buffer);
}

/////////////////////////////////////////////
// allows app to subscribe to time updates //
/////////////////////////////////////////////
static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
  update_time();
}

////////////////////////
// main window unload //
////////////////////////
static void main_window_unload(Window *window) {
  fonts_unload_custom_font(s_time_font);
  fonts_unload_custom_font(s_date_font);
  text_layer_destroy(s_time_layer);
  text_layer_destroy(s_date_layer);
  text_layer_destroy(s_hashtag_layer);
}

////////////////////
// initialize app //
////////////////////
void init(void) {
  s_main_window = window_create();
  window_set_window_handlers(s_main_window, (WindowHandlers) {
    .load = main_window_load,
    .unload = main_window_unload
  });  

  window_stack_push(s_main_window, true);
  
  ////////////////////////////////
  // register for clock updates //
  ////////////////////////////////
  tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);
    
  ////////////////////////////////////////////////////
  // Make sure the time is displayed from the start //
  ////////////////////////////////////////////////////
  update_time();  
}

///////////////////////
// de-initialize app //
///////////////////////
void deinit(void) {
  window_destroy(s_main_window);
}

/////////////
// run app //
/////////////
int main(void) {
  init();
  app_event_loop();
  deinit();
}