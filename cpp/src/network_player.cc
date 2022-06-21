#include "gamelogic.h"
#include "network_player.h"
#include "proto_out/game.pb.h"
#include "proto_out/game.grpc.pb.h"
#include <grpcpp/server_context.h>
#include <grpcpp/support/sync_stream.h>




enum grpcNoDataType {
  askForInput
};
// class ChannelReader {

// public:

//   ChannelReader(std::shared_ptr<channel_t> channel): grpc_chan(channel) {
//   }

//   grpc_ClientRequest wait_for_request() {
//     if(request_queue.empty()) {
//       read_msg();
//       return wait_for_request();
//     } else {
//       auto last = *(request_queue.end()--);
//       request_queue.pop_back();
//       return last;
//     }
//   }

//   grpc_ClientResponse wait_for_response(const std::vector<grpc_ClientResponse::ResponseCase>& awaited_events) {

//     const auto is_awaited = [awaited_events](const grpc_ClientResponse& response ) {
//       return std::any_of(awaited_events.begin(), awaited_events.end(),
//                          [response](const auto& c) {
//                            response.response_case()==c;
//                          });
//     };

//     auto response = std::find_if(response_queue.begin(), response_queue.end(), is_awaited);
//     if(response==response_queue.end()) {
//       read_msg();
//       return wait_for_response(awaited_events);
//     }
//     else {
//       auto result = *response;
//       response_queue.erase(response);
//       return result;
//     }

//   }
// private:

//   void read_msg() {
//     grpc_ClientMessage result;
//     grpc_chan->Read(&result);

//     if(result.has_request()) {
//       request_queue.push_back(result.request());
//     }
//     else if(result.has_response()) {
//       response_queue.push_back(result.response());
//     }
//     else {
//       assert(false);
//     }
//   }
// private:
//   std::vector<grpc_ClientRequest> request_queue;
//   std::vector<grpc_ClientResponse> response_queue;
//   std::shared_ptr<channel_t> grpc_chan;
// };




class Stream {
private:
  using response_t = grpc_ClientResponse;
  using request_t = grpc_ServerRequest;
  using stream_ptr = std::shared_ptr<channel_t>;

public:
  Stream(stream_ptr ptr) {
    this->grpc_chan = ptr;
  }
  response_t send_request(request_t request) {
    send_grpc_request(request);
    return wait_for_response();
  }

private:
  void send_grpc_request( grpc_ServerRequest &req) {
    grpc_ServerMessage msg;
    msg.set_allocated_request(&req);
    grpc_chan->Write(msg);
  }

  void handle_client_request(grpc_ClientRequest req) {

  }

  grpc_ClientResponse wait_for_response() {
    grpc_ClientMessage msg;
    grpc_chan->Read(&msg);
    if(msg.has_request()) {
      handle_client_request(msg.request());
    } else if(msg.has_response()) {
      return msg.response();
    } else {
      assert(false);
    }
  }

  stream_ptr grpc_chan;
};


grpc_ServerRequest form_grpc_request(const GameState& state) {
  grpc_GameState grpc_state;
  grpc_UpdateGameState grpc_update_state;
  grpc_ServerRequest grpc_request;

  grpc_state.set_field_width(state.gameField.size());
  grpc_state.set_your_score(state.your_score);
  grpc_state.set_enemy_score(state.enemy_score);

  for(const auto& row : state.gameField) {
    for(const auto value:row) {
      grpc_state.add_tile(value);
    }
  }

  grpc_update_state.set_allocated_newstate(&grpc_state);
  grpc_request.set_allocated_update(&grpc_update_state);
  return grpc_request;
}

grpc_ServerRequest form_grpc_request(grpcNoDataType noDataType) {
  grpc_ServerRequest grpc_req;

  switch(noDataType) {
  case askForInput: {
    grpc_AskForInput ask{};
    grpc_req.set_allocated_ask(&ask);
    break;
  }
  default:
    assert(false);
  }
  return grpc_req;
}


struct GrpcPlayer::impl {
  Stream stream;

  GameAction grpc_deserilize(grpc_GameAction action) {
      return (GameAction) action;
  }
};

GrpcPlayer::GrpcPlayer(std::shared_ptr<channel_t> grpc_channel) {
  pImpl->stream= Stream(grpc_channel);
}



GameAction GrpcPlayer::get_action() {
  auto req = form_grpc_request(askForInput);
  grpc_ClientResponse response = pImpl->stream.send_request(req);
  if(response.has_action()) {
    return pImpl->grpc_deserilize(response.action());
  }
  assert(false);
}



void GrpcPlayer::read_game_state(const GameState & state) {
  auto req = form_grpc_request(state);
  pImpl->stream.send_request(req);
}
