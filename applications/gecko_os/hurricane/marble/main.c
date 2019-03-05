/*
 * EVALUATION AND USE OF THIS SOFTWARE IS SUBJECT TO THE TERMS AND
 * CONDITIONS OF THE CONTROLLING LICENSE AGREEMENT FOUND AT LICENSE.md
 * IN THIS SDK. IF YOU DO NOT AGREE TO THE LICENSE TERMS AND CONDITIONS,
 * PLEASE RETURN ALL SOURCE FILES TO SILICON LABORATORIES.
 * (c) Copyright 2018, Silicon Laboratories Inc.  All rights reserved.
 */

#include "gos.h"
#include "hurricane.h"
#include "marble.h"

#define DEMO_NAME                   "Marble"

#define BUTTON_DEBOUNCE_TIME_MS             50    
#define BUTTON_CLICK_TIME_MS                500   
#define BUTTON_PRESS_TIME_MS                1000  

static void start_demo_handler(void *arg);
static void game_handler(void *arg);
static void button_pressed_event_handler(void *arg);
static void button_clicked_event_handler(void *arg);


static gos_button_config_t button_config = 
{
  .active_level           = PLATFORM_BUTTON_ACTIVE_STATE,
  .debounce               = BUTTON_DEBOUNCE_TIME_MS,
  .click_time             = BUTTON_CLICK_TIME_MS,
  .press_time             = BUTTON_PRESS_TIME_MS,
  .event_handler.press    = button_pressed_event_handler,
  .event_handler.click    = button_clicked_event_handler,
  .event_handler.toggle   = NULL,
  .event_handler.changing = NULL,
  .execution_context      = GOS_BUTTON_CONTEXT_DEFAULT,             
};



hurricane_demo_t demo_info = 
{ .demo_name = DEMO_NAME,
  .start_demo_callback = start_demo_handler,
  
  .draw_splash_screen_callback = NULL,    
  .demo_splash_screen_arg = "marble_splash.jpg",
  .splash_font_color = HURRICANE_SPLASH_DEFAULT_FONT_COLOR
};


void gos_app_init(void)
{ 
    hurricane_init(&demo_info);
}

void gos_app_deinit(void)
{
    gos_event_unregister_all(game_handler);
}

void gos_app_idle(void)
{
}

bool game_over = false;

static void game_handler(void *arg)
{ 
  
  if (game_over == false)
  {
    game_over = marble_frame();
  }
  else
  {
    
  }
}

static void start_demo_handler(void *arg)
{  
    GOS_LOG("Marble Maze Escape");
    
    gos_button_init(HURRICANE_BUTTON1, &button_config, (void*)1);
    gos_button_init(HURRICANE_BUTTON2, &button_config, (void*)2);

    marble_init();

    gos_event_register_periodic(game_handler, NULL, 10, GOS_EVENT_FLAG_NONE);
}


static void button_pressed_event_handler(void *arg)
{  GOS_LOG("Button Pressed");
    if(gos_button_is_active(HURRICANE_BUTTON1) && gos_button_is_active(HURRICANE_BUTTON2))
    { gos_event_unregister(game_handler, NULL);
      marble_init();
      gos_event_register_periodic(game_handler, NULL, 10, GOS_EVENT_FLAG_NONE);
    }
}


static void button_clicked_event_handler(void *arg)
{
  if((uint32_t)arg == 1)
  { 
    gos_event_unregister(game_handler, NULL);
    hurricane_enter_setup_mode();
  }
  else if((uint32_t)arg == 2)
  {
      gos_event_unregister(game_handler, NULL);
      marble_init();
      gos_event_register_periodic(game_handler, NULL, 10, GOS_EVENT_FLAG_NONE);
  }  
}
