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

void* Socket::Process(void* ptr) {
  char* buffer;
  int len = 1024;
  connection_t* conn;
  long addr = 0;

  char* hello = "Hello from server C Multithread!!!"; 

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

    /* Send message TODO*/
    send(conn->sock, hello, strlen(hello), 0);

   free(buffer); 
  }

  /* Close socket and clean up */
  close(conn->sock);
  free(conn);
  pthread_exit(0);
}

int Socket::Server() {
  int sock = -1;
  struct sockaddr_in address;
  int port = 9094;
  connection_t * connection;
  pthread_t thread;

  /* Create socket */
  sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
  if(sock <= 0) {
    printf("error: cannot create socket\n");
    return -3;
  }

  /* Bind socket to port */
  address.sin_family = AF_INET;
  address.sin_addr.s_addr = INADDR_ANY;
  address.sin_port = htons(port);

  if(bind(sock, (struct sockaddr *)&address, sizeof(struct sockaddr_in)) > 0) {
    printf("error: cannot bind socket to port %d\n", port);
    return -4;
  }
  
  /* Listen on port */
  if(listen(sock, 5) < 0) {
    printf("error: cannot listen on  port\n");
    return -5;
  }
  
  printf("ready and listening\n");

  while(1) {

   // Check system information at 0.01 seconds
   std::this_thread::sleep_for(std::chrono::milliseconds(10));

    /* Accept incoming connections */
    connection = (connection_t *)malloc(sizeof(connection_t));
    connection->sock = accept(sock, &connection->address,&connection->addr_len);
    if(connection->sock <= 0) {
      free(connection);
    } else {
     /* Start a new thread but do not wait for it */
      pthread_create(&thread, 0, Process, (void*)connection);
      pthread_detach(thread);
    }
  }
  return 0;
}
