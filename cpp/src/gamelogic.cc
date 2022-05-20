#include "gamelogic.h"
#include <cstdlib>
#include <string>
#include <vector>
#include <array>
#include <cmath>
#include <cassert>
#include <iostream>
//#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
//#include "include/doctest/doctest.h"

struct Point {
  int32_t y;
  int32_t x;
  Point(size_t x, size_t y);
  Point &operator+=(const Point& p2) {
    this->x+=p2.x;
    this->y+=p2.y;
    return *this;
  }
  Point &operator-=(const Point& p2) {
    this->x-=p2.x;
    this->y-=p2.y;
    return *this;
  }
  friend Point operator+(const Point& p1, const Point& p2) {
    return Point(p1.x+p2.x, p1.y+p2.y);
  }
  friend bool operator==(const Point& p1, const Point& p2) {
    return p1.x == p2.x &&
           p1.y == p2.y;
  }
};

Point direction(GameAction a) {
  switch(a) {
  case MOVE_UP:
    return Point(0, -1);
  case MOVE_DOWN:
    return Point(0, 1);
  case MOVE_LEFT:
    return Point(-1, 0);
  case MOVE_RIGHT:
    return Point(1, 0);
  }
  assert(1!=1);
}

struct GameField {
  uint32_t value_[BOARD_SIZE]= {0}; //values are power of 2

  uint64_t operator()(Point p); //Returns  display value e.g. with value_[y][x] = 5 this function will return 512
  uint32_t& operator[](Point p) ;//directly returns value inside
  const uint32_t& operator[](Point p) const;
};

Point::Point(size_t x, size_t y)  :x(x), y(y) {}

uint32_t& GameField::operator[](const Point p) {
    assert(p.y < BOARD_WIDTH);
    assert(p.x < BOARD_WIDTH);
    return value_[p.y*BOARD_WIDTH + p.x];
}

const uint32_t& GameField::operator[](const Point p) const {
    assert(p.y < BOARD_WIDTH);
    assert(p.x < BOARD_WIDTH);
    return value_[p.y*BOARD_WIDTH + p.x];
}

uint64_t GameField::operator()(const Point p) {
    assert(p.y < BOARD_WIDTH);
    assert(p.x < BOARD_WIDTH);
    if(value_[p.y*BOARD_WIDTH + p.x]==0)
      return 0;
    return pow(2, value_[p.y*BOARD_WIDTH + p.x]);
}

bool is_tile_on_board(const GameField& f, Point where) {
    return
        where.y < BOARD_WIDTH &&
        where.x < BOARD_WIDTH &&
        where.y >= 0 &&
        where.x >= 0;
}

bool is_tile_free(const GameField& f, Point where) {
    return
        is_tile_on_board(f,where) &&
        f[where]==0;
}

void add_tile(GameField &f) {
    std::vector<size_t> free_tiles{};
    free_tiles.reserve(16);
    for(int i=0; i<BOARD_SIZE; ++i) {
        if(f.value_[i]==0)
            free_tiles.push_back(i);
    }
    size_t new_tile_pos = free_tiles[rand() % free_tiles.size()];
    f.value_[new_tile_pos] = 1;
}

bool move_tile(GameField& f, Point from, GameAction action) {
  if(f[from]==0)
    return false;
  const Point change = direction(action);
  Point new_position = from;
  Point test_position = from;
  while(is_tile_free(f, test_position+=change)) {
    new_position = test_position;
  }

  bool is_moved = !(new_position==from); //!= not implemnted
  if(is_moved)
    {
      f[new_position] = f[from];
      f[from] = 0;
    }

  bool will_merge = is_tile_on_board(f, new_position+change) &&
    f[new_position+change] == f[new_position];

  if(will_merge) {
    f[new_position+change]++;
    f[new_position] = 0;
    return true;
  }
  return is_moved;
}


