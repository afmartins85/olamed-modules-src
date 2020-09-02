#include "dapplication.h"
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
  this->m_ptr_Protocol = new PrinterProtocol;
  this->m_netSNMP = new NetSNMP;
  this->m_ptrDevice = new PrinterDevice;
  m_ptr_Socket = new Socket;
  this->m_ptr_Socket->setPort(8080);
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
  this->m_ptr_Protocol = new PrinterProtocol;
  this->m_netSNMP = new NetSNMP;
  this->m_ptrDevice = new PrinterDevice;
  m_ptr_Socket = new Socket;
  this->m_ptr_Socket->setPort(8080);
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

/**
 * @brief DApplication::exec
 */
void DApplication::exec() {
  int tryComm = MAX_TRY_CON;

  LOG_SCOPE_FUNCTION(INFO);
  LOG_F(INFO, "Printer Daemon Started!! Checking for priner Device...");

  this->m_ptrDevice->findPrinter();
  // Set type protocol for response status
  this->m_ptr_Protocol->setType(2);

  // Check initial printer description from SNMP
  if (this->m_ptrDevice->ptrIsFound() == true) {
    // Set true for printer connected.
    this->m_ptr_Protocol->setConnected(true);
    if (this->m_netSNMP->openSession(*this->m_ptrDevice) == true) {
      LOG_F(INFO, "net-SNMP open session is done!!!");
      this->m_netSNMP->readMIBDescription(*this->m_ptr_Protocol);
    } else {
      LOG_F(ERROR, "Failure in open session net-SNMP");
    }
  }

  while (1) {
    // Check system information at 5 seconds
    std::this_thread::sleep_for(std::chrono::seconds(5));

    if (this->m_ptrDevice->ptrIsFound() == true) {
      // Check if printer is ready for print
      //      if (this->m_ptrDevice->isReadyPrinter() == false) {
      //        this->m_ptrDevice->isReadyCUPS();
      //      }

      if (this->m_netSNMP->openSession(*this->m_ptrDevice) == true) {
        this->m_netSNMP->readMIBDescription(*this->m_ptr_Protocol);
        this->m_netSNMP->readMIBLifeCount(*this->m_ptr_Protocol);
        this->m_netSNMP->readMIBStatus(*this->m_ptr_Protocol);
        this->m_netSNMP->readMIBErrors(*this->m_ptr_Protocol);
        //        this->m_netSNMP->closeSession();
        tryComm = MAX_TRY_CON;  // Reset counter

        //        this->m_ptr_Protocol->prepare_json_object();
        //        this->m_ptr_Protocol->prepare_json_object();
        //        this->m_ptr_Socket->setMessage(const_cast<char *>((this->m_ptr_Protocol->json_message()).c_str()));
        //        this->m_ptr_Socket->Client();
      } else {
        tryComm--;
        LOG_F(ERROR, "Failure in open session net-SNMP");
        if (tryComm <= 0) {
          this->m_ptr_Protocol->setConnected(false);
        }
      }
    } else {
      LOG_F(WARNING, "No Printer device available!!!");
      this->m_ptr_Protocol->setConnected(false);
    }
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
