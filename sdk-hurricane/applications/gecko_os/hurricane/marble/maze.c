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
#include "prng.h"

#include <math.h>

static uint8_t maze[MAZE_SIZE_X][MAZE_SIZE_Y];

static uint32_t start_x, start_y;
static uint32_t end_x, end_y;

static uint32_t end_point_color = 0x880000;
static int32_t end_point_delta = -0x010000;

// north-west is 0,0 on the grid

uint8_t get_cell_flags(int32_t x, int32_t y)
{
    // bounds check
    if((x < 0) || (x >= MAZE_SIZE_X) || (y < 0) || (y >= MAZE_SIZE_Y))
    {
        return MAZE_FLAG_INVALID;
    }

    return maze[x][y];
}

uint8_t detect_collision(int32_t x, int32_t y, int32_t r, int32_t *depth_x, int32_t *depth_y)
{
    uint8_t detection_flags = 0;

    // x and y are center points of circular detection area
    // r is radius

    *depth_x = 0;
    *depth_y = 0;

    // get grid cell
    int32_t grid_x = x / WALL_LEN_X;
    int32_t grid_y = y / WALL_LEN_Y;

    // get relative position to grid center
    int32_t cell_x = x - (grid_x * WALL_LEN_X);
    int32_t cell_y = y - (grid_y * WALL_LEN_Y);

    uint8_t cell_flags = get_cell_flags(grid_x, grid_y);

    uint8_t quadrant = 0;
    int32_t corner_x, corner_y;

    // get quadrant of grid cell

    // northwest
    if((cell_x <= WALL_LEN_X / 2) &&
       (cell_y <= WALL_LEN_Y / 2))
    {
        quadrant = 0;
        corner_x = grid_x * WALL_LEN_X;
        corner_y = grid_y * WALL_LEN_Y;
    }
    // northeast
    else if((cell_x > WALL_LEN_X / 2) &&
            (cell_y <= WALL_LEN_Y / 2))
    {
        quadrant = 1;
        corner_x = (grid_x + 1) * WALL_LEN_X;
        corner_y = grid_y * WALL_LEN_Y;
    }
    // southeast
    else if((cell_x > WALL_LEN_X / 2) &&
            (cell_y > WALL_LEN_Y / 2))
    {
        quadrant = 2;
        corner_x = (grid_x + 1) * WALL_LEN_X;
        corner_y = (grid_y + 1) * WALL_LEN_Y;
    }
    // southwest
    else
    {
        quadrant = 3;
        corner_x = grid_x * WALL_LEN_X;
        corner_y = (grid_y + 1) * WALL_LEN_Y;
    }

    int32_t x_dist = x - corner_x;
    int32_t y_dist = y - corner_y;

    int32_t corner_dist = sqrt((x_dist * x_dist) + (y_dist * y_dist));

    // check collisions within cell
    if((cell_y - (int32_t)r) <= 0)
    {
        if(cell_flags & MAZE_FLAG_N)    
        {
            *depth_y = cell_y - (int32_t)r;
            detection_flags |= MAZE_FLAG_N;
        }
    }

    if((cell_x + (int32_t)r) >= WALL_LEN_X)
    {
        if(cell_flags & MAZE_FLAG_E)
        {
            *depth_x = (cell_x + (int32_t)r) - WALL_LEN_X;
            detection_flags |= MAZE_FLAG_E;
        }   
    }

    if((cell_y + (int32_t)r) >= WALL_LEN_Y)    
    {   
        if(cell_flags & MAZE_FLAG_S)
        {
            *depth_y = (cell_y + (int32_t)r) - WALL_LEN_Y;
            detection_flags |= MAZE_FLAG_S;
        }
    }

    if((cell_x - (int32_t)r) <= 0)
    {
        if(cell_flags & MAZE_FLAG_W)
        {
            *depth_x = cell_x - (int32_t)r;
            detection_flags |= MAZE_FLAG_W;
        }
    }

    // check if the marble is colliding with the cell's corner:
    uint8_t corner_flags = 0;
    if((detection_flags == 0) && (corner_dist < r))
    {
        // need to check adjacent cells
        int32_t temp_x, temp_y;

        if(quadrant == 0)
        {
            temp_x = grid_x - 1;
            temp_y = grid_y - 1;
        }
        else if(quadrant == 1)
        {
            temp_x = grid_x + 1;
            temp_y = grid_y - 1;
        }
        else if(quadrant == 2)
        {
            temp_x = grid_x + 1;
            temp_y = grid_y + 1;
        }
        else
        {
            temp_x = grid_x - 1;
            temp_y = grid_y + 1;
        }

        // get diagonal cell and check for walls that hit our corner point.
        // only do this if the cell is not out of bounds
        if((temp_x >= 0) && (temp_x < (MAZE_SIZE_X - 1)) && (temp_y >= 0) && (temp_y < (MAZE_SIZE_Y - 1)))
        {
            corner_flags = get_cell_flags(temp_x, temp_y);

            // filter out invalid walls
            if(quadrant == 0)
            {
                corner_flags &= ~(MAZE_FLAG_N|MAZE_FLAG_W);

                if((corner_flags & MAZE_FLAG_S) || (corner_flags & MAZE_FLAG_E))
                {
                    corner_flags |= MAZE_FLAG_S|MAZE_FLAG_E;
                }
            }
            else if(quadrant == 1)
            {
                corner_flags &= ~(MAZE_FLAG_N|MAZE_FLAG_E);

                if((corner_flags & MAZE_FLAG_S) || (corner_flags & MAZE_FLAG_W))
                {
                    corner_flags |= MAZE_FLAG_S|MAZE_FLAG_W;
                }
            }
            else if(quadrant == 2)
            {
                corner_flags &= ~(MAZE_FLAG_S|MAZE_FLAG_E);

                if((corner_flags & MAZE_FLAG_N) || (corner_flags & MAZE_FLAG_W))
                {
                    corner_flags |= MAZE_FLAG_N|MAZE_FLAG_W;
                }
            }
            else{

                corner_flags &= ~(MAZE_FLAG_S|MAZE_FLAG_W);   

                if((corner_flags & MAZE_FLAG_N) || (corner_flags & MAZE_FLAG_E))
                {
                    corner_flags |= MAZE_FLAG_N|MAZE_FLAG_E;
                }
            }
        }

        if(corner_flags != 0)
        {   
            // indicate corner collision
            detection_flags |= MAZE_FLAG_C;

            // detection is inverted, since the corner detection came from an adjacent cell.
            if(corner_flags & MAZE_FLAG_N)
            {
                detection_flags |= MAZE_FLAG_S;
            }
            else if(corner_flags & MAZE_FLAG_S)
            {
                detection_flags |= MAZE_FLAG_N;
            }

            if(corner_flags & MAZE_FLAG_E)
            {
                detection_flags |= MAZE_FLAG_W;
            }
            else if(corner_flags & MAZE_FLAG_W)
            {
                detection_flags |= MAZE_FLAG_E;
            }


            // process depth, if not already
            if(*depth_x == 0)
            {
                if(detection_flags & MAZE_FLAG_E)
                {
                    *depth_x = (x + r) - corner_x;
                }
                else if(detection_flags & MAZE_FLAG_W)
                {
                    *depth_x = (x - r) - corner_x;
                }
            }

            if(*depth_y == 0)
            {
                if(detection_flags & MAZE_FLAG_N)
                {
                    *depth_y = (y - r) - corner_y;
                }
                else if(detection_flags & MAZE_FLAG_S)
                {
                    *depth_y = (y + r) - corner_y;
                }
            }
        }
    }

    // GOS_LOG("grid (%d,%d) cell (%d,%d) quad %d corner dist %d (%d,%d) flags %x/%x d(%d,%d)", grid_x, grid_y, cell_x, cell_y, quadrant, corner_dist, corner_x, corner_y, (int32_t)corner_flags, (int32_t)detection_flags, *depth_x, *depth_y);



    return detection_flags;
}


