//
// Created by eve on 5/14/22.
//

#include "Player.h"
#include "Main.h"

#include <math.h>

Player::Player(double x_loc, double y_loc, int angle) {
    set_loc(x_loc, y_loc);
    set_angle(angle);
    turning_left = false;
    turning_right = false;
}

void Player::set_loc(double x_loc, double y_loc) {
    Player::x_loc = x_loc;
    Player::y_loc = y_loc;
}

void Player::set_angle(int angle) {
    Player::angle = angle;
}

void Player::set_turning_left(bool b) {
    Player::turning_left = b;
}

void Player::set_turning_right(bool b) {
    Player::turning_right = b;
}

void Player::set_moving_forward(bool b) {
    Player::moving_forward = b;
}

void Player::set_moving_backward(bool b) {
    Player::moving_backward = b;
}

double Player::get_x_loc() {
    return Player::x_loc;
}

double Player::get_y_loc() {
    return Player::y_loc;
}

int Player::get_angle() {
    return Player::angle;
}

bool Player::get_turning_left() {
    return Player::turning_left;
}

bool Player::get_turning_right() {
    return Player::turning_right;
}

bool Player::get_moving_forward() {
    return Player::moving_forward;
}

bool Player::get_moving_backward() {
    return Player::moving_backward;
}

void Player::update() {
    if (turning_left) {
        angle -= SPEED_SCALE;
    }
    if (turning_right) {
        angle += SPEED_SCALE;
    }
    if (moving_forward || moving_backward) {
        int dir = moving_backward ? -1 : 1;

        double x_factor = dir * SPEED_SCALE * cos(angle * PI / 180.0);
        double y_factor = dir * SPEED_SCALE * sin(angle * PI / 180.0);

        //Calculate new and old coordinates for collision detection
        int x_coord = x_loc / TILE_WIDTH;
        int y_coord = y_loc / TILE_LENGTH;

        int new_x_coord = (x_loc + x_factor) / TILE_WIDTH;
        int new_y_coord = (y_loc + y_factor) / TILE_LENGTH;

        if (new_x_coord < 0 || new_x_coord >= MAP_WIDTH || (x_loc + x_factor) < 0) {
            x_factor = 0;
        }

        if (new_y_coord < 0 || new_y_coord >= MAP_LENGTH || (y_loc + y_factor) < 0) {
            y_factor = 0;
        }

        if (x_coord != new_x_coord && (*(g_map + (y_coord * MAP_WIDTH) + new_x_coord) == 1)) {
            x_factor = 0;
        }

        if (y_coord != new_y_coord && (*(g_map + (new_y_coord * MAP_WIDTH) + x_coord) == 1)) {
            y_factor = 0;
        }

        set_loc(x_loc + x_factor, y_loc + y_factor);
    }
}