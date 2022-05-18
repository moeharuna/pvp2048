#include <cstddef>
#include <cstdint>
#include <memory>

#pragma once
const size_t BOARD_WIDTH = 4;
const size_t BOARD_SIZE = 16;
const size_t NUMBER_OF_PLAYERS =2;
enum GameAction {
  MOVE_LEFT,
  MOVE_RIGHT,
  MOVE_UP,
  MOVE_DOWN,
};


using score_t = uint64_t;
class Game;
class Player {
public:
  virtual ~Player() =default;
  virtual GameAction get_action() = 0;
  virtual score_t& player_score() = 0;
  virtual void update_game_state(const Game&) = 0; //FIXME: there is no way to actually read state from Game class.
};
using Winner = std::shared_ptr<Player>;
using player_ptr = std::unique_ptr<Player>;

class Game {
public:
  std::string printable_game_board() const; // Mostly for debugging
  Game(player_ptr p1, player_ptr p2);
  ~Game();
  Winner game_loop();
private:
  struct impl;
  std::unique_ptr<impl> pImpl;
};
