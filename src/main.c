//    By daniel.navarro.gomez@gmail.com
//
//    This program is free software: you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation, either version 3 of the License, or
//    (at your option) any later version.
//
//    This program is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//    <http://www.gnu.org/licenses/>.

#include <pebble.h>
#include "progress_bar.h"

Window *window;
Window *window2;
static SimpleMenuItem  *menu_items;
SimpleMenuLayer *menu_layer; 
ActionBarLayer *action_bar;
static TextLayer *text_layer;

static SimpleMenuSection menu_sections[1];      
static SimpleMenuItem menu_section0_items[10];  

static AppSync sync;
static uint8_t sync_buffer[128];

static AppTimer *timer;
int updateDelay = 10000;

static GBitmap *icon_ff_gbitmap;
static GBitmap *icon_rev_gbitmap;
static GBitmap *icon_playpause_gbitmap;

static GBitmap *icon_media_gbitmap;
static GBitmap *icon_down_gbitmap;
static GBitmap *icon_left_gbitmap;
static GBitmap *icon_right_gbitmap;
static GBitmap *icon_up_gbitmap;
static GBitmap *icon_select_gbitmap;
static GBitmap *icon_home_gbitmap;
static GBitmap *icon_info_gbitmap;
static GBitmap *icon_title_gbitmap;
static GBitmap *icon_back_gbitmap;
static GBitmap *icon_vol_down_gbitmap;
static GBitmap *icon_vol_up_gbitmap;

static ProgressBarLayer *seek_bar;

bool Vol_Control=false;


// Functions

void send_button_javascript(int button_cod){
	DictionaryIterator *iter;
 	app_message_outbox_begin(&iter);
	Tuplet value = TupletInteger(99, button_cod);
	dict_write_tuplet(iter, &value);
	dict_write_end(iter);
	app_message_outbox_send();
	
}

static void increment_click_handler(ClickRecognizerRef recognizer, void *context) {
	if (Vol_Control==true){
		send_button_javascript(30);
	}
	else{
		send_button_javascript(10);
	}
}

static void decrement_click_handler(ClickRecognizerRef recognizer, void *context) {
	if (Vol_Control==true){
		send_button_javascript(31);
	}
	else{
		send_button_javascript(11);
	}
}

static void select_click_handler(ClickRecognizerRef recognizer, void *context) {
	send_button_javascript(12);
}

static void long_increment_click_handler(ClickRecognizerRef recognizer, void *context) {
	if (Vol_Control==true){
		send_button_javascript(32);
	}
	else{
		send_button_javascript(13);
	}
}

static void long_decrement_click_handler(ClickRecognizerRef recognizer, void *context) {
	if (Vol_Control==true){
		send_button_javascript(32);
	}
	else{
		send_button_javascript(14);
	}
}


static void long_select_click_handler(ClickRecognizerRef recognizer, void *context) {
	send_button_javascript(15);
}


static void double_select_click_handler(ClickRecognizerRef recognizer, void *context) {

	if (Vol_Control==true){
		Vol_Control=false;
		action_bar_layer_set_icon(action_bar, BUTTON_ID_UP, icon_rev_gbitmap);
		action_bar_layer_set_icon(action_bar, BUTTON_ID_DOWN, icon_ff_gbitmap);
	}
	else{
		Vol_Control=true;
		action_bar_layer_set_icon(action_bar, BUTTON_ID_UP, icon_vol_up_gbitmap);
		action_bar_layer_set_icon(action_bar, BUTTON_ID_DOWN, icon_vol_down_gbitmap);
	}
	
}

static void click_config_provider(void *context) {
	const uint16_t repeat_interval_ms = 50;
	const uint16_t long_press_ms = 1000;
	window_single_repeating_click_subscribe(BUTTON_ID_UP, repeat_interval_ms, (ClickHandler) increment_click_handler);
	window_single_repeating_click_subscribe(BUTTON_ID_DOWN, repeat_interval_ms, (ClickHandler) decrement_click_handler);
	window_single_repeating_click_subscribe(BUTTON_ID_SELECT, repeat_interval_ms, (ClickHandler) select_click_handler);
	window_long_click_subscribe(BUTTON_ID_UP, long_press_ms , (ClickHandler) long_increment_click_handler,NULL);
	window_long_click_subscribe(BUTTON_ID_DOWN, long_press_ms , (ClickHandler) long_decrement_click_handler,NULL);
	window_long_click_subscribe(BUTTON_ID_SELECT, long_press_ms , (ClickHandler) long_select_click_handler,NULL);
	window_multi_click_subscribe(BUTTON_ID_SELECT, 2,2, 500,true, (ClickHandler) double_select_click_handler);
}


static void sync_error_callback(DictionaryResult dict_error, AppMessageResult app_message_error, void *context) {
  APP_LOG(APP_LOG_LEVEL_DEBUG, "App Message Sync Error: %d", app_message_error);
}

