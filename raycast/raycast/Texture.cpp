//
// Created by eve on 5/15/22.
//
#include <iostream>
#include <assert.h>

#include "Texture.h"

//Creates texture data from bmp file
Texture::Texture(char *filename) {
    // Open file pointer
    FILE *fp;
    fp = fopen(filename, "r");
    assert(fp != NULL);

    //Read size
    fseek(fp, 2, SEEK_CUR);
    fread(&size, sizeof(uint32_t), 1, fp);

    //Read offset
    fseek(fp, 4, SEEK_CUR);
    fread(&offset, sizeof(uint32_t), 1, fp);

    //Read length and width
    fseek(fp, 18, SEEK_SET);
    fread(&width, sizeof(uint32_t), 1, fp);
    fread(&length, sizeof(uint32_t), 1, fp);

    //Move to offset and start read colors
    colors = (uint8_t *) malloc(size - offset + 1);
    fseek(fp, offset, SEEK_SET);
    for (int i = 0; i < size - offset; i++) {
        fread(colors + i, sizeof(uint8_t), 1, fp);
    }

    fclose(fp);
}

Texture::~Texture() {
    free(colors);
}

uint8_t *Texture::get_pixel(int x, int y) {
    if (x >= width) { x = width - 1; }
    if (y >= width) { y = length - 1; }
    return colors + (3 * width * length) - (3 * ((y * width) + x));
}

int Texture::get_length() {
    return length;
}

int Texture::get_width() {
    return width;
}