#include "dapplication.h"
#include "loguru.hpp"

// Define the static DApplication pointer
DApplication *DApplication::instance_ = nullptr;

/**
 * @brief DApplication::DApplication
 */
DApplication::DApplication() {
  this->m_ptr_MachineInfo = new MachineInfo;
}

/**
 * @brief DApplication::DApplication
 * @param argc
 * @param argv
 */
DApplication::DApplication(int argc, char *argv[]) {
  (void)argc;
  (void)argv;
  this->m_ptr_MachineInfo = new MachineInfo;
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
  LOG_F(INFO, "System Monitor Daemon Started!! Checking Systems...");

  cout << " BEFORE: this->mptr_MachineInfo->Temperature:" << endl;  
  this->m_ptr_MachineInfo->Temperature();
  cout << " AFTER : this->mptr_MachineInfo->Temperature:" << endl;
  cout << " TempCPU: ";
  cout << this->m_ptr_MachineInfo->system() << endl;
  //printf("TempCPU %.2f C", this->m_ptr_MachineInfo->system());
  
  this->m_ptr_MachineInfo->RAM_MemoryFreeVerify();
  cout << " RAM avaliable: ";
  cout << this->m_ptr_MachineInfo->ram_avaliable() << endl;
  this->m_ptr_MachineInfo->RAM_MemoryTotalVerify();
  cout << " RAM total: ";
  cout << this->m_ptr_MachineInfo->ram_total() << endl;
  
  this->m_ptr_MachineInfo->setFilepath("/");
  this->m_ptr_MachineInfo->FLASH_MemoryFreeVerify();
  cout << " FLASH avaliable: ";
  cout << this->m_ptr_MachineInfo->flash_avaliable() << endl;

  this->m_ptr_MachineInfo->FLASH_MemoryTotalVerify();
  cout << " FLASH total: ";
  cout << this->m_ptr_MachineInfo->flash_total() << endl;

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
