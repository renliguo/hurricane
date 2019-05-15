/*
 * EVALUATION AND USE OF THIS SOFTWARE IS SUBJECT TO THE TERMS AND
 * CONDITIONS OF THE CONTROLLING LICENSE AGREEMENT FOUND AT LICENSE.md
 * IN THIS SDK. IF YOU DO NOT AGREE TO THE LICENSE TERMS AND CONDITIONS,
 * PLEASE RETURN ALL SOURCE FILES TO SILICON LABORATORIES.
 * (c) Copyright 2018, Silicon Laboratories Inc.  All rights reserved.
 */

#include "gos.h"
#include "hurricane.h"

#define DMS_MSG_TIMEOUT 		90000
#define CLAIM_TIMEOUT           90000

static bool connection_up;
static gos_handler_t pin_handler, claim_handler;
static bool pin_request_pending;
static gos_msgpack_context_t *sensor_msgpack = NULL;
static bool check_dfu_result = false;


static void send_pin_request(void *arg);
static gos_result_t hurricane_dms_leaderboard_ota_result( void);



static void dms_connection_state_handler(bool is_up)
{
    if(!is_up)
    {
        GOS_LOG("Disconnected from DMS");
        connection_up = false;
    }
    else
    {
        GOS_LOG("Connected to DMS");
        connection_up = true;
        
      /* When hurricane_dms_init() is called this flag is set indicating that we rebooted. We must
         check is a DFU occured and if so we should send the DFU result to leaderboard  */
         
        if(check_dfu_result == true)    
        { check_dfu_result = false;
          hurricane_dms_leaderboard_ota_result();
        }
    }
}

static gos_result_t hurricane_dms_leaderboard_ota_result( void)
{ gos_result_t result = GOS_SUCCESS;
  gos_dfu_status_t  dfu_status = gos_dms_get_dfu_status();
  
  gos_dms_clear_dfu_status();
  
  if(dfu_status == GOS_DFU_STATUS_SUCCESS)
  {
      
    GOS_LOG("Sending OTA Success to Leaderboard");

    gos_msgpack_context_t *msgpack = NULL;
    const gos_dms_messsage_config_t config =
    {
            .length                 = 0,
            .is_response            = false,
            .response.handler       = 0,
            .response.timeout_ms    = DMS_MSG_TIMEOUT
    };  

    // Initialize the write context
    if(GOS_FAILED(result, gos_dms_message_write_init(&msgpack, &config)))
    {
        goto end;
    }

    gos_msgpack_write_dict_marker(msgpack, 3);
    gos_msgpack_write_dict_str(msgpack, "request", "webhook");
    gos_msgpack_write_dict_str(msgpack, "code", "LEADERBOARD");
    gos_msgpack_write_dict_dict(msgpack, "data", 1);
    gos_msgpack_write_dict_str(msgpack, "installed", "ok");

    // Send the message to the DMS
    if(GOS_FAILED(result, gos_dms_message_write_flush(msgpack)))
    {
        gos_dms_message_context_destroy(msgpack);
    }

      
  }

end:  
  return GOS_SUCCESS;
}

static gos_result_t debug_msgpack_iterator(const gos_msgpack_object_t *key, const gos_msgpack_object_t *value, void *arg)
{
    char value_str[128];

    // If we're iterating a dictionary
    if(key != NULL)
    {
        char key_str[128];

        GOS_LOG("%s : %s", MSGPACK_STR(key, key_str, sizeof(key_str)), MSGPACK_TO_STR(value, value_str, sizeof(value_str)));
    }
    // Else we're iterating an array
    else
    {
        GOS_LOG("%s", MSGPACK_TO_STR(value, value_str, sizeof(value_str)));
    }

    return GOS_SUCCESS;
}

static void debug_print_msg(void *msg)
{   
    gos_result_t result;

    // Recursively iterate through all element in the message and print to log
    if(GOS_FAILED(result, MSGPACK_FOREACH_RECURSIVE(msg, debug_msgpack_iterator, NULL)))
    {
        GOS_LOG("Failed to iterate msg, err:%d", result);
    }
}



