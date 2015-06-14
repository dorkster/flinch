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


#include "game_engine.h"
#include "engines.h"
#include "utils.h"

#include <algorithm>

GameEngine::GameEngine()
    : done(false)
{
    img_cards.load(render_engine, "data/cards.png");
    init();

    // set physical card positions
    // player 1

    players[0].hand_pos.resize(5);
    for (unsigned i=0; i<5; ++i) {
        players[0].hand_pos[i].x = 16+((CARD_WIDTH+16)*i);
        players[0].hand_pos[i].y = VIEW_H - CARD_HEIGHT - 16;
        players[0].hand_pos[i].w = CARD_WIDTH;
        players[0].hand_pos[i].h = CARD_HEIGHT;
    }
    players[0].reserve_pos.resize(5);
    for (unsigned i=0; i<5; ++i) {
        players[0].reserve_pos[i].x = (VIEW_W/2)-(((CARD_WIDTH+16)*5)/2)+((CARD_WIDTH+16)*i);
        players[0].reserve_pos[i].y = VIEW_H - (CARD_HEIGHT*2) - (16*2);
        players[0].reserve_pos[i].w = CARD_WIDTH;
        players[0].reserve_pos[i].h = CARD_HEIGHT;
    }
    players[0].flinch_pos.x = VIEW_W - CARD_WIDTH - 16;
    players[0].flinch_pos.y = VIEW_H - CARD_HEIGHT - 16;
    players[0].flinch_pos.w = CARD_WIDTH;
    players[0].flinch_pos.h = CARD_HEIGHT;

    // player 2
    players[1].hand_pos.resize(5);
    for (unsigned i=0; i<5; ++i) {
        players[1].hand_pos[i].x = VIEW_W-16-((CARD_WIDTH+16)*5)+((CARD_WIDTH+16)*i);
        players[1].hand_pos[i].y = 16;
        players[1].hand_pos[i].w = CARD_WIDTH;
        players[1].hand_pos[i].h = CARD_HEIGHT;
    }
    players[1].reserve_pos.resize(5);
    for (unsigned i=0; i<5; ++i) {
        players[1].reserve_pos[i].x = (VIEW_W/2)-(((CARD_WIDTH+16)*5)/2)+((CARD_WIDTH+16)*i);
        players[1].reserve_pos[i].y = (CARD_HEIGHT) + (16*2);
        players[1].reserve_pos[i].w = CARD_WIDTH;
        players[1].reserve_pos[i].h = CARD_HEIGHT;
    }
    players[1].flinch_pos.x = 16;
    players[1].flinch_pos.y = 16;
    players[1].flinch_pos.w = CARD_WIDTH;
    players[1].flinch_pos.h = CARD_HEIGHT;

    // played piles
    played_pos.resize(15);
    for (unsigned i=0; i<played_pos.size(); ++i) {
        if (i<8) {
            played_pos[i].x = (VIEW_W/2) + ((CARD_WIDTH+16)*(i-4));
            played_pos[i].y = (VIEW_H/2) - CARD_HEIGHT - 4;
        }
        else {
            played_pos[i].x = (VIEW_W/2) + ((CARD_WIDTH+16)*(i-12));
            played_pos[i].y = (VIEW_H/2) + 4;
        }
        played_pos[i].w = CARD_WIDTH;
        played_pos[i].h = CARD_HEIGHT;
    }
}

GameEngine::~GameEngine() {
}

void GameEngine::init() {
    winner = -1;

    if (players.empty())
        players.resize(2);

    for (unsigned i=0; i<players.size(); ++i) {
        players[i].hand.clear();
        players[i].reserve_piles.clear();
        players[i].reserve_piles.resize(5);
        while (!players[i].flinch_pile.empty()) {
            players[i].flinch_pile.pop();
        }
    }

    active_player = 2;
    hand_selected = -1;

    played_piles.clear();
    played_piles.resize(15);

    // create draw pile
    discard_piles.resize(15);
    for (unsigned i=0; i<15; ++i) {
        while (discard_piles[i].size() < 10)
            discard_piles[i].push(i+1);
    }
    createDrawPile();

    // deal cards
    for (unsigned i=0; i<players.size(); ++i) {
        for (unsigned j=0; j<10; ++j) {
            players[i].flinch_pile.push(draw_pile.top());
            draw_pile.pop();
        }

        drawCards(i);
    }

    // check who goes first
    for (unsigned i=0; i<players.size(); ++i) {
        if (players[i].flinch_pile.top() == 1) {
            active_player = i;
            playFlinch(i);
            break;
        }
    }
    if (active_player == 2) {
        for (unsigned i=0; i<players.size(); ++i) {
            for (unsigned j=0; j<players[i].hand.size(); ++j) {
                if (players[i].hand[j] == 1) {
                    active_player = i;
                    playHand(i, j);
                    break;
                }
            }
            if (active_player != 2)
                break;
        }
    }
    // neither players has a 1
    if (active_player == 2) {
        for (unsigned i=0; i<5; ++i) {
            reserveHand(0, 0, i);
            reserveHand(1, 0, i);
        }
        active_player = 0;
    }

}

