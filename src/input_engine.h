/*
Copyright © 2014-2015 Justin Jacobs

This file is part of GAME_TITLE.

GAME_TITLE is free software: you can redistribute it and/or modify it under the terms
of the GNU General Public License as published by the Free Software Foundation,
either version 3 of the License, or (at your option) any later version.

GAME_TITLE is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
PARTICULAR PURPOSE.  See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with
GAME_TITLE.  If not, see http://www.gnu.org/licenses/
*/


#pragma once

#include "common.h"
#include "utils.h"

const int KEY_COUNT = 7;

enum {
    ACTION = 0,
    EXIT = 1,
    LEFT = 2,
    RIGHT = 3,
    UP = 4,
    DOWN = 5,
    FULLSCREEN_TOGGLE = 6,
};

class InputEngine {
public:
    InputEngine();
    ~InputEngine();
    void logic();

    bool done;

    SDL_Keycode binding[KEY_COUNT];
    bool pressing[KEY_COUNT];
    bool lock[KEY_COUNT];

    bool pressing_mouse;
    bool lock_mouse;
    Point mouse;
};
