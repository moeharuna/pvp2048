#include "gprc_handler.h"
#include "gamelogic.h"
#include "proto_out/game.pb.h"
#include "proto_out/game.grpc.pb.h"
#include <grpcpp/server_context.h>
#include <grpcpp/support/sync_stream.h>

GameAction grpc2server_action(grpc_GameAction action) {
  return (GameAction) action; //I can do that because order of action is the same
                              //but FIXME: i really should replace this with
                              //proper switch
}

typedef grpc_ServerMessage server_t;
typedef grpc_ClientMessage client_t;
typedef grpc::ServerReaderWriter<server_t, client_t> channel_t;

class GrpcImpl final :public grpc_Game::Service {
  public:
  channel_t* grpc_channel() {

  }
  virtual grpc::Status startGameSession(grpc::ServerContext *context,
                                        channel_t* stream) override {
    this->stream = stream;
    return grpc::Status::OK;
  }
  private:
  channel_t *stream;
};
