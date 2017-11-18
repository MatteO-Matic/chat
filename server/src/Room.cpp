#include "Room.h"
#include "Server.h"

Room::Room() {

}

void Room::remove_client(std::shared_ptr<Client> client){
  auto it = std::find(clients.begin(), clients.end(), client);
  if (it != clients.end()){
    clients.erase(it);
  }
}

//Returns -1 if sender isn't connected to the room
int Room::sendmessage_to_room(std::shared_ptr<Client> sender, std::string message){
  Server* server = Server::getServer();

  //Check if the user is connected to the room
  bool is_connected = false;
  for(auto &c : clients){
    if(sender->uid == c->uid){
      is_connected = true;
    }
  }
  if(!is_connected){
    return -1;
  }

  //Send message to all clients in the room
  for(auto &c : clients){
    if(sender->uid != c->uid){
      server->sendmessage(c,
          name + "#"+
          sender->name + "> " + message);
    }
  }
  return 0;
}
