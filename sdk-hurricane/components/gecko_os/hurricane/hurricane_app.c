/*
 * EVALUATION AND USE OF THIS SOFTWARE IS SUBJECT TO THE TERMS AND
 * CONDITIONS OF THE CONTROLLING LICENSE AGREEMENT FOUND AT LICENSE.md
 * IN THIS SDK. IF YOU DO NOT AGREE TO THE LICENSE TERMS AND CONDITIONS,
 * PLEASE RETURN ALL SOURCE FILES TO SILICON LABORATORIES.
 * (c) Copyright 2018, Silicon Laboratories Inc.  All rights reserved.
 */

#include "gos.h"
#include "hurricane.h"
#include "silabs_logo_150x73.h"


/*************************************************************************************************
* Non-Exported Definitions
**************************************************************************************************/
#define SETTINGS_MAGIC_NUMBER               5
#define BUTTON_DEBOUNCE_TIME_MS             50    
#define BUTTON_CLICK_TIME_MS                500   
#define BUTTON_PRESS_TIME_MS                1000  
#define SI1133_UVALS_ADDR                   0x55
#define SI1153_PROX_GEST_ADDR               0x53
#define SI1153_LONG_RANGE_PROX_ADDR         0x52

// The Hurricane Intro video requires a periodic event to process the display.  
// This definition determines the period of the event in ms.  The video should
// be displayed at 40ms per frame.  
#define HURRICANE_INTRO_PROCESS_PERIOD_MS   1     

// After the intro the self test software will toggle LEDs.  This uses a periodic
// event running at the rate specified below 
#define HURRICANE_LED_TEST_PERIOD_MS 500


/*************************************************************************************************
* Types
**************************************************************************************************/
typedef enum hurricaneAppState 
{ HURRICANE_APP_STATE_INTRO,
  HURRICANE_APP_STATE_INTRO_ERROR,
  HURRICANE_APP_STATE_POST,
  HURRICANE_APP_STATE_SPLASH,
  HURRICANE_APP_STATE_SETUP,
  HURRICANE_APP_STATE_DEMO,
  HURRICANE_APP_STATE_UPDATE
} hurricaneAppState_t;


/*************************************************************************************************
* Non-Exported Function Prototypes
**************************************************************************************************/
static void hurricane_button_clicked_event_handler(void *arg);
static void hurricane_start_intro(void *arg);
static void hurricane_start_setup (void *arg);
static void hurricane_start_post (void *arg);
static void hurricane_start_intro_error (void *arg);
static void hurricane_intro_process_handler (void *arg);
static int hurricane_movie_get_data(void *p, const uint8_t ** ppData, unsigned num_bytes, U32 offset);
static void hurricane_movie_notify_callback(GUI_HMEM h_mem, int notification, U32 current_frame);
static void hurricane_self_test_message(char *message);
static void hurricane_reinit_buttons(void);
static void hurricane_launch_demo(void *arg);





/*************************************************************************************************
* Non-Exported Variables
**************************************************************************************************/
static int intro_playing = 0;       //flag set when intro video is playing
static hurricane_demo_t *hurricane_demo_handle;
static GUI_MOVIE_HANDLE intro_movie_handle = 0;
static char intro_filename[] = "silabs_rotate_scaled.emf";
static gos_result_t post_result = HURRICANE_SUCCESS;
static hurricaneAppState_t hurricane_state = HURRICANE_APP_STATE_INTRO;


static gos_button_config_t button_config =
{
  .active_level           = PLATFORM_BUTTON_ACTIVE_STATE,
  .debounce               = BUTTON_DEBOUNCE_TIME_MS,
  .click_time             = BUTTON_CLICK_TIME_MS,
  .press_time             = BUTTON_PRESS_TIME_MS,
  .event_handler.press    = NULL,
  .event_handler.toggle   = NULL,
  .event_handler.changing = NULL,
  .event_handler.click    = hurricane_button_clicked_event_handler,
  .execution_context      = GOS_BUTTON_CONTEXT_DEFAULT
};


