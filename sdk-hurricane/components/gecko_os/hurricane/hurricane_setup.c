/*
 * EVALUATION AND USE OF THIS SOFTWARE IS SUBJECT TO THE TERMS AND
 * CONDITIONS OF THE CONTROLLING LICENSE AGREEMENT FOUND AT LICENSE.md
 * IN THIS SDK. IF YOU DO NOT AGREE TO THE LICENSE TERMS AND CONDITIONS,
 * PLEASE RETURN ALL SOURCE FILES TO SILICON LABORATORIES.
 * (c) Copyright 2018, Silicon Laboratories Inc.  All rights reserved.
 */

#include <ctype.h>
#include <stdio.h>
#include "gos.h"
#include "hurricane.h"


/*************************************************************************************************
* Non-Exported Definitions
**************************************************************************************************/

#define CMD_BUFFER_LENGTH       128

#define VT100_CLEAR_SCREEN     "\x1b[2J"
#define VT100_CURSOR_HOME      "\x1b[;H"


#define HURRICANE_SETUP_START_DEMO_TEXT   "Start Demo"   //this one is different from other menu items because we use it to insent the demo name

#define HURRICANE_SETUP_MENU_SELECT_BUTTON    1
#define HURRICANE_SETUP_BACK_BUTTON           1
#define HURRICANE_SETUP_SCROLL_BUTTON         2
#define HURRICANE_SETUP_CONFIRM_BUTTON        2
#define HURRICANE_SETUP_NEXT_BUTTON           2

#define  HURRICANE_SCROLL_BAR_BACKGROUND_COLOR   0x333333
#define  HURRICANE_SELECTED_BACKGROUND_COLOR     0xD4D4D4
#define  HURRICANE_TITLE_BACKGROUND_COLOR        0x2A1ED9

#define  HURRICANE_TITLE_BAR_HEIGHT              15     //this defines the vertical height of the title bar

#define HURRICANE_SETUP_SYSDIAG_MAX_LINES        7      //the max number of lines that fit on the sys diag screens


/*************************************************************************************************
* Types
**************************************************************************************************/

typedef enum hurricaneSetupState 
{ HURRICANE_SETUP_STATE_MENU = 0,
  HURRICANE_SETUP_STATE_WIFI_1,
  HURRICANE_SETUP_STATE_WIFI_2,
  HURRICANE_SETUP_STATE_WIFI_3,
  HURRICANE_SETUP_STATE_CLAIM,
  HURRICANE_SETUP_STATE_LAUNCH,
  HURRICANE_SETUP_STATE_SYS_DIAG_MENU,
  HURRICANE_SETUP_STATE_NETWORK_STATUS,
  HURRICANE_SETUP_STATE_PLATFORM_DETAILS,
  HURRICANE_SETUP_STATE_SOFTWARE_INFO,
  HURRICANE_SETUP_STATE_SENSOR_ID,
  HURRICANE_SETUP_STATE_SENSOR_DATA,
  HURRICANE_SETUP_STATE_DEFAULT,
} hurricaneSetupState_t;


#define HURRICANE_SETUP_MENU_ITEM_LENGTH     25

typedef struct hurricaneMenuItem
{ char text[HURRICANE_SETUP_MENU_ITEM_LENGTH];
  hurricaneSetupState_t next_state;              //the state to transition to when menu item is selected
} hurricaneMenuItem_t;


typedef struct hurricaneSetupMenu
{ char title[HURRICANE_SETUP_MENU_ITEM_LENGTH];
  uint8_t num_items;
  uint8_t current_page;
  uint8_t current_selection;
  const hurricaneMenuItem_t *items;
} hurricaneSetupMenu_t;

/*************************************************************************************************
* Non-Exported Function Prototypes
**************************************************************************************************/
static void hurricane_setup_state_next(hurricaneSetupState_t next_state);
static void hurricae_setup_menu_scroll (hurricaneSetupMenu_t *menu);
static void hurricane_setup_draw_menu(hurricaneSetupMenu_t *menu);
static void hurricane_setup_draw_wifi(int page);
static void hurricane_setup_draw_factory_reset_confirm(void);
static void hurricane_setup_draw_sysdiag_line(char *message, int index, int scroll_index);
static void hurricane_setup_draw_navigation(char *button1_message, char *button2_message);
static void hurricane_setup_draw_device_claimed(void);
static void hurricane_setup_draw_device_claim_timeout(void);
static void hurricane_setup_draw_PIN(int32_t code);
static void hurricane_setup_draw_title(char *title);
static void hurricane_setup_claim_device(void);
static void hurricane_setup_draw_sensor_data(void);
static void hurricane_setup_draw_network_status(void);
static const char* get_device_name(const char *setting, char *buffer, size_t buffer_length);
static const char* get_setup_url(const char *setting, char *buffer, size_t buffer_length);
static void display_sensor_info_handler(void *arg);
static void display_network_status_handler(void *arg);
static void hurricane_setup_draw_sensor_identification(void);
static void hurricane_setup_draw_platform_details(void);
static void hurricane_setup_draw_software_info(void);




/*************************************************************************************************
* Non-Exported Variables
**************************************************************************************************/
static hurricane_demo_t *demo_handle;
static int sysinfo_scroll_index = 0;
static bool more_available = false;   
static hurricaneSetupState_t setup_state = HURRICANE_SETUP_STATE_MENU;


#define HURRICANE_SETUP_NUM_MENU_ITEMS        5
const hurricaneMenuItem_t main_menu_items[HURRICANE_SETUP_NUM_MENU_ITEMS] = 
{ {"Setup Wi-Fi",                   HURRICANE_SETUP_STATE_WIFI_1}, 
  {"Claim Device",                  HURRICANE_SETUP_STATE_CLAIM}, 
  //{"Install Demo from the Cloud", HURRICANE_SETUP_STATE_INSTALL}|, 
  {HURRICANE_SETUP_START_DEMO_TEXT, HURRICANE_SETUP_STATE_LAUNCH},        
  {"System Diagnostics",            HURRICANE_SETUP_STATE_SYS_DIAG_MENU},
  {"Restore Factory Defaults",      HURRICANE_SETUP_STATE_DEFAULT}, 
  // {"Reboot"}
};

