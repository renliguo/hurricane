/*
 * EVALUATION AND USE OF THIS SOFTWARE IS SUBJECT TO THE TERMS AND
 * CONDITIONS OF THE CONTROLLING LICENSE AGREEMENT FOUND AT LICENSE.md
 * IN THIS SDK. IF YOU DO NOT AGREE TO THE LICENSE TERMS AND CONDITIONS,
 * PLEASE RETURN ALL SOURCE FILES TO SILICON LABORATORIES.
 * (c) Copyright 2018, Silicon Laboratories Inc.  All rights reserved.
 */

#include "gos.h"
#include "hurricane.h"

#define DEMO_NAME               "Blinky"
#define BLINK_PERIOD            500 // milliseconds
#define BLINK_DISPLAY_COLOR     0x247dde

static bool toggle;

static void blink_event_handler(void *arg);
static void start_demo_handler(void *arg);
static void stop_demo_handler(void *arg);
static void blinky_draw (void);


hurricane_demo_t demo_info = 
{ .demo_name = DEMO_NAME,
  .start_demo_callback = start_demo_handler,
  .stop_demo_callback = stop_demo_handler,
  .draw_splash_screen_callback = NULL,    
  .demo_splash_screen_arg = NULL,
  .splash_font_color = HURRICANE_SPLASH_DEFAULT_FONT_COLOR,
  .splash_screen_timeout_ms = 0

};


void gos_app_init(void)
{
   hurricane_init(&demo_info);

}

void gos_app_deinit(void)
{

}


void gos_app_idle(void)
{
}

static void start_demo_handler(void *arg)
{
  
  GOS_LOG("Starting Hurricane Blink Demo");
  gos_event_register_periodic(blink_event_handler, NULL, BLINK_PERIOD, 0);
  
}


static void stop_demo_handler(void *arg)
{
  gos_event_unregister_all(blink_event_handler);
  
}

/*************************************************************************************************/
static void blink_event_handler(void *arg)
{   
  blinky_draw ();

  hurricane_gpio_write(HURRICANE_GPIO_LED1, toggle);
  hurricane_gpio_write(HURRICANE_GPIO_LED2, !toggle);

  toggle = !toggle;
}

static void blinky_draw (void)
{ 
  GUI_RECT rect;

  GUI_MULTIBUF_Begin();

  GUI_Clear();
  GUI_SetFont(&GUI_Font16B_1);
  rect.x0 = 0;
  rect.y0 = 0;
  rect.x1 = 160;
  rect.y1 = 25;
  GUI_SetBkColor(GUI_BLACK);
  GUI_SetColor(GUI_RED);
  GUI_DispStringInRect("BLINKY", &rect, GUI_TA_HCENTER | GUI_TA_TOP);

  rect.x0 = 20;
  rect.x1 = 60;
  rect.y0 = 70;
  rect.y1 = 110;
  
  if(toggle == true)
  { GUI_SetBkColor(GUI_BLACK);
    GUI_SetColor(BLINK_DISPLAY_COLOR);
  }
  else 
  { GUI_SetBkColor(GUI_BLACK);
    GUI_SetColor(GUI_BLACK);
  }
  GUI_FillRectEx(&rect);
 

  rect.x0 = 100;
  rect.x1 = 140;
  rect.y0 = 70;
  rect.y1 = 110;
  if(toggle != true)
  { GUI_SetBkColor(GUI_BLACK);
    GUI_SetColor(BLINK_DISPLAY_COLOR);
  }
  else 
  { GUI_SetBkColor(GUI_BLACK);
    GUI_SetColor(GUI_BLACK);
  }

  GUI_FillRectEx(&rect);
  
  GUI_MULTIBUF_End();
}
