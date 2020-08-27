#include "machineinfo.h"
#include "loguru.hpp"
#include "machineinfo.h"
#include <fstream>
#include <sstream>

MachineInfo::MachineInfo() : m_saveFilesPath("/etc/environment") { sensors_init(NULL); }

void MachineInfo::CPUTemperature(void) {
  double ret = 0.0;
  double* temp = &ret;
  bool systemSet = false;

  sensors_chip_name const* cn = NULL;
  int c = 0;

  while ((cn = sensors_get_detected_chips(0, &c)) != 0) {
    if (strstr(cn->prefix, "temp")) {
      temp = &ret;
    } else {
      temp = &m_system;
      systemSet = true;
    }

    sensors_feature const* feat;
    int f = 0;

    if ((feat = sensors_get_features(cn, &f)) != 0) {
      sensors_subfeature const* subf;
      int s = 0;
      if ((subf = sensors_get_all_subfeatures(cn, feat, &s)) != 0) {
        double val;
        if (subf->flags & SENSORS_MODE_R) {
          int rc = sensors_get_value(cn, subf->number, &val);
          if (rc >= 0) {
            *temp = val;
            continue;
          }
        }
      }
    }
  }

  if (!systemSet) {
    m_system = ret;
  }
}

/**
 * @brief MachineInfo::CPUFanSpeed
 */
void MachineInfo::CPUFanSpeed() {
  sensors_chip_name const* cn = NULL;
  int c = 0;
  while ((cn = sensors_get_detected_chips(0, &c)) != 0) {
    sensors_feature const* feat;
    int f = 0;
    if ((feat = sensors_get_features(cn, &f)) != 0) {
      if (!strcmp(feat->name, "fan1")) {
        sensors_subfeature const* subf;
        int s = 0;
        if ((subf = sensors_get_all_subfeatures(cn, feat, &s)) != 0) {
          if (subf->flags & SENSORS_MODE_R) {
            double val;
            int rc = sensors_get_value(cn, subf->number, &val);
            if (rc >= 0) {
              this->setFanSpeed(static_cast<int>(val));
              return;
            }
          }
        }
      }
    }
  }
  this->setFanSpeed(0);
}

void MachineInfo::RAM_MemoryFreeVerify(void) {
  struct sysinfo memInfo;
  sysinfo(&memInfo);
  m_ram_avaliable = memInfo.freeram * memInfo.mem_unit;
}

void MachineInfo::RAM_MemoryTotalVerify(void) {
  struct sysinfo memInfo;
  sysinfo(&memInfo);
  m_ram_total = memInfo.totalram * memInfo.mem_unit;
}

void MachineInfo::FLASH_MemoryFreeVerify(void) {
  struct statfs stflash;

  if (statfs((const char*)m_saveFilesPath.c_str(), &stflash) < 0) {
    m_flash_avaliable = 0;
  } else {
    m_flash_avaliable = (stflash.f_bavail * stflash.f_frsize);
  }
}

void MachineInfo::FLASH_MemoryTotalVerify(void) {
  struct statfs stflash;

  LOG_SCOPE_FUNCTION(INFO);
  if (statfs((const char*)m_saveFilesPath.c_str(), &stflash) < 0) {
    LOG_F(ERROR, " Read total flash status error.");
    m_flash_total = 0;
  } else {
    m_flash_total = (stflash.f_blocks * stflash.f_frsize);
  }
}

void MachineInfo::JournalErrors(void) {

  FILE* p = Process("journalctl -p 3 -xb | grep -c ERROR");
  char buffer[1024];
  
  fgets(buffer, 1024, p);
  m_journalErrors = atoi(buffer);
  printf(" => %d\n", m_journalErrors);
  fclose(p);
}

FILE * MachineInfo::Process(const char* cmd) {

  int fd[2];
  int read_fd, write_fd;
  int pid;

  pipe(fd);
  read_fd = fd[0];
  write_fd = fd[1];
  
  pid = fork();

  if(pid == 0) {
    close(read_fd);
    dup2(write_fd, 1);
    close(write_fd);
    execl("/bin/sh", "sh", "-c", cmd, NULL);
    return NULL;
  } else { 
    close(write_fd);
    return fdopen(read_fd, "r");
  } 
}

/**
 * @brief MachineInfo::PrintInfoSystem
 */
