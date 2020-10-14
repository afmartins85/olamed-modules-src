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
        //        std::cout << data << std::endl;
        //        std::cout << __LINE__ << " received: " << data.size() <<
        //        std::endl; printf("\n"); printf("\n"); for (size_t i = 0; i <
        //        data.size(); ++i) {
        //          printf("0x%02X ", data.at(i));
        //          if (!(i % 16)) {
        //            printf("\n");
        //          }
        //        }
        //        printf("\n");
        monitor->identifyMessage(data);
      }
    }
    usleep(10);
  }

  pthread_exit(NULL);
}

int TothMonitor::getSpO2() { return m_spo2; }

/**
 * @brief TothMonitor::setSpO2Readed
 * @param value
 */
void TothMonitor::setSpO2Readed(int value) { m_spo2 = value; }
