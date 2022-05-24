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
        int player_start_x;
        int player_start_y;

    public:
        Map(char *filename);
        ~Map();
        int get_tile(int height, int width);
        int get_width();
        int get_height();
        int get_player_start_x();
        int get_player_start_y();

};


#endif //UNTITLED_MAP_H
