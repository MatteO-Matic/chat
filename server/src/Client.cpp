#include "Client.h"

Client::Client() {

}

void Client::remove_from_all_rooms(){
  for(auto& room : rooms){
    room->remove_client(shared_from_this());
  }
}