#define HURRICANE_SETUP_NUM_DIAG_MENU_ITEMS   6  //ToDo:  currently 6 is the max items for a menu.  We need to implement scrolling to create a menu with > 6
const hurricaneMenuItem_t diagnostic_menu_items[HURRICANE_SETUP_NUM_DIAG_MENU_ITEMS] = 
{ {"Go Back",                  HURRICANE_SETUP_STATE_MENU},
  {"Network Status",           HURRICANE_SETUP_STATE_NETWORK_STATUS},
  {"Platform Details",         HURRICANE_SETUP_STATE_PLATFORM_DETAILS},
  {"Software Information",     HURRICANE_SETUP_STATE_SOFTWARE_INFO},
  {"Sensor Identification",    HURRICANE_SETUP_STATE_SENSOR_ID},
  {"View Raw Sensor Data",     HURRICANE_SETUP_STATE_SENSOR_DATA}, 
};

hurricaneSetupMenu_t main_menu =
{ .title = "CONFIGURATION MENU",
  .num_items = HURRICANE_SETUP_NUM_MENU_ITEMS,
  .current_page = 0,
  .current_selection = 0,
  .items = main_menu_items
};

hurricaneSetupMenu_t diagnostic_menu = 
{ .title = "SYSTEM DIAGNOSTICS",
  .num_items = HURRICANE_SETUP_NUM_DIAG_MENU_ITEMS,
  .current_page = 0,
  .current_selection = 0,
  .items = diagnostic_menu_items
};

/*************************************************************************************************
* hurricane_enter_device_setup()
*
**************************************************************************************************/
void hurricane_enter_device_setup(hurricane_demo_t *handle)
{ 
  main_menu.current_page = 0;
  main_menu.current_selection = 0; 
  diagnostic_menu.current_page = 0;
  diagnostic_menu.current_selection = 0; 

  
  
  demo_handle = handle;

  hurricane_setup_draw_menu(&main_menu);
  setup_state = HURRICANE_SETUP_STATE_MENU;
  
}


/*************************************************************************************************
* 
**************************************************************************************************/
static void hurricane_setup_draw_title(char *title)
{ GUI_RECT rect;
  
  GUI_SetFont(&GUI_Font13_1);
  rect.x0 = 0;   
  rect.y0 = 0;
  rect.x1 = LCD_WIDTH;
  rect.y1 = HURRICANE_TITLE_BAR_HEIGHT;
  GUI_SetColor(HURRICANE_TITLE_BACKGROUND_COLOR);
  GUI_SetBkColor(HURRICANE_TITLE_BACKGROUND_COLOR);
  GUI_FillRectEx(&rect);
  GUI_SetColor(GUI_WHITE);
  GUI_DispStringInRect(title, &rect, GUI_TA_HCENTER | GUI_TA_VCENTER);
  
}


/*************************************************************************************************
* 
**************************************************************************************************/
static void hurricane_draw_menu_line(char *line, uint8_t line_number, uint8_t selected)
{
  GUI_RECT rect;

  if(selected == 1) 
  { GUI_SetColor(HURRICANE_SELECTED_BACKGROUND_COLOR);
    GUI_SetBkColor(HURRICANE_SELECTED_BACKGROUND_COLOR);
  }
  else
  { GUI_SetColor(GUI_BLACK);
    GUI_SetBkColor(GUI_BLACK);
  }
  rect.x0 = 0;   
  rect.y0 = 15*(line_number+1) + 2;
  rect.x1 = LCD_WIDTH;
  rect.y1 = rect.y0 + 15;
  GUI_FillRectEx(&rect);
  if(selected == 1) 
  { GUI_SetColor(GUI_BLACK);
    GUI_SetFont(&GUI_Font13_1);
    GUI_DispStringInRect("\xBB", &rect, GUI_TA_LEFT | GUI_TA_VCENTER);
  }
  else
    GUI_SetColor(GUI_WHITE);
      
  GUI_SetFont(&GUI_Font10_1);
  rect.x0 = 8;   //we want to indent the text a little...must do this after the GUI_FillRectEx so that fill will go to edge
  
  GUI_DispStringInRect(line, &rect, GUI_TA_LEFT | GUI_TA_VCENTER);
      
}


/*************************************************************************************************
* hurricae_setup_menu_scroll()
*
**************************************************************************************************/
static void hurricae_setup_menu_scroll (hurricaneSetupMenu_t *menu)
{
  menu->current_selection++;
  if(menu->current_selection >= menu->num_items)
    menu->current_selection = 0;
}


//ToDo: Implement scrolling including showing a scroll bar as needed.  Currently the menu cannot be more than 6 items because we have not implemented scrolling
/*************************************************************************************************
* 
*
**************************************************************************************************/
static void hurricane_setup_draw_menu(hurricaneSetupMenu_t *menu)
{ int32_t i = 0;
  char buffer[64];

// Show the menu on the display  
  GUI_MULTIBUF_Begin();
  GUI_Clear();

/*  Draw the menu title  */
  hurricane_setup_draw_title(menu->title);

  GUI_SetFont(&GUI_Font13_1);
  for(i=0; i<menu->num_items; i++)
  { 
    if(strcmp((const char *)menu->items[i].text, HURRICANE_SETUP_START_DEMO_TEXT) == 0)
    { sprintf(buffer, "%s (%s)", menu->items[i].text, demo_handle->demo_name);
    }
    else
      strcpy(buffer, menu->items[i].text);

    hurricane_draw_menu_line(buffer, i, (i == menu->current_selection));
  }
  
  hurricane_setup_draw_navigation("Select", "Scroll");
  
  GUI_MULTIBUF_End();

}


/*************************************************************************************************
* received_pin()
*
**************************************************************************************************/
static void received_pin(void *arg)
{
  uint32_t *pin = arg;

  GOS_LOG("Received PIN: %u", *pin);

  hurricane_setup_draw_PIN(*pin);
}


/*************************************************************************************************
* device_claimed()
*
**************************************************************************************************/
static void device_claimed(void *arg)
{
  // check if we are still on this menu item
  if(setup_state != HURRICANE_SETUP_STATE_CLAIM)
  {
    return;
  }

  if(arg == 0)
  {
    GOS_LOG("Device claimed");

    hurricane_setup_draw_device_claimed();
  }
  else
  {
    hurricane_setup_draw_device_claim_timeout();
  }
}


/*************************************************************************************************
* hurricane_setup_claim_device()
*
**************************************************************************************************/
static void hurricane_setup_claim_device(void)
{ 
  // check if network is up
  if(gos_network_is_up(GOS_INTERFACE_ANY))
  {
    hurricane_setup_draw_PIN(-1);
    hurricane_dms_request_pin(received_pin, device_claimed);
  }
  else
  { hurricane_draw_message_screen("The network is disconnected.\n\nYou must join a network to claim your device.", GUI_TA_LEFT | GUI_TA_TOP, "Back", "");

  }
}


/*************************************************************************************************
* hurricane_setup_draw_install()
*
**************************************************************************************************/
// static void hurricane_setup_draw_install(void)
// { GUI_RECT rect;
  
