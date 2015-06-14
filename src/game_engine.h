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

#include <vector>
#include <stack>

#define CARD_WIDTH 48
#define CARD_HEIGHT 64

class Player {
public:
    std::stack<short> flinch_pile;
    std::vector<short> hand;
    std::vector< std::stack<short> > reserve_piles;

    std::vector<Rect> hand_pos;
    std::vector<Rect> reserve_pos;
    Rect flinch_pos;
};

class GameEngine {
private:
    std::vector< std::stack<short> > played_piles;
    std::vector< std::stack<short> > discard_piles;
    std::stack<short> draw_pile;

    std::vector<Player> players;
    unsigned active_player;

    void init();
    void drawCards(unsigned player_id);
    void clearCompleted();
    void createDrawPile();

    int canPlay(short value);
    bool canReserve(unsigned player_id, unsigned reserve_id);

    void checkClickHand();
    void checkClickReserve();
    void checkClickFlinch();

    void AIPlayFlinch();
    void AIPlayReserve();
    void AIPlayHand();

    void winnerCheck();

    bool playFlinch(unsigned player_id);
    bool playReserve(unsigned player_id, unsigned reserve_id);
    bool playHand(unsigned player_id, unsigned hand_id);
    bool reserveHand(unsigned player_id, unsigned hand_id, unsigned reserve_id);

    Image img_cards;

    std::vector<Rect> played_pos;

    int hand_selected;
    int winner;

public:
    GameEngine();
    ~GameEngine();
    void logic();
    void render();

    bool done;
};
