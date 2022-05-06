#include <cstddef>
#include <cstdint>
#include <memory>

#pragma once
const size_t FIELD_WIDTH = 4;
const size_t FIELD_SIZE = 16;
const size_t NUMBER_OF_PLAYERS =2;
enum GameAction {
  MOVE_LEFT,
  MOVE_RIGHT,
  MOVE_UP,
  MOVE_DOWN
};

struct Point {
  int32_t y;
  int32_t x;
  Point(size_t x, size_t y);
};

struct GameField {
  uint32_t value_[FIELD_SIZE]= {0}; //values are power of 2

  uint64_t operator()(Point p); //Returns  display value e.g. with value_[y][x] = 5 this function will return 512
  uint32_t& operator[](Point p) ;//directly returns value inside
  const uint32_t& operator[](Point p) const;
};
class Game;
class Player {
public:
  virtual GameAction get_action() = 0;
  virtual uint64_t& player_score() = 0;
  virtual void update_game_state(const Game&) = 0; //FIXME: there is no way to actually read state from Game class.
};

using Winner = std::shared_ptr<Player>;
class Game { //TODO: Maybe forget about oop and make this struct public? this will solve a lot of issues
private:
  GameField tiles_;
  std::shared_ptr<Player> players_[NUMBER_OF_PLAYERS];
  size_t player_to_make_turn_;
  bool make_turn(); //FIXME: probably not smartest idea to put implementaion detail into header file
public:
  Game(std::shared_ptr<Player> p1, std::shared_ptr<Player> p2);
  Winner game_loop();

};
