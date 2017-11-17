#ifndef ROOM_H
#define ROOM_H
#include <memory>
#include <vector>
#include <algorithm>

class Client;
class Room {
  public:
    Room();
    int roomid;
    std::string name;
    //clients connected to the room
    std::vector<std::shared_ptr<Client>> clients;
    void remove_client(std::shared_ptr<Client> client);
};
#endif //ROOM_H
