#include <iostream>
#include <SDL2/SDL.h>
#include <math.h>
#include <assert.h>
#include <inttypes.h>

#include "Texture.h"
#include "Player.h"
#include "Main.h"
#include "Map.h"

double *g_lookup_table;

Texture **init_textures(int *len);
void init_lookup_table();
void draw_map(Map *map, uint32_t **pixel_buffer);
void draw_player(Player *player, Map *map, uint32_t **pixel_buffer);
void draw_rays(Map *map, Player *player, Texture **texture_array, bool minimap_mode, uint32_t **pixel_buffer);

int main(int argc, char *argv[]) {
    if (SDL_Init(SDL_INIT_VIDEO) == 0) {
        SDL_Window *window = nullptr;
        SDL_Renderer *renderer = nullptr;

        auto *pixel_buffer = (uint32_t *) malloc(sizeof(uint32_t) * ((SCREEN_LENGTH * SCREEN_WIDTH) + 1));

        //Used to calculate fps
        double render_speed_rolling_average = 30.0;

        //This table removes need for float division in inner loop
        init_lookup_table();

        //These are the textures rendered on tiles
        auto number_of_textures = (int *) malloc(sizeof(int));
        Texture **texture_array = init_textures(number_of_textures);

        //This is the map we are using
        auto *map = new Map("map1");

        //Player class structure
        auto *player = new Player(map->get_player_start_x(), map->get_player_start_y());

        if (SDL_CreateWindowAndRenderer(SCREEN_WIDTH, SCREEN_LENGTH, 0, &window, &renderer) == 0) {
            SDL_bool done = SDL_FALSE;
            Uint32 ticks_at_last_update = SDL_GetTicks();

            //Minimap only displayed when tab is pressed
            bool minimap_mode = false;

            //Create fullscreen texture that we can render all at once from pixel_buffer
            SDL_Texture *sdl_texture = SDL_CreateTexture( renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, SCREEN_WIDTH, SCREEN_LENGTH);
            SDL_Rect texture_rect;
            texture_rect.x = 0;
            texture_rect.y = 0;
            texture_rect.w = SCREEN_WIDTH;
            texture_rect.h = SCREEN_LENGTH;

            while (!done) {
                SDL_Event event;

                if (SDL_GetTicks() - ticks_at_last_update > 1000.0 / FRAME_CAP) {
                    ticks_at_last_update = SDL_GetTicks();

                    //Draw 2D map if tab is held
                    if (minimap_mode) {
                        draw_map(map, &pixel_buffer);
                        draw_player(player, map, &pixel_buffer);
                    } else {
                        draw_rays(map, player, texture_array, minimap_mode, &pixel_buffer);
                    }

                    SDL_UpdateTexture( sdl_texture , nullptr, pixel_buffer, SCREEN_WIDTH * sizeof (uint32_t));

                    SDL_RenderCopy(renderer, sdl_texture, nullptr, &texture_rect);
                    SDL_RenderPresent(renderer);

                    player->update(map);

                    int speed = SDL_GetTicks() - ticks_at_last_update;
                    render_speed_rolling_average = (.9) * render_speed_rolling_average + (.1) * speed;
                    printf("%f fps\n", 1000.0 / render_speed_rolling_average);
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
        delete player;
        delete map;
        for (int i = 0; i < *number_of_textures; i++) {
            delete *(texture_array + i);
        }
        free(number_of_textures);
        free(texture_array);
        free(g_lookup_table);
    }
    SDL_Quit();
    return 0;
}

void init_lookup_table() {
    g_lookup_table = (double *) malloc(sizeof(double) * (SCREEN_LENGTH >> 1));
    for (int i = 0; i < (SCREEN_LENGTH >> 1); i++) {
        int y = i + (SCREEN_LENGTH >> 1) + 1;
        g_lookup_table[i] = (TILE_WIDTH >> 1) * SCREEN_LENGTH / (double) (2.0 * y - SCREEN_LENGTH);
    }
}

Texture **init_textures(int *len) {
    char *path = NULL;
    size_t max_line_size = 100;

    FILE *fp;
    fp = fopen("textures", "r");
    assert(fp != nullptr);

    //Process number of textures
    fscanf(fp, "%d", len);
    fseek(fp, 1, SEEK_CUR);

    //Create new array
    auto texture_array = (Texture **) malloc(sizeof(Texture*) * (*len));

    //Fill array with each texture listed
    for (int i = 0; i < *len; i++) {
        getline(&path, &max_line_size, fp);

        //Clear out newline character
        for (int j = 0; j < max_line_size; j++) {
            if (*(path + j) == '\n') {
                *(path + j) = '\0';
                break;
            }
        }
        *(texture_array + i) = new Texture(path);
    }
    fclose(fp);
    return texture_array;
}

void draw_map(Map *map, uint32_t **pixel_buffer) {
    int w = SCREEN_WIDTH / map->get_width();
    int l = SCREEN_LENGTH / map->get_height();
    for (int i = 0; i < SCREEN_WIDTH; i++) {
        for (int j = 0; j < SCREEN_LENGTH; j++) {
            int x = i / w;
            int y = j / l;
            (*pixel_buffer)[j*SCREEN_WIDTH + i] = map->get_tile(y, x) ? 0xFFFFFFFF : 0xFF000000;
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
            if (screenx - 5 + j >= SCREEN_WIDTH || screeny - 5 + i >= SCREEN_LENGTH) {
                break;
            }
            (*pixel_buffer)[(screeny - 5 + i) * SCREEN_WIDTH + (screenx - 5 + j)] = 0xFFFF0000;
        }
    }
}

void draw_rays(Map *map, Player *player, Texture **texture_array, bool minimap_mode, uint32_t **pixel_buffer) {
    double x_loc = player->get_x_loc();
    double y_loc = player->get_y_loc();
    double angle = player->get_angle();

    double step = (double) FOV / (double) SCREEN_WIDTH;

    for (int x = 0; x <= SCREEN_WIDTH; x++) {
        double a = (-FOV >> 1) + angle + (x * step);

        double x_factor = cos(a * PI / 180.0);
        double y_factor = -sin(a * PI / 180.0);

        //This is so we don't divide by 0
        if (abs(x_factor) < 0.001) {
            x_factor = 0.001;
        } else if (abs(y_factor) < 0.001) {
            y_factor = 0.001;
        }

        //Find first edge of grid ray touches
        int xdiv1 = floor(x_loc / TILE_WIDTH);
        int ydiv1 = floor(y_loc / TILE_LENGTH);

        double t1 = x_factor > 0 ? (((xdiv1 + 1) * TILE_WIDTH) - x_loc) / x_factor : ((xdiv1 * TILE_WIDTH) - x_loc) / x_factor;
        double t2 = y_factor > 0 ? (((ydiv1 + 1) * TILE_LENGTH) - y_loc) / y_factor : ((ydiv1 * TILE_LENGTH) - y_loc) / y_factor;

        double box_x = x_loc + x_factor * t1;
        double box_y = y_loc + y_factor * t1;

        double box_y_2 = y_loc + y_factor * t2;

        double slope = y_factor / x_factor;
        double b = box_y - (slope * box_x);

        //Search x axis
        double i, j;
        int x_texture_index = 1;
        int c = x_factor < 0 ? -1 : 1;
        for (i = box_x; i < TILE_WIDTH * map->get_width() && i > 0; i += c * TILE_WIDTH) {
            j = (slope * i) + b;
            int xcoord = floor((i + x_factor / 64) / TILE_WIDTH);
            int ycoord = floor((j + y_factor / 64) / TILE_LENGTH);
            if (ycoord < 0 || ycoord >= map->get_height() || xcoord < 0) {
                break;
            }
            if (map->get_tile(ycoord, xcoord)) {
                x_texture_index = map->get_tile(ycoord, xcoord);
                break;
            }
        }
        double xt = (i - x_loc) / x_factor;

        //Search y axis
        int y_texture_index = 1;
        c = y_factor < 0 ? -1 : 1;
        for (j = box_y_2; j < TILE_LENGTH * map->get_height() && j > 0; j += c * TILE_LENGTH) {
            i = (j - b) / slope;
            int xcoord = floor((i + x_factor / 64) / TILE_WIDTH);
            int ycoord = floor((j + y_factor / 64) / TILE_LENGTH);
            if (xcoord < 0 || xcoord >= map->get_width() || ycoord < 0) {
                break;
            }
            if (map->get_tile(ycoord, xcoord)) {
                y_texture_index = map->get_tile(ycoord, xcoord);
                break;
            }
        }
        double yt = (j - y_loc) / y_factor;

        //t will give the time of first intersection after this
        double t = std::min(xt, yt);
        int texture_index = xt <= yt ? x_texture_index : y_texture_index;

        double dx = t * x_factor;
        double dy = t * y_factor;

        double wall_distance = sqrt((dx * dx) + (dy * dy));

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

        int scale_factor = SCREEN_LENGTH * (TILE_WIDTH >> 2);
        double len = 2 * (scale_factor / fisheye_correction_factor);

        int top = 1 + floor((SCREEN_LENGTH - len) / 2);
        int bottom = floor((SCREEN_LENGTH + len) / 2);

        //Set these constraints so only points in screen get rendered
        int y = std::max(top, 0);
        int max = std::min(bottom, SCREEN_LENGTH);

        Texture *texture = *(texture_array + texture_index - 1);
        double recip = 1.0 / len;
        int w = texture->get_width();
        int l = texture->get_length();
        for (;y < max; y++) {
            double texture_length_point = (y - top) * recip;
            uint8_t *color = texture->get_pixel(floor(texture_width_point * w), floor(texture_length_point * l));
            uint8_t red = *(color + 2) >> lighting_constant;
            uint8_t green = *(color + 1) >> lighting_constant;
            uint8_t blue = *(color) >> lighting_constant;
            (*pixel_buffer)[y*SCREEN_WIDTH + (SCREEN_WIDTH - x)] = 0xFF000000 | (red << 16) | (green << 8) | blue;
        }

        //Now render floor
        //A lot of these are computed outside the loop to save on frames by having to do them over and over in inner loop
        double inverse_fisheye = 1.0 / fisheye_correction_factor;
        double tw_recip = 1.0 / TILE_WIDTH;
        double tl_recip = 1.0 / TILE_LENGTH;
        texture = *(texture_array);
        w = texture->get_width();
        l = texture->get_length();
        double dxfish = inverse_fisheye * dx;
        double dyfish = inverse_fisheye * dy;
        for (;y < SCREEN_LENGTH; y++) {
            double current_dist = g_lookup_table[y - 1 - (SCREEN_LENGTH >> 1)];
            double current_floor_x = (current_dist * dxfish) + x_loc;
            double current_floor_y = (current_dist * dyfish) + y_loc;
            current_floor_x -= TILE_WIDTH * ((int) current_floor_x / TILE_WIDTH);
            current_floor_y -= TILE_LENGTH * ((int) current_floor_y / TILE_LENGTH);
            texture_width_point = current_floor_x * tw_recip;
            double texture_length_point = current_floor_y * tl_recip;
            uint8_t *color = texture->get_pixel(texture_width_point * w, texture_length_point * l);
            uint8_t red = *(color + 2);
            uint8_t green = *(color + 1);
            uint8_t blue = *(color);
            uint32_t col32 = 0xFF000000 | (red << 16) | (green << 8) | blue;
            (*pixel_buffer)[y * SCREEN_WIDTH + (SCREEN_WIDTH - x)] = col32;
            (*pixel_buffer)[(SCREEN_LENGTH - y) * SCREEN_WIDTH + (SCREEN_WIDTH - x)] = col32;
        }
    }
}
