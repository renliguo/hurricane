/*
 * EVALUATION AND USE OF THIS SOFTWARE IS SUBJECT TO THE TERMS AND
 * CONDITIONS OF THE CONTROLLING LICENSE AGREEMENT FOUND AT LICENSE.md
 * IN THIS SDK. IF YOU DO NOT AGREE TO THE LICENSE TERMS AND CONDITIONS,
 * PLEASE RETURN ALL SOURCE FILES TO SILICON LABORATORIES.
 * (c) Copyright 2019, Silicon Laboratories Inc.  All rights reserved.
 */

#include "gos.h"

#include "hurricane.h"
#include "maze.h"
#include "marble.h"

#include "prng.h"

#include <math.h>

static float marble_pos_x, marble_pos_y, marble_vel_x, marble_vel_y, marble_accel_x, marble_accel_y;
static float gravity = 250.0;
static float elasticity = 0.6;
static float friction = 0.001;
static uint8_t detection_flags;
static float dt = 0.1;
static uint64_t prev;

static uint32_t end_x, end_y;
static float game_time;
static bool game_over;


#define REST_ACCEL      0.5
#define REST_VELOCITY   3.0


void marble_init(void)
{
    generate_maze();

    uint32_t start_x, start_y;
    get_maze_starting_point(&start_x, &start_y);
    marble_pos_x = start_x;
    marble_pos_y = start_y;
    
    detection_flags = 0;
    marble_vel_x = 0;
    marble_vel_y = 0;
    marble_accel_x = 0;
    marble_accel_y = 0;
    dt = 0.1;

    get_maze_end_point(&end_x, &end_y);

    gos_time_get_uptime(&prev);

    game_time = 0.0;
    game_over = false;




    // game_over = true;

    // gos_result_t result;
    // gos_handle_t h;
    // result = gos_tcp_connect(GOS_INTERFACE_WLAN, "10.1.54.11", 12354, &h);

    // GOS_LOG("connect %d", result);

    // // uint32_t seed = 1;
    // for(uint32_t i = 0; i < 10000000; i++)
    // {
    //     // uint32_t val = pseudo_rand(&seed);
    //     uint32_t val = rand();

    //     result = gos_tcp_write(h, &val, sizeof(val), true);   

    //     if(result != GOS_SUCCESS)
    //     {
    //         GOS_LOG("fail %d", result);
    //         break;
    //     }

    //     if((i % 10000) == 0){  

    //         GOS_LOG("%d", i);
    //         gos_rtos_delay_milliseconds(10);
    //     }
    // }


    // gos_tcp_disconnect(h);


    // gos_handle_t h;
    // gos_file_t info = GOS_FILE_DEFAULT_USER_FILE_INFO;
    // info.size = 8000000;
    // strcpy(info.name, "test.bin");

    // gos_result_t result;

    // result = gos_file_create(&info, &h);

    // GOS_LOG("file %d", result);



    // GOS_LOG("BEGIN");

    // uint32_t seed = 1;

    // uint64_t start, elapsed, start2;

    // gos_time_get_uptime(&start);
    // start2 = start;

    // for(uint32_t i = 0; i < info.size / 4; i++)
    // {
    //     // GOS_LOG("%lu", pseudo_rand(&seed));
        
    //     uint32_t val = pseudo_rand(&seed);
    //     // uint32_t val = i;

    //     result = gos_file_write(h, &val, sizeof(val));

    //     if(result != GOS_SUCCESS)
    //     {
    //         GOS_LOG("error %d", result);
    //         break;
    //     }

    //     // GOS_LOG("%lu", i);

    //     if((i % 10000) == 0){  

    //         gos_time_get_uptime(&elapsed);
    //         GOS_LOG("%d %lu", i, (uint32_t)(elapsed - start2));
    //         start2 = elapsed;

    //         // break;
    //     }
    // }

    // gos_time_get_uptime(&elapsed);

    // GOS_LOG("%lu", (uint32_t)(elapsed - start));

    // gos_file_close(h);

    // GOS_LOG("END");
}

