#include "Room.h"

Room::Room() {

}

void Room::remove_client(std::shared_ptr<Client> client){
  auto it = std::find(clients.begin(), clients.end(), client);
  if (it != clients.end()){
    clients.erase(it);
  }
}
