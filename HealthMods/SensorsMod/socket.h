#ifndef SOCKET_H
#define SOCKET_H

#include <ctype.h>
#include <iostream>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
//#include <linux/in.h>
#include <arpa/inet.h>
#include <chrono>
#include <sys/socket.h>
#include <thread>
#include <vector>

using namespace std;

class Socket {
 public:
  enum CConnectionState { LostConnection = -9, ForceClose, Timeout, Disconnected, Connected = 0, DataAvailable };

 public:
  Socket();
  inline uint64_t port() { return m_port; }
  inline void setPort(const uint64_t port) { m_port = port; }

  inline char* address() { return m_address; }
  inline void setAddress(char* address) { m_address = address; }

  inline string message() { return m_message; }
  inline void setMessage(string message) { m_message = message; }

  inline CConnectionState ConStatus() const { return m_ConStatus; }
  inline void changeConStatus(CConnectionState status) { m_ConStatus = status; }

  bool clientConnect(void);
  void clientClose(void);
  CConnectionState clientSelect(void);
  int clientSendMessage(void);
  void clientReadMessage(void);
  int Server(void);
  static void* serverProcessRequest(void* ptr);

 private:
  uint64_t m_port;
  char* m_address;
  string m_message;
  CConnectionState m_ConStatus;

  struct sockaddr_in serv_addr;
  int m_sock;

  typedef struct {
    int sock;
    struct sockaddr address;
    socklen_t addr_len;
  } connection_t;
};
#endif  // SOCKET_H
