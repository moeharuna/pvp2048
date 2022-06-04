#include "gamelogic.h"
#include "grpc_handler.h"


class GrpcPlayer final : public Player{
  public:
  GrpcPlayer() {
  }
  ~GrpcPlayer() {

  }
  virtual void read_game_state(const GameState&) override {

  }
  virtual GameAction get_action() override {

  }
  private:

};
