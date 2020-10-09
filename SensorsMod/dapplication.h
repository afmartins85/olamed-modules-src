#ifndef DAPPLICATION_H
#define DAPPLICATION_H

#include "socket.h"
#include <iostream>
#include <map>
#include <string>

using namespace std;

#define MAX_TRY_CON 3

class DApplication {
public:
  static DApplication *getInstance();
  static DApplication *getInstance(int argc, char *argv[]);

  void exec();

  static void *serverListen(void *arg);
  static void parseMessageReceive(const char *message);

  void startServer();

protected:
private:
  static DApplication *instance_;
  DApplication();
  DApplication(int argc, char *argv[]);
  DApplication(const DApplication &);
  Socket *m_ptr_Socket;

  DApplication &operator=(const DApplication &);
};

#endif