void GameEngine::drawCards(unsigned player_id) {
    if (draw_pile.empty())
        createDrawPile();

    while (players[player_id].hand.size() < 5) {
        players[player_id].hand.push_back(draw_pile.top());
        draw_pile.pop();
    }
    std::sort(players[player_id].hand.begin(), players[player_id].hand.end());
}

void GameEngine::checkClickHand() {
    if (hand_selected == -1) {
        // select a card from the hand
        for (unsigned i=0; i<players[0].hand.size(); ++i) {
            if (isWithin(players[0].hand_pos[i], input_engine->mouse)) {
                hand_selected = i;
                return;
            }
        }
    }
    else {
        // deselect the selected card
        if (isWithin(players[0].hand_pos[hand_selected], input_engine->mouse)) {
            hand_selected = -1;
            return;
        }

        // select a differenct card from the hand
        for (unsigned i=0; i<players[0].hand.size(); ++i) {
            if (isWithin(players[0].hand_pos[i], input_engine->mouse)) {
                hand_selected = i;
                return;
            }
        }

        // reserve the selected card on a specific slot
        for (unsigned i=0; i<5; ++i) {
            if (isWithin(players[0].reserve_pos[i], input_engine->mouse)) {
                if (reserveHand(0, hand_selected, i)) {
                    hand_selected = -1;
                    active_player = 1;
                    return;
                }
            }
        }

        // play the selected card
        for (unsigned i=0; i<15; ++i) {
            if (isWithin(played_pos[i], input_engine->mouse)) {
                if (playHand(0, hand_selected)) {
                    hand_selected = -1;
                }
                return;
            }
        }
    }
}

void GameEngine::checkClickReserve() {
    if (hand_selected != -1) return;

    // play the selected card
    for (unsigned i=0; i<5; ++i) {
        if (isWithin(players[0].reserve_pos[i], input_engine->mouse)) {
            playReserve(0, i);
            return;
        }
    }
}

void GameEngine::checkClickFlinch() {
    if (isWithin(players[0].flinch_pos, input_engine->mouse)) {
        hand_selected = -1;

        if (playFlinch(0))
            winnerCheck();
    }
}

bool GameEngine::canReserve(unsigned player_id, unsigned reserve_id) {
    bool have_empty_slots = false;

    for (unsigned i=0; i<5; ++i) {
        if (players[player_id].reserve_piles[i].empty()) {
            have_empty_slots = true;
        }
    }

    if (have_empty_slots && !players[player_id].reserve_piles[reserve_id].empty())
        return false;
    else if (have_empty_slots && players[player_id].reserve_piles[reserve_id].empty())
        return true;
    else if (!have_empty_slots)
        return true;

    return true;
}

int GameEngine::canPlay(short value) {
    for (unsigned i=0; i<15; ++i) {
        if (played_piles[i].empty() && value == 1)
            return (int)i;
        else if (!played_piles[i].empty() && played_piles[i].top()+1 == value)
            return (int)i;
    }
    return -1;
}

void GameEngine::clearCompleted() {
    for (unsigned i=0; i<15; ++i) {
        if (played_piles[i].size() == 15) {
            while (!played_piles[i].empty()) {
                discard_piles[played_piles[i].top()-1].push(played_piles[i].top());
                played_piles[i].pop();
            }
            played_piles.erase(played_piles.begin()+i);
            i--;
        }
    }
    played_piles.resize(15);
}

void GameEngine::createDrawPile() {
    while (!discard_piles.empty()) {
        unsigned i = rand() % discard_piles.size();
        draw_pile.push(discard_piles[i].top());
        discard_piles[i].pop();
        if (discard_piles[i].empty())
            discard_piles.erase(discard_piles.begin()+i);
    }
    discard_piles.resize(15);
}

bool GameEngine::AIPlayFlinch() {
    if (playFlinch(1)) {
        winnerCheck();
        return true;
    }
    return false;
}

bool GameEngine::AIPlayReserve() {
    for (unsigned i=0; i<5; ++i) {
        if (players[1].reserve_piles[i].empty())
            continue;

        if (playReserve(1, i))
            return true;
    }
    return false;
}

void GameEngine::AIPlayHand() {
    for (unsigned i=0; i<players[1].hand.size(); ++i) {
        if (playHand(1, i))
            return;
    }

    for (unsigned i=0; i<players[1].hand.size(); ++i) {
        for (unsigned j=0; j<5; ++j) {
            if (players[1].reserve_piles[j].empty() && reserveHand(1, i, j)) {
                active_player = 0;
                return;
            }
        }
    }

    for (unsigned i=0; i<players[1].hand.size(); ++i) {
        for (unsigned j=0; j<5; ++j) {
            if (reserveHand(1, i, j)) {
                active_player = 0;
                return;
            }
        }
    }
}