//   GUI_MULTIBUF_Begin();
//   GUI_Clear();

//   GUI_SetFont(&GUI_Font10_1);
//   GUI_SetBkColor(GUI_BLACK);
//   rect.x0 = HURRICANE_SETUP_TYPICAL_MARGIN;
//   rect.x1 = LCD_WIDTH - HURRICANE_SETUP_TYPICAL_MARGIN;
//   rect.y0 = 0;
//   rect.y1 = 15;
//   GUI_SetColor(GUI_WHITE);
//   GUI_DispStringInRect("1. Login to", &rect, GUI_TA_LEFT | GUI_TA_TOP);

//   rect.y0 = 15;
//   rect.y1 = 30;
//   GUI_SetColor(GUI_BLUE);
//   GUI_DispStringInRect("leaderboard.zentri.com", &rect, GUI_TA_HCENTER | GUI_TA_TOP);

//   rect.y0 = 30;
//   rect.y1 = 45;
//   GUI_SetColor(GUI_WHITE);
//   GUI_DispStringInRect("2. Click 'Demos'", &rect, GUI_TA_LEFT | GUI_TA_TOP);

//   rect.y0 = 45;
//   rect.y1 = 60;
//   GUI_DispStringInRect("3. Select a demo and install", &rect, GUI_TA_LEFT | GUI_TA_TOP);


//   hurricane_setup_draw_navigation("Back", "");

//   GUI_MULTIBUF_End();
  
// }


/*************************************************************************************************
* hurricane_setup_draw_device_claimed()
*
**************************************************************************************************/
static void hurricane_setup_draw_device_claimed(void)
{
  GUI_RECT rect;
  GUI_MULTIBUF_Begin();
  GUI_Clear();

  GUI_SetFont(&GUI_Font10_1);
  GUI_SetBkColor(GUI_BLACK);
  rect.x0 = HURRICANE_SETUP_TYPICAL_MARGIN;
  rect.x1 = LCD_WIDTH - HURRICANE_SETUP_TYPICAL_MARGIN;
  rect.y0 = 0;
  rect.y1 = LCD_GetYSize();
  GUI_SetColor(GUI_WHITE);
  GUI_DispStringInRect("Device claimed", &rect, GUI_TA_HCENTER | GUI_TA_VCENTER);

  hurricane_setup_draw_navigation("Back", "");

  GUI_MULTIBUF_End();
}


/*************************************************************************************************
* hurricane_setup_draw_device_claim_timeout()
*
**************************************************************************************************/
static void hurricane_setup_draw_device_claim_timeout(void)
{
  GUI_RECT rect;
  GUI_MULTIBUF_Begin();
  GUI_Clear();

  GUI_SetFont(&GUI_Font10_1);
  GUI_SetBkColor(GUI_BLACK);
  rect.x0 = HURRICANE_SETUP_TYPICAL_MARGIN;
  rect.x1 = LCD_WIDTH - HURRICANE_SETUP_TYPICAL_MARGIN;
  rect.y0 = 0;
  rect.y1 = LCD_GetYSize();
  GUI_SetColor(GUI_WHITE);
  GUI_DispStringInRect("Timeout, please try again", &rect, GUI_TA_HCENTER | GUI_TA_VCENTER);

  hurricane_setup_draw_navigation("Back", "");

  GUI_MULTIBUF_End();
}


/*************************************************************************************************
* hurricane_setup_draw_PIN()
*
**************************************************************************************************/
static void hurricane_setup_draw_PIN(int32_t code)
{ char buffer[32];
  uint8_t digits[3];
  GUI_RECT rect;

  digits[0] = code / 100;
  code %= 100;
  digits[1] = code / 10;
  code %= 10;
  digits[2] = code;

  GUI_MULTIBUF_Begin();
  GUI_Clear();

  GUI_SetFont(&GUI_Font10_1);
  GUI_SetBkColor(GUI_BLACK);
  rect.x0 = HURRICANE_SETUP_TYPICAL_MARGIN;
  rect.x1 = LCD_GetXSize() - HURRICANE_SETUP_TYPICAL_MARGIN;
  rect.y0 = 0;
  rect.y1 = 12;
  GUI_SetColor(GUI_WHITE);
  GUI_DispStringInRect("1. Login to", &rect, GUI_TA_LEFT | GUI_TA_TOP);

  rect.y0 = 12;
  rect.y1 = 24;
  GUI_SetColor(GUI_BLUE);
  GUI_DispStringInRect("leaderboard.zentri.com", &rect, GUI_TA_HCENTER | GUI_TA_TOP);

  rect.y0 = 24;
  rect.y1 = 36;
  GUI_SetColor(GUI_WHITE);
  GUI_DispStringInRect("2. Click 'Claim Device'", &rect, GUI_TA_LEFT | GUI_TA_TOP);

  if(code < 0)
  {
    // code not ready yet
    rect.y0 = 50;
    rect.y1 = 100;
    GUI_DispStringInRect("Retrieving code...", &rect, GUI_TA_HCENTER | GUI_TA_VCENTER);
  }
  else
  {
    rect.y0 = 36;
    rect.y1 = 48;
    GUI_DispStringInRect("3. Enter the code below", &rect, GUI_TA_LEFT | GUI_TA_TOP);

  //  GUI_SetFont(&GUI_Font32B_1);
    GUI_SetFont(&GUI_FontD48);
    GUI_SetColor(GUI_RED);
    GUI_SetBkColor(GUI_RED);
    rect.x0 = 10;
    rect.x1 = 50;
    rect.y0 = 50;
    rect.y1 = 100;
    GUI_FillRectEx(&rect);
    GUI_SetColor(GUI_BLACK);
    sprintf(buffer, "%d", digits[0]);
    GUI_DispStringInRect(buffer, &rect, GUI_TA_HCENTER | GUI_TA_VCENTER);

    GUI_SetColor(GUI_GREEN);
    GUI_SetBkColor(GUI_GREEN);
    rect.x0 = 60;
    rect.x1 = 100;
    GUI_FillRectEx(&rect);
    GUI_SetColor(GUI_BLACK);
    sprintf(buffer, "%d", digits[1]);
    GUI_DispStringInRect(buffer, &rect, GUI_TA_HCENTER | GUI_TA_VCENTER);

    GUI_SetColor(GUI_BLUE);
    GUI_SetBkColor(GUI_BLUE);
    rect.x0 = 110;
    rect.x1 = 150;
    GUI_FillRectEx(&rect);
    GUI_SetColor(GUI_BLACK);
    sprintf(buffer, "%d", digits[2]);
    GUI_DispStringInRect(buffer, &rect, GUI_TA_HCENTER | GUI_TA_VCENTER);
  }

  hurricane_setup_draw_navigation("Back", "");

  GUI_MULTIBUF_End();
}


