#include <iostream>
#include <SDL2/SDL.h>
#include <math.h>
#include <assert.h>
#include <inttypes.h>

#include "Texture.h"
#include "Player.h"
#include "Main.h"

int *g_map = NULL;
bool g_mapmode = false;
Texture *g_texture;
Player  *g_player;
uint32_t *g_texture_buffer;

void init_map();
void draw_map(SDL_Renderer **);
void draw_player(SDL_Renderer **);
void draw_rays(SDL_Renderer **);

int main(int argc, char *argv[]) {
    if (SDL_Init(SDL_INIT_VIDEO) == 0) {
        SDL_Window *window = NULL;
        SDL_Renderer *renderer = NULL;

        if (SDL_CreateWindowAndRenderer(SCREEN_WIDTH, SCREEN_LENGTH, 0, &window, &renderer) == 0) {
            SDL_bool done = SDL_FALSE;
            Uint32 ticks_at_last_update = SDL_GetTicks();

            //This is the texture rendered on tiles
            g_texture = new Texture("sample.bmp");

            //Pixels are written to g_texture_buffer
            SDL_Texture *sdl_texture = SDL_CreateTexture( renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, SCREEN_WIDTH, SCREEN_LENGTH);
            g_texture_buffer = (uint32_t *) malloc(sizeof(uint32_t) * ((SCREEN_LENGTH * SCREEN_WIDTH) + 1));
            SDL_Rect texture_rect;
            texture_rect.x = 0;
            texture_rect.y = 0;
            texture_rect.w = SCREEN_WIDTH;
            texture_rect.h = SCREEN_LENGTH;

            g_player = new Player(32, 32, 0);
            init_map();

            while (!done) {
                SDL_Event event;

                if (SDL_GetTicks() - ticks_at_last_update > 1000.0 / FRAME_CAP) {
                    ticks_at_last_update = SDL_GetTicks();
                    //SDL_RenderClear(renderer);

                    //Draw floor and ceiling to buffer
                    uint32_t black = 0xFF000000;
                    uint32_t grey = 0xFF999999;
                    for (int i = 0; i < SCREEN_WIDTH; i++) {
                        for (int j = 0; j < SCREEN_LENGTH; j++) {
                            g_texture_buffer[j*SCREEN_WIDTH + i] = (j > SCREEN_LENGTH >> 1) ? black : grey;
                        }
                    }

                    //TODO rewrite these to work with buffer
                    /*//Draw 2D map if tab is held
                    if (g_mapmode) {
                        draw_map(&renderer);
                        draw_player(&renderer);

                        SDL_SetRenderDrawColor(renderer, 0, 0, 255, SDL_ALPHA_OPAQUE);
                    }*/

                    draw_rays(&renderer);

                    SDL_UpdateTexture( sdl_texture , NULL, g_texture_buffer, SCREEN_WIDTH * sizeof (uint32_t));

                    SDL_RenderCopy(renderer, sdl_texture, NULL, &texture_rect);
                    SDL_RenderPresent(renderer);

                    g_player->update();
                }

                while (SDL_PollEvent(&event)) {
                    switch(event.type) {
                        case SDL_QUIT:
                            done = SDL_TRUE;
                            break;
                        case SDL_KEYUP:
                            switch(event.key.keysym.sym) {
                                case SDLK_LEFT:
                                    g_player->set_turning_left(false);
                                    break;
                                case SDLK_RIGHT:
                                    g_player->set_turning_right(false);
                                    break;
                                case SDLK_UP:
                                    g_player->set_moving_forward(false);
                                    break;
                                case SDLK_DOWN:
                                    g_player->set_moving_backward(false);
                                    break;
                                case SDLK_TAB:
                                    g_mapmode = false;
                                    break;
                                default:
                                    break;
                            }
                            break;
                        case SDL_KEYDOWN:
                            switch(event.key.keysym.sym) {
                                case SDLK_LEFT:
                                    g_player->set_turning_left(true);
                                    break;
                                case SDLK_RIGHT:
                                    g_player->set_turning_right(true);
                                    break;
                                case SDLK_UP:
                                    g_player->set_moving_forward(true);
                                    break;
                                case SDLK_DOWN:
                                    g_player->set_moving_backward(true);
                                    break;
                                case SDLK_TAB:
                                    g_mapmode = true;
                                    break;
                                default:
                                    break;
                            }
                            break;
                    }
                }
            }
        }

        if (renderer) {
            SDL_DestroyRenderer(renderer);
        }
        if (window) {
            SDL_DestroyWindow(window);
        }
    }
    free(g_texture_buffer);
    free(g_map);
    SDL_Quit();
    return 0;
}

