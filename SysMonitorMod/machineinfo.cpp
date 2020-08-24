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

void MachineInfo::DateTime(void) {

  struct tm *date_time;
  time_t seconds;

  time(&seconds);
  date_time = localtime(&seconds);

  cout << " Day....: "; cout << date_time->tm_mday << endl;
  cout << " Mouth..: "; cout << date_time->tm_mon+1 << endl;
  cout << " Year...: "; cout << date_time->tm_year+1900 << endl;
  cout << " Hour...: "; cout << date_time->tm_hour << endl;
  cout << " Minute.: "; cout << date_time->tm_min << endl;
  cout << " Second.: "; cout << date_time->tm_sec << endl;

  m_datetime = to_string(date_time->tm_year+1900) +
              '-' +
              to_string(date_time->tm_mon+1) +
              '-' +
              to_string(date_time->tm_mday) +
              'T' +
              to_string(date_time->tm_hour) +
              ':' +
              to_string(date_time->tm_min) +
              ':' +
              to_string(date_time->tm_sec) +
              'Z';

  cout << m_datetime << endl;               
}