/*************************************************************************************************
* hurricane_setup_draw_navigation()
*
**************************************************************************************************/
static void hurricane_setup_draw_navigation(char *button1_message, char *button2_message)
{
  GUI_RECT rect;
  GUI_SetFont(&GUI_Font10_1);
  GUI_SetColor(GUI_BLACK);
  GUI_SetBkColor(GUI_BLACK);
  rect.x0 = HURRICANE_SETUP_TYPICAL_MARGIN;
  rect.x1 = LCD_GetXSize()/2;
  rect.y0 = HURRICANE_NAVIGATION_Y0_COORD;
  rect.y1 = 120;
  GUI_FillRectEx(&rect);
  GUI_SetColor(HURRICANE_SETUP_DISPLAY_NAV_FONT_COLOR);
  GUI_DispStringInRect(button1_message, &rect, GUI_TA_LEFT | GUI_TA_BOTTOM);
 
  GUI_SetColor(GUI_BLACK);
  GUI_SetBkColor(GUI_BLACK);
  rect.x0 = LCD_GetXSize()/2;
  rect.x1 = LCD_GetXSize() - HURRICANE_SETUP_TYPICAL_MARGIN;
  rect.y0 = HURRICANE_NAVIGATION_Y0_COORD;
  rect.y1 = 120;
  GUI_FillRectEx(&rect);
  GUI_SetColor(HURRICANE_SETUP_DISPLAY_NAV_FONT_COLOR);
  GUI_DispStringInRect(button2_message, &rect, GUI_TA_RIGHT | GUI_TA_BOTTOM);
}


/*************************************************************************************************
* hurricane_setup_draw_wifi()
*
**************************************************************************************************/
static void hurricane_setup_draw_wifi(int page)
{ char buffer[128];
  char ssid_url_buffer[64];
  char passkey_buffer[64];
  GUI_RECT rect;


  GUI_MULTIBUF_Begin();
  GUI_Clear();


  sprintf(buffer, "WI-FI CONFIGURATION");
  hurricane_setup_draw_title(buffer);
  GOS_LOG(buffer);

  switch(page)
  { case 1:
      GUI_SetFont(&GUI_Font13_1);
      rect.x0 = HURRICANE_SETUP_TYPICAL_MARGIN;
      rect.x1 = LCD_GetXSize();
      rect.y0 = 25;
      rect.y1 = 60;
      GUI_SetBkColor(GUI_BLACK);
      GUI_SetColor(GUI_WHITE);
      sprintf(buffer, "From a mobile device or PC connect to Wi-Fi network: "); 
      GUI_DispStringInRectWrap(buffer, &rect, GUI_TA_LEFT | GUI_TA_TOP, GUI_WRAPMODE_WORD);
      GOS_LOG(buffer);
    
      GUI_SetFont(&GUI_Font13_1);
      GUI_SetBkColor(GUI_BLACK);
      GUI_SetColor(GUI_WHITE);
      rect.x0 = 0;
      rect.x1 = LCD_WIDTH/2 - HURRICANE_SETUP_TYPICAL_MARGIN;
      rect.y0 = 60;
      rect.y1 = HURRICANE_NAVIGATION_Y0_COORD;
      GUI_DispStringInRectWrap("SSID  :\nPassKey  :", &rect, GUI_TA_RIGHT | GUI_TA_TOP, GUI_WRAPMODE_WORD);


      rect.x0 = LCD_WIDTH/2 - HURRICANE_SETUP_TYPICAL_MARGIN;
      rect.x1 = LCD_WIDTH;

      get_device_name("setup.web.ssid", ssid_url_buffer, sizeof(ssid_url_buffer));
      get_device_name("setup.web.passkey", passkey_buffer, sizeof(ssid_url_buffer));
      sprintf(buffer, "  %s\n  %s", ssid_url_buffer, passkey_buffer); 
      GUI_DispStringInRectWrap(buffer, &rect, GUI_TA_LEFT | GUI_TA_TOP, GUI_WRAPMODE_WORD);
      sprintf(buffer, "SSID: %s\nPassKey: %s", ssid_url_buffer, passkey_buffer); 
      GOS_LOG(buffer);
      hurricane_setup_draw_navigation("Back", "Next");
      break;
    case 2:
      GUI_SetFont(&GUI_Font13_1);
      rect.x0 = HURRICANE_SETUP_TYPICAL_MARGIN;
      rect.x1 = LCD_WIDTH;
      rect.y0 = 25;
      rect.y1 = 55;
      GUI_SetBkColor(GUI_BLACK);
      GUI_SetColor(GUI_WHITE);
      sprintf(buffer, "Next, open an internet browser (eg. Chrome) and browse to:");
      GUI_DispStringInRectWrap(buffer, &rect, GUI_TA_LEFT | GUI_TA_TOP, GUI_WRAPMODE_WORD);
      GOS_LOG(buffer);
    

      GUI_SetFont(&GUI_Font13_1);
      rect.x0 = 0;
      rect.x1 = LCD_WIDTH;
      rect.y0 = 55;
      rect.y1 = rect.y0 + 15;
      GUI_SetBkColor(GUI_BLACK);
      GUI_SetColor(GUI_BLUE);
      sprintf(buffer, "http://%s", get_setup_url("setup.web.url", ssid_url_buffer, sizeof(ssid_url_buffer)));
      GUI_DispStringInRect(buffer, &rect, GUI_TA_HCENTER | GUI_TA_TOP);

      rect.y0 = rect.y1;
      rect.y1 = rect.y0 + 15;
      GUI_SetBkColor(GUI_BLACK);
      GUI_SetColor(GUI_WHITE);
      GUI_DispStringInRect("or", &rect, GUI_TA_HCENTER | GUI_TA_TOP);


      rect.y0 = rect.y1;
      rect.y1 = rect.y0 + 15;
      GUI_SetBkColor(GUI_BLACK);
      GUI_SetColor(GUI_BLUE);
      GUI_DispStringInRect("http://10.10.10.1", &rect, GUI_TA_HCENTER | GUI_TA_TOP);

      sprintf(buffer,"%s or http://10.10.10.1", ssid_url_buffer);
      GOS_LOG(buffer);
      hurricane_setup_draw_navigation("Back", "Next");
      break;
    case 3:
      GUI_SetFont(&GUI_Font13_1);
      rect.x0 = HURRICANE_SETUP_TYPICAL_MARGIN;
      rect.x1 = LCD_WIDTH;
      rect.y0 = 25;
      rect.y1 = HURRICANE_NAVIGATION_Y0_COORD;
      GUI_SetBkColor(GUI_BLACK);
      GUI_SetColor(GUI_WHITE);
      sprintf(buffer, "Use the webpage to configure the Wi-Fi connection.\n\n** The device will automatically reset when settings are applied.");
      GUI_DispStringInRectWrap(buffer, &rect, GUI_TA_LEFT | GUI_TA_TOP, GUI_WRAPMODE_WORD);
      GOS_LOG(buffer);

      hurricane_setup_draw_navigation("Back", "");
      break;
    default:
      break;
  }
  
  GUI_MULTIBUF_End();

 }


