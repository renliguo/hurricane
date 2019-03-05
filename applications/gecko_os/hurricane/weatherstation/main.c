/*
 * EVALUATION AND USE OF THIS SOFTWARE IS SUBJECT TO THE TERMS AND
 * CONDITIONS OF THE CONTROLLING LICENSE AGREEMENT FOUND AT LICENSE.md
 * IN THIS SDK. IF YOU DO NOT AGREE TO THE LICENSE TERMS AND CONDITIONS,
 * PLEASE RETURN ALL SOURCE FILES TO SILICON LABORATORIES.
 * (c) Copyright 2018, Silicon Laboratories Inc.  All rights reserved.
 */

#include "gos.h"
#include "hurricane.h"
#include "background_night.h"

#define DEMO_NAME                   "Weather World"
#define DISPLAY_PERIOD_MS           500         // milliseconds
#define SENSOR_REPORT_PERIOD_MS     1000        // milliseconds
#define WEATHER_REPORT_PERIOD_MS    10000       // milliseconds
//#define USE_FAHRENHEIT


typedef struct 
{ float tData_float;
  float rhData_float;
  float pressure_inHg;
  float uvi;
  float als;
} weather_data_t;

static weather_data_t weather_data;
static gos_msgpack_context_t *weather_msgpack = NULL;


static void display_event_handler(void *arg);
static void cloud_event_handler(void *arg);
static void start_demo_handler(void *arg);
static void draw_current_weather (weather_data_t *data);
static void weather_report_event_handler(void *arg);

void start_demo_handler(void *arg);

hurricane_demo_t demo_info = 
{ .demo_name = DEMO_NAME,
  .start_demo_callback = start_demo_handler,
  .draw_splash_screen_callback = NULL,    
  .demo_splash_screen_arg = "weather_splash.jpg",
  .splash_font_color = HURRICANE_SPLASH_DEFAULT_FONT_COLOR
};

#define BACKGROUND_FILENAME "background_night.jpg"
static uint8_t image_cache[1024 * 6];


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
  gos_handle_t file_handle;
  uint32_t bytes_read;

  GOS_LOG("Weatherstation Demo...");

// load background image into cache
  if(gos_file_open(BACKGROUND_FILENAME, GOS_FILE_LOCATION_ANY, false, &file_handle) == GOS_SUCCESS)
  {
    gos_file_read(file_handle, image_cache, sizeof(image_cache), &bytes_read);

    gos_file_close(file_handle);
  }


  gos_event_register_periodic(display_event_handler, 0, DISPLAY_PERIOD_MS, 0);
  gos_event_register_periodic(cloud_event_handler, 0, SENSOR_REPORT_PERIOD_MS, 0);
  gos_event_register_periodic(weather_report_event_handler, 0, WEATHER_REPORT_PERIOD_MS, 0);
}


static void display_event_handler(void *arg)
{   
  weather_data.tData_float    = hurricane_sensor_get_temp();
  weather_data.rhData_float   = hurricane_sensor_get_RH();
  weather_data.pressure_inHg  = hurricane_sensor_get_pressure();
  weather_data.uvi            = hurricane_sensor_get_UV();
  weather_data.als            = hurricane_sensor_get_ALS();

  #ifdef USE_FAHRENHEIT
  weather_data.tData_float = weather_data.tData_float * (9.0 / 5.0) + 32.0;
  #endif

  draw_current_weather(&weather_data);
}

static void cloud_event_handler(void *arg)
{   
  // post data to DMS
  hurricane_init_sensor_data(5);
  hurricane_add_sensor_data("Relative Humidity", weather_data.rhData_float);
  hurricane_add_sensor_data("Temperature", weather_data.tData_float);
  hurricane_add_sensor_data("UV Index", weather_data.uvi);
  hurricane_add_sensor_data("Ambient Light", weather_data.als);
  hurricane_add_sensor_data("Pressure", weather_data.pressure_inHg);
  hurricane_flush_sensor_data();
}


#define DMS_MSG_TIMEOUT 		90000

