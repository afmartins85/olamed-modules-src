#include "dapplication.h"
#include "Sensor/TOTH/tothmonitor.h"
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
  this->m_sensProto = new SensorProtocol;
  m_sensors = new TothMonitor;
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
  this->m_sensProto = new SensorProtocol;
  m_sensors = new TothMonitor;
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
  LOG_F(INFO, "Sensors Daemon Started!!");

  while (true) {

    if (m_sensors->isEquipAddressReady()) {
      this->m_sensProto->setAddr(m_sensors->getEquipAddress());
    }
    if (m_sensors->isSpo2Ready()) {
      double spo2 = m_sensors->getSpO2();
      this->m_sensProto->setConnected(true);
      this->m_sensProto->DateTime();
      this->m_sensProto->setDate(this->m_sensProto->datetime());
      this->m_sensProto->setSpo2(spo2);
      this->m_sensProto->prepare_json_oximetry();
      this->m_ptr_Socket->setMessage(
          const_cast<char *>((this->m_sensProto->json_message()).c_str()));
      this->m_ptr_Socket->clientSendMessage();
    } else if (m_sensors->isTempReady()) {
      int temp = m_sensors->getTemp();
      this->m_sensProto->setConnected(true);
      this->m_sensProto->DateTime();
      this->m_sensProto->setDate(this->m_sensProto->datetime());
      this->m_sensProto->setTemp(temp);
      this->m_sensProto->prepare_json_BodyTemperature();
      this->m_ptr_Socket->setMessage(
          const_cast<char *>((this->m_sensProto->json_message()).c_str()));
      this->m_ptr_Socket->clientSendMessage();
    } else if (m_sensors->isBlooPressReady()) {
      this->m_sensProto->setConnected(true);
      this->m_sensProto->DateTime();
      this->m_sensProto->setDate(this->m_sensProto->datetime());
      this->m_sensProto->setPressBloodSys(m_sensors->getPressBloodSys());
      this->m_sensProto->setPressBloodDia(m_sensors->getPressBloodDia());
      this->m_sensProto->setPressBloodMean(m_sensors->getPressBloodMean());
      this->m_sensProto->prepare_json_BloodPressure();
      this->m_ptr_Socket->setMessage(
          const_cast<char *>((this->m_sensProto->json_message()).c_str()));
      this->m_ptr_Socket->clientSendMessage();
      printf("MESAAGE SENT TO SERVER !!!!!!!!");
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
 * @param message
 */
void DApplication::parseMessageReceive(const char *message) {
  DApplication *app = getInstance();
  LOG_SCOPE_FUNCTION(INFO);
  if (app != nullptr) {
    app->m_sensProto->string_parse_json_object(message);
    int type = static_cast<int>(reinterpret_cast<intptr_t>(
        app->m_sensProto->value_json_object("type")));
    bool reply = static_cast<bool>(reinterpret_cast<intptr_t>(
        app->m_sensProto->value_json_object("reply")));
    switch (type) {
    case 7:
    case 8:
    case 9: {
      if (reply == true) {
        LOG_F(INFO, "Reply %s for register type %d!!",
              (reply ? "true" : "false"), type);
        // TODO: sinaliza para o proximo registro
        app->m_sensors->isNextRegister(true);
      }
    } break;
    case 13:
      app->m_ptr_Socket->setMessage("{ \"type\": 13, \"reply\": true }");
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
 * @brief DApplication::operator =
 * @return
 */
DApplication &DApplication::operator=(const DApplication &d) {
  this->instance_ = d.instance_;
  return *this;
}
