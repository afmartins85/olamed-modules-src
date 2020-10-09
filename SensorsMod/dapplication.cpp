#include "dapplication.h"
#include "SpO2Mod/TOTH/tothspo2.h"
#include "healtsensors.h"
#include "loguru.hpp"
#include <chrono>
#include <iostream>
#include <thread>
#include <vector>

// Define the static DApplication pointer
DApplication *DApplication::instance_ = nullptr;

/**
 * @brief DApplication::DApplication
 */
DApplication::DApplication() {
  m_ptr_Socket = new Socket;
  this->m_ptr_Socket->setPort(8080);
  //  this->m_ptr_Socket->setAddress((char *)"10.8.0.2");
  this->m_ptr_Socket->setAddress((char *)"127.0.0.1");
}

/**
 * @brief DApplication::DApplication
 * @param argc
 * @param argv
 */
DApplication::DApplication(int argc, char *argv[]) {
  (void)argc;
  (void)argv;
  m_ptr_Socket = new Socket;
  this->m_ptr_Socket->setPort(8080);
  //  this->m_ptr_Socket->setAddress((char *)"10.8.0.2");
  this->m_ptr_Socket->setAddress((char *)"127.0.0.1");
}

/**
 * @brief DApplication::getInstance
 * @return
 */
DApplication *DApplication::getInstance() {
  if (instance_ == nullptr) {
    instance_ = new DApplication();
  }
  return instance_;
}

/**
 * @brief DApplication::getInstance
 * @param argc
 * @param argv
 * @return
 */
DApplication *DApplication::getInstance(int argc, char *argv[]) {
  if (instance_ == nullptr) {
    instance_ = new DApplication(argc, argv);
  }
  return instance_;
}

void *DApplication::serverListen(void *ptr) {
  pthread_setname_np(pthread_self(), __FUNCTION__);
  Socket *sock = reinterpret_cast<Socket *>(ptr);
  sock->Server();
  pthread_exit(NULL);
}

void DApplication::startServer() {
  pthread_t ptid;
  pthread_create(&ptid, NULL, &serverListen, &m_ptr_Socket);
  pthread_detach(ptid);
}

/**
 * @brief DApplication::exec
 */
void DApplication::exec() {

  HealtSensors *sensors = new TothSpO2;

  sensors->getSpO2();

  LOG_SCOPE_FUNCTION(INFO);
  LOG_F(INFO, "Sensors Daemon Started!!");

  while (true) {

    // Machine states for connection handle
    switch (this->m_ptr_Socket->ConStatus()) {
    case Socket::Disconnected:
      if (this->m_ptr_Socket->clientConnect() == true) {
        this->m_ptr_Socket->changeConStatus(Socket::Connected);
        LOG_F(INFO, "Successfully connected to the Sync Module!!");
      } else {
        // Check system information at 5 seconds
        std::this_thread::sleep_for(std::chrono::seconds(5));
      }
      break;
    case Socket::Connected:
      // Check for sockect received data
      Socket::CConnectionState retSel = this->m_ptr_Socket->clientSelect();
      if (retSel == Socket::DataAvailable) {
        this->m_ptr_Socket->clientReadMessage();
      } else if (retSel == Socket::LostConnection) {
        LOG_F(WARNING, "Lost connection with Sync Module!!");
        this->m_ptr_Socket->changeConStatus(Socket::Disconnected);
      } else if ((retSel == Socket::ForceClose) ||
                 (retSel == Socket::Timeout)) {
        LOG_F(WARNING, "Force Close Connection!!");
        this->m_ptr_Socket->clientClose();
        this->m_ptr_Socket->changeConStatus(Socket::Disconnected);
      }
      break;
    }
  }
}

/**
 * @brief DApplication::parseMessageReceive
 */
void DApplication::parseMessageReceive(const char *message) {
  DApplication *app = getInstance();
  (void)message;
  LOG_SCOPE_FUNCTION(INFO);
  if (app != nullptr) {
    LOG_F(ERROR, "App context is null!!!");
  }
}

/**
 * @brief DApplication::operator =
 * @return
 */
DApplication &DApplication::operator=(const DApplication &d) {
  this->instance_ = d.instance_;
  return *this;
}
