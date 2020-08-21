#ifndef MACHINEINFO_H
#define MACHINEINFO_H

#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <iostream>
#include <sys/statfs.h>
#include <sys/sysinfo.h>
#include <sensors/sensors.h>

using namespace std;

class MachineInfo {

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
    
    void Temperature(void);
    void RAM_MemoryTotalVerify(void);
    void RAM_MemoryFreeVerify(void);
    void FLASH_MemoryTotalVerify(void);
    void FLASH_MemoryFreeVerify(void);

  private:
  
    double m_system;
    double m_cpu;
    unsigned long m_ram_avaliable;
    unsigned long m_ram_total;
    unsigned long m_flash_avaliable;
    unsigned long m_flash_total;
    string m_saveFilesPath; 
};







#endif // MACHINEINFO_H