/* This is a copy of the function found in hardware/boards/gecko_os/hurricane/gos_board_hardware_init.c
   There is a build issue where this function does not correctly override the weak function of the same name
   when the function is declared in a component.  
   
   ToDo:  Once the issue discussed above is fixed the below copy should be deleted
*/
gos_result_t gos_board_hardware_init(void)
{
    gos_result_t result;

    GOS_LOG("Initializing Hurricane-specific hardware ...");


    // Initialize the GPIO Expanders and turn on key components
    if(GOS_FAILED(result, hurricane_gpio_init()))
    {
    }
    else if(GOS_FAILED(result, hurricane_gpio_write(HURRICANE_GPIO_3V3_AUX_EN, true)))
    {
    }
    else if(GOS_FAILED(result, hurricane_gpio_write(HURRICANE_GPIO_LCD_BACKLIGHT_EN, true)))
    {
    }
    else if(GOS_FAILED(result, hurricane_gpio_write(HURRICANE_GPIO_LCD_SUPPLY_EN, true)))
    {
    }
    else if(GOS_FAILED(result, hurricane_gpio_write(HURRICANE_GPIO_I2C_MUX, true)))
    {
    }
    else
    {
        //delay to give components time to power on (the si115x/3x sensors require min 25ms start time)
        gos_rtos_delay_milliseconds(100);
    }

    return result;
}


/*************************************************************************************************
* hurricane_init()
*
* See function comments in hurricane_app.h
**************************************************************************************************/
void hurricane_init(hurricane_demo_t *demo_handle)
{
  gos_result_t result;
  
// init DMS handlers
  hurricane_dms_init();

  if(GOS_FAILED(result, gos_load_app_settings("hurricane_settings.ini")))
  {
    GOS_LOG("Failed to load settings, err:%d", result);
  }
  else if(GOS_FAILED(result, gos_settings_save()))
  {
    GOS_LOG("Failed to save settings, err:%d", result);
  }

// Initialize the hurricane board
  gos_board_hardware_init();        

// Initialize the display and emwin graphics library
  hurricane_init_display();
  GUI_Init();

// Configure the buttons
// For the first rev of the board it is important to enable 3V3_AUX before using the buttons because the buttons are pulled up to +3V3_AUX.  
  gos_button_init(HURRICANE_BUTTON1, &button_config, (void*)1);
  gos_button_init(HURRICANE_BUTTON2, &button_config, (void*)2);

// Save the demo handle
  hurricane_demo_handle = demo_handle;

#ifdef HURRICANE_SKIP_INTRO
  gos_event_issue(hurricane_start_post, 0, 0);
#else
// Start the intro video
  hurricane_start_intro(NULL);
#endif
// The remainder of boot and setup process is event driven so we exit here
}

#ifndef HURRICANE_SKIP_INTRO

/******************************************************************************
* hurricane_start_intro
*
* The hurricane intro is in the form of a EMWIN movie file (.emf).  This 
* function will verify the file and then start the movie.  The processing
* of the movie requires a periodic call to GUI_Ex().  Therefore this function
* registes a periodic event.  The processing it performen in the event handler.
*******************************************************************************/
static void hurricane_start_intro(void *arg)
{ hurricane_state = HURRICANE_APP_STATE_INTRO;
  
  GUI_MOVIE_INFO   Info;
  int              xSize, ySize;
  gos_file_t       file_info;

// Get display size
  xSize = LCD_GetXSize();
  ySize = LCD_GetYSize();

  //check if the intro video file exists and if so display video
  if(gos_file_stat(intro_filename, GOS_FILE_LOCATION_ANY, &file_info, NULL) == GOS_SUCCESS)
  {
  // Get physical size of movie
    if (GUI_MOVIE_GetInfoEx(hurricane_movie_get_data, intro_filename, &Info) == 0)
    {
      intro_movie_handle = GUI_MOVIE_CreateEx(hurricane_movie_get_data, intro_filename, hurricane_movie_notify_callback);
      if (intro_movie_handle) 
      { intro_playing = 1;
        GUI_MOVIE_Show(intro_movie_handle, (xSize - Info.xSize) / 2, (ySize - Info.ySize) / 2, 0);
      }
    }
  }
  
  
  if(intro_movie_handle != 0)
  {
  //The processing of the video requires a periodic event
    gos_event_register_periodic(hurricane_intro_process_handler, 0, HURRICANE_INTRO_PROCESS_PERIOD_MS, 0);
  }
  else     
  {
  //There was an error displaying the intro video so show a message
    gos_event_issue(hurricane_start_intro_error, arg, 0);
  }
  
}