uint8_t flip_wall(uint8_t flags)
{
    uint8_t new_flags = 0;

    if(flags & MAZE_FLAG_N)
    {
        new_flags |= MAZE_FLAG_S;
    }

    if(flags & MAZE_FLAG_E)
    {
        new_flags |= MAZE_FLAG_W;
    }

    if(flags & MAZE_FLAG_S)
    {
        new_flags |= MAZE_FLAG_N;
    }

    if(flags & MAZE_FLAG_W)
    {
        new_flags |= MAZE_FLAG_E;
    }

    return new_flags;
}

void remove_wall(uint32_t x, uint32_t y, uint8_t wall_flags)
{
    int32_t adj_x = x;
    int32_t adj_y = y;

    // compute neighbor position
    if(wall_flags & MAZE_FLAG_N)
    {
        // check bounds
        if(y == 0)
        {
            adj_y = -1;
        }  
        else
        {
            adj_y = y - 1;
        }
    }

    if(wall_flags & MAZE_FLAG_S)
    {
        // check bounds
        if(y >= (MAZE_SIZE_Y - 1))
        {
            adj_y = -1;
        }  
        else
        {
            adj_y = y + 1;
        }
    }

    if(wall_flags & MAZE_FLAG_E)
    {
        // check bounds
        if(x >= (MAZE_SIZE_X - 1))
        {
            adj_x = -1;
        }  
        else
        {
            adj_x = x + 1;
        }
    }

    if(wall_flags & MAZE_FLAG_W)
    {
        // check bounds
        if(x == 0)
        {
            adj_x = -1;
        }  
        else
        {
            adj_x = x - 1;
        }
    }

    // check if adjacent cell exists
    if((adj_x >= 0) && (adj_y >= 0))
    {
        // check if adjacent cell is within bounds
        if((maze[adj_x][adj_y] & MAZE_FLAG_B) == 0)
        {
            // delete wall on current cell
            maze[x][y] &= ~wall_flags;

            // delete wall on adjacent cell
            maze[adj_x][adj_y] &= ~flip_wall(wall_flags);
        }
        else
        {
            // GOS_LOG("attempted to open out of bounds: %d,%d", adj_x, adj_y);
        }
    }
}

