#include <iostream>
#include <SDL2/SDL.h>
#include <math.h>
#include <assert.h>
#include <inttypes.h>

#include "Texture.h"
#include "Player.h"
#include "Main.h"
#include "Map.h"

void draw_map(Map *map, uint32_t **pixel_buffer);
void draw_player(Player *player, Map *map, uint32_t **pixel_buffer);
void draw_rays(Map *map, Player *player, Texture *texture, bool minimap_mode, uint32_t **pixel_buffer);

int main(int argc, char *argv[]) {
    if (SDL_Init(SDL_INIT_VIDEO) == 0) {
        SDL_Window *window = nullptr;
        SDL_Renderer *renderer = nullptr;
        auto *pixel_buffer = (uint32_t *) malloc(sizeof(uint32_t) * ((SCREEN_LENGTH * SCREEN_WIDTH) + 1));

        if (SDL_CreateWindowAndRenderer(SCREEN_WIDTH, SCREEN_LENGTH, 0, &window, &renderer) == 0) {
            SDL_bool done = SDL_FALSE;
            Uint32 ticks_at_last_update = SDL_GetTicks();

            //This is the map we are using
            auto *map = new Map("map1");

            //Minimap only displayed when tab is pressed
            bool minimap_mode = false;

            //This is the texture rendered on tiles
            auto *texture = new Texture("sample.bmp");

            //Create fullscreen texture that we can render all at once from pixel_buffer
            SDL_Texture *sdl_texture = SDL_CreateTexture( renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, SCREEN_WIDTH, SCREEN_LENGTH);
            SDL_Rect texture_rect;
            texture_rect.x = 0;
            texture_rect.y = 0;
            texture_rect.w = SCREEN_WIDTH;
            texture_rect.h = SCREEN_LENGTH;

            //Player class structure
            auto *player = new Player(map->get_player_start_x(), map->get_player_start_y());

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
                            pixel_buffer[j*SCREEN_WIDTH + i] = (j > SCREEN_LENGTH >> 1) || minimap_mode ? black : grey;
                        }
                    }

                    //Draw 2D map if tab is held
                    if (minimap_mode) {
                        draw_map(map, &pixel_buffer);
                        draw_player(player, map, &pixel_buffer);
                    }

                    draw_rays(map, player, texture, minimap_mode, &pixel_buffer);

                    SDL_UpdateTexture( sdl_texture , nullptr, pixel_buffer, SCREEN_WIDTH * sizeof (uint32_t));

                    SDL_RenderCopy(renderer, sdl_texture, nullptr, &texture_rect);
                    SDL_RenderPresent(renderer);

                    player->update(map);
                }

                //Hardware controls
                while (SDL_PollEvent(&event)) {
                    switch(event.type) {
                        case SDL_QUIT:
                            done = SDL_TRUE;
                            break;
                        case SDL_KEYUP:
                            switch(event.key.keysym.sym) {
                                case SDLK_LEFT:
                                    player->set_turning_left(false);
                                    break;
                                case SDLK_RIGHT:
                                    player->set_turning_right(false);
                                    break;
                                case SDLK_UP:
                                    player->set_moving_forward(false);
                                    break;
                                case SDLK_DOWN:
                                    player->set_moving_backward(false);
                                    break;
                                case SDLK_TAB:
                                    minimap_mode = false;
                                    break;
                                default:
                                    break;
                            }
                            break;
                        case SDL_KEYDOWN:
                            switch(event.key.keysym.sym) {
                                case SDLK_LEFT:
                                    player->set_turning_left(true);
                                    break;
                                case SDLK_RIGHT:
                                    player->set_turning_right(true);
                                    break;
                                case SDLK_UP:
                                    player->set_moving_forward(true);
                                    break;
                                case SDLK_DOWN:
                                    player->set_moving_backward(true);
                                    break;
                                case SDLK_TAB:
                                    minimap_mode = true;
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
        if (pixel_buffer) {
            free(pixel_buffer);
        }
    }
    SDL_Quit();
    return 0;
}

void draw_map(Map *map, uint32_t **pixel_buffer) {
    int w = SCREEN_WIDTH / map->get_width();
    int l = SCREEN_LENGTH / map->get_height();
    for (int i = 0; i < SCREEN_WIDTH; i++) {
        for (int j = 0; j < SCREEN_LENGTH; j++) {
            int x = i / w;
            int y = j / l;
            if (map->get_tile(y, x)) {
                (*pixel_buffer)[j*SCREEN_WIDTH + i] = 0xFFFFFFFF;
            }
        }
    }
}

void draw_player(Player *player, Map *map, uint32_t **pixel_buffer) {
    double player_x = player->get_x_loc();
    double player_y = player->get_y_loc();
    int w = map->get_width() * TILE_WIDTH;
    int l = map->get_height() * TILE_LENGTH;
    int screenx = floor((player_x / (double) w) * SCREEN_WIDTH);
    int screeny = floor((player_y / (double) l) * SCREEN_LENGTH);
    for (int i = 0; i < 10; i++) {
        for (int j = 0; j < 10; j++) {
            (*pixel_buffer)[(screeny - 5 + i) * SCREEN_WIDTH + (screenx - 5 + j)] = 0xFFFF0000;
        }
    }
}

void draw_rays(Map *map, Player *player, Texture *texture, bool minimap_mode, uint32_t **pixel_buffer) {
    double x_loc = player->get_x_loc();
    double y_loc = player->get_y_loc();
    double angle = player->get_angle();

    double step = (double) FOV / (double) SCREEN_WIDTH;

    for (int x = 0; x <= SCREEN_WIDTH; x++) {
        double a = (-FOV >> 1) + angle + (x * step);

        double x_factor = cos(a * PI / 180.0);
        double y_factor = -sin(a * PI / 180.0);

        while(abs(x_factor) < 0.00001 || abs(y_factor) < 0.00001) {
            //TODO: Fix this loop
            x++;
            a = (-FOV >> 1) + angle + (x * step);
            x_factor = cos(a * PI / 180.0);
            y_factor = -sin(a * PI / 180.0);
        }

        //Find first edge of grid ray touches
        int xdiv1 = floor(x_loc / TILE_WIDTH);
        int ydiv1 = floor(y_loc / TILE_LENGTH);

        double t1 = x_factor > 0 ? (((xdiv1 + 1) * TILE_WIDTH) - x_loc) / x_factor : ((xdiv1 * TILE_WIDTH) - x_loc) / x_factor;
        double t2 = y_factor > 0 ? (((ydiv1 + 1) * TILE_LENGTH) - y_loc) / y_factor : ((ydiv1 * TILE_LENGTH) - y_loc) / y_factor;

        double box_x = x_loc + x_factor * t1;
        double box_y = y_loc + y_factor * t1;

        double box_x_2 = x_loc + x_factor * t2;
        double box_y_2 = y_loc + y_factor * t2;

        double slope = y_factor / x_factor;
        double b = box_y - (slope * box_x);

        //Search x axis
        double i, j;
        int c = x_factor < 0 ? -1 : 1;
        for (i = box_x; i < TILE_WIDTH * map->get_width() && i > 0; i += c * TILE_WIDTH) {
            j = (slope * i) + b;
            int xcoord = floor((i + x_factor / 64) / TILE_WIDTH);
            int ycoord = floor((j + y_factor / 64) / TILE_LENGTH);
            if (ycoord < 0 || ycoord >= map->get_height()) {
                break;
            }
            if (map->get_tile(ycoord, xcoord)) {
                break;
            }
        }
        double xt = (i - x_loc) / x_factor;

        //Search y axis
        c = y_factor < 0 ? -1 : 1;
        for (j = box_y_2; j < TILE_LENGTH * map->get_height() && j > 0; j += c * TILE_LENGTH) {
            i = (j - b) / slope;
            int xcoord = floor((i + x_factor / 64) / TILE_WIDTH);
            int ycoord = floor((j + y_factor / 64) / TILE_LENGTH);
            if (xcoord < 0 || xcoord >= map->get_height()) {
                break;
            }
            if (map->get_tile(ycoord, xcoord)) {
                break;
            }
        }
        double yt = (j - y_loc) / y_factor;

        //t will give the time of first intersection after this
        double t = xt <= yt ? xt : yt;

        double dx = t * x_factor;
        double dy = t * y_factor;

        //Fisheye correction factor
        double fisheye_correction_factor = (dx * cos(angle * PI / 180.0)) - (dy * sin(angle * PI / 180.0));
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
        if (minimap_mode) {
            //TODO rewrite this to work on buffer
            //SDL_RenderDrawLine(*renderer, x_loc, y_loc, x_loc + t * x_factor, y_loc + t * y_factor);
        } else {
            int scale_factor = SCREEN_LENGTH * 8;
            double len = 2 * (scale_factor / fisheye_correction_factor);

            int top = 1 + floor((SCREEN_LENGTH - len) / 2);
            int bottom = floor((SCREEN_LENGTH + len) / 2);

            //Set these constraints so only points in screen get rendered
            int y = top > 0 ? top : 0;
            int max = bottom > SCREEN_LENGTH ? SCREEN_LENGTH : bottom;

            for (;y < max; y++) {
                double texture_length_point = (y - top) / len;
                uint8_t *color = texture->get_pixel(floor(texture_width_point * texture->get_width()), floor(texture_length_point * texture->get_length()));
                uint8_t red = *(color + 2) >> lighting_constant;
                uint8_t green = *(color + 1) >> lighting_constant;
                uint8_t blue = *(color) >> lighting_constant;
                (*pixel_buffer)[y*SCREEN_WIDTH + (SCREEN_WIDTH - x)] = 0xFF000000 | (red << 16) | (blue << 8) | green;
            }
        }
    }
}