/*************************************************************************************************
* get_device_name()
*
* Convert '#' to last three chars of device's MAC address
**************************************************************************************************/
static const char* get_device_name(const char *setting, char *buffer, size_t buffer_length)
{
    int setting_len;
    char *setting_ptr;

    gos_settings_get_print_str(setting, buffer, buffer_length);
    setting_len = strlen(buffer);
    setting_ptr = &buffer[setting_len-1];

    if(*setting_ptr == '#')
    {
        char mac_str[20];

        // example output:
        // 4C:55:CC:10:78:9B
        gos_settings_get_print_str("softap.mac", mac_str, sizeof(mac_str));


        *setting_ptr++ = mac_str[13];
        *setting_ptr++ = mac_str[15];
        *setting_ptr++ = mac_str[16];
        *setting_ptr = 0;
    }

    return buffer;
}


/*************************************************************************************************
* get_setup_url()
*
* When the first entry in the setup.web.url domain list
**************************************************************************************************/
static const char* get_setup_url(const char *setting, char *buffer, size_t buffer_length)
{
    gos_settings_get_print_str("setup.web.url", buffer, buffer_length);

    char *comma_ptr = strchr(buffer, ',');
    *comma_ptr = 0;

    return buffer;
}


/*************************************************************************************************
* display_sensor_info_handler()
*
**************************************************************************************************/
static void display_sensor_info_handler(void *arg)
{
  hurricane_setup_draw_sensor_data();
}


/*************************************************************************************************
* display_network_status_handler()
*
**************************************************************************************************/
static void display_network_status_handler(void *arg)
{
  hurricane_setup_draw_network_status();
}


/*************************************************************************************************
* hurricane_setup_process_button()
*
**************************************************************************************************/
void hurricane_setup_process_button(uint32_t button)
{ 
  char buffer[CMD_BUFFER_LENGTH]; 
  char cmd[CMD_BUFFER_LENGTH];
  hurricaneSetupState_t next_setup_state = setup_state;     //by default do not change states unless explicit by button
  
  switch (setup_state)
  {
    case HURRICANE_SETUP_STATE_MENU:
      if(button == HURRICANE_SETUP_MENU_SELECT_BUTTON)
        next_setup_state = main_menu.items[main_menu.current_selection].next_state;
      else if(button == HURRICANE_SETUP_SCROLL_BUTTON)
      { hurricae_setup_menu_scroll(&main_menu);
        next_setup_state = HURRICANE_SETUP_STATE_MENU;
      }
      break;
    case HURRICANE_SETUP_STATE_WIFI_1:
      if(button == HURRICANE_SETUP_BACK_BUTTON)
      { gos_setup_stop();  
        next_setup_state = HURRICANE_SETUP_STATE_MENU;
      }
      if(button == HURRICANE_SETUP_NEXT_BUTTON)
      { next_setup_state = HURRICANE_SETUP_STATE_WIFI_2;
      }
      break;
    case HURRICANE_SETUP_STATE_WIFI_2:
      if(button == HURRICANE_SETUP_BACK_BUTTON)
      { next_setup_state = HURRICANE_SETUP_STATE_WIFI_1;
      }
      if(button == HURRICANE_SETUP_NEXT_BUTTON)
      { next_setup_state = HURRICANE_SETUP_STATE_WIFI_3;
      }
      break;
    case HURRICANE_SETUP_STATE_WIFI_3:
      if(button == HURRICANE_SETUP_BACK_BUTTON)
      { next_setup_state = HURRICANE_SETUP_STATE_WIFI_2;
      }
      break;
    case HURRICANE_SETUP_STATE_CLAIM:
      if(button == HURRICANE_SETUP_BACK_BUTTON)
        next_setup_state = HURRICANE_SETUP_STATE_MENU;
      break;
    case HURRICANE_SETUP_STATE_LAUNCH:  
      // nothing here because the splash screen will be displayed and this function will not be called in that state
      break;
    case HURRICANE_SETUP_STATE_SYS_DIAG_MENU:
      if(button == HURRICANE_SETUP_MENU_SELECT_BUTTON)
      { next_setup_state = diagnostic_menu.items[diagnostic_menu.current_selection].next_state;
        sysinfo_scroll_index=0;
      }
      else if(button == HURRICANE_SETUP_SCROLL_BUTTON)
      { hurricae_setup_menu_scroll(&diagnostic_menu);
        next_setup_state = HURRICANE_SETUP_STATE_SYS_DIAG_MENU;
      }
      break;
    case HURRICANE_SETUP_STATE_NETWORK_STATUS:     //All the system diagnostics sub-states use the same code structure
    case HURRICANE_SETUP_STATE_SOFTWARE_INFO:
    case HURRICANE_SETUP_STATE_PLATFORM_DETAILS:
    case HURRICANE_SETUP_STATE_SENSOR_ID:
    case HURRICANE_SETUP_STATE_SENSOR_DATA:
      if(button == HURRICANE_SETUP_SCROLL_BUTTON)
      { if(more_available == true)   //only scroll if there is more lines available
          sysinfo_scroll_index++;
        next_setup_state = setup_state;
      }
      else if(button == HURRICANE_SETUP_BACK_BUTTON)
      { next_setup_state = HURRICANE_SETUP_STATE_SYS_DIAG_MENU;

      // Some pages register events.  These events must be unregistered here
        if(setup_state == HURRICANE_SETUP_STATE_SENSOR_DATA)
          gos_event_unregister(display_sensor_info_handler, 0);
        else if(setup_state == HURRICANE_SETUP_STATE_NETWORK_STATUS)
          gos_event_unregister(display_network_status_handler, 0);
      }
      break;
    case HURRICANE_SETUP_STATE_DEFAULT:
      if(button == HURRICANE_SETUP_BACK_BUTTON)
        next_setup_state = HURRICANE_SETUP_STATE_MENU;
      else if(button == HURRICANE_SETUP_CONFIRM_BUTTON)
      { gos_network_get_mac(gos_network_get_default_interface(), buffer);
        sprintf(cmd, "fac %s", buffer);
        gos_cmd_issue_return_str(buffer, CMD_BUFFER_LENGTH, cmd);
      }
      break;
    default:
      break;
  }
  hurricane_setup_state_next(next_setup_state);
  
}