uint32_t count_unvisited_neighbors(uint32_t x, uint32_t y)
{
    uint32_t count = 0;
    int32_t nei_x, nei_y;

    // north
    nei_x = x;
    nei_y = y - 1;

    if(nei_y >= 0)
    {
        if((maze[nei_x][nei_y] & MAZE_FLAG_V) == 0)
        {
            count++;
        }
    }

    // south
    nei_x = x;
    nei_y = y + 1;

    if(nei_y <= (MAZE_SIZE_Y - 1))
    {
        if((maze[nei_x][nei_y] & MAZE_FLAG_V) == 0)
        {
            count++;
        }
    }

    // west
    nei_x = x - 1;
    nei_y = y;

    if(nei_x >= 0)
    {
        if((maze[nei_x][nei_y] & MAZE_FLAG_V) == 0)
        {
            count++;
        }
    }

    // east
    nei_x = x + 1;
    nei_y = y;

    if(nei_x <= (MAZE_SIZE_X - 1))
    {
        if((maze[nei_x][nei_y] & MAZE_FLAG_V) == 0)
        {
            count++;
        }
    }

    return count;
}


uint8_t get_visited_neighbors(uint32_t x, uint32_t y)
{
    uint8_t flags = 0;
    int32_t nei_x, nei_y;

    // north
    nei_x = x;
    nei_y = y - 1;

    if(nei_y >= 0)
    {
        if(((maze[nei_x][nei_y] & MAZE_FLAG_V) != 0) &&
           ((maze[nei_x][nei_y] & MAZE_FLAG_B) == 0))
        {
            flags |= MAZE_FLAG_N;
        }
    }

    // south
    nei_x = x;
    nei_y = y + 1;

    if(nei_y <= (MAZE_SIZE_Y - 1))
    {
        if(((maze[nei_x][nei_y] & MAZE_FLAG_V) != 0) &&
           ((maze[nei_x][nei_y] & MAZE_FLAG_B) == 0))
        {
            flags |= MAZE_FLAG_S;
        }
    }

    // west
    nei_x = x - 1;
    nei_y = y;

    if(nei_x >= 0)
    {
        if(((maze[nei_x][nei_y] & MAZE_FLAG_V) != 0) &&
           ((maze[nei_x][nei_y] & MAZE_FLAG_B) == 0))
        {
            flags |= MAZE_FLAG_W;
        }
    }

    // east
    nei_x = x + 1;
    nei_y = y;

    if(nei_x <= (MAZE_SIZE_X - 1))
    {
        if(((maze[nei_x][nei_y] & MAZE_FLAG_V) != 0) &&
           ((maze[nei_x][nei_y] & MAZE_FLAG_B) == 0))
        {
            flags |= MAZE_FLAG_E;
        }
    }

    return flags;
}

