#include "gamelogic.h"
#include "network_player.h"
#include "proto_out/game.pb.h"
#include "proto_out/game.grpc.pb.h"
#include <grpcpp/server_context.h>
#include <grpcpp/support/sync_stream.h>


enum grpcNoDataType {
  askForInput
};

class ChannelReader {

public:

  ChannelReader(std::shared_ptr<channel_t> channel): grpc_chan(channel) {
  }

  grpc_ClientRequest wait_for_request() {
    if(request_queue.empty()) {
      read_msg();
      return wait_for_request();
    } else {
      auto last = *(request_queue.end()--);
      request_queue.pop_back();
      return last;
    }
  }

  grpc_ClientResponse wait_for_response(const std::vector<grpc_ClientResponse::ResponseCase>& awaited_events) {

    const auto is_awaited = [awaited_events](const grpc_ClientResponse& response ) {
      return std::any_of(awaited_events.begin(), awaited_events.end(),
                         [response](const auto& c) {
                           response.response_case()==c;
                         });
    };

    auto response = std::find_if(response_queue.begin(), response_queue.end(), is_awaited);
    if(response==response_queue.end()) {
      read_msg();
      return wait_for_response(awaited_events);
    }
    else {
      auto result = *response;
      response_queue.erase(response);
      return result;
    }

  }
private:

  void read_msg() {
    grpc_ClientMessage result;
    grpc_chan->Read(&result);

    if(result.has_request()) {
      request_queue.push_back(result.request());
    }
    else if(result.has_response()) {
      response_queue.push_back(result.response());
    }
    else {
      assert(false);
    }
  }
private:
  std::vector<grpc_ClientRequest> request_queue;
  std::vector<grpc_ClientResponse> response_queue;
  std::shared_ptr<channel_t> grpc_chan;
};


class ChannelWriter {
  public:
  ChannelWriter(std::shared_ptr<channel_t> channel):grpc_chan{channel} {
  }
  void send_request(grpc_ServerRequest &req) {
    grpc_ServerMessage msg;
    msg.set_allocated_request(&req);
    grpc_chan->Write(msg);
  }
  void send_response(grpc_ServerResponse &res) {
    grpc_ServerMessage msg;
    msg.set_allocated_response(&res);
    grpc_chan->Write(msg);
  }
  private:
  void send_msg(const grpc_ServerMessage &msg) {

  }
  std::shared_ptr<channel_t> grpc_chan;
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
  ChannelReader reader;
  ChannelWriter writer;

  GameAction grpc_deserilize(grpc_GameAction action) {
      return (GameAction) action;
  }
};

GrpcPlayer::GrpcPlayer(std::shared_ptr<channel_t> grpc_channel) {
  pImpl->reader = ChannelReader(grpc_channel);
  pImpl->writer = ChannelWriter(grpc_channel);
}



GameAction GrpcPlayer::get_action() {
  auto req = form_grpc_request(askForInput);
  pImpl->writer.send_request(req);
  //TODO: Smarter reading
  grpc_ClientResponse response = pImpl->reader.wait_for_response({grpc_ClientResponse::kAction});
  if(response.has_action()) {
    return pImpl->grpc_deserilize(response.action());
  }
  assert(false);
}



void GrpcPlayer::read_game_state(const GameState & state) {
  auto req = form_grpc_request(state);
  pImpl->writer.send_request(req);
}
