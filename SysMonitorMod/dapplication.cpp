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
  this->m_ptr_MachineInfo = new MachineInfo;
  this->m_ptr_MachineProtocol = new MachineProtocol;
  this->m_ptr_Socket = new Socket;
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
  this->m_ptr_MachineProtocol = new MachineProtocol;
  this->m_ptr_Socket = new Socket;
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
  std::vector<MachineInfo::CPUData> entries1;
  std::vector<MachineInfo::CPUData> entries2;

  LOG_SCOPE_FUNCTION(INFO);
  LOG_F(INFO, "System Monitor Daemon Started!! Checking Systems...");

  while (1) {
    // Check system information at 5 seconds
    std::this_thread::sleep_for(std::chrono::seconds(5));

    this->m_ptr_MachineInfo->CPUTemperature();
    this->m_ptr_MachineInfo->CPUFanSpeed();
    this->m_ptr_MachineInfo->RAM_MemoryFreeVerify();
    this->m_ptr_MachineInfo->RAM_MemoryTotalVerify();
    this->m_ptr_MachineInfo->FLASH_MemoryFreeVerify();
    this->m_ptr_MachineInfo->FLASH_MemoryTotalVerify();

    // snapshot 1
    this->m_ptr_MachineInfo->ReadStatsCPU(entries1);

    // 100ms pause
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    // snapshot 2
    this->m_ptr_MachineInfo->ReadStatsCPU(entries2);

    // print output
    this->m_ptr_MachineInfo->PrintStats(entries1, entries2);

    this->m_ptr_MachineInfo->DateTime();

    this->m_ptr_MachineInfo->JournalErrors();

    this->m_ptr_MachineInfo->GracefulShutdown();

    this->m_ptr_MachineProtocol->setType(3);
    this->m_ptr_MachineProtocol->setTotal_disk(this->m_ptr_MachineInfo->flash_avaliable());
    this->m_ptr_MachineProtocol->setFree_disk(this->m_ptr_MachineInfo->flash_total());
    this->m_ptr_MachineProtocol->setTotal_ram(this->m_ptr_MachineInfo->ram_avaliable());
    this->m_ptr_MachineProtocol->setFree_ram(this->m_ptr_MachineInfo->ram_total());
    this->m_ptr_MachineProtocol->setFan_speed(this->m_ptr_MachineInfo->fanSpeed());
    this->m_ptr_MachineProtocol->setCPU_temperature(this->m_ptr_MachineInfo->system());
    this->m_ptr_MachineProtocol->setDate(this->m_ptr_MachineInfo->datetime());
    this->m_ptr_MachineProtocol->setJournal_err(this->m_ptr_MachineInfo->journalErrors());
    this->m_ptr_MachineProtocol->setGraceful_shutdown(this->m_ptr_MachineInfo->gracefulShutdown());

    this->m_ptr_MachineProtocol->prepare_json_object();

    this->m_ptr_Socket->setPort(8080);
    this->m_ptr_Socket->setAddress((char *)"127.0.0.1");
    cout << "DApplication::exec():" << endl;

    cout << "((this->m_ptr_MachineProtocol->json_message()).c_str()):" << endl;
    cout << ((this->m_ptr_MachineProtocol->json_message()).c_str()) << endl;
    this->m_ptr_Socket->setMessage(const_cast<char *>((this->m_ptr_MachineProtocol->json_message()).c_str()));
    cout << "this->m_ptr_Socket->message():" << endl;
    cout << this->m_ptr_Socket->message() << endl;

    this->m_ptr_Socket->Client();

    this->m_ptr_MachineInfo->PrintInfoSystem();
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