uint32_t count_unvisited_cells(void)
{
    uint32_t count = 0;

    for(uint32_t x = 0; x < MAZE_SIZE_X; x++)
    {
        for(uint32_t y = 0; y < MAZE_SIZE_Y; y++)
        {
            if((maze[x][y] & MAZE_FLAG_V) == 0)
            {
                count++;
            }
        }       
    }  

    return count;
}

void reset_maze_grid(void)
{
    for(uint32_t x = 0; x < MAZE_SIZE_X; x++)
    {
        for(uint32_t y = 0; y < MAZE_SIZE_Y; y++)
        {
            maze[x][y] |= MAZE_FLAG_N | MAZE_FLAG_E | MAZE_FLAG_S | MAZE_FLAG_W;    

            if(maze[x][y] & MAZE_FLAG_B)
            {
                // set out of bounds to visited
                maze[x][y] |= MAZE_FLAG_V;
            }
        }       
    }
} 

void clear_visitors(void)
{
    for(uint32_t x = 0; x < MAZE_SIZE_X; x++)
    {
        for(uint32_t y = 0; y < MAZE_SIZE_Y; y++)
        {
            maze[x][y] &= ~MAZE_FLAG_V;
        }       
    }
}

void render_maze(void)
{
    GUI_Clear();

    // draw maze
    GUI_SetPenSize(1);

    GUI_SetColor(0xFFFFFF);
    
    for(uint32_t x = 0; x < MAZE_SIZE_X; x++)
    {
        for(uint32_t y = 0; y < MAZE_SIZE_Y; y++)
        {
            uint32_t x_coord = x * WALL_LEN_X;
            uint32_t y_coord = y * WALL_LEN_Y;


// GUI_SetColor(0x444444);
// // north
// GUI_DrawHLine(y_coord, x_coord, x_coord + WALL_LEN_X);
// // south
// GUI_DrawHLine(y_coord + WALL_LEN_Y, x_coord, x_coord + WALL_LEN_X);
// // west
// GUI_DrawVLine(x_coord, y_coord, y_coord + WALL_LEN_Y);
// // east
// GUI_DrawVLine(x_coord + WALL_LEN_X, y_coord, y_coord + WALL_LEN_Y);


            if(maze[x][y] & MAZE_FLAG_B)
            {
                // GUI_SetColor(0x666600);

                // out of bounds
                // GUI_FillRect(x_coord + 1, y_coord + 1, x_coord + WALL_LEN_X - 1, y_coord + WALL_LEN_Y - 1);
            }
            else if(maze[x][y] & MAZE_FLAG_V)
            {
                GUI_SetColor(0x660066);

                // visited
                GUI_FillRect(x_coord + 1, y_coord + 1, x_coord + WALL_LEN_X - 1, y_coord + WALL_LEN_Y - 1);
                // GUI_DrawCircle(x_coord + WALL_LEN_X / 2, y_coord + WALL_LEN_Y / 2, 4);
            }
            else if((x == end_x) && (y == end_y))
            {
                GUI_SetColor(end_point_color);  
                end_point_color += end_point_delta;

                if((end_point_color < 0x330000) || (end_point_color > 0xcc0000))
                {
                    end_point_delta *= -1;
                }

                GUI_FillRect(x_coord + 4, y_coord + 4, x_coord + WALL_LEN_X - 4, y_coord + WALL_LEN_Y - 4);
            }

            GUI_SetColor(0xCCCCCC);
            
            if(maze[x][y] & MAZE_FLAG_N)
            {
                // north
                GUI_DrawHLine(y_coord, x_coord, x_coord + WALL_LEN_X);
            }

            if(maze[x][y] & MAZE_FLAG_S)
            {
                // south
                GUI_DrawHLine(y_coord + WALL_LEN_Y, x_coord, x_coord + WALL_LEN_X);
            }

            if(maze[x][y] & MAZE_FLAG_W)
            {
                // west
                GUI_DrawVLine(x_coord, y_coord, y_coord + WALL_LEN_Y);
            }

            if(maze[x][y] & MAZE_FLAG_E)
            {
                // east
                GUI_DrawVLine(x_coord + WALL_LEN_X, y_coord, y_coord + WALL_LEN_Y);
            }
        }       
    }    

    GUI_SetColor(0xFFFFFF);
    GUI_DrawRect(0, 0, LCD_GetXSize() - 1, LCD_GetYSize() - 1);
}