static void sync_tuple_changed_callback(const uint32_t key, const Tuple* new_tuple, const Tuple* old_tuple, void* context) {
  switch (key) {
    case 0: // Play info
	  	APP_LOG(APP_LOG_LEVEL_DEBUG, "TITTLE: %s", new_tuple->value->cstring);
      	text_layer_set_text(text_layer, new_tuple->value->cstring);
	  	text_layer_set_font(text_layer, fonts_get_system_font(FONT_KEY_GOTHIC_24));
      break;
	case 1: // percent done
	  APP_LOG(APP_LOG_LEVEL_DEBUG, "PerCent: %d", new_tuple->value->int16);
	  progress_bar_layer_set_value(seek_bar, new_tuple->value->int16);
	  break;
  }
}


static void timer_handle(void* data){
	send_button_javascript(20);
  	timer = app_timer_register(updateDelay, timer_handle, NULL);
}



void launch_action_bar() {
	send_button_javascript(20);
	Tuplet initial_values[] = {
		TupletCString(0, "Loading..."),
		TupletInteger(1, 0),
	};
	app_sync_init(&sync, sync_buffer, sizeof(sync_buffer), initial_values, ARRAY_LENGTH(initial_values), sync_tuple_changed_callback, sync_error_callback, NULL);
	icon_rev_gbitmap = gbitmap_create_with_resource(RESOURCE_ID_ICON_REV);
	icon_ff_gbitmap = gbitmap_create_with_resource(RESOURCE_ID_ICON_FF);	
	icon_playpause_gbitmap = gbitmap_create_with_resource(RESOURCE_ID_ICON_PLAY_PAUSE);
	
	window_stack_push(window, true /* Animated */);
	Layer *window_layer = window_get_root_layer(window);
	
	
	GRect bounds = layer_get_frame(window_layer);
	text_layer = text_layer_create((GRect){ .origin = { 0, 0 }, .size = { bounds.size.w - 28, bounds.size.h } });
	text_layer_set_font(text_layer, fonts_get_system_font(FONT_KEY_GOTHIC_24));
	text_layer_set_text_alignment(text_layer, GTextAlignmentLeft);
	layer_add_child(window_layer, text_layer_get_layer(text_layer));
		
	seek_bar = progress_bar_layer_create((GRect) { .origin = { 5, 138 }, .size = { 115, 8 } });
	progress_bar_layer_set_orientation(seek_bar, ProgressBarOrientationHorizontal);
	progress_bar_layer_set_range(seek_bar, 0, 100);
	progress_bar_layer_set_frame_color(seek_bar, GColorBlack);
	progress_bar_layer_set_bar_color(seek_bar, GColorBlack);
	layer_add_child(window_layer, seek_bar);	
	
	// Initialize the action bar:
	action_bar = action_bar_layer_create(); 
	// Associate the action bar with the window:
	action_bar_layer_add_to_window(action_bar, window);
	action_bar_layer_set_click_config_provider(action_bar, click_config_provider);
	// Set the icons:
	action_bar_layer_set_icon(action_bar, BUTTON_ID_UP, icon_rev_gbitmap);
	action_bar_layer_set_icon(action_bar, BUTTON_ID_DOWN, icon_ff_gbitmap);
	action_bar_layer_set_icon(action_bar, BUTTON_ID_SELECT, icon_playpause_gbitmap);
	Vol_Control=false;
	
	//#####TIMER
	timer = app_timer_register(updateDelay, timer_handle, NULL);
}


void buttonUP (int index, void *context){
	send_button_javascript(0);
	
}

void buttonDOWN (int index, void *context){
	send_button_javascript(1);
}

void buttonSELECT (int index, void *context){
	send_button_javascript(2);
}

void buttonBACK (int index, void *context){
	send_button_javascript(3);
}

void buttonLEFT (int index, void *context){
	send_button_javascript(4);
}

void buttonRIGHT (int index, void *context){
	send_button_javascript(5);
}

void buttonHOME (int index, void *context){
	send_button_javascript(6);
}

void buttonTITLE (int index, void *context){
	send_button_javascript(7);
}

void buttonINFO (int index, void *context){
	send_button_javascript(8);
}

void buttonVolUp (int index, void *context){
	send_button_javascript(30);
}

void buttonVolDown (int index, void *context){
	send_button_javascript(31);
}

void buttonMUTE (int index, void *context){
	send_button_javascript(32);
}