bool marble_frame(void)
{
    uint64_t now, elapsed;

    if(game_over)
    {
        return game_over;
    }

    gos_time_get_uptime(&now);
    elapsed = now - prev;
    prev = now;
    dt = (float)elapsed / 1000.0;

    game_time += dt;

    float accel_x, accel_y, accel_z;
    // note that X and Y axes are flipped
    hurricane_sensor_read_accel_immediate(&accel_y, &accel_x, &accel_z);

// accel_y = 0;
// accel_x = 0;


    marble_accel_x = -1.0 * accel_x * gravity;
    marble_accel_y = -1.0 * accel_y * gravity;

    accel_x = marble_accel_x;
    accel_y = marble_accel_y;

    if(fabs(marble_accel_x) < REST_ACCEL)
    {
        marble_accel_x = 0.0;
    }

    if(fabs(marble_accel_y) < REST_ACCEL)
    {
        marble_accel_y = 0.0;
    }

    // check detection flags for previous collision and prevent acceleration
    // into that collision
    if(detection_flags & MAZE_FLAG_N)
    {
        if(marble_accel_y < 0.0)
        {
            marble_accel_y = 0.0;
        }
    }

    if(detection_flags & MAZE_FLAG_S)
    {
        if(marble_accel_y > 0.0)
        {
            marble_accel_y = 0.0;
        }
    }

    if(detection_flags & MAZE_FLAG_E)
    {
        if(marble_accel_x > 0.0)
        {
            marble_accel_x = 0.0;
        }
    }

    if(detection_flags & MAZE_FLAG_W)
    {
        if(marble_accel_x < 0.0)
        {
            marble_accel_x = 0.0;
        }
    }


    // update velocity
    marble_vel_x += marble_accel_x * dt;
    marble_vel_y += marble_accel_y * dt;  
    
    // apply friction
    if(marble_vel_x > 0.0)
    { 
        marble_vel_x -= friction;

        if(marble_vel_x < 0.0)
        {
            marble_vel_x = 0.0;
        }
    }
    else if(marble_vel_x < 0.0)
    {  
        marble_vel_x += friction;

        if(marble_vel_x > 0.0)
        {
            marble_vel_x = 0.0;
        }
    }

    if(marble_vel_y > 0.0)
    {
        marble_vel_y -= friction;

        if(marble_vel_y < 0.0)
        {
            marble_vel_y = 0.0;
        }
    }
    else if(marble_vel_y < 0.0)
    {
        marble_vel_y += friction;

        if(marble_vel_y > 0.0)
        {
            marble_vel_y = 0.0;
        }
    }

    // update position
    marble_pos_x += marble_vel_x * dt;
    marble_pos_y += marble_vel_y * dt;  

    // bounds check against screen
    if(marble_pos_x > LCD_WIDTH)
    {
        marble_pos_x = LCD_WIDTH;
        marble_vel_x *= -1.0 * elasticity;
    }
    else if(marble_pos_x < 0.0)
    {
        marble_pos_x = 0.0;
        marble_vel_x *= -1.0 * elasticity;
    }

    if(marble_pos_y > LCD_HEIGHT)
    {
        marble_pos_y = LCD_HEIGHT;
        marble_vel_y *= -1.0 * elasticity;
    }
    else if(marble_pos_y < 0.0)
    {
        marble_pos_y = 0.0;
        marble_vel_y *= -1.0 * elasticity;
    }

    // collision detection
    uint16_t grid_x = (uint16_t)marble_pos_x;
    uint16_t grid_y = (uint16_t)marble_pos_y;

    int32_t depth_x, depth_y;
    detection_flags = detect_collision(grid_x, grid_y, MARBLE_RADIUS, &depth_x, &depth_y);

    // process collision on each axis
    // if below minimum velocity, correct position directly to resolve collision
    // and set velocity to 0. On the next frame, acceleration will be set to 0
    // in that axis because the detection flag will be set.
    // if above minimum, bounce in the other direction by flipping velocity 
    // and applying elasticity (to reduce energy in the system)

    if(detection_flags & MAZE_FLAG_C)
    {   
        
        // use X and Y depth components plus magnitude of velocity to
        // compute a new velocity vector

        // get current magnitude of velocity
        float mag_v = sqrt((marble_vel_x * marble_vel_x) + (marble_vel_y * marble_vel_y));

        // compute angle of the collision

        // float angle = acos((float)depth_x / (float)depth_y);
        float angle = atan((float)depth_y / (float)depth_x);
        // note that angle is in radians

        // check which quadrant we're in and adjust the angle 
        // accordingly.
        /*
        X, Y      |
        0, 0      | quad 1
        quad 0    |
                  |
        ----------------------  0 deg.
                  |
        quad 3    | quad 2
                  |
                  |

                  90 deg.
        */
        // Note this for the position of the object within the marble,
        // not the marble itself.

        // quad 0
        if((depth_x < 0.0) && (depth_y < 0.0))
        {
            angle += M_PI; // add 180 degrees
        }
        // quad 1
        else if((depth_x > 0.0) && (depth_y < 0.0))
        {
            angle += 2.0 * M_PI; // add 360 degrees
        }
        // quad 2
        else if((depth_x > 0.0) && (depth_y > 0.0))
        {
            // angle does not need to be adjusted
        }
        // quad 3
        else
        {
            angle += M_PI; // add 180 degrees
        }

        // add 180 degrees to collision angle, because we want to resolve in the
        // opposite direction
        angle += M_PI;

        // apply elasticity to velocity
        float adjusted_vel = mag_v * elasticity;

        // calculate new velocity vector
        float new_vel_x = cos(angle) * adjusted_vel;
        float new_vel_y = sin(angle) * adjusted_vel;

        // char float_str[10];
        // char float_str2[10];
        // char float_str3[10];
        // char float_str4[10];
        // char float_str5[10];
        // char float_str6[10];
        // char float_str7[10];
        // float_to_str(mag_v, float_str, 3);
        // float_to_str(angle, float_str2, 3);

        // float_to_str(new_vel_x, float_str3, 3);
        // float_to_str(new_vel_y, float_str4, 3);

        // float_to_str(marble_vel_x, float_str5, 3);
        // float_to_str(marble_vel_y, float_str6, 3);

        // float vel_angle = atan(marble_vel_y / marble_vel_x);
        // float_to_str(vel_angle, float_str7, 3);

        // GOS_LOG("corner: %d,%d mag: %s angle %s new %s,%s old %s,%s vel_theta %s", depth_x, depth_y, float_str, float_str2, float_str3, float_str4, float_str5, float_str6, float_str7);


        // apply new velocity vector
        marble_vel_x = new_vel_x;
        marble_vel_y = new_vel_y;

        // resolve collision position directly
        if(abs(mag_v) < REST_VELOCITY)
        {
            marble_pos_x -= depth_x;
            marble_pos_y -= depth_y;
            marble_vel_x = 0.0;
            marble_vel_y = 0.0;
        }
    }
    else
    {
        if(detection_flags & MAZE_FLAG_N)
        {
            if(marble_vel_y < 0.0)
            {
                // marble_pos_y -= depth_y;
                if(marble_vel_y > -REST_VELOCITY)
                {
                    marble_pos_y -= depth_y;
                    marble_vel_y = 0.0;
                }
                else
                {
                    marble_vel_y *= -1.0 * elasticity;
                }
            }
        }

        if(detection_flags & MAZE_FLAG_S)
        {
            if(marble_vel_y > 0.0)
            {
                // marble_pos_y -= depth_y;
                if(marble_vel_y < REST_VELOCITY)
                {
                    marble_pos_y -= depth_y;
                    marble_vel_y = 0.0;
                }
                else
                {
                    marble_vel_y *= -1.0 * elasticity;
                }
            }
        }

        if(detection_flags & MAZE_FLAG_E)
        {
            if(marble_vel_x > 0.0)
            {
                // marble_pos_x -= depth_x;
                if(marble_vel_x < REST_VELOCITY)
                {
                    marble_pos_x -= depth_x;
                    marble_vel_x = 0.0;
                }
                else
                {
                    marble_vel_x *= -1.0 * elasticity;
                }
            }
        }

        if(detection_flags & MAZE_FLAG_W)
        {
            if(marble_vel_x < 0.0)
            {
                // marble_pos_x -= depth_x;
                if(marble_vel_x > -REST_VELOCITY)
                {
                    marble_pos_x -= depth_x;
                    marble_vel_x = 0.0;
                }
                else
                {
                    marble_vel_x *= -1.0 * elasticity;
                }
            }
        }
    }

    uint32_t color = 0x0000FF;
    // if(detection_flags != 0)
    // {
    //     color = 0xFF00FF;
    // }

    game_over = is_coord_at_end_point(grid_x, grid_y);
   
    // begin drawing
    GUI_MULTIBUF_Begin();

    render_maze(); // maze will clear display

    char c[64];
    char float_str[10];
    if(game_time < 100.0)
    {
        float_to_str(game_time, float_str, 2);
    }
    else
    {   
        // reduce precision if over 100 seconds
        float_to_str(game_time, float_str, 1);   
    }

    if(game_over)
    {
        GUI_RECT rect;
        rect.x0 = 0;
        rect.x1 = LCD_WIDTH;
        rect.y0 = LCD_HEIGHT * 0.4;
        rect.y1 = LCD_HEIGHT * 0.6;
        GUI_SetBkColor(GUI_BLACK);
    
        GUI_SetColor(GUI_BLACK);
        GUI_FillRectEx(&rect);

        GUI_SetColor(GUI_RED);
        sprintf(c, "You made it in %s seconds!", float_str);
        GUI_DispStringInRect(c, &rect, GUI_TA_HCENTER | GUI_TA_VCENTER);    

        hurricane_post_game_score(game_time);
    }
    else
    {
        // draw the marble
        GUI_SetPenSize(1);
        GUI_SetColor(color);
        GUI_FillCircle(grid_x, grid_y, MARBLE_RADIUS);


        GUI_RECT rect;
        rect.x0 = 0;
        rect.x1 = WALL_LEN_X * 2 - 1;
        rect.y0 = 0;
        rect.y1 = WALL_LEN_Y * 1 - 1;

        GUI_SetColor(GUI_BLACK);
        GUI_FillRectEx(&rect);

        GUI_SetColor(0xFFFFFF);
        // display game time
        GUI_DispStringInRect(float_str, &rect, GUI_TA_LEFT | GUI_TA_VCENTER);

        // debug
        // GUI_SetColor(0x00FF00);
        // GUI_GotoXY(0, 0);
        // GUI_DispString("Vx:");
        // float_to_str(marble_vel_x, c, 2);
        // GUI_DispString(c);      
        // GUI_DispString(" Ax:");
        // float_to_str(marble_accel_x, c, 2);
        // GUI_DispString(c);      
        // GUI_DispString(" Px:");
        // float_to_str(marble_pos_x, c, 2);
        // GUI_DispString(c);      

        // GUI_SetColor(0x0000FF);
        // GUI_GotoXY(0, 10);
        // GUI_DispString("Vy:");
        // float_to_str(marble_vel_y, c, 2);
        // GUI_DispString(c);
        // GUI_DispString(" Ay:");
        // float_to_str(marble_accel_y, c, 2);
        // GUI_DispString(c);   
        // GUI_DispString(" Py:");
        // float_to_str(marble_pos_y, c, 2);
        // GUI_DispString(c);      

        // GUI_SetColor(0xFF00FF);
        // GUI_GotoXY(0, 20);   
        // // sprintf(c, "0x%x depth X %ld Y %ld", detection_flags, depth_x, depth_y);
        // sprintf(c, "d 0x%x", detection_flags);
        // GUI_DispString(c);    
        // GUI_DispString("Accel x:");
        // float_to_str(accel_x, c, 2);
        // GUI_DispString(c);   
        // GUI_DispString("y:");
        // float_to_str(accel_y, c, 2);
        // GUI_DispString(c);      
  
    }

    GUI_MULTIBUF_End();

    return game_over;

}







