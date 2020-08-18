#ifndef DAPPLICATION_H
#define DAPPLICATION_H

#include "netsnmp.h"
#include "printerprotocol.h"
#include <iostream>

using namespace std;

class DApplication {
 public:
  static DApplication* getInstance();
  static DApplication* getInstance(int argc, char* argv[]);

  void exec();

 protected:
 private:
  static DApplication* instance_;
  DApplication();
  DApplication(int argc, char* argv[]);
  DApplication(const DApplication&);
  DApplication& operator=(const DApplication&);
  PrinterProtocol* m_ptr_Protocol;
  NetSNMP* m_netSNMP;
};

#endif