void createMenuButtons(){
	icon_media_gbitmap=gbitmap_create_with_resource(RESOURCE_ID_ICON_MEDIA);
	icon_down_gbitmap=gbitmap_create_with_resource(RESOURCE_ID_ICON_DOWN);
	icon_left_gbitmap=gbitmap_create_with_resource(RESOURCE_ID_ICON_LEFT);
	icon_right_gbitmap=gbitmap_create_with_resource(RESOURCE_ID_ICON_RIGHT);
	icon_up_gbitmap=gbitmap_create_with_resource(RESOURCE_ID_ICON_UP);
	icon_select_gbitmap=gbitmap_create_with_resource(RESOURCE_ID_ICON_SELECT);
	icon_home_gbitmap=gbitmap_create_with_resource(RESOURCE_ID_ICON_HOME);
	icon_info_gbitmap=gbitmap_create_with_resource(RESOURCE_ID_ICON_INFO);
	icon_title_gbitmap=gbitmap_create_with_resource(RESOURCE_ID_ICON_TITLE);
	icon_back_gbitmap=gbitmap_create_with_resource(RESOURCE_ID_ICON_BACK);
	icon_vol_down_gbitmap=gbitmap_create_with_resource(RESOURCE_ID_ICON_VOL_DOWN);
	icon_vol_up_gbitmap=gbitmap_create_with_resource(RESOURCE_ID_ICON_VOL_UP);
	window_stack_push(window2, true /* Animated */);
	    
    // Section "Settings..."
	menu_section0_items[0] = (SimpleMenuItem) {
        .title = "Media Control",
        .icon = icon_media_gbitmap,
        .callback = launch_action_bar,
    };
    menu_section0_items[1] = (SimpleMenuItem) {
        .title = "UP",
        .icon = icon_up_gbitmap,
        .callback = buttonUP,
    };
	menu_section0_items[2] = (SimpleMenuItem) {
        .title = "DOWN",
        .icon = icon_down_gbitmap,
        .callback = buttonDOWN,
    };
	menu_section0_items[3] = (SimpleMenuItem) {
        .title = "SELECT",
        .icon = icon_select_gbitmap,
        .callback = buttonSELECT,
    };
	menu_section0_items[4] = (SimpleMenuItem) {
        .title = "BACK",
        .icon = icon_back_gbitmap,
        .callback = buttonBACK,
    };
	menu_section0_items[5] = (SimpleMenuItem) {
        .title = "LEFT",
        .icon = icon_left_gbitmap,
        .callback = buttonLEFT,
    };
	menu_section0_items[6] = (SimpleMenuItem) {
        .title = "RIGHT",
        .icon = icon_right_gbitmap,
        .callback = buttonRIGHT,
    };
	menu_section0_items[7] = (SimpleMenuItem) {
        .title = "HOME",
        .icon = icon_home_gbitmap,
        .callback = buttonHOME,
    };
	menu_section0_items[8] = (SimpleMenuItem) {
        .title = "TITLE",
        .icon = icon_title_gbitmap,
        .callback = buttonTITLE,
    };
	menu_section0_items[9] = (SimpleMenuItem) {
        .title = "INFO",
        .icon = icon_info_gbitmap,
        .callback = buttonINFO,
    };
		
    // Header
    menu_sections[0] = (SimpleMenuSection) {
        .title = "XBMC Remote",
        .items = menu_section0_items,
        .num_items = ARRAY_LENGTH(menu_section0_items)
    };
    
	
	Layer *window_layer = window_get_root_layer(window2);
    GRect bounds = layer_get_frame(window_layer);
	menu_layer = simple_menu_layer_create(bounds, window2, menu_sections, 1, NULL);
    layer_add_child(window_layer, simple_menu_layer_get_layer(menu_layer));
}



void handle_init(void) {
	
	const uint32_t inbound_size = 64;
	const uint32_t outbound_size = 64;
	app_message_open(inbound_size, outbound_size);
	window = window_create();
	window2 = window_create();
	createMenuButtons();
	
}

void handle_deinit(void) {
	text_layer_destroy(text_layer);
	progress_bar_layer_destroy(seek_bar);
	gbitmap_destroy(icon_rev_gbitmap);
	gbitmap_destroy(icon_ff_gbitmap);
	gbitmap_destroy(icon_playpause_gbitmap);
	gbitmap_destroy(icon_media_gbitmap);
	gbitmap_destroy(icon_down_gbitmap);
	gbitmap_destroy(icon_left_gbitmap);
	gbitmap_destroy(icon_right_gbitmap);
	gbitmap_destroy(icon_up_gbitmap);
	gbitmap_destroy(icon_select_gbitmap);
	gbitmap_destroy(icon_home_gbitmap);
	gbitmap_destroy(icon_info_gbitmap);
	gbitmap_destroy(icon_title_gbitmap);
	gbitmap_destroy(icon_back_gbitmap);
	gbitmap_destroy(icon_vol_down_gbitmap);
	gbitmap_destroy(icon_vol_up_gbitmap);
	action_bar_layer_destroy(action_bar);
	simple_menu_layer_destroy(menu_layer);
	free(menu_items);
	window_destroy(window);
	window_destroy(window2);
}

int main(void) {
	  handle_init();
	  app_event_loop();
	  handle_deinit();
}
