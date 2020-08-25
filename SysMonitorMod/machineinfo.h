#ifndef MACHINEINFO_H
#define MACHINEINFO_H

#include <ctype.h>
#include <iostream>
#include <sensors/sensors.h>
#include <stdio.h>
#include <string.h>
#include <sys/statfs.h>
#include <sys/sysinfo.h>
#include <time.h>
#include <vector>

#define NUM_CPU_STATES 10

using namespace std;

class MachineInfo {
 public:
  enum CPUStates { S_User = 0, S_Nice, S_System, S_Idle, S_IOwait, S_Irq, S_Softirq, S_Steal, S_Guest, S_Guest_Nice };

  typedef struct CPUData {
    std::string cpu;
    size_t times[NUM_CPU_STATES];
  } CPUData;

 public:
  MachineInfo();

  inline double system() { return m_system; }
  inline void setSystem(const double system) { m_system = system; }

  inline double cpu() { return m_cpu; }
  inline void setCpu(const double cpu) { m_cpu = cpu; }

  inline unsigned long ram_avaliable() { return m_ram_avaliable; }
  inline void setRam_avaliable(unsigned long ram_avaliable) { m_ram_avaliable = ram_avaliable; }

  inline unsigned long ram_total() { return m_ram_total; }
  inline void setRam_total(unsigned long ram_total) { m_ram_total = ram_total; }

  inline string filepath() { return m_saveFilesPath; }
  inline void setFilepath(string filepath) { m_saveFilesPath = filepath; }

  inline unsigned long flash_avaliable() { return m_flash_avaliable; }
  inline void setFlash_avaliable(unsigned long flash_avaliable) { m_flash_avaliable = flash_avaliable; }

  inline unsigned long flash_total() { return m_flash_total; }
  inline void setFlash_total(unsigned long flash_total) { m_flash_total = flash_total; }

  inline string datetime() { return m_datetime; }
  inline void setDatetime(string datetime) { m_datetime = datetime; }

  inline int fanSpeed() const { return m_fanSpeed; }
  inline void setFanSpeed(int fanSpeed) { m_fanSpeed = fanSpeed; }

  void CPUTemperature(void);
  void CPUFanSpeed(void);
  void RAM_MemoryTotalVerify(void);
  void RAM_MemoryFreeVerify(void);
  void FLASH_MemoryTotalVerify(void);
  void FLASH_MemoryFreeVerify(void);

  void PrintInfoSystem();
  void ReadStatsCPU(std::vector<CPUData> &entries);
  size_t GetIdleTime(const CPUData &e);
  size_t GetActiveTime(const CPUData &e);
  void PrintStats(const std::vector<CPUData> &entries1, const std::vector<CPUData> &entries2);

  void DateTime(void);

 private:
  int m_fanSpeed;
  double m_system;
  double m_cpu;
  unsigned long m_ram_avaliable;
  unsigned long m_ram_total;
  unsigned long m_flash_avaliable;
  unsigned long m_flash_total;
  string m_saveFilesPath;
  string m_datetime;
};

#endif  // MACHINEINFO_H
