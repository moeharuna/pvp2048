#include "gamelogic.h"
#include <cstdlib>
#include <vector>
#include <cmath>
#include <cassert>

Point::Point(size_t x, size_t y)  :x(x), y(y) {}

uint32_t& GameField::operator[](const Point p) {
    assert(p.y < FIELD_WIDTH);
    assert(p.x < FIELD_WIDTH);
    return value_[p.y*FIELD_WIDTH + p.x];
}

const uint32_t& GameField::operator[](const Point p) const {
    assert(p.y < FIELD_WIDTH);
    assert(p.x < FIELD_WIDTH);
    return value_[p.y*FIELD_WIDTH + p.x];
}

uint64_t GameField::operator()(const Point p) {
    assert(p.y < FIELD_WIDTH);
    assert(p.x < FIELD_WIDTH);
    return pow(value_[p.y*FIELD_WIDTH + p.x],2);
}

bool is_tile_on_field(const GameField& f, Point where) {
    return
        where.y < FIELD_WIDTH &&
        where.x < FIELD_WIDTH &&
        where.y >= 0 &&
        where.x >= 0;
}

bool is_tile_free(const GameField& f, Point where) {
    return
        is_tile_on_field(f,where) &&
        f[where]==0;
}

void add_tile(GameField &f) {
    std::vector<size_t> free_tiles{};
    free_tiles.reserve(16);
    for(int i=0; i<FIELD_SIZE; ++i) {
        if(f.value_[i]==0)
            free_tiles.push_back(i);
    }
    size_t new_tile_pos = free_tiles[rand() % free_tiles.size()];
    f.value_[new_tile_pos] = 1;
}

bool move_tile(GameField& f, Point from, GameAction direction) {
    auto change = Point(0, 0);
    switch(direction) {
        case MOVE_UP:
            change.y = -1;
            break;
        case MOVE_DOWN:
            change.y = 1;
            break;
        case MOVE_LEFT:
            change.x = -1;
            break;
        case MOVE_RIGHT:
            change.x  = 1;
            break;
    }
    auto most_further_point = from;
    while(is_tile_free(f, most_further_point)) {
        most_further_point.x+=change.x;
        most_further_point.y+=change.y;
    }
    auto is_moved = most_further_point.x != from.x || most_further_point.y != from.y;
    if(is_moved) {
        most_further_point.x-=change.x;
        most_further_point.y-=change.y;

        f[most_further_point] = f[from];
        f[from] = 0;
    }
    return is_moved;
}

bool merge_tile(GameField& f, Point from, GameAction direction) {
    auto tile_to_merge_with = Point(FIELD_WIDTH, FIELD_WIDTH); //Should be invalid by default
    switch(direction) {
        case MOVE_UP:
            tile_to_merge_with = Point(from.x, from.y-1);
            break;
        case MOVE_DOWN:
            tile_to_merge_with = Point(from.x, from.y+1);
            break;
        case MOVE_LEFT:
            tile_to_merge_with = Point(from.x-1, from.y);
            break;
        case MOVE_RIGHT:
            tile_to_merge_with = Point(from.x+1, from.y);
            break;
    }
    auto will_merge = is_tile_on_field(f, tile_to_merge_with) && f[from]==f[tile_to_merge_with];
    if(will_merge) {
        f[tile_to_merge_with]++;
        f[from]=  0;
    }
    return will_merge;
}


bool move_and_merge_tiles(GameField& f, GameAction direction) {
    bool is_state_changed = false;
    for(int y=0; y<FIELD_WIDTH; ++y) {
        for(int x=0; x<FIELD_WIDTH; ++x) {
            is_state_changed = is_state_changed || move_tile(f, Point(x, y), direction);
            is_state_changed = is_state_changed || merge_tile(f, Point(x, y), direction);
        }
    }
    return is_state_changed;
}

Game::Game(std::shared_ptr<Player> p1, std::shared_ptr<Player> p2) {
    srand(time(nullptr));
    this->players_[0] = p1;
    this->players_[1] = p2;
    this->player_to_make_turn_ = rand() % 2;
}

bool Game::make_turn() {
    auto action = this->players_[player_to_make_turn_]->get_action();
    return move_and_merge_tiles(this->tiles_, action);
    //TODO: Increse player score
}

Winner Game::game_loop() {
    for (;;) {
        add_tile(this->tiles_);
        auto action = this->players_[player_to_make_turn_]->get_action();
        while(!make_turn());
        for(const auto &p: this->players_) {
            p->update_game_state(*this);
            p->update_game_state(*this);
        }
        this->player_to_make_turn_ = ++player_to_make_turn_ % 2;
    }
}