/*************************************************************************************************
* hurricane_setup_draw_sysdiag_line()
*
* index : the line index where the message should be displayed
* scroll_index : the current line index shown at the top based on scrolling.  If 0 then the 
*                scrolling is at the top and index 0 is shown first.  If 1 then the second line
*                is shown at the top and so forth.
**************************************************************************************************/
static void hurricane_setup_draw_sysdiag_line(char *message, int index, int scroll_index)
{
  GUI_RECT rect;
  uint8_t scroll_bar_line_height;

  if((index >= scroll_index) && (index < (scroll_index + HURRICANE_SETUP_SYSDIAG_MAX_LINES)))
  {  
    GUI_SetFont(&GUI_Font10_1);
    GUI_SetColor(GUI_WHITE);
    rect.x0 = 5;   
    rect.x1 = LCD_WIDTH;
    rect.y0 = 12*(index - scroll_index) + HURRICANE_TITLE_BAR_HEIGHT+5;
    rect.y1 = rect.y0 + 12;
    GUI_DispStringInRect(message, &rect, GUI_TA_LEFT | GUI_TA_VCENTER);
  }
  
  //Add a scroll bar if needed
  if((index + 1) > HURRICANE_SETUP_SYSDIAG_MAX_LINES)        //index is 0-based so we must do +1
  {  
    rect.x0 = LCD_WIDTH - 5;   
    rect.x1 = LCD_WIDTH;
    rect.y0 = HURRICANE_TITLE_BAR_HEIGHT + 5;
    rect.y1 = HURRICANE_TITLE_BAR_HEIGHT + 5 + (HURRICANE_SETUP_SYSDIAG_MAX_LINES*12);
    GUI_SetColor(HURRICANE_SCROLL_BAR_BACKGROUND_COLOR);
    GUI_FillRectEx(&rect);

    scroll_bar_line_height = (HURRICANE_SETUP_SYSDIAG_MAX_LINES*12)/(index+1);         //index is 0-based so we must do +1
    rect.y0 = HURRICANE_TITLE_BAR_HEIGHT + 5 + scroll_index*scroll_bar_line_height;   
    rect.y1 = rect.y0 + scroll_bar_line_height * HURRICANE_SETUP_SYSDIAG_MAX_LINES;
    
    /*  Do not let it go past the navigation coordinate.  Since this is called repeatedly for all indexes
     *  not just the last one, this to happens.
     */
    if(rect.y1 > (HURRICANE_TITLE_BAR_HEIGHT + 5 + (HURRICANE_SETUP_SYSDIAG_MAX_LINES*12)))
      rect.y1 = HURRICANE_TITLE_BAR_HEIGHT + 5 + (HURRICANE_SETUP_SYSDIAG_MAX_LINES*12);
      
    GUI_SetColor(HURRICANE_SELECTED_BACKGROUND_COLOR);
    GUI_FillRectEx(&rect);
  }
  
//if we are attempting to show a line off the page then return 1 for more
  if((index+1) > HURRICANE_SETUP_SYSDIAG_MAX_LINES + scroll_index)     
    more_available = true;
  else
    more_available = false;
}


/*************************************************************************************************
* 
*
**************************************************************************************************/
static int sensor_info_index = 0;
void hurricane_setup_draw_sensor_info_line(char *message)
{  
  hurricane_setup_draw_sysdiag_line(message, sensor_info_index++, sysinfo_scroll_index);
}


/*************************************************************************************************
* hurricane_setup_draw_software_info()
*
**************************************************************************************************/
static void hurricane_setup_draw_software_info(void)
{ int i = 0;
  char buffer[64];
  char setting_str[128];
  gos_version_details_t version;

  GUI_MULTIBUF_Begin();
  GUI_Clear();
  
  hurricane_setup_draw_title("SOFTWARE INFORMATION");

  gos_system_get_version_details	(	&version, setting_str);
  sprintf(buffer, "Kernel Version : %s", version.kernel.version_str);      ///< The Gecko OS version as a string
  hurricane_setup_draw_sysdiag_line(buffer, i++, sysinfo_scroll_index);
  
  if(version.kernel.tag){
    sprintf(buffer, "OS Tag : %s", version.kernel.tag);              ///< The os tag string
  }
  else{
    sprintf(buffer, "OS Tag : <none>");
  }
  hurricane_setup_draw_sysdiag_line(buffer, i++, sysinfo_scroll_index);
  
  sprintf(buffer, "OS License : %s", version.kernel.license);          ///< The Gecko OS license code
  hurricane_setup_draw_sysdiag_line(buffer, i++, sysinfo_scroll_index);
  
  sprintf(buffer, "Bundle Version : %s", version.bundle.version_str);      ///< The bundle version number as a string
  hurricane_setup_draw_sysdiag_line(buffer, i++, sysinfo_scroll_index);
  
  if(version.bundle.tag){
    sprintf(buffer, "Bundle Tag : %s", version.bundle.tag);              ///< The bundle tag string
  }
  else{
    sprintf(buffer, "Bundle Tag : <none>");
  }
  hurricane_setup_draw_sysdiag_line(buffer, i++, sysinfo_scroll_index);

  sprintf(buffer, "SDK Version : %d", (int)version.sdk_number);    //int32             ///< If the app was built with the SDK then this contains its version number
  hurricane_setup_draw_sysdiag_line(buffer, i++, sysinfo_scroll_index);

  sprintf(buffer, "Customer Code : %s", version.bundle.customer_code);    ///< Product's customer code
  hurricane_setup_draw_sysdiag_line(buffer, i++, sysinfo_scroll_index);

  sprintf(buffer, "Product Code : %s", version.bundle.product_code);     ///< The product name
  hurricane_setup_draw_sysdiag_line(buffer, i++, sysinfo_scroll_index);

  sprintf(buffer, "Demo : %s", demo_handle->demo_name);
  hurricane_setup_draw_sysdiag_line(buffer, i++, sysinfo_scroll_index);
  
  if(more_available == false)
    hurricane_setup_draw_navigation("Back", "");
  else    
    hurricane_setup_draw_navigation("Back", "More");
    
  GUI_MULTIBUF_End();
}