/******************************************************************************
* hurricane_start_intro_error
*
* If there is an error in displaying the intro video, this funtion will be 
* called
*******************************************************************************/
static void hurricane_start_intro_error (void *arg)
{ char buffer[128];
  
  hurricane_state = HURRICANE_APP_STATE_INTRO_ERROR;
  
  sprintf(buffer, "Error occured displaying the intro video. Please confirm the file %s exists in the filesystem.", intro_filename);
  hurricane_draw_message_screen(buffer, GUI_TA_LEFT | GUI_TA_VCENTER, "Continue", "");

}

#endif

/*************************************************************************************************
* hurricane_led_test_handler 
*
* During boot-up the software performs a self test of the board.  Part of the self test is
* illuminating all the LED's.  This is done using this event handler.
**************************************************************************************************/
static void hurricane_led_test_handler (void *arg)
{ static uint8_t count = 0;
  uint8_t led_val;
   
// For the first 5 events we illuminate eash LED in a round robin fashion
// Next we illuminate all the LEDs
// Then we turn them all off and unregister the event   
  if(count < 5)
  { led_val = 0x1 << count;
  } 
  else if (count < 6)
    led_val = 0x1F;
  else
  { gos_event_unregister(hurricane_led_test_handler, arg);
    led_val = 0;
  }

  count++;
    
  hurricane_gpio_write(HURRICANE_GPIO_LED1, ((led_val & 0x1) != 0));
  hurricane_gpio_write(HURRICANE_GPIO_LED2, ((led_val & 0x2) != 0));
  hurricane_gpio_write(HURRICANE_GPIO_LED4, ((led_val & 0x4) != 0));
  hurricane_gpio_write(HURRICANE_GPIO_LED5, ((led_val & 0x8) != 0));
  hurricane_gpio_write(HURRICANE_GPIO_LED3, ((led_val & 0x10) != 0));    //This order is based off the board layout 
}


/******************************************************************************
* hurricane_self_test_message
*
* The function hurricane_sensor_self_test() requires a callback funtion to
* output messages.  This function is used for POST.
*******************************************************************************/
static void hurricane_self_test_message(char *message)
{ GOS_LOG(message);
  
}


/******************************************************************************
* hurricane_start_post
*
* After the intro video is complete this function is called to run the Power On
* Self Test.
*******************************************************************************/
static void hurricane_start_post (void *arg)
{
  hurricane_state = HURRICANE_APP_STATE_POST;
 
/* The LED test simply flashes the LEDs in sequence to allow the user to visually inspect
   them.  There is no automatic feedback so this test will always pass.
   
   Flashing the LEDs requires a periodic event.  Note that this event is run in the Event Thread
   so that the rest of the self test can continue while the LEDs are being tested.
*/   
  gos_event_register_periodic(hurricane_led_test_handler, arg, HURRICANE_LED_TEST_PERIOD_MS, GOS_EVENT_FLAG_IN_EVENT_THREAD );


/* Test the sensors on the board.  This test is a simple 'roll call' to make sure each
   sensor responds.
*/   

  post_result = hurricane_sensor_self_test(hurricane_self_test_message);


/* We don't want to exit before the led test is complete.  The LED periodic handler will deresiter
   the event upon completion.  Therefore we can wait here until the event is no longer registered.
*/     
  while (gos_event_is_registered(hurricane_led_test_handler, arg))  //do not exit until LED test is complete
  {}
  
/*  Continue with the boot-up  */  
  gos_event_issue(hurricane_start_setup, arg, 0);

}


/******************************************************************************
* hurricane_start_setup
*
* This function is called after the intro video and POST is complete.  It will
* perform final initialization then display the demo splash screen.  
*******************************************************************************/
static void hurricane_start_setup (void *arg)
{ 
  GOS_LOG("\r\n\r\nWGM160P HURRICANE DEMO PLATFORM");

//ToDo: What to do if POST fails?

// init sensors
  hurricane_sensors_init();
  
// Attempt to connect to wi-fi network
  gos_network_up(GOS_INTERFACE_WLAN, true);

  hurricane_show_demo_splash_screen(hurricane_demo_handle->splash_screen_timeout_ms);
}


