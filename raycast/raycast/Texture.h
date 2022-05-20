//
// Created by eve on 5/15/22.
//
#include <inttypes.h>

#ifndef UNTITLED_TEXTURE_H
#define UNTITLED_TEXTURE_H


class Texture {
    private:
        uint32_t size;
        uint32_t offset;
        uint32_t width;
        uint32_t length;
        uint8_t *colors;

    public:
        Texture(char *filename);
        ~Texture();
        uint8_t *get_pixel(int x, int y);
        int get_width();
        int get_length();

};


#endif //UNTITLED_TEXTURE_H
