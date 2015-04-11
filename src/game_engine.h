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

#include "animation.h"
#include "common.h"
#include "engines.h"
#include "text.h"
#include "utils.h"

class GameEngine {
public:
    GameEngine();
    ~GameEngine();
    void logic();
    void render();

    bool done;
};