//static int weather_report_id = 0;
static void weather_report_event_handler(void *arg)
{
    gos_result_t result;
    char float_str[32];
    weather_msgpack = NULL;
    const gos_dms_messsage_config_t config =
    {
            .length                 = 0,
            .is_response            = false,
            .response.handler       = 0,
            .response.timeout_ms    = DMS_MSG_TIMEOUT
    };  

    // Initialize the write context
    if(GOS_FAILED(result, gos_dms_message_write_init(&weather_msgpack, &config)))
    {
      return;
    }

    gos_msgpack_write_dict_marker(weather_msgpack, 3);
    gos_msgpack_write_dict_str(weather_msgpack, "request", "webhook");
    gos_msgpack_write_dict_str(weather_msgpack, "code", "LEADERBOARD");
    gos_msgpack_write_dict_dict(weather_msgpack, "data", 1);
    gos_msgpack_write_dict_dict(weather_msgpack, "result", 5);

    float_to_str(weather_data.tData_float, float_str, 2);
    gos_msgpack_write_dict_str(weather_msgpack, "temperature", float_str);

    float_to_str(weather_data.rhData_float, float_str, 2);
    gos_msgpack_write_dict_str(weather_msgpack, "relative_humidity", float_str);

    float_to_str(weather_data.uvi, float_str, 2);
    gos_msgpack_write_dict_str(weather_msgpack, "uv_index", float_str);

    float_to_str(weather_data.als, float_str, 2);
    gos_msgpack_write_dict_str(weather_msgpack, "ambient_light", float_str);

    float_to_str(weather_data.pressure_inHg, float_str, 2);
    gos_msgpack_write_dict_str(weather_msgpack, "pressure", float_str);


    // Send the message to the DMS
    if(GOS_FAILED(result, gos_dms_message_write_flush(weather_msgpack)))
    {
        gos_dms_message_context_destroy(weather_msgpack);
    }

    weather_msgpack = NULL;

}


/*********************************************************************
*
*       _GetData
*
* Function description
*   Reading data directly from file system
*/
int _GetData(void * p, const U8 ** ppData, unsigned NumBytes, U32 Off) {
  // U32 NumBytesRead = 0;
  // gos_handle_t     pFile;
  // int i;
  // static U8 file_data[4096];
  
  // gos_file_open(p, GOS_FILE_LOCATION_ANY, false, &pFile);
  
  // if(Off < 128)
  // {
  //   gos_file_read(pFile, file_data, Off, &NumBytesRead);
  // }
  // else
  // { 
  //   for(i=0; i<(Off/128); i++)
  //   { 
  //     gos_file_read(pFile, file_data, 128, &NumBytesRead);
  //   }

  //   gos_file_read(pFile, file_data, (Off%128), &NumBytesRead);
  // }  
  
  // if(NumBytes > sizeof(file_data)) 
  // {
  //   NumBytes = sizeof(file_data);
  // }
  
  // gos_file_read(pFile, file_data, NumBytes, &NumBytesRead);
  // *ppData = file_data;
  
  // gos_file_close(pFile);
  // GOS_LOG("Read %d bytes from %d", NumBytesRead, Off);

  U32 NumBytesRead = NumBytes;

  if((NumBytesRead + Off) > sizeof(image_cache))
  {
    NumBytesRead = sizeof(image_cache) - Off;
  }

  *ppData = &image_cache[Off];
   
  return NumBytesRead;
}



