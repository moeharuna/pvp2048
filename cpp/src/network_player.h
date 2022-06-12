#ifndef NETWORK_PLAYER_H_
#define NETWORK_PLAYER_H_
#include "gamelogic.h"
#include <grpcpp/server_context.h>
#include <grpcpp/support/sync_stream.h>
#include "proto_out/game.pb.h"

typedef grpc_ServerMessage server_t;
typedef grpc_ClientMessage client_t;
typedef grpc::ServerReaderWriter<server_t, client_t> channel_t;

class GrpcPlayer final : public Player {
public:
  GrpcPlayer(std::shared_ptr<channel_t> grpc_chan);
  ~GrpcPlayer();
  virtual void read_game_state(const GameState&) override;
  virtual GameAction get_action() override;
  void write_state_to_client();
private:
  struct impl;
  std::unique_ptr<impl> pImpl;
};


#endif // NETWORK_PLAYER_H_
