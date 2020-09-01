#ifndef SOCKET_H
#define SOCKET_H

#include <ctype.h>
#include <iostream>

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

using namespace std;

class Socket {
  public:
    Socket();
    
    inline uint64_t port() { return m_port; }
    inline void setPort(const uint64_t port){ m_port = port; }
    
    inline char * address() { return m_address; }
    inline void setAddress(char * address){ m_address = address; }

    inline string  message() { return m_message; }
    inline void setMessage(string message){ m_message = message; }

    int Client(void);
    void* Process(void* ptr);

  private:
    uint64_t m_port;
    char * m_address;
    string m_message;

    struct sockaddr_in serv_addr;
    char buffer[1024] = {0};
    int sock;
    int valread;

    type struct {
      int sock;
      struct sockaddr address;
      int addr_len;
    }connection_t;

};
#endif  // SOCKET_H