static void draw_current_weather (weather_data_t *data)
{
  GUI_RECT rect;
  gos_iso8601_str_t time_str;
  char float_str[10];
  char buffer[128];
  GUI_JPEG_INFO Info;
  int gui_result;
  
  GUI_MULTIBUF_Begin();

  GUI_Clear();
  
//  GUI_DrawBitmap(&bmbackground_night, 0,0);

  GUI_JPEG_GetInfoEx(_GetData, BACKGROUND_FILENAME, &Info);
  gui_result = GUI_JPEG_DrawEx(_GetData, BACKGROUND_FILENAME,0,0);
  if(gui_result != 0)
  {
    GOS_LOG("result = %d", gui_result);
  }
  
  GUI_SetTextMode(GUI_TM_TRANS);
  GUI_SetFont(&GUI_Font13B_1);
  rect.x0 = 0;
  rect.x1 = 160;
  rect.y0 = 0;
  rect.y1 = 15;
  GUI_SetBkColor(GUI_BLACK);
  GUI_SetColor(GUI_WHITE);
  GUI_DispStringInRect("CURRENT WEATHER", &rect, GUI_TA_HCENTER | GUI_TA_TOP);


//ToDO:  check if we can get location and time from internet...if not then display message

  if (gos_time_get_current_utc_as_iso8601_str(&time_str, false) == GOS_SUCCESS)

  {
    time_str.T = ' ';   //Change the 'T' to a space
    time_str.timezone = 0; // null terminate before time zone marker
  }
  else
  {
    // set time string to null
    memset(&time_str, 0, sizeof(time_str));
  }


  GUI_SetFont(&GUI_Font10_1);
  rect.x0 = 0;
  rect.x1 = 160;
  rect.y0 = 15;
  rect.y1 = 30;
  GUI_SetBkColor(GUI_BLACK);
  GUI_SetColor(GUI_WHITE);
  GUI_DispStringInRect((char *)&time_str, &rect, GUI_TA_HCENTER | GUI_TA_TOP);


  GUI_SetFont(&GUI_Font10_1);
  rect.x0 = 0;
  rect.x1 = 80;
  rect.y0 = 35;
  rect.y1 = 70;
  GUI_SetBkColor(GUI_BLACK);
  GUI_SetColor(GUI_WHITE);
  GUI_DispStringInRect("Temperature", &rect, GUI_TA_HCENTER | GUI_TA_TOP);

  GUI_SetFont(&GUI_Font16B_1);
  rect.y0 = rect.y0 + 5;
  GUI_SetBkColor(GUI_BLACK);
  GUI_SetColor(GUI_WHITE);
  #ifdef USE_FAHRENHEIT
  sprintf(buffer, "%s F", float_to_str(data->tData_float, float_str, 1));
  #else
  sprintf(buffer, "%s C", float_to_str(data->tData_float, float_str, 1));
  #endif
  GUI_DispStringInRect(buffer, &rect, GUI_TA_HCENTER | GUI_TA_VCENTER);


  GUI_SetFont(&GUI_Font10_1);
  rect.x0 = 0;
  rect.x1 = 80;
  rect.y0 = 70;
  rect.y1 = 105;
  GUI_SetBkColor(GUI_BLACK);
  GUI_SetColor(GUI_WHITE);
  GUI_DispStringInRect("Humidity", &rect, GUI_TA_HCENTER | GUI_TA_TOP);

  GUI_SetFont(&GUI_Font16B_1);
  rect.y0 = rect.y0 + 5;
  GUI_SetBkColor(GUI_BLACK);
  GUI_SetColor(GUI_WHITE);
  sprintf(buffer, "%s %%", float_to_str(data->rhData_float, float_str, 1));
  GUI_DispStringInRect(buffer, &rect, GUI_TA_HCENTER | GUI_TA_VCENTER);


  GUI_SetFont(&GUI_Font10_1);
  rect.x0 = 80;
  rect.x1 = 160;
  rect.y0 = 35;
  rect.y1 = 70;

  GUI_SetBkColor(GUI_BLACK);
  GUI_SetColor(GUI_WHITE);
  GUI_DispStringInRect("Pressure", &rect, GUI_TA_HCENTER | GUI_TA_TOP);

  GUI_SetFont(&GUI_Font16B_1);
  rect.y0 = rect.y0 + 5;
  GUI_SetBkColor(GUI_BLACK);
  GUI_SetColor(GUI_WHITE);
  
  sprintf(buffer, "%s in", float_to_str(data->pressure_inHg, float_str, 2));
  
  GUI_DispStringInRect(buffer, &rect, GUI_TA_HCENTER | GUI_TA_VCENTER);

  GUI_SetFont(&GUI_Font10_1);
  rect.x0 = 80;
  rect.x1 = 160;
  rect.y0 = 70;
  rect.y1 = 105;


  // GUI_SetBkColor(GUI_BLACK);
  // GUI_SetColor(GUI_WHITE);
  // GUI_DispStringInRect("UV Index", &rect, GUI_TA_HCENTER | GUI_TA_TOP);

  // GUI_SetFont(&GUI_Font16B_1);
  // rect.y0 = rect.y0 + 5;
  // GUI_SetBkColor(GUI_BLACK);
  // GUI_SetColor(GUI_WHITE);
  // sprintf(buffer, "%d", (uint8_t)data->uvi);
  // GUI_DispStringInRect(buffer, &rect, GUI_TA_HCENTER | GUI_TA_VCENTER);
  
  GUI_SetBkColor(GUI_BLACK);
  GUI_SetColor(GUI_WHITE);
  GUI_DispStringInRect("Ambient Light", &rect, GUI_TA_HCENTER | GUI_TA_TOP);

  GUI_SetFont(&GUI_Font16B_1);
  rect.y0 = rect.y0 + 5;
  GUI_SetBkColor(GUI_BLACK);
  GUI_SetColor(GUI_WHITE);
  sprintf(buffer, "%lu", (uint32_t)data->als);
  GUI_DispStringInRect(buffer, &rect, GUI_TA_HCENTER | GUI_TA_VCENTER);

#if 0  
  GUI_SetFont(&GUI_Font4x6);
  rect.x0 = 0;
  rect.x1 = 160;
  rect.y0 = 115;
  rect.y1 = 120;
  GUI_SetBkColor(GUI_BLACK);
  GUI_SetColor(GUI_WHITE);
  GUI_DispStringInRect("Press Button 2 to View Forcast", &rect, GUI_TA_HCENTER | GUI_TA_BOTTOM);
#endif

  GUI_MULTIBUF_End();

//  GOS_LOG("RH: %s %%  |  Temp: %s Degrees Celcius", float_to_str(data->rhData/1000.0, rhData_float_str, 3) , float_to_str(data->tData/1000.0, tData_float_str, 3));
}
