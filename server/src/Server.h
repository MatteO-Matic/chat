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
    std::vector<std::unique_ptr<Client>> m_clients;

    void disconnect_client(std::unique_ptr<Client> client);
    ssize_t read_packet(std::unique_ptr<Client> client, Packet packet);
    static void process_client(void *void_client);

  public:
    Server(const int port);
    static Server* getServer();
    void run();
};

#endif
