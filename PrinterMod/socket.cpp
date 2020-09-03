#include "socket.h"
#include "dapplication.h"
#include "loguru.hpp"

/**
 * @brief Socket::Socket
 */
Socket::Socket() {}

int Socket::Client(void) {
  if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
    LOG_F(ERROR, "\nSocket creation error \n");
    return -1;
  }

  serv_addr.sin_family = AF_INET;
  serv_addr.sin_port = htons(m_port);

  if (inet_pton(AF_INET, address(), &serv_addr.sin_addr) <= 0) {
    LOG_F(ERROR, "\nInvalid address/ Address not  supported \n");
    return -1;
  }

  if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
    LOG_F(ERROR, "\nConnection Failed \n");
    return -1;
  }

  send(sock, const_cast<char *>(m_message.c_str()), strlen(const_cast<char *>(m_message.c_str())), 0);

  valread = read(sock, buffer, 1024);
  return 0;
}

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

    DApplication::parseMessageReceive(buffer);
    free(buffer);
  }

  /* Close socket and clean up */
  close(conn->sock);
  //  free(conn);
  pthread_exit(0);
  return nullptr;
}

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

  // set master socket to allow multiple connections , this is just a good habit, it will work without this
  if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)) < 0) {
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
    connection->sock = accept(sock, &connection->address, &connection->addr_len);
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