/******************************************************************************
* hurricane_get_jpeg_data
*
* See function comments in hurricane_app.h
*******************************************************************************/
int hurricane_get_jpeg_data(void * p, const uint8_t ** ppData, unsigned num_bytes, U32 offset) 
{
  U32 num_bytes_read = 0;
  gos_handle_t     pFile;
  int i;
  gos_result_t result;
  static uint8_t file_data[4096];
  
  if(GOS_FAILED(result, gos_file_open(p, GOS_FILE_LOCATION_ANY, false, &pFile)))
  {
    return -1;
  }
  
  if(offset < 128)
    gos_file_read(pFile, file_data, offset, &num_bytes_read);
  else
  { for(i=0; i<(offset/128); i++)
    { gos_file_read(pFile, file_data, 128, &num_bytes_read);
    }
    gos_file_read(pFile, file_data, (offset%128), &num_bytes_read);
  }  
  
  if (num_bytes > sizeof(file_data)) 
  {
    num_bytes = sizeof(file_data);
  }
  
  gos_file_read(pFile, file_data, num_bytes, &num_bytes_read);
  *ppData = file_data;
  
  gos_file_close(pFile);
  
  return num_bytes_read;
}


/******************************************************************************
* hurricane_draw_jpeg
*
* See function comments in hurricane_app.h
*******************************************************************************/
int hurricane_draw_jpeg(void *arg)
{ int gui_result;   
  GUI_JPEG_INFO Info;
   
  gui_result = GUI_JPEG_GetInfoEx(hurricane_get_jpeg_data, (char *)arg, &Info);
  if (gui_result == 0)
  { gui_result = GUI_JPEG_DrawEx(hurricane_get_jpeg_data, (char *)arg,0,0);
  }
  return gui_result;
}


/******************************************************************************
* hurricane_show_demo_splash_screen
*
* See function comments in hurricane_app.h
*******************************************************************************/
void hurricane_show_demo_splash_screen(uint32_t timeout_ms)
{ GUI_RECT rect;

  if(hurricane_state == HURRICANE_APP_STATE_DEMO)
    gos_event_issue(hurricane_demo_handle->stop_demo_callback, NULL, 0);

  hurricane_state = HURRICANE_APP_STATE_SPLASH;

  
  hurricane_reinit_buttons();   // reinit button in case this is called from demo

  GUI_MULTIBUF_Begin();
  GUI_Clear();
  
// If the demo passes NULL to the callback function then used the default function
  if(hurricane_demo_handle->draw_splash_screen_callback == NULL)
    hurricane_demo_handle->draw_splash_screen_callback = HURRICANE_SPLASH_DEFAULT_FUNCTION;
  
  if(hurricane_demo_handle->draw_splash_screen_callback(hurricane_demo_handle->demo_splash_screen_arg) != 0)
  {
    GUI_SetFont(&GUI_Font13_1);
    rect.x0 = HURRICANE_SETUP_TYPICAL_MARGIN;
    rect.x1 = LCD_GetXSize() - HURRICANE_SETUP_TYPICAL_MARGIN;
    rect.y0 = HURRICANE_SETUP_TYPICAL_MARGIN;
    rect.y1 = HURRICANE_NAVIGATION_Y0_COORD;
    GUI_SetBkColor(GUI_BLACK);
    GUI_SetColor(GUI_WHITE);
    GUI_DispStringInRectWrap("Let's Get Started", &rect, GUI_TA_HCENTER | GUI_TA_TOP, GUI_WRAPMODE_WORD);
    
    rect.x0 = HURRICANE_SETUP_TYPICAL_MARGIN;
    rect.x1 = LCD_GetXSize() - HURRICANE_SETUP_TYPICAL_MARGIN;
    rect.y0 = HURRICANE_SETUP_TYPICAL_MARGIN + 25;
    rect.y1 = HURRICANE_NAVIGATION_Y0_COORD;
    GUI_SetBkColor(GUI_BLACK);
    GUI_SetColor(GUI_WHITE);
    GUI_DispStringInRectWrap((char *)hurricane_demo_handle->demo_name, &rect, GUI_TA_HCENTER | GUI_TA_TOP, GUI_WRAPMODE_WORD);

    hurricane_demo_handle->splash_font_color = HURRICANE_SETUP_DISPLAY_NAV_FONT_COLOR;
  }
  
  GUI_SetTextMode(GUI_TM_TRANS);
 
  GUI_SetFont(&GUI_Font10_1);
  GUI_SetColor(GUI_BLACK);
  GUI_SetBkColor(GUI_BLACK);
  rect.x0 = HURRICANE_SETUP_TYPICAL_MARGIN;
  rect.x1 = LCD_GetXSize()/2;
  rect.y0 = HURRICANE_NAVIGATION_Y0_COORD;
  rect.y1 = 120;
  GUI_SetColor(hurricane_demo_handle->splash_font_color);
  GUI_DispStringInRect("Menu", &rect, GUI_TA_LEFT | GUI_TA_BOTTOM);
 
  GUI_SetColor(GUI_BLACK);
  GUI_SetBkColor(GUI_BLACK);
  rect.x0 = LCD_GetXSize()/2;
  rect.x1 = LCD_GetXSize() - HURRICANE_SETUP_TYPICAL_MARGIN;
  rect.y0 = HURRICANE_NAVIGATION_Y0_COORD;
  rect.y1 = 120;
  GUI_SetColor(hurricane_demo_handle->splash_font_color);
  GUI_DispStringInRect("Run", &rect, GUI_TA_RIGHT | GUI_TA_BOTTOM);

  GUI_MULTIBUF_End();
  
  if(timeout_ms != 0)
    gos_event_register_timed(hurricane_launch_demo, NULL, timeout_ms, GOS_EVENT_FLAG_NONE);
}


