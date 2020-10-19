#include "socket.h"
#include "dapplication.h"
#include "loguru.hpp"
#include <sys/ioctl.h>

/**
 * @brief Socket::Socket
 */
Socket::Socket() : m_ConStatus(Disconnected) {}

/**
 * @brief Socket::clientConnect
 * @return
 */
bool Socket::clientConnect() {
  struct timeval tv;

  tv.tv_sec = 0;
  tv.tv_usec = 10;

  if ((m_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_IP)) < 0) {
    LOG_F(ERROR, "Client socket creation error");
    return false;
  }

  serv_addr.sin_family = AF_INET;
  serv_addr.sin_port = htons(m_port);

  if (inet_pton(AF_INET, address(), &serv_addr.sin_addr) <= 0) {
    LOG_F(ERROR, "Invalid address/ Address not  supported");
    return false;
  }

  if (setsockopt(m_sock, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv)) < 0) {
    LOG_F(ERROR, "Set timeout with sync server error");
    return false;
  }

  if (connect(m_sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
    LOG_F(ERROR, "Connection Failed. SyncMod can be down!!");
    return false;
  }

  std::string handshake("PrinterMod");
  send(m_sock, handshake.c_str(), handshake.size(), 0);
  return true;
}

/**
 * @brief Socket::clientClose
 */
void Socket::clientClose() { close(this->m_sock); }

/**
 * @brief Socket::receivedData
 * @return
 */
Socket::CConnectionState Socket::clientSelect() {
  fd_set rfds;
  struct timeval tv;
  int retval;

  /* Watch stdin (fd 0) to see when it has input. */

  FD_ZERO(&rfds);
  FD_SET(this->m_sock, &rfds);

  /* Wait up to five seconds. */

  tv.tv_sec = 5;
  tv.tv_usec = 0;

  retval = select(this->m_sock + 1, &rfds, NULL, NULL, &tv);
  /* Don't rely on the value of tv now! */

  LOG_F(INFO, "retval %d", retval);
  if (retval == -1) {
    LOG_F(ERROR, "select()");
    return ForceClose;
  } else if (retval) {
    //    LOG_F(INFO, "Data is available now.\n");
    if (FD_ISSET(this->m_sock, &rfds)) {
      int count;
      ioctl(this->m_sock, FIONREAD, &count);
      if (!count) {
        return LostConnection;
      } else {
        LOG_F(INFO, "%d bytes available..", count);
        return DataAvailable;
      }
    } else {
      return Timeout;
    }
  }
  return Connected;
}

/**
 * @brief Socket::clientSendMessage
 * @return
 */
int Socket::clientSendMessage(void) {
  if (this->ConStatus() == Connected) {
    send(m_sock, const_cast<char *>(m_message.c_str()),
         strlen(const_cast<char *>(m_message.c_str())), 0);
  } else {
    return -1;
  }

  return 0;
}

/**
 * @brief Socket::clientReadMessage
 */
void Socket::clientReadMessage() {
  string buffer;
  char ch;
  int braces = 0;

  while (1) {
    /* Read message */
    read(this->m_sock, &ch, 1);
    if (ch == '{') {
      braces++;
    } else if (ch == '}') {
      if (braces > 0) {
        braces--;
      }
    }
    buffer.push_back(ch);
    if (!braces) {
      break;
    }
  }
  DApplication::parseMessageReceive(buffer.c_str());
}

/**
 * @brief Socket::serverProcessRequest
 * @param ptr
 * @return
 */
void *Socket::serverProcessRequest(void *ptr) {
  char *buffer;
  int len = 1024;
  connection_t *conn;
  long addr = 0;

  pthread_setname_np(pthread_self(), __FUNCTION__);

  if (!ptr) {
    pthread_exit(0);
  }
  conn = (connection_t *)ptr;

  if (len > 0) {
    addr = (long)((struct sockaddr_in *)&conn->address)->sin_addr.s_addr;
    buffer = (char *)malloc((len + 1) * sizeof(char));

    /* Read message */
    read(conn->sock, buffer, len);

    free(buffer);
  }

  /* Close socket and clean up */
  close(conn->sock);
  //  free(conn);
  pthread_exit(0);
  return nullptr;
}

/**
 * @brief Socket::Server
 * @return
 */
int Socket::Server() {
  int sock = -1;
  int opt = 1;
  struct sockaddr_in address;
  int port = 9094;
  pthread_t thread;

  /* Create socket */
  sock = socket(AF_INET, SOCK_STREAM, 0);
  if (sock <= 0) {
    LOG_F(ERROR, "error: cannot create socket\n");
    return -3;
  }

  // set master socket to allow multiple connections , this is just a good
  // habit, it will work without this
  if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt,
                 sizeof(opt)) < 0) {
    return -6;
  }

  /* Bind socket to port */
  address.sin_family = AF_INET;
  address.sin_addr.s_addr = INADDR_ANY;
  address.sin_port = htons(port);

  if (bind(sock, (struct sockaddr *)&address, sizeof(struct sockaddr_in)) > 0) {
    LOG_F(ERROR, "error: cannot bind socket to port %d\n", port);
    return -4;
  }

  /* Listen on port */
  if (listen(sock, 5) < 0) {
    LOG_F(ERROR, "error: cannot listen on  port\n");
    return -5;
  }

  LOG_F(INFO, "Server socket is ready and listening on port: %d\n", port);

  while (1) {
    /* Accept incoming connections */
    connection_t *connection = (connection_t *)malloc(sizeof(connection_t));
    LOG_F(INFO, "Check for new connection...");
    connection->sock =
        accept(sock, &connection->address, &connection->addr_len);
    if (connection->sock <= 0) {
      LOG_F(WARNING, "No valid socket!!!");
      free(connection);
    } else {
      connection_t copy_connect;
      memcpy(&copy_connect, connection, sizeof(connection_t));
      /* Start a new thread but do not wait for it */
      pthread_create(&thread, 0, serverProcessRequest, &copy_connect);
      pthread_detach(thread);
      free(connection);
    }
    // Check system information at 0.01 seconds
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
  }
  return 0;
}
