#ifndef DAPPLICATION_H
#define DAPPLICATION_H

#include "netsnmp.h"
#include "printerdevice.h"
#include "printerprotocol.h"
#include "socket.h"
#include <iostream>
#include <string>

using namespace std;

class DApplication {
 public:
  static DApplication* getInstance();
  static DApplication* getInstance(int argc, char* argv[]);

  void exec();

  static void* func(void* arg);
  void shazam();

 protected:
 private:
  static DApplication* instance_;
  DApplication();
  DApplication(int argc, char* argv[]);
  DApplication(const DApplication&);
  DApplication& operator=(const DApplication&);
  PrinterProtocol* m_ptr_Protocol;
  NetSNMP* m_netSNMP;
  Socket* m_ptr_Socket;
  PrinterDevice* m_ptrDevice;
};

#endif