void get_maze_starting_point(uint32_t *x, uint32_t *y)
{
    *x = start_x * WALL_LEN_X + WALL_LEN_X / 2;
    *y = start_y * WALL_LEN_Y + WALL_LEN_Y / 2;

    // *y -= 10;

    // *y -= 30;
    // *x -= 5;
    // *x -= 10;
}

void get_maze_end_point(uint32_t *x, uint32_t *y)
{
    *x = end_x * WALL_LEN_X + WALL_LEN_X / 2;
    *y = end_y * WALL_LEN_Y + WALL_LEN_Y / 2;    
}

// Hunt and Kill algorithm
static void hunt_and_kill(void)
{
    reset_maze_grid();
    
    // uint32_t seed = 130; 
    // uint32_t seed = 129; 
    uint32_t seed = rand(); 
    
    uint32_t x;
    uint32_t y;

    // select starting location, avoid out of bounds cells
    do
    {
        x = (uint32_t)pseudo_rand(&seed) % MAZE_SIZE_X;
        y = (uint32_t)pseudo_rand(&seed) % MAZE_SIZE_Y;
    }
    while((maze[x][y] & MAZE_FLAG_B) != 0);

    start_x = x;
    start_y = y;

    // mark current cell as visited
    maze[x][y] |= MAZE_FLAG_V;

    int32_t nei_x, nei_y;
    uint8_t flags;
    uint32_t path_limit = 0;
    uint32_t path_length = 0;

    while(count_unvisited_cells() > 0)
    {
        path_limit = ((uint32_t)pseudo_rand(&seed) % (MAZE_PATH_LIMIT_MAX - MAZE_PATH_LIMIT_MIN)) + MAZE_PATH_LIMIT_MIN;
        path_length = 0;

        while(count_unvisited_neighbors(x, y) > 0)
        {
            do
            {
                // get some entropy
                uint32_t random = pseudo_rand(&seed);
                nei_x = x;
                nei_y = y;

                // select neighbor for random walk
                if(random & 1)
                {
                    if(random & 2)
                    {
                        nei_x = x + 1;
                        flags = MAZE_FLAG_E;
                    }
                    else
                    {
                        nei_x = x - 1;
                        flags = MAZE_FLAG_W;
                    }
                }
                else
                {
                    if(random & 4)
                    {
                        nei_y = y + 1;
                        flags = MAZE_FLAG_S;
                    }
                    else
                    {
                        nei_y = y - 1;
                        flags = MAZE_FLAG_N;
                    }
                }
            }
            while((get_cell_flags(nei_x, nei_y) & MAZE_FLAG_INVALID) ||
                  (get_cell_flags(nei_x, nei_y) & MAZE_FLAG_V));
            
            // GOS_LOG("open %d, %d to %d, %d flags: 0x%x", x, y, nei_x, nei_y, flags);

            // open passage way
            remove_wall(x, y, flags);

            // advance cell
            x = nei_x;
            y = nei_y;

            // mark current cell as visited
            maze[x][y] |= MAZE_FLAG_V;

            path_length++;

            GUI_MULTIBUF_Begin();
            render_maze();

            // render current position
            GUI_SetColor(0xFF0000);
            uint32_t x_coord = x * WALL_LEN_X;
            uint32_t y_coord = y * WALL_LEN_Y;
            GUI_FillRect(x_coord + 1, y_coord + 1, x_coord + WALL_LEN_X - 1, y_coord + WALL_LEN_Y - 1);

            // render starting position
            GUI_SetColor(0x00FF00);
            x_coord = start_x * WALL_LEN_X;
            y_coord = start_y * WALL_LEN_Y;
            GUI_FillRect(x_coord + 1, y_coord + 1, x_coord + WALL_LEN_X - 1, y_coord + WALL_LEN_Y - 1);
            
            GUI_MULTIBUF_End();
            
            gos_rtos_delay_milliseconds(20);
        
            if(path_length >= path_limit)
            {
                break;
            }
        }

        // hunt
        // GOS_LOG("hunt");

        bool target_found = false;

        uint32_t new_x, new_y;
        // start at a random location, avoiding bounds
        do
        {
            new_x = (uint32_t)pseudo_rand(&seed) % MAZE_SIZE_X;
            new_y = (uint32_t)pseudo_rand(&seed) % MAZE_SIZE_Y;
        }
        while((maze[x][y] & MAZE_FLAG_B) != 0);

        // look for unvisited cell that is adjacent to at least one visited cell
        for(uint32_t i = 0; i < (MAZE_SIZE_X * MAZE_SIZE_Y); i++)
        {
            uint8_t visited_neighbors = get_visited_neighbors(new_x, new_y);

            if((new_x != x) && 
               (new_y != y) &&
               ((maze[new_x][new_y] & MAZE_FLAG_V) == 0) &&
               (visited_neighbors > 0))
            {
                x = new_x;
                y = new_y;
                // GOS_LOG("new target %d, %d", x, y);
            
                // mark current cell as visited
                maze[x][y] |= MAZE_FLAG_V;

                // get some entropy
                while(1)
                {
                    uint32_t random = pseudo_rand(&seed);

                    // get visited neighbor (but ensure only one)
                    if((random & 1) && (visited_neighbors & MAZE_FLAG_N))
                    {
                        visited_neighbors = MAZE_FLAG_N;
                        break;
                    }
                    else if((random & 2) && (visited_neighbors & MAZE_FLAG_S))
                    {
                        visited_neighbors = MAZE_FLAG_S;
                        break;
                    }
                    else if((random & 4) && (visited_neighbors & MAZE_FLAG_E))
                    {
                        visited_neighbors = MAZE_FLAG_E;
                        break;
                    }
                    else if((random & 8) && (visited_neighbors & MAZE_FLAG_W))
                    {
                        visited_neighbors = MAZE_FLAG_W;
                        break;
                    }
                }

                remove_wall(x, y, visited_neighbors); 
                // GOS_LOG("opening %d, %d flags 0x%x", x, y, visited_neighbors);

                end_x = x;
                end_y = y;

                target_found = true;

                GUI_MULTIBUF_Begin();
                render_maze();

                // render current position
                GUI_SetColor(0xFF0000);
                uint32_t x_coord = x * WALL_LEN_X;
                uint32_t y_coord = y * WALL_LEN_Y;
                GUI_FillRect(x_coord + 1, y_coord + 1, x_coord + WALL_LEN_X - 1, y_coord + WALL_LEN_Y - 1);

                // render starting position
                GUI_SetColor(0x00FF00);
                x_coord = start_x * WALL_LEN_X;
                y_coord = start_y * WALL_LEN_Y;
                GUI_FillRect(x_coord + 1, y_coord + 1, x_coord + WALL_LEN_X - 1, y_coord + WALL_LEN_Y - 1);
                
                GUI_MULTIBUF_End();
                
                gos_rtos_delay_milliseconds(20);

                // goto target_found_break;
                break;
            }
            
            new_x = (new_x + 1) % MAZE_SIZE_X;
            if((i % MAZE_SIZE_X) == 0)
            {
                new_y = (new_y + 1) % MAZE_SIZE_Y;
            }
        }
// target_found_break:
        if(!target_found)
        {
            break;
        }
    }
}

void generate_maze(void)
{
    // clear the grid
    memset(maze, 0, sizeof(maze));
    
    // maze[4][2] = MAZE_FLAG_E;
    // maze[4][3] = MAZE_FLAG_E;
    // maze[4][4] = MAZE_FLAG_E;
    // maze[4][5] = MAZE_FLAG_E;
    // // maze[4][6] = MAZE_FLAG_E;
    // maze[5][2] = MAZE_FLAG_W;
    // maze[5][3] = MAZE_FLAG_W;
    // maze[5][4] = MAZE_FLAG_W;
    // maze[5][5] = MAZE_FLAG_W;
    // // maze[5][6] = MAZE_FLAG_W;

    // start_x = 5;
    // start_y = 2;

    // set out of bounds area (for game timer)
    maze[0][0] = MAZE_FLAG_B;
    maze[1][0] = MAZE_FLAG_B;
             
    hunt_and_kill();

    clear_visitors();
}

bool is_coord_at_end_point(uint32_t x, uint32_t y)
{
    // get grid cell
    uint32_t grid_x = x / WALL_LEN_X;
    uint32_t grid_y = y / WALL_LEN_Y;

    return (grid_x == end_x) && (grid_y == end_y);
}