/*************************************************************************************************
* hurricane_movie_notify_callback
*
* Callback function for movie player. Uses multiple buffering if available to avoid tearing effects.
**************************************************************************************************/
static void hurricane_movie_notify_callback(GUI_HMEM h_mem, int notification, U32 current_frame) 
{
	switch (notification) {
  case GUI_MOVIE_NOTIFICATION_PREDRAW:
    GUI_MULTIBUF_Begin();
    break;
  case GUI_MOVIE_NOTIFICATION_POSTDRAW:
    GUI_MULTIBUF_End();
    break;
  case GUI_MOVIE_NOTIFICATION_STOP:
    intro_playing = 0;
    break;
  }
}

/*************************************************************************************************
* hurricane_movie_get_data
* 
* Reads data for the intro movie directly from file system
**************************************************************************************************/
static int hurricane_movie_get_data(void * p, const uint8_t ** ppData, unsigned num_bytes, U32 offset) 
{
  U32 num_bytes_read = 0;
  gos_handle_t     pFile;
  uint8_t buffer[128];
  int i;
  
  if(gos_file_open(p, GOS_FILE_LOCATION_ANY, false, &pFile) == GOS_SUCCESS)
  { 
    if(offset < 128)
      gos_file_read(pFile, buffer, offset, &num_bytes_read);
    else
    { for(i=0; i<(offset/128); i++)
      { gos_file_read(pFile, buffer, 128, &num_bytes_read);
      }
      gos_file_read(pFile, buffer, (offset%128), &num_bytes_read);
    }  
 
    gos_file_read(pFile, (uint8_t *)*ppData, num_bytes, &num_bytes_read);
    
    gos_file_close(pFile);
  }
  return num_bytes_read;
}

/*************************************************************************************************
* hurricane_intro_process_handler
*
* Process the intro video by calling GUI_Exec().  This event handler is only active during the 
* intro video.
**************************************************************************************************/
static void hurricane_intro_process_handler (void *arg)
{ 
//Frame processing will occur inside GUI_Exec()
  GUI_Exec();
  
// When the movie is complete, start the rest of the boot/setup process
  if(intro_playing == 0)
  { 
    GUI_MOVIE_Delete(intro_movie_handle);

    gos_event_unregister(hurricane_intro_process_handler, arg);
    gos_event_issue(hurricane_start_post, arg, 0);
  }
}


