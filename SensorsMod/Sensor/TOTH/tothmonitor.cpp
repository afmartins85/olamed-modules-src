#include "tothmonitor.h"
#include <unistd.h>

pthread_mutex_t TothMonitor::m_tothCommMutex(PTHREAD_MUTEX_INITIALIZER);

/**
 * @brief TothMonitor::TothMonitor
 */
TothMonitor::TothMonitor()
    : m_temp(0), m_tempReady(false), m_spo2(0), m_spo2Ready(false),
      m_pressBloodSys(0), m_pressBloodDia(0), m_pressBloodMean(0),
      m_bloodPressReady(false) {

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
        // LOG_F(WARNING, "Close TOTH Monitor connection.\n");
        close(monitor->m_client->getSocket());
      } else {
        // Identify message received
        monitor->identifyMessage(data);
        if (monitor->getError() == HL7BaseError::MessageOk) {
          // Get result records
          monitor->ObservationOrResult(data);
          if (monitor->getError() == HL7BaseError::MessageOk) {
            // TODO: informar o resultado para a camada de aplicação
            monitor->messageProcess();
            pthread_mutex_lock(&m_tothCommMutex);
            if (monitor->getIsTemperature() == true) {
              monitor->setTempReaded(monitor->getTemperature());
              monitor->setTempReady(true);
            } else if (monitor->getIsOximeter() == true) {
              monitor->setSpO2Readed(monitor->getOximeter());
              monitor->setSpo2Ready(true);
            } else if (monitor->getIsBloodPressure() == true) {
              monitor->setPressBloodSys(monitor->getPressBldSys()); // OK
              // monitor->setPressBldSys(monitor->getPressBldSys()); // Wrong
              monitor->setPressBloodDia(monitor->getPressBldDia());
              monitor->setPressBloodMean(monitor->getPressBldMean());
              monitor->setBloodReady(true);
            } else if (monitor->getIsEquipAddress() == true) {
              monitor->setAddress(monitor->getEquipAddress());
              monitor->setAddressReady(true);
            }
            pthread_mutex_unlock(&m_tothCommMutex);
            machState = MonitorCommStatus::WaitingConfirmation;
          }
        }
      }
    } else if (machState == MonitorCommStatus::WaitingConfirmation) {
      if (monitor->getNextRegister() == true) {
        // Send Acknowledge
        // monitor->sendAckMessage(monitor->m_mllp);
        if (monitor->getError() == HL7BaseError::MessageOk) {
          machState = MonitorCommStatus::ReceiveMessage;
        } else {
          // LOG_F(WARNING, "Force close TOTH Monitor connection.\n");
          close(monitor->m_client->getSocket());
          machState = MonitorCommStatus::Disconnected;
        }
      }
      usleep(10);
    }
  }

  pthread_exit(NULL);
}
