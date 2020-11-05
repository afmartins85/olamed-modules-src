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

  inline bool unixsigns() {
    bool b;
    pthread_mutex_lock(&m_appMutex);
    b = m_unixsigns;
    pthread_mutex_unlock(&m_appMutex);
    return b;
  }

  inline void setUnixsigns(bool unixsigns) {
    pthread_mutex_lock(&m_appMutex);
    m_unixsigns = unixsigns;
    pthread_mutex_unlock(&m_appMutex);
  }

  void exec();
  void finishSafely();

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
  pthread_mutex_t m_appMutex;
};

#endif
