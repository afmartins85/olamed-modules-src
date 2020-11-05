#include "dapplication.h"
#include "balancesensor.h"
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
DApplication::DApplication() : m_unixsigns(true), m_appMutex(PTHREAD_MUTEX_INITIALIZER) {
  m_ptr_Socket = new Socket;
  this->m_ptr_Socket->setPort(8080);
  //  this->m_ptr_Socket->setAddress((char *)"10.8.0.2");
  this->m_ptr_Socket->setAddress((char *)"127.0.0.1");
  this->m_balProto = new BalanceProtocol;
  this->m_sensors = new BalanceSensor;
}

/**
 * @brief DApplication::DApplication
 * @param argc
 * @param argv
 */
DApplication::DApplication(int argc, char *argv[]) : m_unixsigns(true), m_appMutex(PTHREAD_MUTEX_INITIALIZER) {
  (void)argc;
  (void)argv;
  m_ptr_Socket = new Socket;
  this->m_ptr_Socket->setPort(8080);
  //  this->m_ptr_Socket->setAddress((char *)"10.8.0.2");
  this->m_ptr_Socket->setAddress((char *)"127.0.0.1");
  this->m_balProto = new BalanceProtocol;
  this->m_sensors = new BalanceSensor;
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

/**
 * @brief DApplication::exec
 */
void DApplication::exec() {
  LOG_SCOPE_FUNCTION(INFO);
  LOG_F(INFO, "Balance Daemon Started!!");

  registerUNIX_SIGNS();

  while (unixsigns()) {
    if (m_sensors->isSensorOnline() == true) {
      if (m_sensors->isBalanceReady() == true) {
        m_balProto->prepare_json_balance(m_sensors->getBalance(), m_sensors->getEquipAddress());
        this->m_ptr_Socket->setMessage(const_cast<char *>((this->m_balProto->json_message()).c_str()));
        this->m_ptr_Socket->clientSendMessage();
      }
    } else {
      if (m_balProto->prepare_json_balanceOff() == true) {
        this->m_ptr_Socket->setMessage(const_cast<char *>((this->m_balProto->json_message()).c_str()));
        this->m_ptr_Socket->clientSendMessage();
      }
    }

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
        } else if ((retSel == Socket::ForceClose) || (retSel == Socket::Timeout)) {
          LOG_F(WARNING, "Force Close Connection!!");
          this->m_ptr_Socket->clientClose();
          this->m_ptr_Socket->changeConStatus(Socket::Disconnected);
        }
        break;
    }
  }

  finishSafely();
}

/**
 * @brief DApplication::finishSafely
 */
void DApplication::finishSafely() {
  LOG_F(WARNING, "Securely finalizing the application...");
  m_sensors->shutdown();
  delete m_sensors;
  //  delete m_balProto;
  //  delete m_ptr_Socket;
}
/**
 * @brief DApplication::parseMessageReceive
 * @param message
 */
void DApplication::parseMessageReceive(const char *message) {
  DApplication *app = getInstance();
  LOG_SCOPE_FUNCTION(INFO);
  if (app != nullptr) {
    app->m_balProto->string_parse_json_object(message);
    int type = static_cast<int>(reinterpret_cast<intptr_t>(app->m_balProto->value_json_object("type")));
    switch (type) {
      case 13:
        app->m_ptr_Socket->setMessage("{ \"type\": 13, \"reply\": true }");
        app->m_ptr_Socket->clientSendMessage();
        app->m_ptr_Socket->setMessage("{ \"type\": 6, \"reply\": true }");
        app->m_ptr_Socket->clientSendMessage();
        break;
      default:
        LOG_F(WARNING, "Command (%d) not supported!!", type);
        break;
    }
  } else {
    LOG_F(ERROR, "App context is null!!!");
  }
}

/**
 * @brief DApplication::registerSIG_UNIX
 * @param message
 */
void DApplication::registerUNIX_SIGNS() {
  if ((signal(SIGINT, handlerUNIX_SIGNS) == SIG_ERR) || (signal(SIGHUP, handlerUNIX_SIGNS) == SIG_ERR) ||
      (signal(SIGTERM, handlerUNIX_SIGNS) == SIG_ERR)) {
    LOG_F(INFO, "Error while setting a signal handler\n");
  }
}

/**
 * @brief DApplication::handlerUNIX_SIGNS
 * @param signal
 */
void DApplication::handlerUNIX_SIGNS(int signal) {
  DApplication *app = getInstance();
  LOG_SCOPE_FUNCTION(INFO);
  if (app != nullptr) {
    LOG_F(INFO, "UNIX Signal received: %d", signal);
    app->setUnixsigns(false);
  } else {
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