void init_map() {
    g_map = (int *) malloc((sizeof(int) * MAP_LENGTH * MAP_WIDTH) + 1);
    int map[16][16] = {
            {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0},
            {1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 0},
            {1, 0, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 0},
            {1, 0, 1, 0, 0, 0, 0, 1, 0, 1, 1, 1, 1, 1, 1, 0},
            {1, 0, 1, 0, 0, 0, 0, 1, 0, 1, 1, 1, 1, 1, 1, 0},
            {1, 0, 1, 0, 0, 1, 0, 1, 0, 1, 1, 1, 1, 1, 1, 0},
            {1, 0, 1, 0, 0, 1, 0, 1, 0, 1, 1, 1, 1, 1, 1, 0},
            {1, 0, 1, 1, 1, 1, 0, 1, 0, 1, 1, 1, 1, 1, 1, 0},
            {1, 0, 0, 0, 0, 0, 0, 1, 0, 1, 1, 1, 1, 1, 1, 0},
            {1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 0},
            {1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 0},
            {1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 0},
            {1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 0},
            {1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 0},
            {1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 0},
            {1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0}
    };

    for (int i = 0; i < MAP_WIDTH; i++) {
        for (int j = 0; j < MAP_LENGTH; j++) {
            *(g_map + (i * MAP_WIDTH) + j) = map[i][j];
        }
    }
}

//TODO rewrite
/*void draw_map(SDL_Renderer **renderer) {
    for (int i = 0; i < MAP_LENGTH; i++) {
        for (int j = 0; j < MAP_WIDTH; j++) {
            if (*(g_map + (i * MAP_LENGTH) + j) == 1) {
                for (int k = 0; k < SCALED_LENGTH; k++) {
                    SDL_RenderDrawLine(*renderer,
                                       j * SCALED_WIDTH,
                                       i * SCALED_LENGTH + k,
                                       (j + 1) * SCALED_WIDTH,
                                       i * SCALED_LENGTH + k);
                }
            }
        }
    }
}*/

//TODO rewrite
/*void draw_player(SDL_Renderer **renderer) {
    for (int i = 0; i < 5; i++) {
        SDL_RenderDrawLine(*renderer,
                           g_player->get_x_loc() - 2,
                           g_player->get_y_loc() - 2 + i,
                           g_player->get_x_loc() + 2,
                           g_player-> get_y_loc() - 2 + i);
    }
}*/