bool GameEngine::playFlinch(unsigned player_id) {
    int can_play = canPlay(players[player_id].flinch_pile.top());
    if (can_play != -1) {
        played_piles[can_play].push(players[player_id].flinch_pile.top());
        players[player_id].flinch_pile.pop();
        return true;
    }
    return false;
}

bool GameEngine::playReserve(unsigned player_id, unsigned reserve_id) {
    if (players[player_id].reserve_piles[reserve_id].empty())
        return false;

    int can_play = canPlay(players[player_id].reserve_piles[reserve_id].top());
    if (can_play != -1) {
        played_piles[can_play].push(players[player_id].reserve_piles[reserve_id].top());
        players[player_id].reserve_piles[reserve_id].pop();
        return true;
    }
    return false;
}

bool GameEngine::playHand(unsigned player_id, unsigned hand_id) {
    int can_play = canPlay(players[player_id].hand[hand_id]);
    if (can_play != -1) {
        played_piles[can_play].push(players[player_id].hand[hand_id]);
        players[player_id].hand.erase(players[player_id].hand.begin()+hand_id);
        return true;
    }
    return false;
}

bool GameEngine::reserveHand(unsigned player_id, unsigned hand_id, unsigned reserve_id) {
    if (canReserve(player_id, reserve_id)) {
        players[player_id].reserve_piles[reserve_id].push(players[player_id].hand[hand_id]);
        players[player_id].hand.erase(players[player_id].hand.begin()+hand_id);
        return true;
    }
    return false;
}

void GameEngine::winnerCheck() {
    if (players[active_player].flinch_pile.empty())
        winner = active_player;
}

void GameEngine::logic() {
    // exit game
    if (input_engine->pressing[EXIT]) {
        done = true;
        return;
    }

    // toggle fullscreen
    if (input_engine->pressing[FULLSCREEN_TOGGLE] && !input_engine->lock[FULLSCREEN_TOGGLE]) {
        input_engine->lock[FULLSCREEN_TOGGLE] = true;
        render_engine->toggleFullscreen();
    }

    // game logic goes here

    // game over
    if (winner != -1)
        return;

    if (active_player == 0) {
        if (players[0].hand.empty())
            drawCards(0);

        if (input_engine->pressing_mouse && !input_engine->lock_mouse) {
            input_engine->lock_mouse = true;

            checkClickFlinch();
            checkClickReserve();
            checkClickHand();

            clearCompleted();
        }
    }
    else {
        if (players[1].hand.empty())
            drawCards(1);

        if (!(AIPlayFlinch() || AIPlayReserve()))
            AIPlayHand();

        clearCompleted();
    }
}

void GameEngine::render() {
    for(unsigned j=0; j<players.size(); ++j) {
        // hand
        for (unsigned i=0; i<players[j].hand.size(); ++i) {
            img_cards.setPos(players[j].hand_pos[i].x, players[j].hand_pos[i].y);

            if (j == 0)
                img_cards.setClip(players[j].hand[i]*CARD_WIDTH, 0, CARD_WIDTH, CARD_HEIGHT);
            else
                img_cards.setClip(16*CARD_WIDTH, 0, CARD_WIDTH, CARD_HEIGHT);

            img_cards.render();

            if (j == 0 && i == (unsigned)hand_selected) {
                img_cards.setClip(17*CARD_WIDTH, 0, CARD_WIDTH, CARD_HEIGHT);
                img_cards.render();
            }
        }
        // reserve piles
        for (unsigned i=0; i<5; ++i) {
            img_cards.setPos(players[j].reserve_pos[i].x, players[j].reserve_pos[i].y);

            if (!players[j].reserve_piles[i].empty())
                img_cards.setClip(players[j].reserve_piles[i].top()*CARD_WIDTH, 0, CARD_WIDTH, CARD_HEIGHT);
            else
                img_cards.setClip(0, 0, CARD_WIDTH, CARD_HEIGHT);

            img_cards.render();
        }
        // flinch pile
        if (!players[j].flinch_pile.empty()) {
            img_cards.setPos(players[j].flinch_pos.x, players[j].flinch_pos.y);
            img_cards.setClip(players[j].flinch_pile.top()*CARD_WIDTH, 0, CARD_WIDTH, CARD_HEIGHT);
            img_cards.render();
        }
    }

    // played piles
    for (unsigned i=0; i<played_pos.size(); ++i) {
        img_cards.setPos(played_pos[i].x, played_pos[i].y);

        if (!played_piles[i].empty())
            img_cards.setClip(played_piles[i].top()*CARD_WIDTH, 0, CARD_WIDTH, CARD_HEIGHT);
        else
            img_cards.setClip(0, 0, CARD_WIDTH, CARD_HEIGHT);

        img_cards.render();
    }
}