bool move_tiles(GameField& f, GameAction direction) {
  bool is_state_changed = false;
  switch(direction) {
  case MOVE_UP:
    for(int y=0; y<BOARD_WIDTH; ++y) {
      for(int x=0; x<BOARD_WIDTH; ++x) {
         is_state_changed |= move_tile(f, Point(x, y), direction);
      }
    }
    break;
  case MOVE_DOWN:
    for(int y=BOARD_WIDTH-1; y>=0; --y) {
      for(int x=0; x<BOARD_WIDTH; ++x) {
         is_state_changed |= move_tile(f, Point(x, y), direction);
      }
    }
    break;
  case MOVE_LEFT:
    for(int y=0; y<BOARD_WIDTH; ++y) {
      for(int x=0; x<BOARD_WIDTH; ++x) {
        is_state_changed |= move_tile(f, Point(x, y), direction);
      }
    }
    break;
  case MOVE_RIGHT:
    for(int y=0; y<BOARD_WIDTH; ++y) {
      for(int x=BOARD_WIDTH-1; x>=0; --x) {
        is_state_changed |= move_tile(f, Point(x, y), direction);
      }
    }
    break;
  }
  return is_state_changed;
}

struct Game::impl {
  std::array<player_ptr, NUMBER_OF_PLAYERS> players_;
  GameField tiles_;
  size_t player_to_make_turn_;
  bool player_turn() {
    auto action = this->players_[player_to_make_turn_]->get_action();
    return move_tiles(this->tiles_, action);
    //TODO: Increse player score
  }
};



std::string Game::printable_game_board() const {
  std::string board{};
//  board+=std::string(BOARD_WIDTH*2+1, '_');
  board+="\n";
  for(int y=0; y<BOARD_WIDTH; ++y) {
    board+="|";
    for(int x=0; x<BOARD_WIDTH; ++x) {
      auto value = pImpl->tiles_(Point(x, y));
      if(value==0)
        board+=" ";
      else
        board+=std::to_string(value);
      board+="|";
    }

    board+=" "+ std::to_string(y)+ " \n";
  }
//  board+=std::string(BOARD_WIDTH*2+1, '-');
  board+="\n\n\n";
  return board;
}


Winner Game::game_loop() {
  add_tile(pImpl->tiles_);
    for (;;) {
      for(const auto &p: pImpl->players_) {
        p->update_game_state(*this);
      }
      while(!pImpl->player_turn());
      for(const auto &p: pImpl->players_) {
        p->update_game_state(*this);
      }
      add_tile(pImpl->tiles_);
      pImpl->player_to_make_turn_ = ++pImpl->player_to_make_turn_ % 2;
    }
}

Game::Game(player_ptr p1, player_ptr p2) :pImpl{std::make_unique<impl>()} {
    srand(time(nullptr));
    pImpl->players_ = { std::move(p1), std::move(p2)};
    pImpl->player_to_make_turn_ = rand() % 2;
}
Game::~Game() = default;

#define ENABLE_TESTS //TODO move this to cmake or build.sh
#ifdef ENABLE_TESTS

class TestPlayer : public Player {
public:
  TestPlayer(): score(0) {
  }
  ~TestPlayer() override {}
  GameAction get_action() override {
    char action;
    std::cin>>action;
    switch(action) {
    case 'u':
      return MOVE_UP;
      break;
    case 'd':
      return MOVE_DOWN;
      break;
    case 'l':
      return MOVE_LEFT;
      break;
    case 'r':
      return MOVE_RIGHT;
      break;
    default:
      return MOVE_UP;
    }
  }
  score_t& player_score() override{
    return score;
  }
 void update_game_state(const Game& g)  override{
    std::cout<<g.printable_game_board();
  }
private:
  score_t score;
};

class TestPlayer2 : public TestPlayer {
  void update_game_state(const Game& g)  override{

  }
};



// TEST_CASE("Game testing") {
//   //FIXME: Threre should be unit tests

// }
#endif //ENABLE_TESTS
int main(int argc, char ** argv) {
  player_ptr p1 = std::make_unique<TestPlayer>();
  player_ptr p2 = std::make_unique<TestPlayer2>();

  Game g{std::move(p1), std::move(p2)};
  g.game_loop();
}
