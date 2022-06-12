#include <array>
#include <cstddef>
#include <cstdint>
#include <memory>

#pragma once
const size_t NUMBER_OF_PLAYERS =2;
const size_t BOARD_WIDTH = 4;
const size_t BOARD_SIZE = 16;

enum GameAction {
  MOVE_LEFT,
  MOVE_RIGHT,
  MOVE_UP,
  MOVE_DOWN,
};



struct GameState;
class Player {
public:
  virtual ~Player() =default;
  virtual GameAction get_action() = 0;
  virtual void read_game_state(const GameState&) = 0;
};



using score_t = uint64_t;
struct GameState {
  public:
  std::string to_string() const;

  std::array<std::array<uint32_t, BOARD_WIDTH>, BOARD_WIDTH> gameField; // power of two
  bool your_turn_now = false;
  score_t your_score = 0;
  score_t enemy_score = 0;
} emptyState;

using player_ptr = std::unique_ptr<Player>;
class Game {
public:
  Game(player_ptr p1, player_ptr p2);
  ~Game();
  void game_loop();
private:
  struct impl;
  std::unique_ptr<impl> pImpl;
};
