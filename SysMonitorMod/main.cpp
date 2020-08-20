#include "dapplication.h"
#include "loguru.hpp"
#include <iostream>

using namespace std;

int main(int argc, char *argv[]) {
  DApplication *daemon;
  loguru::init(argc, argv);

  LOG_F(INFO, "Starting OlaMed System Monitor Daemon ...");
  daemon = DApplication::getInstance(argc, argv);

  daemon->exec();

  return 0;
}