void draw_rays(SDL_Renderer **renderer) {
    double x_loc = g_player->get_x_loc();
    double y_loc = g_player->get_y_loc();
    double angle = g_player->get_angle();

    double step = (double) FOV / (double) SCREEN_WIDTH;

    int x = 0;
    for (double a = (-FOV >> 1) + angle; a < (FOV >> 1) + angle; a = (-FOV >> 1) + angle + (x * step)) {
        double x_factor = cos(a * PI / 180.0);
        double y_factor = sin(a * PI / 180.0);

        //This is done to make sure we don't divide by zero at right angles
        int w = TILE_WIDTH * MAP_WIDTH;
        int l = TILE_LENGTH * MAP_LENGTH;
        double t1 = x_factor == 0 ? -1 : -x_loc / x_factor;
        double t2 = y_factor == 0 ? -1 : -y_loc / y_factor;
        double t3 = x_factor == 0 ? -1 : (w - x_loc) / x_factor;
        double t4 = y_factor == 0 ? -1 : (l - y_loc) / y_factor;
        double t_arr[4] = {t1, t2, t3, t4};

        //Find the point at which ray intersects end of screen
        double t_min = INFINITY;
        for (double i : t_arr) {
            if (i < t_min && i > 0) {
                t_min = i;
            }
        }

        double t;

        //Find first edge of grid ray touches
        int xdiv1 = floor(x_loc / TILE_WIDTH);
        int ydiv1 = floor(y_loc / TILE_LENGTH);

        t_arr[0] = x_factor == 0 ? -1 : ((xdiv1 * TILE_WIDTH) - x_loc) / x_factor;
        t_arr[1] = y_factor == 0 ? -1 : ((ydiv1 * TILE_LENGTH) - y_loc) / y_factor;
        t_arr[2] = x_factor == 0 ? -1 : (((xdiv1 + 1) * TILE_WIDTH) - x_loc) / x_factor;
        t_arr[3] = y_factor == 0 ? -1 : (((ydiv1 + 1) * TILE_LENGTH) - y_loc) / y_factor;

        double t_min2 = INFINITY;
        for (double i : t_arr) {
            if (i < t_min2 && i >= 0) {
                t_min2 = i;
            }
        }

        t = t_min2;

        //TODO: Rewrite dda to see if it can be more precise
        //For both y and x axes check all boxes for first intersection
        double ystep = abs(1.0 / x_factor);
        double xstep = abs(1.0 / y_factor);

        double xt = t;
        double yt = t;

        int xcoord = (x_loc + (t * x_factor)) / TILE_WIDTH;
        int ycoord = (y_loc + (t * y_factor)) / TILE_LENGTH;

        //Check x grid
        int count = 0;
        for (;xt < t_min; xt = t + (count * xstep)) {
            xcoord = floor((x_loc + (xt + .001) * x_factor) / TILE_WIDTH);
            ycoord = floor((y_loc + (xt + .001) * y_factor) / TILE_LENGTH);
            if (*(g_map + (ycoord * MAP_WIDTH) + xcoord) == 1) {
                break;
            }
            count++;
        }

        //Check y grid
        count = 0;
        for (;yt < t_min; yt = t + (count * ystep)) {
            xcoord = floor((x_loc + (yt + .001) * x_factor) / TILE_WIDTH);
            ycoord = floor((y_loc + (yt + .001) * y_factor) / TILE_LENGTH);
            if (*(g_map + (ycoord * MAP_WIDTH) + xcoord) == 1) {
                break;
            }
            count++;
        }

        //t will give the time of first intersection after this
        t = xt <= yt ? xt : yt;

        double dx = t * x_factor;
        double dy = t * y_factor;

        //Fisheye correction factor
        double fisheye_correction_factor = (dx * cos(angle * PI / 180.0)) + (dy * sin(angle * PI / 180.0));
        if (fisheye_correction_factor == 0) { fisheye_correction_factor = 0.0001; }

        //Find out what width of box ray hits for texture mapping
        int ydiv = floor((y_loc + dy) / TILE_LENGTH);
        int xdiv = floor((x_loc + dx) / TILE_WIDTH);

        double y_collision_loc = (y_loc + dy) - (TILE_LENGTH * ydiv);
        double x_collision_loc = (x_loc + dx) - (TILE_WIDTH * xdiv);

        double abx = (TILE_WIDTH >> 1) - abs((TILE_WIDTH >> 1) - x_collision_loc);
        double aby = (TILE_LENGTH >> 1) - abs((TILE_LENGTH >> 1) - y_collision_loc);

        double texture_width_point = abx > aby ?
                (((double) x_collision_loc) / ((double) TILE_WIDTH)) :
                (((double) y_collision_loc) / ((double) TILE_LENGTH));

        //Walls facing one axis will be darker
        int lighting_constant = abx > aby ? 0 : 1;

        //Draw 2D rays vs actual color depending on mode
        if (g_mapmode) {
            //TODO rewrite this to work on buffer
            //SDL_RenderDrawLine(*renderer, x_loc, y_loc, x_loc + t * x_factor, y_loc + t * y_factor);
        } else {
            int scale_factor = SCREEN_LENGTH * 8;
            double len = 2 * (scale_factor / fisheye_correction_factor);

            int w = g_texture->get_width();
            int l = g_texture->get_length();

            int top = 1 + floor((SCREEN_LENGTH - len) / 2);
            int bottom = floor((SCREEN_LENGTH + len) / 2);

            //Set these constraints so only points in screen get rendered
            int y = top > 0 ? top : 0;
            int max = bottom > SCREEN_LENGTH ? SCREEN_LENGTH : bottom;

            for (;y < max; y++) {
                double texture_length_point = (y - top) / len;
                uint8_t *color = g_texture->get_pixel(floor(texture_width_point * w), floor(texture_length_point * l));
                uint8_t red = *(color + 2) >> lighting_constant;
                uint8_t green = *(color + 1) >> lighting_constant;
                uint8_t blue = *(color) >> lighting_constant;
                g_texture_buffer[y*SCREEN_WIDTH + x] = 0xFF000000 | (red << 16) | (blue << 8) | green;
            }
        }
        x++;
    }
}