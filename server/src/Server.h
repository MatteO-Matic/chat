#ifndef SERVER_H
#define SERVER_H
#include "Client.h"
#include "Room.h"
#include <memory>
#include <vector>

class Server
{
  private:
    static Server* m_self;
    int m_sock;
    int m_uid_counter;
    int m_roomid_counter;
    std::vector<std::shared_ptr<Client>> m_clients;
    std::vector<std::shared_ptr<Room>> m_rooms;

    typedef struct{
    unsigned int length;
    int type;
    std::string payload;
    } packet_t;

    void create_room(std::shared_ptr<Client> sender, std::string room_name);

    void disconnect_client(std::shared_ptr<Client> client);
    ssize_t read_packet(std::shared_ptr<Client> client, packet_t& packet);
    static void process_client(void *void_client);

    ssize_t sendmessage(std::shared_ptr<Client> receiver, std::string user_message);

  public:
    Server(const int port);
    static Server* getServer();
    void run();
};

#endif