/******************************************************************************
* hurricane_enter_setup_mode
*
* See function comments in hurricane_app.h
*******************************************************************************/
void hurricane_enter_setup_mode(void)         
{ 
  if(hurricane_state == HURRICANE_APP_STATE_DEMO)
    gos_event_issue(hurricane_demo_handle->stop_demo_callback, NULL, 0);

  hurricane_state = HURRICANE_APP_STATE_SETUP;
  hurricane_reinit_buttons();   // reinit button in case this is called from demo
  hurricane_enter_device_setup(hurricane_demo_handle);
}


/******************************************************************************
* hurricane_enter_update_mode
*
* See function comments in hurricane_app.h
*******************************************************************************/
void hurricane_firmware_update(void)         
{ 
  if(hurricane_state == HURRICANE_APP_STATE_DEMO)
    gos_event_issue(hurricane_demo_handle->stop_demo_callback, NULL, 0);

  hurricane_state = HURRICANE_APP_STATE_UPDATE;

//Do not handle buttons during update mode.  The only way to exit this mode is reset
  gos_button_deinit(HURRICANE_BUTTON1);   
  gos_button_deinit(HURRICANE_BUTTON2);
  
  hurricane_dms_dfu();

}


/******************************************************************************
* hurricane_reinit_buttons
*
* When the hurricance code is called from the demo application we need to 
* reinitialize the buttons. This removes the existing configuration including
* button handlers and initializes it according to the need of the setup code.
*******************************************************************************/
static void hurricane_reinit_buttons(void)
{ gos_button_deinit(HURRICANE_BUTTON1);
  gos_button_deinit(HURRICANE_BUTTON2);
  gos_button_init(HURRICANE_BUTTON1, &button_config, (void*)1);
  gos_button_init(HURRICANE_BUTTON2, &button_config, (void*)2);
}


/*************************************************************************************************
* hurricane_button_clicked_event_handler
*
* Event handler for the clicked event for both buttons.  The execution of this function uses the
* hurricane_state variable to determine the proper action.  This event handler is only active 
* during initialization, setup and splash screen states. Once a demo is started the control of the 
* buttons are relinquished so that the demo can configure and handle the buttons using the 
* standard GOS button API.  
**************************************************************************************************/
static void hurricane_button_clicked_event_handler(void *arg)
{
  switch(hurricane_state)
  {
    case HURRICANE_APP_STATE_SPLASH:
      // If a splash screen timeout was set then we must unregister the event
      if(gos_event_is_registered(hurricane_launch_demo, NULL) == true)
        gos_event_unregister_all(hurricane_launch_demo);

      if((uint32_t)arg == 1)
      { hurricane_enter_setup_mode();
      }
      else if((uint32_t)arg == 2)
      { hurricane_launch_demo(NULL);
      }  
      break;
    case HURRICANE_APP_STATE_SETUP:
      hurricane_setup_process_button((uint32_t)arg);
      break;
    case HURRICANE_APP_STATE_INTRO_ERROR:
      if((uint32_t)arg == 1)
      { hurricane_draw_message_screen("Running Self Test", GUI_TA_HCENTER | GUI_TA_VCENTER, "", "");
        gos_event_issue(hurricane_start_post, arg, 0);
      }
      break;  
    case HURRICANE_APP_STATE_INTRO:  //do nothing 
    case HURRICANE_APP_STATE_POST:   //do nothing
    case HURRICANE_APP_STATE_DEMO:   //do nothing
    case HURRICANE_APP_STATE_UPDATE: //do nothing
      break;
    default:
      break;    
  }

}


/*************************************************************************************************
* hurricane_launch_demo
*
**************************************************************************************************/
static void hurricane_launch_demo(void *arg)
{ 
//deinit buttons and other resources used by setup
  gos_button_deinit(HURRICANE_BUTTON1);
  gos_button_deinit(HURRICANE_BUTTON2);
  
//Clear the screen
  GUI_MULTIBUF_Begin();
  GUI_Clear();
  GUI_MULTIBUF_End();
  gos_rtos_delay_milliseconds(250);  //Give the display thread time to clear the screen

// Start the demo
  hurricane_state = HURRICANE_APP_STATE_DEMO;
  gos_event_issue(hurricane_demo_handle->start_demo_callback, NULL, 0);
}

/*************************************************************************************************/