/*************************************************************************************************
* hurricane_setup_draw_platform_details()
*
**************************************************************************************************/
static void hurricane_setup_draw_platform_details(void)
{ int i = 0;
  char buffer[64];
  char setting_str[128];
  char device_uuid[42];
  gos_version_details_t version;

  GUI_MULTIBUF_Begin();
  GUI_Clear();
  
  hurricane_setup_draw_title("PLATFORM DETAILS");

  gos_system_get_version_details	(	&version, setting_str);

  hurricane_setup_draw_sysdiag_line((char *)version.platform, i++, sysinfo_scroll_index);

  gos_settings_get_print_str("system.uuid", device_uuid, sizeof(device_uuid));
  sprintf(buffer, "UUID :");
  hurricane_setup_draw_sysdiag_line(buffer, i++, sysinfo_scroll_index);
  
  setting_str[0] = device_uuid[20];  //temporarily tuncate the string at the 20th char
  device_uuid[20] = 0;
  sprintf(buffer, "   %s", device_uuid);
  hurricane_setup_draw_sysdiag_line(buffer, i++, sysinfo_scroll_index);
  
  device_uuid[20] = setting_str[0];  //restore the char that was used
  
  sprintf(buffer, "   %s", &device_uuid[20]);
  hurricane_setup_draw_sysdiag_line(buffer, i++, sysinfo_scroll_index);
  
  if(more_available == false)
    hurricane_setup_draw_navigation("Back", "");
  else    
    hurricane_setup_draw_navigation("Back", "More");
    
  GUI_MULTIBUF_End();
}


/*************************************************************************************************
* hurricane_setup_draw_sensor_identification()
*
**************************************************************************************************/
static void hurricane_setup_draw_sensor_identification(void)
{
  GUI_MULTIBUF_Begin();
  GUI_Clear();

  sensor_info_index = 0;
  hurricane_setup_draw_title("SENSOR IDENTIFICATION");
      
  hurricane_sensor_self_test(hurricane_setup_draw_sensor_info_line);

  if(more_available == false)
    hurricane_setup_draw_navigation("Back", "");
  else    
    hurricane_setup_draw_navigation("Back", "More");
  
  GUI_MULTIBUF_End();

}

/*************************************************************************************************
* hurricane_setup_draw_network_status()
*
**************************************************************************************************/
static void hurricane_setup_draw_network_status(void)
{
  char buffer[64];
  int32_t wlan_rssi;
  char setting_str[128];
  char *token;
  int i=0;

  GUI_MULTIBUF_Begin();
  GUI_Clear();

  hurricane_setup_draw_title("NETWORK_STATUS");

  GUI_SetFont(&GUI_Font10_1);
  GUI_SetColor(GUI_WHITE);

  gos_settings_get_print_str("wlan.info", setting_str, sizeof(setting_str));
  token= strtok(setting_str, "\r\n");
  while(token != NULL)
  { token[0] = toupper((int)token[0]);   //capitalize the first letter on each line
    hurricane_setup_draw_sysdiag_line(token, i++, sysinfo_scroll_index);
    token = strtok(NULL, "\r\n");
    
  }
  
  sprintf(buffer, "IP : %s", gos_settings_get_print_str("wlan.network.ip", setting_str, sizeof(setting_str)));
  hurricane_setup_draw_sysdiag_line(buffer, i++, sysinfo_scroll_index);

  sprintf(buffer, "MAC : %s", gos_settings_get_print_str("wlan.mac", setting_str, sizeof(setting_str)));
  hurricane_setup_draw_sysdiag_line(buffer, i++, sysinfo_scroll_index);

  if(gos_network_get_rssi(&wlan_rssi) == GOS_NOTUP)
    sprintf(buffer, "RSSI : Not Connected");
  else
    sprintf(buffer, "RSSI : %d dBm", (int)wlan_rssi);
  hurricane_setup_draw_sysdiag_line(buffer, i++, sysinfo_scroll_index);

  if(more_available == false)
    hurricane_setup_draw_navigation("Back", "");
  else
    hurricane_setup_draw_navigation("Back", "More");
  
  GUI_MULTIBUF_End();
}


/*************************************************************************************************
* hurricane_setup_draw_sensor_info()
*
**************************************************************************************************/
static void hurricane_setup_draw_sensor_data(void)
{
  uint32_t index = 0;
  char buffer[64];
  char float_str[10];
  char float_str2[10];
  char float_str3[10];

  GUI_MULTIBUF_Begin();
  GUI_Clear();

  hurricane_setup_draw_title("SENSOR READINGS");

  GUI_SetFont(&GUI_Font10_1);
  GUI_SetColor(GUI_WHITE);
  
  sprintf(buffer, "Si7012 Temp C: %s RH: %s", 
    float_to_str(hurricane_sensor_get_temp(), float_str, 1),
    float_to_str(hurricane_sensor_get_RH(), float_str2, 1));
  hurricane_setup_draw_sysdiag_line(buffer, index++, 0);

  
  sprintf(buffer, "Si1133 UV: %s ALS: %s", 
    float_to_str(hurricane_sensor_get_UV(), float_str, 1),
    float_to_str(hurricane_sensor_get_ALS(), float_str2, 1));
  hurricane_setup_draw_sysdiag_line(buffer, index++, 0);

  sprintf(buffer, "BMP280 Press: %s", float_to_str(hurricane_sensor_get_pressure(), float_str, 1));
  hurricane_setup_draw_sysdiag_line(buffer, index++, 0);

  sprintf(buffer, "Si1153 Gest 1: %6s 2: %6s", 
    float_to_str(hurricane_sensor_get_gesture_1(), float_str, 1),
    float_to_str(hurricane_sensor_get_gesture_2(), float_str2, 1));
  hurricane_setup_draw_sysdiag_line(buffer, index++, 0);
  
  sprintf(buffer, "Si1153 Prox: %6s", 
    float_to_str(hurricane_sensor_get_prox(), float_str, 1));
  hurricane_setup_draw_sysdiag_line(buffer, index++, 0);
  
  sprintf(buffer, "Si7210 Mag: %6s", 
    float_to_str(hurricane_sensor_get_mag(), float_str, 1));
  hurricane_setup_draw_sysdiag_line(buffer, index++, 0);

  sprintf(buffer, "ICM20648 X:%s Y:%s Z:%s", 
    float_to_str(hurricane_sensor_get_accel_x(), float_str, 1),
    float_to_str(hurricane_sensor_get_accel_y(), float_str2, 1),
    float_to_str(hurricane_sensor_get_accel_z(), float_str3, 1));
  hurricane_setup_draw_sysdiag_line(buffer, index++, 0);
    
  if(more_available == false)
    hurricane_setup_draw_navigation("Back", "");
  else
    hurricane_setup_draw_navigation("Back", "More");
  
  GUI_MULTIBUF_End();
}