void MachineInfo::PrintInfoSystem() {
  LOG_SCOPE_FUNCTION(INFO);
  LOG_F(INFO, "TempCPU %.2f C", this->m_system);
  LOG_F(INFO, "Value %d RPM", this->m_fanSpeed);
  LOG_F(INFO, "FreeRAM %lu B", this->m_ram_avaliable);
  LOG_F(INFO, "FreeFlash %lu B", this->m_flash_avaliable);
  LOG_F(INFO, "TotalRAM %lu B", this->m_ram_total);
  LOG_F(INFO, "TotalFlash %lu B", this->m_flash_total);
  LOG_F(INFO, "JournalErrors %d ERROR(S)", this->m_journalErrors);
}

/**
 * @brief MachineInfo::ReadStatsCPU
 * @param entries
 */
void MachineInfo::ReadStatsCPU(std::vector<MachineInfo::CPUData>& entries) {
  std::ifstream fileStat("/proc/stat");

  std::string line;

  const std::string STR_CPU("cpu");
  const std::size_t LEN_STR_CPU = STR_CPU.size();
  const std::string STR_TOT("tot");

  while (std::getline(fileStat, line)) {
    // cpu stats line found
    if (!line.compare(0, LEN_STR_CPU, STR_CPU)) {
      std::istringstream ss(line);

      // store entry
      entries.emplace_back(CPUData());
      CPUData& entry = entries.back();

      // read cpu label
      ss >> entry.cpu;

      // remove "cpu" from the label when it's a processor number
      if (entry.cpu.size() > LEN_STR_CPU) entry.cpu.erase(0, LEN_STR_CPU);
      // replace "cpu" with "tot" when it's total values
      else
        entry.cpu = STR_TOT;

      // read times
      for (int i = 0; i < NUM_CPU_STATES; ++i) ss >> entry.times[i];
    }
  }
}

/**
 * @brief MachineInfo::GetIdleTime
 * @param e
 * @return
 */
size_t MachineInfo::GetIdleTime(const MachineInfo::CPUData& e) { return e.times[S_Idle] + e.times[S_IOwait]; }

/**
 * @brief MachineInfo::GetActiveTime
 * @param e
 * @return
 */
size_t MachineInfo::GetActiveTime(const MachineInfo::CPUData& e) {
  return e.times[S_User] + e.times[S_Nice] + e.times[S_System] + e.times[S_Irq] + e.times[S_Softirq] +
         e.times[S_Steal] + e.times[S_Guest] + e.times[S_Guest_Nice];
}

/**
 * @brief MachineInfo::PrintStats
 * @param entries1
 * @param entries2
 */
void MachineInfo::PrintStats(const std::vector<MachineInfo::CPUData>& entries1,
                             const std::vector<MachineInfo::CPUData>& entries2) {
  const size_t NUM_ENTRIES = entries1.size();

  LOG_SCOPE_FUNCTION(INFO);
  for (size_t i = 0; i < NUM_ENTRIES; ++i) {
    const CPUData& e1 = entries1[i];
    const CPUData& e2 = entries2[i];

    std::cout.width(3);
    std::cout << e1.cpu << "] ";

    const float ACTIVE_TIME = static_cast<float>(GetActiveTime(e2) - GetActiveTime(e1));
    const float IDLE_TIME = static_cast<float>(GetIdleTime(e2) - GetIdleTime(e1));
    const float TOTAL_TIME = ACTIVE_TIME + IDLE_TIME;

    std::cout << "active: ";
    std::cout.setf(std::ios::fixed, std::ios::floatfield);
    std::cout.width(6);
    std::cout.precision(2);
    std::cout << (100.f * ACTIVE_TIME / TOTAL_TIME) << "%";

    std::cout << " - idle: ";
    std::cout.setf(std::ios::fixed, std::ios::floatfield);
    std::cout.width(6);
    std::cout.precision(2);
    std::cout << (100.f * IDLE_TIME / TOTAL_TIME) << "%" << std::endl;
  }
}

void MachineInfo::DateTime(void) {
  struct tm* date_time;
  time_t seconds;

  time(&seconds);
  date_time = localtime(&seconds);

  cout << " Day....: ";
  cout << date_time->tm_mday << endl;
  cout << " Mouth..: ";
  cout << date_time->tm_mon + 1 << endl;
  cout << " Year...: ";
  cout << date_time->tm_year + 1900 << endl;
  cout << " Hour...: ";
  cout << date_time->tm_hour << endl;
  cout << " Minute.: ";
  cout << date_time->tm_min << endl;
  cout << " Second.: ";
  cout << date_time->tm_sec << endl;

  m_datetime = to_string(date_time->tm_year + 1900) + '-' + to_string(date_time->tm_mon + 1) + '-' +
               to_string(date_time->tm_mday) + 'T' + to_string(date_time->tm_hour) + ':' +
               to_string(date_time->tm_min) + ':' + to_string(date_time->tm_sec) + 'Z';

  cout << m_datetime << endl;
}
