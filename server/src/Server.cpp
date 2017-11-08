#include "Server.h"
#include <memory>
#include <algorithm>
#include <sstream>
#include <cstring>
#include <iostream>
#include <string>
#include <sys/socket.h>
#include <sys/types.h>
#include <thread>
#include <unistd.h>
#include <vector>

using namespace std;

const int MAX_ROOMCHARACTERS = 16;
const int MAX_CLIENTS = 10;
const int DEFAULT_PORT = 3333;
const int MAX_CHARACTERS = 512;
const int MAX_NAMECHARACTERS = 32;

Server* Server::m_self = nullptr;
Server* Server::getServer(){
  if(m_self == nullptr)
  {
    m_self = new Server(DEFAULT_PORT);
  }
  return m_self;
}

Server::Server(const int port){
  m_self = this;
  //Socket settings
  m_sock = 0;
  struct sockaddr_in myaddr;
  myaddr.sin_family = AF_INET;
  myaddr.sin_port = htons(port);
  myaddr.sin_addr.s_addr = htonl(INADDR_ANY);

  //Setup socket for listening
  if((m_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0){
    throw std::runtime_error(std::string("Error create socket: ") + strerror(errno));
  }
  std::cout << "Created socket" << std::endl;

  if (bind(m_sock, (struct sockaddr*)&myaddr, sizeof(myaddr)) < 0) {
    throw std::runtime_error(std::string("Error bind socket: ") + strerror(errno));
  }
  std::cout << "Binded socket on " << ":" << port << std::endl;

  if (listen(m_sock, MAX_CLIENTS) < 0) {
    throw std::runtime_error(std::string("Socket listening failed: ") + strerror(errno));
  }
  std::cout << "Begin listening." << std::endl;

  //m_uidcounter = 100;
  //m_roomidcounter = 100;
}

void Server::run(){
  struct sockaddr_in cli_addr;
  int cli_sock = 0;
  //Spin to accept clients
  while (true) {
    socklen_t clisize = sizeof(cli_sock);
    cli_sock = accept(m_sock, (struct sockaddr*)&cli_addr, &clisize);
    if (cli_sock < 0) {
      close(m_sock);
      std::cerr << "Error on accepting socket: " << strerror(errno) << std::endl;
      continue;
    }
    std::cout << "Accepted new connection!" << std::endl;

    //Create our client
    //std::unique_ptr<client_t> cli = std::make_unique<client_t>();
    //cli->uid = m_uidcounter;
    //m_uidcounter++;
    //cli->addr = cli_addr;
    //cli->sock = cli_sock;

    //clients.push_back(std::move(cli));
    //std::thread client_thread(process_client, cli);
    //client_thread.detach();
  }
  //close(m_sock);
  //for(auto &c : clients){
  //  close(c->sock);
  //}
}
