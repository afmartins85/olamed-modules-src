#ifndef DAPPLICATION_H
#define DAPPLICATION_H

#include <iostream>
#include "printerprotocol.h"

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
  PrinterProtocol * m_ptr_Protocol;
};

#endif
