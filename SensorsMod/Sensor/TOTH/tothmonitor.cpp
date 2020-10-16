#include "tothmonitor.h"
#include <unistd.h>

/**
 * @brief TothMonitor::TothMonitor
 */
TothMonitor::TothMonitor() : m_spo2(0) {

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
  bool stateMonitorConnected = false;

  while (true) {
    std::string data;

    if (stateMonitorConnected == false) {
      // accept client connection
      monitor->m_client = monitor->m_hl7Serv->run();
      if (!monitor->m_client) {
        LOG_F(ERROR, "ERROR accepting client connection.\n");
        continue;
      }
      // init MLLP object to receive MLLP rotocol messages
      monitor->m_mllp = new HL7MLLP(monitor->m_client->getSocket());
      stateMonitorConnected = true;
    } else {
      int ret = monitor->m_mllp->read_msg_mllp(data);
      if ((ret <= 0) && (data.empty())) {
        LOG_F(INFO, "Close TOTH Monitor connection.\n");
        stateMonitorConnected = false;
        close(monitor->m_client->getSocket());
      } else {
        // Identify message received
        monitor->identifyMessage(data);
        if (monitor->getError() == HL7BaseError::MessageOk) {
          // Get result records
          monitor->ObservationOrResult(data);
          if (monitor->getError() == HL7BaseError::MessageOk) {
            // Send Acknowledge
            monitor->sendAckMessage(monitor->m_mllp);
            if (monitor->getError() == HL7BaseError::MessageOk) {
              // TODO: informar o resultado para a camada de aplicação
              monitor->messageProcess();
              if (monitor->getIsTemperature() == true) {
                monitor->setTempReaded(monitor->getTemperature());
                monitor->setTempReady(true);
              } else if (monitor->getIsOximeter() == true) {
                monitor->setSpO2Readed(monitor->getOximeter());
                monitor->setSpo2Ready(true);
              } else if (monitor->getIsBloodPressure() == true) {
                monitor->setBloodReaded(monitor->getBloodPressure());
                monitor->setBloodReady(true);
              }
            }
          }
        }
      }
    }
    usleep(10);
  }

  pthread_exit(NULL);
}
