#include "Server.h"
#include "Client.h"
#include "Room.h"
#include <algorithm>
#include <cstring>
#include <iostream>
#include <memory>
#include <netinet/in.h>
#include <sstream>
#include <string>
#include <sys/socket.h>
#include <sys/types.h>
#include <thread>
#include <unistd.h>
#include <vector>

const int MAX_ROOMCHARACTERS = 16;
const int MAX_CLIENTS = 10;
const int DEFAULT_PORT = 3333;
const int MAX_CHARACTERS = 512;
const int MAX_NAMECHARACTERS = 32;

const std::string SERVERNAME = "Server";
const std::string MESSAGE_SEPERATOR = "> ";
const std::string MESSAGE_ROOM_SEPERATOR = "#";

typedef struct{
  unsigned int length;
  int type;
  std::string payload;
} packet_t;

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

  m_uid_counter = 100;
  m_roomid_counter = 100;
}

void Server::run(){
  struct sockaddr_in cli_addr;
  int connfd = 0;
  //Spin to accept clients
  while (true) {
    socklen_t clisize = sizeof(connfd);
    connfd = accept(m_sock, (struct sockaddr*)&cli_addr, &clisize);
    if (connfd < 0) {
      std::cerr << "Error on accepting socket: " << strerror(errno) << std::endl;
      continue;
    }
    std::cout << "Accepted new connection!" << std::endl;

    //Create our client
    Client* new_client = new Client();
    new_client->uid = m_uid_counter;
    m_uid_counter++;
    new_client->addr = cli_addr;
    new_client->socket = connfd;

    std::thread client_thread(process_client, new_client);
    client_thread.detach();
  }
  close(m_sock);
  for(auto &c : m_clients){
    close(c->socket);
  }
}


ssize_t Server::sendmessage(std::shared_ptr<Client> receiver, std::string user_message){
  //TODO sendmessage to offline clients?

  packet_t packet;
  packet.length = user_message.size();
  packet.type = 0;

  ssize_t result = write(receiver->socket, &packet, sizeof(packet) - sizeof(std::string));
  if(result < 0){
    std::cerr << "Error writing header to socket" << strerror(errno) << std::endl;
    disconnect_client(receiver);
    return result;
  }
  result = write(receiver->socket, user_message.c_str(), user_message.size());
  if(result < 0){
    std::cerr << "Error writing to socket" << strerror(errno) << std::endl;
    disconnect_client(receiver);
    return result;
  }
  return result;
}

void Server::process_client(void *void_client){
  auto client = std::shared_ptr<Client>(static_cast<Client*>(void_client));

  //TODO unique?
  Server* server = Server::getServer();
  server->m_clients.push_back(client);
  bool initial = true;

  packet_t packet;
  ssize_t pResult;

  //Spin for messages
  while((pResult = server->read_packet(client, packet)) > 0){
    std::cout << packet.type << std::endl;
    if(pResult == 0){
      server->disconnect_client(client);
      return;
    }

    if(initial){
      //expect first packet to be a namepacket
      if(packet.type != 1){
        server->sendmessage(client, SERVERNAME + MESSAGE_SEPERATOR + "Expecting namepacket as initial");
        continue;
      }
    }

    //TODO msgtype enum
    switch(packet.type){
      case 0: //Normal message to room
        {
          break;
        }
      case 1: //Change name
        {
          client->name = packet.payload;
          initial = false;
          std::cout << "Connected: "<< client->name << std::endl;
          break;
        }
      case 2: //Whisper
        {
          break;
        }
      case 3: //join
        {
          server->create_room(client, packet.payload);
          break;
        }
      default:
        {
          std::cout << "Unknown messagetype: " << packet.type << std::endl;
          break;
        }
    }
  }

  if(pResult < 0){
    std::cerr << std::string("Couldn't read packet: ") + strerror(errno) << std::endl;
    //Close the socket and remove client
    //server->disconnect_client(client);
  }
}
//Create a new room and join it with sender client
void Server::create_room(std::shared_ptr<Client> sender, std::string room_name){
  std::shared_ptr<Room> new_room = std::make_shared<Room>();
  new_room->name = room_name;

  new_room->roomid = m_roomid_counter;

  new_room->clients.push_back(sender);
  sender->rooms.push_back(new_room);
  //TODO lock counter
  m_roomid_counter++;
  m_rooms.push_back(new_room);
  std::cout << "room created: " << new_room->name << std::endl;
}

ssize_t Server::read_packet(std::shared_ptr<Client> client, packet_t& packet){
  //Read in the packet header
  //blocking, just to make sure we don't split the header
  //std::string: payload
  ssize_t hresult = recv(client->socket, &packet, sizeof(packet) - sizeof(std::string), MSG_WAITALL);

  if (hresult <= 0) {
    if(hresult < 0){
      std::cerr << "Failed to read header from socket: " << strerror(errno) << std::endl;
    }
    disconnect_client(client);
    return hresult;
  }
  std::cout << "head: " << packet.length << " : " << packet.type << std::endl;

  //Check max characters
  if(packet.length > MAX_CHARACTERS){
    std::cout << "Incorrect header length" << std::endl;
    disconnect_client(client);
    return -1;
  }

  //Get the payload
  std::string rbuf(packet.length, 0);
  ssize_t presult = 0;

  //read the full packet
  ssize_t datalen = 0;
  while(datalen < packet.length){
    presult = read(client->socket, &rbuf[datalen], rbuf.size() - 1);
    if (presult < 0) {
      std::cerr << "Failed to read data from socket: " << strerror(errno) << std::endl;
      disconnect_client(client);
      return presult;
    }
    datalen += presult;
  }
  packet.payload = rbuf;
  return datalen;
}

void Server::disconnect_client(std::shared_ptr<Client> client){
  close(client->socket);
  //remove client from connected rooms

  auto it = std::find(m_clients.begin(), m_clients.end(), client);
  if (it != m_clients.end()){
    m_clients.erase(it);
  }
  //std::cout << "Disconnected: " << client->name << std::endl;
}
