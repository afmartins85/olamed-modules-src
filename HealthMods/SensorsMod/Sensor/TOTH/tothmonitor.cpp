#include "tothmonitor.h"
#include <chrono>
#include <unistd.h>
#include <wait.h>

pthread_mutex_t TothMonitor::m_tothCommMutex(PTHREAD_MUTEX_INITIALIZER);

typedef chrono::seconds seconds;
typedef chrono::high_resolution_clock Clock;

/**
 * @brief TothMonitor::TothMonitor
 */
TothMonitor::TothMonitor()
    : m_bNextRegister(false), m_defaultAddress("172.30.1.30") {

  // create socket server
  this->m_hl7Serv = new HL7SocketServer(50000);

  pthread_create(&ptid, NULL, &TothMonitor::sensorListen, this);
  pthread_detach(ptid);
}

/**
 * @brief TothMonitor::sensorListen
 * @param arg
 * @return
 */
void *TothMonitor::sensorListen(void *arg) {
  (void)arg;
  Clock::time_point t0;
  time_t timestamp = 0;

  pthread_setname_np(pthread_self(), __FUNCTION__);
  TothMonitor *monitor = reinterpret_cast<TothMonitor *>(arg);
  MonitorCommStatus machState = MonitorCommStatus::Disconnected;

  while (true) {
    std::string data;

    if (machState == MonitorCommStatus::Disconnected) {
      // accept client connection
      monitor->m_client = monitor->m_hl7Serv->run();
      if (!monitor->m_client) {
        LOG_F(ERROR, "ERROR accepting client connection.\n");
        continue;
      }
      // init MLLP object to receive MLLP rotocol messages
      monitor->m_mllp = new HL7MLLP(monitor->m_client->getSocket());
      machState = MonitorCommStatus::ReceiveMessage;
    } else if (machState == MonitorCommStatus::ReceiveMessage) {
      int ret = monitor->m_mllp->read_msg_mllp(data);
      if ((ret <= 0) && (data.empty())) {
        if (ret == -1) {
          LOG_F(WARNING, "Close TOTH Monitor connection.\n");
          close(monitor->m_client->getSocket());
        }
      } else {
        // Identify message received
        monitor->identifyMessage(data);
        if (monitor->getError() == HL7BaseError::MessageOk) {
          // Get result records
          monitor->ObservationOrResult(data);
          if (monitor->getError() == HL7BaseError::MessageOk) {
            pthread_mutex_lock(&m_tothCommMutex);
            monitor->messageProcess();
            pthread_mutex_unlock(&m_tothCommMutex);
            machState = MonitorCommStatus::WaitingConfirmation;
            localtime(&timestamp);
            t0 = chrono::high_resolution_clock::now();
            LOG_F(WARNING, "MonitorCommStatus::WaitingConfirmation");
          }
        }
      }
    } else if (machState == MonitorCommStatus::WaitingConfirmation) {

      if (monitor->getNextRegister() == true) {
        // Send Acknowledge
        monitor->sendAckMessage(monitor->m_mllp);
        if (monitor->getError() == HL7BaseError::MessageOk) {
          machState = MonitorCommStatus::ReceiveMessage;
          monitor->isNextRegister(false);
          continue;
        } else {
          LOG_F(WARNING, "Force close TOTH Monitor connection.");
          close(monitor->m_client->getSocket());
          machState = MonitorCommStatus::Disconnected;
        }
      }

      auto end = chrono::high_resolution_clock::now();
      auto elapsed = chrono::duration_cast<chrono::seconds>(end - t0);
      if (elapsed.count() >= CONFIRMATION_TIMEOUT) {
        LOG_F(WARNING, "Timeout for data monitor confirmation.\n");
        close(monitor->m_client->getSocket());
        machState = MonitorCommStatus::Disconnected;
      }
    }
    usleep(10);
  }

  pthread_exit(NULL);
}

/**
 * @brief TothMonitor::isSensorOnline
 * @return
 */
bool TothMonitor::isSensorOnline() {

  char buff[1024];
  string result;
  FILE *f = ping_host();

  if (f == nullptr) {
    return false;
  }

  fgets(buff, 1024, f);

  for (char c : buff) {
    result.push_back(c);
  }

  LOG_F(INFO, "res: %s", result.c_str());
  fclose(f);
  return true;
}

/**
 * @brief TothMonitor::ping_host
 * @return
 */
FILE *TothMonitor::ping_host() {
  int fd[2];
  int read_fd, write_fd;
  int pid;

  string ping("ping ");
  ping.append(m_defaultAddress);
  ping.append(" -W 1 -c 1");

  pipe(fd);
  read_fd = fd[0];
  write_fd = fd[1];

  pid = fork();
  if (pid == 0) {
    close(read_fd);
    dup2(write_fd, 1);
    close(write_fd);
    execl("/bin/sh", "sh", "-c", ping.c_str(), NULL);
    return nullptr;
  } else {
    wait(&pid);
    close(write_fd);
    return fdopen(read_fd, "r");
  }
}
