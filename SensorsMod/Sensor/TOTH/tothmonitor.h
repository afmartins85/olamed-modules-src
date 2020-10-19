#ifndef TOTHMONITOR_H
#define TOTHMONITOR_H

#include "../../AurigaHL7/2.4/message/ACK.h"
#include "../../AurigaHL7/2.4/message/ADR_A19.h"
#include "../../AurigaHL7/2.4/message/ORU_R01.h"
#include "../../AurigaHL7/2.4/message/QRY_A19.h"
#include "../../AurigaHL7/common/ObjToPipe.h"
#include "../../AurigaHL7/common/Util.h"
#include "../../AurigaHL7/common/dataencode.h"
#include "../../AurigaHL7/common/hl7mllp.h"
#include "../../AurigaHL7/common/hl7socket.h"
#include "../../AurigaHL7/common/validation.h"
#include "../../AurigaHL7/hl7basedecode.h"
#include "../../healthsensors.h"
#include "../../loguru.hpp"
#include <pthread.h>

enum class MonitorCommStatus {
  Disconnected,
  ReceiveMessage,
  WaitingConfirmation
};

class TothMonitor : public HealthSensors, public HL7BaseDecode {
public:
  TothMonitor();
  static void *sensorListen(void *arg);

  double getTemp() override {
    double temp = 0;
    pthread_mutex_lock(&m_tothCommMutex);
    temp = m_temp;
    setTempReady(false);
    pthread_mutex_unlock(&m_tothCommMutex);
    return temp;
  }
  bool isTempReady() override {
    bool ready = 0;
    pthread_mutex_lock(&m_tothCommMutex);
    ready = m_tempReady;
    pthread_mutex_unlock(&m_tothCommMutex);
    return ready;
  }

  double getSpO2() override {
    double spo2 = 0;
    pthread_mutex_lock(&m_tothCommMutex);
    spo2 = m_spo2;
    setSpo2Ready(false);
    pthread_mutex_unlock(&m_tothCommMutex);
    return spo2;
  }
  bool isSpo2Ready() override {
    bool ready = 0;
    pthread_mutex_lock(&m_tothCommMutex);
    ready = m_spo2Ready;
    pthread_mutex_unlock(&m_tothCommMutex);
    return ready;
  }

  int getBlooPress() override {
    int bloodPress = 0;
    pthread_mutex_lock(&m_tothCommMutex);
    bloodPress = m_bloodPress;
    setBloodReady(false);
    pthread_mutex_unlock(&m_tothCommMutex);
    return bloodPress;
  }

  bool isBlooPressReady() override {
    bool ready = 0;
    pthread_mutex_lock(&m_tothCommMutex);
    ready = m_bloodPressReady;
    pthread_mutex_unlock(&m_tothCommMutex);
    return ready;
  }

  void isNextRegister(bool next) override {
    pthread_mutex_lock(&m_tothCommMutex);
    m_bNextRegister = next;
    pthread_mutex_unlock(&m_tothCommMutex);
  }

  bool getNextRegister() {
    bool next = false;
    pthread_mutex_lock(&m_tothCommMutex);
    next = m_bNextRegister;
    pthread_mutex_unlock(&m_tothCommMutex);
    return next;
  }

  inline void setTempReaded(double value) { m_temp = value; }
  inline void setTempReady(bool tempReady) { m_tempReady = tempReady; }

  //! Set SpO2 value readed.
  inline void setSpO2Readed(double value) { m_spo2 = value; }
  inline void setSpo2Ready(bool spo2Ready) { m_spo2Ready = spo2Ready; }

  inline void setBloodReaded(int value) { m_bloodPress = value; }
  inline void setBloodReady(bool bloodPressReady) {
    m_bloodPressReady = bloodPressReady;
  }

private:
  pthread_t ptid;
  HL7SocketServer *m_hl7Serv;
  HL7Socket *m_client;
  HL7MLLP *m_mllp;
  double m_temp;
  bool m_tempReady;
  double m_spo2;
  bool m_spo2Ready;
  int m_bloodPress;
  bool m_bloodPressReady;
  bool m_bNextRegister;
  static pthread_mutex_t m_tothCommMutex;
};

#endif // TOTHMONITOR_H
