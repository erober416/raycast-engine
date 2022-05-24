//
// Created by eve on 5/14/22.
//

#ifndef UNTITLED_PLAYER_H
#define UNTITLED_PLAYER_H

#define SPEED_SCALE 5
#define ANGLE_SPEED_SCALE 5

#include "Map.h"
#include "Main.h"

class Player {
    private:
        double x_loc;
        double y_loc;
        int angle;

        bool turning_left;
        bool turning_right;
        bool moving_forward;
        bool moving_backward;

    public:
        Player(int x_coord, int y_coord);

        void update(Map *map);

        double get_x_loc();
        double get_y_loc();
        int get_angle();

        bool get_turning_left();
        bool get_turning_right();
        bool get_moving_forward();
        bool get_moving_backward();

        void set_loc(double x_loc, double y_loc);
        void set_angle(int angle);

        void set_turning_left(bool b);
        void set_turning_right(bool b);
        void set_moving_forward(bool b);
        void set_moving_backward(bool b);
};


#endif //UNTITLED_PLAYER_H
