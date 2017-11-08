#ifndef SERVER_H
#define SERVER_H
#include <string>
#include <vector>
#include <netinet/in.h>
#include <memory>

class Server
{

  private:
    static Server* m_self;
    int m_sock;
  public:
    Server(const int port);
    static Server* getServer();
    void run();
};

#endif
