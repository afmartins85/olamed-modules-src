#include "socket.h"

/**
 * @brief Socket::Socket
 */
Socket::Socket() {}

int Socket::Client(void) {
  if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
    perror("\nSocket creation error \n");
    return -1;
  }

  serv_addr.sin_family = AF_INET;
  serv_addr.sin_port = htons(m_port);

  if (inet_pton(AF_INET, address(), &serv_addr.sin_addr) <= 0) {
    printf("\nInvalid address/ Address not  supported \n");
    return -1;
  }

  if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
    printf("\nConnection Failed \n");
    return -1;
  }

  cout << "---" << endl;
  cout << "Socket::Client():" << endl;
  cout << "---" << endl;
  cout << "m_port: " << endl;
  cout << m_port << endl;
  cout << "---" << endl;
  cout << "m_address: " << endl;
  cout << m_address << endl;
  cout << "---" << endl;

  send(sock, const_cast<char *>(m_message.c_str()), strlen(const_cast<char *>(m_message.c_str())), 0);
  cout << "Message sent to Server: " << endl;
  cout << m_message << endl;
  cout << "---" << endl;

  valread = read(sock, buffer, 1024);
  cout << "Message received from Server: " << endl;
  printf("%s\n", buffer);

  return 0;
}

// Socket::Server()
