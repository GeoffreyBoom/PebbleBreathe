#include <pebble.h>
#include <pebble_fonts.h>

//========================GUI ELEMENTS========================//

static Window *window;
static TextLayer *displayer;

static TextLayer *s_timelayer;
static TextLayer *s_datelayer;
static TextLayer *s_monthlayer;


//========================SETTINGS ELEMENTS========================//

//menu layer is used for menuing
static const SimpleMenuLayer *settings;
//a menu layer contains an array of sections
static const SimpleMenuSection *sections;
//a menu section contains an array of items
static const SimpleMenuItem *Items;


//========================SETTINGS FUNCTIONS========================//

static void Set_Short_Pulse();
static void Set_Long_Pulse();
static void Set_Hardcore_Pulse();

static void Set_Short_Wait();
static void Set_Long_Wait();
static void Set_Hardcore_Wait();

static void Set_Inhale_Time();
static void Set_Hold_Time();
static void Set_Exhale_Time();
static void Settings();


//========================BREATH ELEMENTS========================//

#define Short_Pulse  50
#define Long_Pulse 100
#define Hardcore_Pulse 150
#define Short_Wait 100
#define Long_Wait 150
#define Hardcore_Wait 200

#define Short 0
#define Long 1
#define Hardcore 2

int Pulse_Setting = Short;
int Wait_Setting  = Short;


//========================BREATH FUNCTIONS========================//

static void four(void*);
static void seven(void*);
static void eight(void*);


//========================INIT DEINIT========================//

static void init(void);
static void deinit(void);

static void loadwindow(Window* window);
static void unloadwindow(Window* window);


//========================HELPER FUNCTIONS========================//

static void tick_handler(struct tm *tick_time, TimeUnits units_changed );
static void update_time();

static void vibe();
static void vibe2();
static void vibe3();
static void vibecreate(uint32_t segments[], int num_vibes);


//========================FUNCTION DEFINITIONS========================//
static void up_click_handler(ClickRecognizerRef recognizer, void *context) {
  text_layer_set_text(displayer, "Up pressed!");
}

static void select_click_handler(ClickRecognizerRef recognizer, void *context) {
  text_layer_set_text(displayer, "Select pressed!");
}

static void down_click_handler(ClickRecognizerRef recognizer, void *context) {
  text_layer_set_text(displayer, "Down pressed!");
}

static void Set_Short_Pulse(){
  Pulse_Setting = Short;
}
static void Set_Long_Pulse(){
  Pulse_Setting = Long;
}
static void Set_Hardcore_Pulse(){
  Pulse_Setting = Hardcore;
}

static void Set_Short_Wait(){
  Wait_Setting = Short;
}
static void Set_Long_Wait(){
  Wait_Setting = Long;
}
static void Set_Hardcore_Wait(){
  Wait_Setting = Hardcore;
}

static void Set_Inhale_Time();
static void Set_Hold_Time();
static void Set_Exhale_Time();

static void Settings();

//========================BREATH FUNCTIONS========================//

static void four(void* nil){
  vibe();
  app_timer_register(4000, seven, NULL);
}
static void seven(void* nil){
  vibe2();
  app_timer_register(7000, eight, NULL);
}
static void eight(void* nil){
  vibe3();
  app_timer_register(8000,four, NULL);
}

//========================INIT DEINIT========================//

static void click_config_provider(void *context) {
  // Register the ClickHandlers
  window_long_click_subscribe(BUTTON_ID_UP,100, up_click_handler, NULL);
  window_single_click_subscribe(BUTTON_ID_BACK, up_click_handler);
  window_single_click_subscribe(BUTTON_ID_SELECT, select_click_handler);
  window_single_click_subscribe(BUTTON_ID_DOWN, down_click_handler);
}

static void loadwindow(Window* window){
  
//---------------------create clock----------------------//
  //create text layer
  s_timelayer = text_layer_create(GRect(0,50,144,50));
  s_datelayer = text_layer_create(GRect(0,85,144,50));
  s_monthlayer = text_layer_create(GRect(0,105,144,50));

  
  //set background color
  text_layer_set_background_color(s_timelayer,GColorClear);
  text_layer_set_background_color(s_datelayer,GColorClear);
  text_layer_set_background_color(s_monthlayer,GColorClear);

  
  //setting color
  text_layer_set_text_color(s_timelayer,GColorBlack);
  
  //setting text
  update_time();
  
  //beautification
  text_layer_set_font(s_timelayer, fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD));
  text_layer_set_font(s_datelayer, fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD));
  text_layer_set_font(s_monthlayer, fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD));

  text_layer_set_text_alignment(s_timelayer, GTextAlignmentCenter);
  text_layer_set_text_alignment(s_datelayer, GTextAlignmentCenter);
  text_layer_set_text_alignment(s_monthlayer, GTextAlignmentCenter);


  //adding the text layer to the window
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_timelayer));
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_datelayer));
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_monthlayer));
  
  
  
//---------------------create breath----------------------//
  displayer = text_layer_create(GRect(0, 20, 144, 50));
  text_layer_set_background_color(displayer, GColorClear);
  text_layer_set_text_color(displayer, GColorBlack);
  text_layer_set_font(displayer, fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD));
  text_layer_set_text_alignment(displayer, GTextAlignmentCenter);
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(displayer));

  
  window_set_click_config_provider(window, click_config_provider);
}

static void unloadwindow(Window* window){
  text_layer_destroy(displayer);
  text_layer_destroy(s_timelayer);
  text_layer_destroy(s_monthlayer);
  text_layer_destroy(s_datelayer);

}

