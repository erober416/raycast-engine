//
// Created by eve on 5/14/22.
//
#include <iostream>
#include <assert.h>

#include "Map.h"
#include "Main.h"

Map::Map(char *filename) {
    // Open file pointer
    FILE *fp;
    fp = fopen(filename, "r");
    assert(fp != NULL);

    //Read width and height
    fscanf(fp, "%d %d\n", &width, &height);

    //Allocate space for map data
    tiles = (int *) malloc(sizeof(int) * ((width * height) + 1));
    assert(tiles != NULL);

    //Store map data
    char c;
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            fscanf(fp, "%c", &c);
            switch(c) {
                case 'P':
                    player_start_x = j;
                    player_start_y = i;
                    *(tiles + (i * width) + j) = 0;
                    break;
                default:
                    *(tiles + (i * width) + j) = c - '0';
                    break;
            }
            fseek(fp, 1, SEEK_CUR);
        }
    }

    fclose(fp);
}

Map::~Map() {
    free(tiles);
}

int Map::get_tile(int h, int w) {
    assert(h <= height);
    assert(w <= width);
    assert(h >= 0);
    assert(w >= 0);
    return *(tiles + (width * h) + w);
}

int Map::get_width() {
    return width;
}

int Map::get_height() {
    return height;
}

int Map::get_player_start_x() {
    return player_start_x;
}

int Map::get_player_start_y() {
    return player_start_y;
}