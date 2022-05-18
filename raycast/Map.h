//
// Created by eve on 5/14/22.
//

#ifndef UNTITLED_MAP_H
#define UNTITLED_MAP_H


class Map {
    private:
        int *tiles;
        int width;
        int height;

    public:
        Map(char *filename);
        int get_tile(int height, int width);

};


#endif //UNTITLED_MAP_H