static void init(void){
  window = window_create();
  window_set_window_handlers(window, (WindowHandlers){
    .load = loadwindow,
    .unload = unloadwindow
  });
  
  tick_timer_service_subscribe(SECOND_UNIT, tick_handler);

  window_stack_push(window, true);
  four(NULL);
}

static void deinit(void) {
  window_destroy(window);
}


//========================HELPER FUNCTIONS========================//



static void tick_handler(struct tm *tick_time, TimeUnits units_changed ){
  update_time();
}
static void update_time(){
  time_t temp = time(NULL);
  struct tm *tick_time = localtime(&temp);
  static char date[19];
  static char month[17];
  static char buff[] = "00:00:00";
  if(clock_is_24h_style() == true) {
    // Use 24 hour format
    strftime(buff, sizeof("00:00:00"), "%R", tick_time);
  } else {
    // Use 12 hour format
    strftime(buff, sizeof("00:00:00"), "%I:%M:%S", tick_time);
  }
  strftime(date, sizeof(char) * 19, "%a", tick_time);
  strftime(month,sizeof(char) * 17, "%b/%d/%G", tick_time);
  
  text_layer_set_text(s_timelayer, buff);
  text_layer_set_text(s_datelayer, date);
  text_layer_set_text(s_monthlayer, month);

}

static void vibe(){
  vibes_cancel();
  //static uint32_t segments[1];
  switch(Pulse_Setting){
    case Short:
      vibecreate( (uint32_t[]) { Short_Pulse }, 1);
      break;
    case Long:
      vibecreate( (uint32_t[]){ Long_Pulse }, 1);
      break;
    case Hardcore:
      vibecreate( (uint32_t[]){ Hardcore_Pulse }, 1);
      break;
  }  
  text_layer_set_text(displayer, "Inhale");

}

static void vibe2(){
  vibes_cancel();
  switch(Pulse_Setting){
    case Short:
      switch(Wait_Setting){
        case Short:
          vibecreate((uint32_t[]) { Short_Pulse , Short_Wait, Short_Pulse},3);
          break;
        case Long:
          vibecreate((uint32_t[]) { Short_Pulse , Long_Wait, Short_Pulse},3);
          break;
        case Hardcore:
          vibecreate((uint32_t[]) { Short_Pulse , Hardcore_Wait, Short_Pulse},3);
          break;
      }
    case Long:
      switch(Wait_Setting){
        case Short:
          vibecreate((uint32_t[]){ Long_Pulse , Short_Wait, Long_Pulse },3);
          break;
        case Long:
          vibecreate((uint32_t[]){ Long_Pulse , Long_Wait, Long_Pulse },3);
          break;
        case Hardcore:
          vibecreate((uint32_t[]){ Long_Pulse , Hardcore_Wait, Long_Pulse },3);
          break;
      }
    case Hardcore:
      switch(Wait_Setting){
        case Short:
          vibecreate((uint32_t[]){ Hardcore_Pulse , Short_Wait, Hardcore_Pulse},3);
          break;
        case Long:
          vibecreate((uint32_t[]){ Hardcore_Pulse , Long_Wait, Hardcore_Pulse},3);
          break;
        case Hardcore:
          vibecreate((uint32_t[]){ Hardcore_Pulse , Hardcore_Wait, Hardcore_Pulse },3);
          break;
      }

  }  
  text_layer_set_text(displayer, "Hold");

  
}
static void vibe3(){  
  vibes_cancel();
  switch(Pulse_Setting){
    case Short:
      switch(Wait_Setting){
        case Short:
          vibecreate((uint32_t[]) { Short_Pulse , Short_Wait, Short_Pulse, Short_Wait, Short_Pulse},5);
          break;
        case Long:
          vibecreate((uint32_t[]) { Short_Pulse , Long_Wait, Short_Pulse, Long_Wait, Short_Pulse},5);
          break;
        case Hardcore:
          vibecreate((uint32_t[]) { Short_Pulse , Hardcore_Wait, Short_Pulse, Hardcore_Wait, Short_Pulse},5);
          break;
      }
    case Long:
      switch(Wait_Setting){
        case Short:
          vibecreate((uint32_t[]){ Long_Pulse , Short_Wait, Long_Pulse, Short_Wait, Long_Pulse },5);
          break;
        case Long:
          vibecreate((uint32_t[]){ Long_Pulse , Long_Wait, Long_Pulse , Long_Wait, Long_Pulse },5);
          break;
        case Hardcore:
          vibecreate((uint32_t[]){ Long_Pulse , Hardcore_Wait, Long_Pulse , Hardcore_Wait, Long_Pulse },5);
          break;
      }
    case Hardcore:
      switch(Wait_Setting){
        case Short:
          vibecreate((uint32_t[]){ Hardcore_Pulse , Short_Wait, Hardcore_Pulse , Short_Wait, Hardcore_Pulse},5);
          break;
        case Long:
          vibecreate((uint32_t[]){ Hardcore_Pulse , Long_Wait, Hardcore_Pulse , Long_Wait, Hardcore_Pulse},5);
          break;
        case Hardcore:
          vibecreate((uint32_t[]){ Hardcore_Pulse , Hardcore_Wait, Hardcore_Pulse , Hardcore_Wait, Hardcore_Pulse },5);
          break;
      }

  }
  
  text_layer_set_text(displayer, "Exhale");
}

static void vibecreate(uint32_t segments[], int num_vibes){
  VibePattern pat = {
    .durations = segments,
    .num_segments = num_vibes
  };
  vibes_enqueue_custom_pattern(pat);
}




int main(void){
  init();
  app_event_loop();
  deinit();
}