/*************************************************************************************************
* hurricane_draw_message_screen()
*
**************************************************************************************************/
void hurricane_draw_message_screen(char *message, int align, char *button1_str, char *button2_str)
{
  GUI_RECT rect;

  GUI_MULTIBUF_Begin();
  GUI_Clear();
  GUI_SetFont(&GUI_Font10_1);
  rect.x0 = HURRICANE_SETUP_TYPICAL_MARGIN;
  rect.x1 = LCD_WIDTH - HURRICANE_SETUP_TYPICAL_MARGIN;
  rect.y0 = HURRICANE_SETUP_TYPICAL_MARGIN;
  rect.y1 = HURRICANE_NAVIGATION_Y0_COORD;
  GUI_SetBkColor(GUI_BLACK);
  GUI_SetColor(GUI_WHITE);
  GUI_DispStringInRectWrap(message, &rect, align, GUI_WRAPMODE_WORD);
  GOS_LOG(message);

  hurricane_setup_draw_navigation(button1_str, button2_str);

  GUI_MULTIBUF_End();
}


/*************************************************************************************************
* hurricane_draw_lcd_console()
*
**************************************************************************************************/
#define  LCD_CONSOLE_MAX_LINES  10
#define  LCD_CONSOLE_MAX_CHARS  512

char console_buffer[LCD_CONSOLE_MAX_CHARS];

void hurricane_draw_lcd_console(char *message, bool append, char *button1_str, char *button2_str)
{ int num_lines = 0;
  int message_num_lines = 0;
  int i = 0;

  GUI_MULTIBUF_Begin();
  GUI_Clear();
  GUI_SetFont(&GUI_Font10_1);

  if(append == false)
  { strlcpy(console_buffer, message, LCD_CONSOLE_MAX_CHARS);
  }
  else
  { 
  //Handle scrolling if necessary by removing lines at the beginning then append
    num_lines = GUI_WrapGetNumLines(console_buffer, LCD_WIDTH - (2*HURRICANE_SETUP_TYPICAL_MARGIN), GUI_WRAPMODE_WORD);
    message_num_lines = GUI_WrapGetNumLines(message, LCD_WIDTH - (2*HURRICANE_SETUP_TYPICAL_MARGIN), GUI_WRAPMODE_WORD);

    i = 0;
    while((num_lines + message_num_lines) > LCD_CONSOLE_MAX_LINES)          //do we need to scroll?
    { while((console_buffer[i] != 0xA) && (i<(LCD_CONSOLE_MAX_CHARS-2)))    //find the next linefeed (\n = 0xA)
      { i++;
      } 
      i++;
      num_lines = GUI_WrapGetNumLines(&console_buffer[i], LCD_WIDTH - (2*HURRICANE_SETUP_TYPICAL_MARGIN), GUI_WRAPMODE_WORD);
    }
    
    if(i>0)
    { strcpy(console_buffer, &console_buffer[i]);     //shift the buffer to remove line(s) from the beginning
    }
    snprintf(&console_buffer[strlen(console_buffer)], (LCD_CONSOLE_MAX_CHARS - strlen(console_buffer)), "\n%s", message);
  }
  
  GUI_RECT rect;

  rect.x0 = HURRICANE_SETUP_TYPICAL_MARGIN;
  rect.x1 = LCD_WIDTH - HURRICANE_SETUP_TYPICAL_MARGIN;
  rect.y0 = HURRICANE_SETUP_TYPICAL_MARGIN;
  rect.y1 = HURRICANE_NAVIGATION_Y0_COORD;
  GUI_SetBkColor(GUI_BLACK);
  GUI_SetColor(GUI_WHITE);
  GUI_DispStringInRectWrap(console_buffer, &rect, GUI_TA_TOP | GUI_TA_LEFT, GUI_WRAPMODE_WORD);
  hurricane_setup_draw_navigation(button1_str, button2_str);

  GUI_MULTIBUF_End();
  
  GOS_LOG(message);

}



/*************************************************************************************************
* hurricane_setup_draw_factory_reset_confirm()
*
**************************************************************************************************/
static void hurricane_setup_draw_factory_reset_confirm(void)
{ char buffer[128];
  sprintf(buffer, "Are you sure you want to reset the settings to the factory default?\n\nAll settings will be lost and the device will reboot"); 
  hurricane_draw_message_screen(buffer, GUI_TA_LEFT | GUI_TA_TOP, "Back", "Confirm");

  GOS_LOG("Press Button 1 to confirm or Button 2 to go back to Setup");
}


/*************************************************************************************************
* hurricane_setup_state_next()
*
**************************************************************************************************/
static void hurricane_setup_state_next(hurricaneSetupState_t next_state)
{
  switch (next_state)
  {
    case HURRICANE_SETUP_STATE_MENU:
      hurricane_setup_draw_menu(&main_menu);
      break;
    case HURRICANE_SETUP_STATE_WIFI_1:
      gos_setup_start();                  
      hurricane_setup_draw_wifi(1);
      break;
    case HURRICANE_SETUP_STATE_WIFI_2:
      hurricane_setup_draw_wifi(2);
      break;
    case HURRICANE_SETUP_STATE_WIFI_3:
      hurricane_setup_draw_wifi(3);
      break;
    case HURRICANE_SETUP_STATE_CLAIM:
      hurricane_setup_claim_device();
      break;
    case HURRICANE_SETUP_STATE_LAUNCH:
      hurricane_show_demo_splash_screen(0);
      break;
    case HURRICANE_SETUP_STATE_SYS_DIAG_MENU:
      hurricane_setup_draw_menu(&diagnostic_menu);
      break;
    case HURRICANE_SETUP_STATE_NETWORK_STATUS:
      if(!gos_event_is_registered(display_network_status_handler, 0))
      {
        gos_event_register_periodic(display_network_status_handler, 0, 1000, 0);
      }
      hurricane_setup_draw_network_status();
      break;
    case HURRICANE_SETUP_STATE_SOFTWARE_INFO:
      hurricane_setup_draw_software_info();
      break;
    case HURRICANE_SETUP_STATE_PLATFORM_DETAILS:
      hurricane_setup_draw_platform_details();
      break;
    case HURRICANE_SETUP_STATE_SENSOR_ID:
      hurricane_setup_draw_sensor_identification();
      break;
    case HURRICANE_SETUP_STATE_SENSOR_DATA:
      if(!gos_event_is_registered(display_sensor_info_handler, 0))
      {
        gos_event_register_periodic(display_sensor_info_handler, 0, 500, 0);
      }
      hurricane_setup_draw_sensor_data();
      break;
    case HURRICANE_SETUP_STATE_DEFAULT:
      hurricane_setup_draw_factory_reset_confirm();
      break;
    default:
      break;
  }
  setup_state = next_state;
}
