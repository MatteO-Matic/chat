#ifndef CLIENT_H
#define CLIENT_H
#include <netinet/in.h>

class Client {

  public:
    Client();
    unsigned int uid;
    int socket;
    struct sockaddr_in addr;


};
#endif
