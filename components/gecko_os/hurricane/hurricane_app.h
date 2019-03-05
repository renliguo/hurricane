/*
 * EVALUATION AND USE OF THIS SOFTWARE IS SUBJECT TO THE TERMS AND
 * CONDITIONS OF THE CONTROLLING LICENSE AGREEMENT FOUND AT LICENSE.md
 * IN THIS SDK. IF YOU DO NOT AGREE TO THE LICENSE TERMS AND CONDITIONS,
 * PLEASE RETURN ALL SOURCE FILES TO SILICON LABORATORIES.
 * (c) Copyright 2018, Silicon Laboratories Inc.  All rights reserved.
 */

#pragma once

#include <stdint.h>


/*************************************************************************************************
* Exported Defines
**************************************************************************************************/

#define HURRICANE_SUCCESS 	0
#define HURRICANE_FAIL 		1


/*************************************************************************************************
* Exported Functions
**************************************************************************************************/

/*************************************************************************************************
* hurricane_init()
*
* This is the entry function to the hurricane base code.  To make an application a 'hurricane app'
* simply call this function from gos_app_init().
*
* This function will initialize the board then start an the intro video and exit.  From that point 
* on the operation is event driven.  The application provides a callback that is used to start the
* demo via the hurricane_demo_t parameter.  The application should not perform any tasks in 
* gos_app_init() following the call to hurricane_init().  Rather is should wait for the callback to
* be called before performing any further operation.     
**************************************************************************************************/
void hurricane_init(hurricane_demo_t *demo_handle);


/******************************************************************************
* hurricane_show_demo_splash_screen
*
* This function will show the splash screen for the demo.
*
* Note: This function will deinit the handler(s) for the buttons and then 
*       reconfigure them for the setup menu. If this function is called from 
*       the demo, all other events should also be de-registered.
*******************************************************************************/
void hurricane_show_demo_splash_screen(void);


/******************************************************************************
* hurricane_enter_setup_mode
*
* This function will enter the setup mode which displays the setup menu
* allowing the user to configure the board.
*
* Note: This function will deinit the handler(s) for the buttons and then 
*       reconfigure them for the setup menu. If this function is called from 
*       the demo, all other events should also be de-registered.
*******************************************************************************/
void hurricane_enter_setup_mode(void);


/******************************************************************************
* hurricane_draw_jpeg
*
* Draws a JPEG on the display using the EMWIN library.  Note that the JPEG is
* drawn the size specified in the file.  There is no scaling in this function.
*
* arg - the filename of the jpeg.  Note that the file is opened using 
*       GOS_FILE_LOCATION_ANY therefore the path is not required.
*******************************************************************************/
int hurricane_draw_jpeg(void *arg);


/******************************************************************************
* hurricane_get_jpeg_data
*
* Call back function used by EMWIN functions to read JPEG data from the file
* system.
*
* Refer to the EMWIN API reference for parameter details.
*******************************************************************************/
int hurricane_get_jpeg_data(void *p, const uint8_t **ppData, unsigned num_bytes, U32 offset);   


