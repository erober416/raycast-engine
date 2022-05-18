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
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            fscanf(fp, "%d", tiles + (i * height) + j);
            fseek(fp, 1, SEEK_CUR);
        }
    }

    fclose(fp);
}

int Map::get_tile(int h, int w) {
    assert(h < height);
    assert(w < width);
    assert(h >= 0);
    assert(w >= 0);
    return *(tiles + (height * h) + w);
}