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
#include "../../loguru.hpp"
#include <healthsensors.h>
#include <pthread.h>

#define CONFIRMATION_TIMEOUT 30

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
    temp = getTemperature();
    setIsTemperature(false);
    pthread_mutex_unlock(&m_tothCommMutex);
    return temp;
  }
  bool isTempReady() override {
    bool ready = 0;
    pthread_mutex_lock(&m_tothCommMutex);
    ready = getIsTemperature();
    pthread_mutex_unlock(&m_tothCommMutex);
    return ready;
  }

  int getSpO2() override {
    int spo2 = 0;
    pthread_mutex_lock(&m_tothCommMutex);
    spo2 = getOximeter();
    setIsOximeter(false);
    pthread_mutex_unlock(&m_tothCommMutex);
    return spo2;
  }
  bool isSpo2Ready() override {
    bool ready = 0;
    pthread_mutex_lock(&m_tothCommMutex);
    ready = getIsOximeter();
    pthread_mutex_unlock(&m_tothCommMutex);
    return ready;
  }

  int getPressBloodSys() override {
    int bloodPress = 0;
    pthread_mutex_lock(&m_tothCommMutex);
    bloodPress = getPressBldSys();
    setIsBloodPressure(false);
    pthread_mutex_unlock(&m_tothCommMutex);
    return bloodPress;
  }

  int getPressBloodDia() override {
    int bloodPress = 0;
    pthread_mutex_lock(&m_tothCommMutex);
    bloodPress = getPressBldDia();
    setIsBloodPressure(false);
    pthread_mutex_unlock(&m_tothCommMutex);
    return bloodPress;
  }

  int getPressBloodMean() override {
    int bloodPress = 0;
    pthread_mutex_lock(&m_tothCommMutex);
    bloodPress = getPressBldMean();
    setIsBloodPressure(false);
    pthread_mutex_unlock(&m_tothCommMutex);
    return bloodPress;
  }

  bool isBlooPressReady() override {
    bool ready = 0;
    pthread_mutex_lock(&m_tothCommMutex);
    ready = getIsBloodPressure();
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

  string getEquipAddress() override {
    string address;
    pthread_mutex_lock(&m_tothCommMutex);
    address = gethl7baseEquipAddress();
    setIsEquipAddress(false);
    pthread_mutex_unlock(&m_tothCommMutex);
    return address;
  }

  bool isEquipAddressReady() override {
    bool ready = 0;
    pthread_mutex_lock(&m_tothCommMutex);
    ready = getIsEquipAddress();
    pthread_mutex_unlock(&m_tothCommMutex);
    return ready;
  }

  string getSerialSensor() override {
    string serial;
    pthread_mutex_lock(&m_tothCommMutex);
    serial = gethl7baseSerial();
    sethl7baseIsSerial(false);
    pthread_mutex_unlock(&m_tothCommMutex);
    return serial;
  }

  bool isSerialSensorReady() override {
    bool ready = 0;
    pthread_mutex_lock(&m_tothCommMutex);
    ready = gethl7baseIsSerial();
    pthread_mutex_unlock(&m_tothCommMutex);
    return ready;
  }

  bool isSensorOnline() override;

private:
  pthread_t ptid;
  HL7SocketServer *m_hl7Serv;
  HL7Socket *m_client;
  HL7MLLP *m_mllp;
  bool m_bNextRegister;
  string m_defaultAddress;
  static pthread_mutex_t m_tothCommMutex;

  FILE *ping_host();
};

#endif // TOTHMONITOR_H
