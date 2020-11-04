#ifndef DAPPLICATION_H
#define DAPPLICATION_H

#include "balanceprotocol.h"
#include "socket.h"
#include <healthsensors.h>
#include <iostream>
#include <map>
#include <signal.h>
#include <string>

using namespace std;

#define MAX_TRY_CON 3

class DApplication {
 public:
  static DApplication *getInstance();
  static DApplication *getInstance(int argc, char *argv[]);

  static void parseMessageReceive(const char *);

  static void registerUNIX_SIGNS();

  static void handlerUNIX_SIGNS(int signal);

  inline bool unixsigns() { return m_unixsigns; }
  inline void setUnixsigns(bool unixsigns) { m_unixsigns = unixsigns; }

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

  bool m_unixsigns;
};

#endif
