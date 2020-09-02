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
  m_ptr_Socket = new Socket;
  this->m_ptrDevice = new PrinterDevice;
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
  m_ptr_Socket = new Socket;
  this->m_ptrDevice = new PrinterDevice;
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

void* DApplication::func(void* ptr){
  Socket* sock = reinterpret_cast<Socket*>(ptr);
  sock->Server();
  pthread_exit(NULL);
} 
   
void DApplication::shazam(){
  pthread_t ptid;
  pthread_create(&ptid, NULL, &func, &m_ptr_Socket);
  pthread_detach(ptid);
} 

/**
 * @brief DApplication::exec
 */
void DApplication::exec() {
  LOG_SCOPE_FUNCTION(INFO);
  LOG_F(INFO, "Printer Daemon Started!! Checking for priner Device...");

  this->m_ptrDevice->findPrinter();
  // Set type protocol for response status
  this->m_ptr_Protocol->setType(2);

  shazam();

  while (1) {
    // Check system information at 5 seconds
    std::this_thread::sleep_for(std::chrono::seconds(5));

    if (this->m_ptrDevice->ptrIsFound() == true) {
      // Check if printer is ready for print
      if (this->m_ptrDevice->isReadyPrinter() == false) {
        this->m_ptrDevice->isReadyCUPS();
      }

      if (this->m_netSNMP->openSession(*this->m_ptrDevice) == true) {
        LOG_F(INFO, "net-SNMP open session is done!!!");
        this->m_netSNMP->readIodsPrinter(*this->m_ptr_Protocol);
      } else {
        LOG_F(ERROR, "Failure in open session net-SNMP");
      }
    } else {
      LOG_F(WARNING, "No Printer device available!!!");
    }

    //  this->m_ptr_Protocol->setType(2);
    //  this->m_ptr_Protocol->setSerial("7558100415344-106-0");
    //  this->m_ptr_Protocol->setDescription("7558100415344-106-0");
    //  this->m_ptr_Protocol->setConnected(false);
    //  this->m_ptr_Protocol->setState("Lexmark Laser, Mono, MX421ADE");
    //  this->m_ptr_Protocol->setError("no paper");
    //  this->m_ptr_Protocol->setSupply_type("tonner");
    //  this->m_ptr_Protocol->setCyan_level(a);
    //  this->m_ptr_Protocol->setMagenta_level(98.89);
    //  this->m_ptr_Protocol->setYellow_level(88.00);
    //  this->m_ptr_Protocol->setBlack_level(23.00);

    //  //  this->m_ptr_Protocol->setYellow_level(&c);
    //  //  this->m_ptr_Protocol->setBlack_level(&d);

    //  this->m_ptr_Protocol->prepare_json_object();

    //  this->m_ptr_Protocol->prepare_json_object();

    //  this->m_ptr_Socket->setPort(8080);
    //  this->m_ptr_Socket->setAddress((char *)"127.0.0.1");
    //  cout << "DApplication::exec():" << endl;
    //  cout << "((this->m_ptr_Protocol->json_message()).c_str()):" << endl;
    //  cout << ((this->m_ptr_Protocol->json_message()).c_str()) << endl;
    //  this->m_ptr_Socket->setMessage(const_cast<char *>((this->m_ptr_Protocol->json_message()).c_str()));
    //  cout << "this->m_ptr_Socket->message():" << endl;
    //  cout << this->m_ptr_Socket->message() << endl;

    //  this->m_ptr_Socket->Client();
    
    this->m_ptr_Socket->Server();
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
