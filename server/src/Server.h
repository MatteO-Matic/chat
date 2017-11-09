#ifndef SERVER_H
#define SERVER_H
#include "Client.h"
#include "Packet.h"
#include <memory>
#include <vector>

class Server
{
  private:
    static Server* m_self;
    int m_sock;
    int m_uidcounter;
    std::vector<std::shared_ptr<Client>> m_clients;

    void disconnect_client(std::shared_ptr<Client> client);
    ssize_t read_packet(std::shared_ptr<Client> client, Packet packet);
    static void process_client(void *void_client);

    ssize_t sendmessage(std::shared_ptr<Client> receiver, std::string user_message);

  public:
    Server(const int port);
    static Server* getServer();
    void run();
};

#endif
