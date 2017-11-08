#ifndef PACKET_H
#define PACKET_H
#include <string>

class Packet {
  public:
    Packet();
    unsigned int length;
    int type; //TODO enum this
    std::string payload;
    size_t get_header_size();
};
#endif