void hurricane_dms_dfu(void)
{
    gos_result_t result;
    char message_buffer[128];

    
    hurricane_draw_lcd_console("Firmware update requested...", false, "","");

  /* The hurricane code can consume an unusual percentage of RAM because of the large dislay buffers.
     This can cause low-memory issues during DFU. To improve the reliability of the DFU operation we
     reduce memory consumption by disabling services that are not needed includeing the HTTP server 
     and mdns.
  */
    hurricane_draw_lcd_console("Disabling http_server and mdns ...", true, "","");

    gos_cmd_issue("set mdns.enabled 0");
    gos_cmd_issue("set http.server.enabled 0");
    gos_network_restart(GOS_INTERFACE_WLAN);

  
  /* gos_network_restart() is blocking therefore when we get here the restart is complete
     or has failed.  We need to check if the network is up before continuing.
  */
    if(gos_network_is_up(GOS_INTERFACE_WLAN) == true)   
    { 
      hurricane_draw_lcd_console("Invoking firmware update ...", true, "","");
        
      hurricane_draw_lcd_console("** View device console output for further messages.**\n\nDevice will automatically reboot after update completes.", true, "", "");

      // Then invoke an OTA firmware update
      // NOTE: This does NOT return if successful
      if(GOS_FAILED(result, gos_dms_ota_update(NULL, true)))
      {
        sprintf(message_buffer,"Failed to invoke firmware update, err:%d", result);
        hurricane_draw_lcd_console(message_buffer, true, "","");
      }
    }
    else
    { hurricane_draw_lcd_console("Network failed to restart ...", true, "","");
    }

    hurricane_draw_lcd_console("Press reset to exit update mode.", true, "","");

    // // If we get here then something failed or no update is available
    //restart the http_server and mdns
    gos_cmd_issue("set mdns.enabled 1");
    gos_cmd_issue("set http.server.enabled 1");

    //restart network if nework is up otherwise the settings will take affect next network up
    if(gos_network_is_up(GOS_INTERFACE_WLAN) == true)   
      gos_network_restart(GOS_INTERFACE_WLAN);
}

GOS_CMD_CREATE_COMMAND(leader, claim, "leader.claimed",     "claimed",   0, 0, false);
GOS_CMD_CREATE_COMMAND(leader, ota,   "leader.ota",       "ota",     0, 0, false);


GOS_DEFINE_COMMAND(leader, claim)
{
    GOS_LOG("Leader command: Claimed");

    if(claim_handler)
    {
        claim_handler(0);

        claim_handler = 0;
    }

    return GOS_SUCCESS;
}


GOS_DEFINE_COMMAND(leader, ota)
{
    GOS_LOG("Leader command: OTA");

    hurricane_firmware_update();

    return GOS_SUCCESS;
}


// static void command_listener(void *msg)
// {
//     char buffer[512];

//     gos_msgpack_object_t *data = gos_msgpack_get_dict_object((gos_msgpack_object_dict_t *)msg, "command", MSGPACK_TYPE_STR);

//     if(gos_msgpack_get_str(data, buffer, sizeof(buffer)) == NULL)
//     {
//         GOS_LOG("Command parse failed");
//         return;
//     }

//     GOS_LOG("Command: %s", buffer);

//     if(strcmp(buffer, "claimed") == 0)
//     {
//         if(claim_handler)
//         {
//             claim_handler(0);

//             claim_handler = 0;
//         }
//     }
//     else if(strcmp(buffer, "ota") == 0)
//     {
//         ota();
//         // GOS_LOG("Cloud initiated OTA is disabled");
//     }
// }


static void echo_response_handler(void *arg)
{
    char buffer[512];

    gos_msgpack_object_t *msg = arg;
    
    if(msg == NULL)
    {
    	GOS_LOG("Echo timed out");
    	return;
    }

    gos_msgpack_object_t *data = gos_msgpack_get_dict_object((gos_msgpack_object_dict_t *)msg, "data", MSGPACK_TYPE_STR);

    GOS_LOG("%s", gos_msgpack_get_str(data, buffer, sizeof(buffer)));

    // Cleanup message
    gos_msgpack_free_objects(msg);
}

