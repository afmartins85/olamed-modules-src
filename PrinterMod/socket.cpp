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

<<<<<<< HEAD
Socket::Server() {
}


void* Socket::Process(void* ptr) {
  char* buffer;
  int len = 1024;
  connection_t* conn;
  long addr = 0;

  if(!ptr)
    pthread_exit(0);

  conn = (connection_t *)ptr;

  if(len >0) {
    addr = (long)((struct sockaddr_in *)&conn->address)->sin_addr.s_addr;
    buffer = (char *)malloc((len+1)*sizeof(char)); 
    
    /* Read message */ 
    read(conn->sock, buffer, len);
    
    /* Print message */
    printf("%d.%d.%d.%d: %s\n", (int)((addr) & 0xff), (int)((addr >> 8) & 0xff), (int)((addr >> 16) & 0xff), (int)((addr >> 24) & 0xff), buffer);

    /*Send message*/
    //send(conn->sock, hello, strlen(hello), 0);

   free(buffer); 
  }

  /* Close socket and clean up */
  close(conn->sock);
  free(conn);
  pthread_exit(0);

}
=======
// Socket::Server()
>>>>>>> 1ba5ab70edecc8e1ad99bb1e8e8a407f30d2544a
