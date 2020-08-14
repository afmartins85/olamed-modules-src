#include "dapplication.h"
#include "loguru.hpp"

// Define the static DApplication pointer
DApplication *DApplication::instance_ = nullptr;

/**
 * @brief DApplication::DApplication
 */
DApplication::DApplication() { m_ptr_Protocol = new PrinterProtocol; }

/**
 * @brief DApplication::DApplication
 * @param argc
 * @param argv
 */
DApplication::DApplication(int argc, char *argv[]) {
  (void)argc;
  (void)argv;
  m_ptr_Protocol = new PrinterProtocol;
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
  LOG_F(INFO, "Printer Daemon Started!! Checking for priner Device...");
 
  double a, b, c, d;
  a = 100.00;
  b = 98.89;
  c = 88.00;
  d = 23.00;

  this->m_ptr_Protocol->setType(2);
  this->m_ptr_Protocol->setSerial("7558100415344-106-0");
  this->m_ptr_Protocol->setDescription("7558100415344-106-0");
  this->m_ptr_Protocol->setConnected(false);
  this->m_ptr_Protocol->setState("Lexmark Laser, Mono, MX421ADE");
  this->m_ptr_Protocol->setError("no paper");
  this->m_ptr_Protocol->setSupply_type("tonner");   
  this->m_ptr_Protocol->setCyan_level(&a);   
//  this->m_ptr_Protocol->setMagenta_level(&b);   
//  this->m_ptr_Protocol->setYellow_level(&c);   
//  this->m_ptr_Protocol->setBlack_level(&d);   
  
  this->m_ptr_Protocol->prepare_json_object();

  while (1) {
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
