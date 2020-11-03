#ifndef DAPPLICATION_H
#define DAPPLICATION_H

#include "balanceprotocol.h"
#include "socket.h"
#include <healthsensors.h>
#include <iostream>
#include <map>
#include <string>

using namespace std;

#define MAX_TRY_CON 3

class DApplication {
 public:
  static DApplication *getInstance();
  static DApplication *getInstance(int argc, char *argv[]);

  static void parseMessageReceive(const char *);

  void exec();

 protected:
 private:
  static DApplication *instance_;
  DApplication();
  DApplication(int argc, char *argv[]);
  DApplication(const DApplication &);
  Socket *m_ptr_Socket;
  BalanceProtocol *m_balProto;
  HealthSensors *m_sensors;
  DApplication &operator=(const DApplication &);
};

#endif
