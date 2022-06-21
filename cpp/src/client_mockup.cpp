#include "proto_out/game.pb.h"
#include "proto_out/game.grpc.pb.h"
#include <grpcpp/client_context.h>
#include <grpcpp/channel.h>
#include <grpcpp/create_channel.h>

using  stream_t = std::shared_ptr<grpc::ClientReaderWriter<grpc_ClientMessage, grpc_ServerMessage>>;


enum Action {
  LEFT,
  RIGHT,
  UP,
  DOWN
};

grpc_GameAction action2grpc(Action a) {
  return (grpc_GameAction) a;
}

Action char2action(char ch) {
  switch(ch) {
    case 'w':
      return UP;
    case 'a':
      return LEFT;
    case 'd':
      return RIGHT;
    case 's':
      return DOWN;
    default:
      return UP;
    }
}

grpc_ClientMessage form_response() {
  grpc_ClientMessage msg;
  grpc_ClientResponse response;
  grpc_EmptyResponse empty{};
  response.set_allocated_empty(&empty);
  msg.set_allocated_response(&response);
  return msg;
}

grpc_ClientMessage form_response(Action a) {
  grpc_ClientMessage msg;
  grpc_ClientResponse response;
  grpc_GameAction action = action2grpc(a);
  response.set_action(action);
  msg.set_allocated_response(&response);
  return msg;
}


void send_message(stream_t grpc_chan, grpc_ClientMessage &msg) {
  grpc_chan->Write(msg);
}

void handle_request(stream_t grpc_chan, grpc_ServerRequest req) {
  switch(req.request_case()) {
  case grpc_ServerRequest::kAsk: {
    char ch;
    std::cin>>ch;
    auto response = form_response(char2action(ch));
    send_message(grpc_chan, response);
    break;
  }
  case grpc_ServerRequest::kUpdate: {
    auto update = req.update();
    int row_size = update.newstate().tile_size();

    for(int y=0; y<row_size; ++y) {
      for(int x=0; x<row_size; ++x) {
        std::cout<<update.newstate().tile(y*update.newstate().tile_size()+x);
      }
      std::cout<<"\n";
    }

    auto response_to_send = form_response();
    send_message(grpc_chan, response_to_send);
    break;
  }
  default:
    assert(false);
  }
}

stream_t start_game_session() {

  auto channel = grpc::CreateChannel("localhost:66666", grpc::InsecureChannelCredentials());
  //FIXME: Maybe lifetime issue here. Idk if  stub will destruct stream or not
  auto stub = grpc_Game::NewStub(channel);


  grpc::ClientContext context;
  // The actual RPC.
  stream_t stream = stub->startGameSession(&context);
  return stream;
}
int main() {
  stream_t grpc_chan = start_game_session();
  grpc_ServerMessage msg;
  grpc_chan->Read(&msg);
  while(true) {
    if(msg.has_request()) {
      handle_request(grpc_chan, msg.request());
    } else {
      assert(false);
    }
  }
  grpc_chan->Finish();
}