static void timeout_pin_request(void *arg)
{
    // clear request
    pin_request_pending = false;    

    GOS_LOG("PIN timed out");

    if(claim_handler)
    {
        claim_handler((void *)-1);

        claim_handler = 0;
    }
}

static void pin_response_handler(void *arg)
{
    char buffer[512];
    uint32_t pin;

    gos_msgpack_object_t *msg = arg;
    
    if(msg == NULL)
    {
    	GOS_LOG("Pin timed out");

        // re-register event to try again 
        gos_event_register_timed(send_pin_request, NULL, 4000, GOS_EVENT_FLAG_NONE);

    	return;
    }

    gos_msgpack_object_t *data;
    // check for error
    data = gos_msgpack_get_dict_object((gos_msgpack_object_dict_t *)msg, "error", MSGPACK_TYPE_STR);

    if(data != NULL)
    {
        if(gos_msgpack_get_str(data, buffer, sizeof(buffer)) != NULL)
        {
            GOS_LOG("Error Msg: %s", buffer);

            debug_print_msg(msg);
        }

        // re-register event to try again 
        gos_event_register_timed(send_pin_request, NULL, 4000, GOS_EVENT_FLAG_NONE);

        return;
    }

    data = gos_msgpack_get_dict_object((gos_msgpack_object_dict_t *)msg, "data", MSGPACK_TYPE_DICT);
    data = gos_msgpack_get_dict_object((gos_msgpack_object_dict_t *)data, "pin", MSGPACK_TYPE_STR);

    if(data == NULL)
    {
        return;
    }

    if(gos_msgpack_get_str(data, buffer, sizeof(buffer)) == NULL)
    {
    	return;
    }

    // Cleanup message
    gos_msgpack_free_objects(msg);

    // clear request
    pin_request_pending = false;

    // remove timeout
    gos_event_unregister(timeout_pin_request, 0);

    if(pin_handler)
    {
        pin = atoi(buffer);
        
	    // call handler
	    pin_handler(&pin);
	    pin_handler = NULL;
	}
}

static void send_pin_request(void *arg)
{
    gos_result_t result;

    if(!connection_up)
    {
        GOS_LOG("PIN request fail, connection is not up");
        result = GOS_ERROR;
        goto end;
    }

    GOS_LOG("Sending PIN request");

    gos_msgpack_context_t *msgpack = NULL;
    const gos_dms_messsage_config_t config =
    {
            .length                 = 0,
            .is_response            = false,
            .response.handler       = pin_response_handler,
            .response.timeout_ms    = DMS_MSG_TIMEOUT
    };  

    // Initialize the write context
    if(GOS_FAILED(result, gos_dms_message_write_init(&msgpack, &config)))
    {
        goto end;
    }

    gos_msgpack_write_dict_marker(msgpack, 3);
    gos_msgpack_write_dict_str(msgpack, "request", "webhook");
    gos_msgpack_write_dict_str(msgpack, "code", "LEADERBOARD");
    gos_msgpack_write_dict_dict(msgpack, "data", 1);
    gos_msgpack_write_dict_str(msgpack, "request", "pin");

    // Send the message to the DMS
    if(GOS_FAILED(result, gos_dms_message_write_flush(msgpack)))
    {
        gos_dms_message_context_destroy(msgpack);
    }

end:
    if(result == GOS_SUCCESS)
    {
        GOS_LOG("Sent PIN request");   
    }
    else
    {
        // re-register event to try again 
        gos_event_register_timed(send_pin_request, NULL, 1000, GOS_EVENT_FLAG_NONE);
    }
}


void hurricane_dms_init(void)
{
    gos_dms_set_message_stream_state_handler(dms_connection_state_handler);
    check_dfu_result = true;    //if Init was called then we need to check if DFU occured once DMS connection occurs (see dms_connection_state_handler())
}	

