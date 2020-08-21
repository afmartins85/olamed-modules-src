#include "machineinfo.h"

MachineInfo::MachineInfo() {}

void MachineInfo::Temperature(void) {

  double ret =  0.0;
  double* temp = &ret;
  bool systemSet = false;

  sensors_chip_name const* cn = NULL;
  int c = 0;

  cout << " I'm inside Temperature Method:" << endl; 

  while((cn = sensors_get_detected_chips(0, &c)) != 0) {

    cout << " I'm inside while loop:" << endl; 

    if(strstr(cn->prefix, "temp")) {
      temp = &ret;
    }
    else {
      temp = &m_system;    
      systemSet = true;
    }

    sensors_feature const* feat;
    int f = 0;

    if((feat = sensors_get_features(cn, &f)) != 0) {
      sensors_subfeature const* subf;
      int s = 0;
      if((subf = sensors_get_all_subfeatures(cn, feat, &s)) != 0) {
        double val;
        if(subf->flags & SENSORS_MODE_R) {
          int rc = sensors_get_value(cn, subf->number, &val);
          if(rc >= 0) {
            *temp = val;
            continue;
          }
        }
      }
    }
  }

  if(!systemSet) {
    m_system = ret;
  }

  cout << " I'm outside Temperature Method:" << endl; 

}

void MachineInfo::RAM_MemoryFreeVerify(void) {
  
  struct sysinfo memInfo;
  sysinfo(&memInfo);

  m_ram_avaliable = memInfo.freeram * memInfo.mem_unit/1024;
}

void MachineInfo::RAM_MemoryTotalVerify(void) {

  struct sysinfo memInfo;
  sysinfo(&memInfo);

  m_ram_total = memInfo.totalram * memInfo.mem_unit/1024;
}

void MachineInfo::FLASH_MemoryFreeVerify(void) {
  struct statfs stflash;

  if(statfs((const char*)m_saveFilesPath.c_str(), &stflash) < 0) {
    cout << " Read avaliable flash status error." << endl;
    m_flash_avaliable = 0;
  }
  else {
    m_flash_avaliable = (stflash.f_bavail * stflash.f_frsize / 1024) / 1024;
  }
}

void MachineInfo::FLASH_MemoryTotalVerify(void) {
  struct statfs stflash;

  if(statfs((const char*)m_saveFilesPath.c_str(), &stflash) < 0) {
    cout << " Read total flash status error." << endl;
    m_flash_total = 0;
  }
  else {
    m_flash_total = (stflash.f_blocks * stflash.f_frsize / 1024) / 1024;
  }
}

