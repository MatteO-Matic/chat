#ifndef CLIENT_H
#define CLIENT_H
#include "Room.h"
#include <netinet/in.h>
#include <string>
#include <vector>
#include <memory>

class Client : public std::enable_shared_from_this<Client> {
  public:
    Client();
    unsigned int uid;
    int socket;
    struct sockaddr_in addr;
    std::string name;
    std::vector<std::shared_ptr<Room>> rooms;
    void remove_from_all_rooms();
};
#endif
