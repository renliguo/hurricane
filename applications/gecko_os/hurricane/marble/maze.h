/*
 * EVALUATION AND USE OF THIS SOFTWARE IS SUBJECT TO THE TERMS AND
 * CONDITIONS OF THE CONTROLLING LICENSE AGREEMENT FOUND AT LICENSE.md
 * IN THIS SDK. IF YOU DO NOT AGREE TO THE LICENSE TERMS AND CONDITIONS,
 * PLEASE RETURN ALL SOURCE FILES TO SILICON LABORATORIES.
 * (c) Copyright 2019, Silicon Laboratories Inc.  All rights reserved.
 */

#pragma once

#define MAZE_SIZE_X 10
#define MAZE_SIZE_Y 8

#define WALL_LEN_X (LCD_WIDTH / MAZE_SIZE_X)
#define WALL_LEN_Y WALL_LEN_X

#define MAZE_PATH_LIMIT_MIN 2
#define MAZE_PATH_LIMIT_MAX 8

#define MAZE_FLAG_N         0b00000001
#define MAZE_FLAG_E         0b00000010
#define MAZE_FLAG_S         0b00000100
#define MAZE_FLAG_W         0b00001000
#define MAZE_FLAG_C         0b00010000 // corner
#define MAZE_FLAG_B         0b00100000 // bounds
#define MAZE_FLAG_V         0b01000000 // visited
#define MAZE_FLAG_INVALID   0b10000000

uint8_t detect_collision(int32_t x, int32_t y, int32_t r, int32_t *depth_x, int32_t *depth_y);
// void clear_visitors(void);
void render_maze(void);
void get_maze_starting_point(uint32_t *x, uint32_t *y);
void get_maze_end_point(uint32_t *x, uint32_t *y);
void generate_maze(void);
bool is_coord_at_end_point(uint32_t x, uint32_t y);