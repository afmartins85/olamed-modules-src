#ifndef DAPPLICATION_H
#define DAPPLICATION_H

#include "netsnmp.h"
#include "printerdevice.h"
#include "printerprotocol.h"
#include "socket.h"
#include <iostream>
#include <map>
#include <string>

using namespace std;

#define MAX_TRY_CON 3

class DApplication {
 public:
  static DApplication* getInstance();
  static DApplication* getInstance(int argc, char* argv[]);

  void exec();

  static void parseMessageReceive(const char*);
  static void* printServerListen(void* arg);
  void startServer();
  string fileExtension(string);

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
  int m_lifeCount;
  string m_pathFile;
};

#endif