gos_result_t hurricane_dms_request_echo(char *echo_str, gos_handler_t handler)
{
	gos_result_t result;
    gos_msgpack_context_t *msgpack = NULL;
    const gos_dms_messsage_config_t config =
    {
            .length                 = 0,
            .is_response            = false,
            .response.handler       = echo_response_handler,
            .response.timeout_ms    = DMS_MSG_TIMEOUT
    };	

    // Initialize the write context
    if(GOS_FAILED(result, gos_dms_message_write_init(&msgpack, &config)))
    {
      return result;
    }

    gos_msgpack_write_dict_marker(msgpack, 2);
    gos_msgpack_write_dict_str(msgpack, "request", "echo");
    gos_msgpack_write_dict_str(msgpack, "data", echo_str);    

    // Send the message to the DMS
    if(GOS_FAILED(result, gos_dms_message_write_flush(msgpack)))
    {
    	gos_dms_message_context_destroy(msgpack);
    }

    return result;
}


void hurricane_dms_request_pin(gos_handler_t pin, gos_handler_t claim)
{
    if(pin_request_pending)
    {
        return;
    }

    GOS_LOG("Initiate PIN request");

    // remove timeout, if one was pending
    gos_event_unregister(timeout_pin_request, 0);

    // register timeout handler
    gos_event_register_timed(timeout_pin_request, NULL, CLAIM_TIMEOUT, GOS_EVENT_FLAG_NONE);

    pin_request_pending = true;
    pin_handler = pin;
    claim_handler = claim;
    gos_event_issue(send_pin_request, NULL, GOS_EVENT_FLAG_NONE);
}

void hurricane_init_sensor_data(uint32_t number_of_items)
{
    gos_result_t result;
    sensor_msgpack = NULL;
    const gos_dms_messsage_config_t config =
    {
            .length                 = 0,
            .is_response            = false,
            .response.handler       = 0,
            .response.timeout_ms    = DMS_MSG_TIMEOUT
    };  

    // Initialize the write context
    if(GOS_FAILED(result, gos_dms_message_write_init(&sensor_msgpack, &config)))
    {
      return;
    }

    gos_msgpack_write_dict_marker(sensor_msgpack, 3);
    gos_msgpack_write_dict_str(sensor_msgpack, "request", "webhook");
    gos_msgpack_write_dict_str(sensor_msgpack, "code", "LEADERBOARD");
    gos_msgpack_write_dict_dict(sensor_msgpack, "data", 1);
    gos_msgpack_write_dict_dict(sensor_msgpack, "sensors", number_of_items);
}

void hurricane_add_sensor_data(char *name, float data)
{
    if(sensor_msgpack == NULL)
    {
        return;
    }

    char float_str[32];
    float_to_str(data, float_str, 2);

    // gos_msgpack_write_dict_float(sensor_msgpack, name, data);
    gos_msgpack_write_dict_str(sensor_msgpack, name, float_str);
}

void hurricane_flush_sensor_data(void)
{
    gos_result_t result;

    if(sensor_msgpack == NULL)
    {
        return;
    }

    // Send the message to the DMS
    if(GOS_FAILED(result, gos_dms_message_write_flush(sensor_msgpack)))
    {
        gos_dms_message_context_destroy(sensor_msgpack);
    }

    sensor_msgpack = NULL;
}


void hurricane_post_game_score(float points)
{
    gos_result_t result;

    if(!connection_up)
    {
        return;
    }

    gos_msgpack_context_t *msgpack = NULL;
    const gos_dms_messsage_config_t config =
    {
            .length                 = 0,
            .is_response            = false,
            .response.handler       = 0,
            .response.timeout_ms    = DMS_MSG_TIMEOUT
    };  

    // Initialize the write context
    if(GOS_FAILED(result, gos_dms_message_write_init(&msgpack, &config)))
    {
        return;
    }

    gos_msgpack_write_dict_marker(msgpack, 3);
    gos_msgpack_write_dict_str(msgpack, "request", "webhook");
    gos_msgpack_write_dict_str(msgpack, "code", "LEADERBOARD");
    gos_msgpack_write_dict_dict(msgpack, "data", 1);
    gos_msgpack_write_dict_dict(msgpack, "result", 1);

    char float_str[10];
    float_to_str(points, float_str, 3);

    gos_msgpack_write_dict_str(msgpack, "time", float_str);

    // Send the message to the DMS
    if(GOS_FAILED(result, gos_dms_message_write_flush(msgpack)))
    {
        gos_dms_message_context_destroy(msgpack);
    }
}
















