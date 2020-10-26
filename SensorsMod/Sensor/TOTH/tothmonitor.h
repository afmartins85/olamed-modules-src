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

  int getSpO2() override {
    int spo2 = 0;
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

  int getPressBloodSys() override {
    int bloodPress = 0;
    pthread_mutex_lock(&m_tothCommMutex);
    bloodPress = m_pressBloodSys;
    setBloodReady(false);
    pthread_mutex_unlock(&m_tothCommMutex);
    return bloodPress;
  }

  int getPressBloodDia() override {
    int bloodPress = 0;
    pthread_mutex_lock(&m_tothCommMutex);
    bloodPress = m_pressBloodDia;
    setBloodReady(false);
    pthread_mutex_unlock(&m_tothCommMutex);
    return bloodPress;
  }

  int getPressBloodMean() override {
    int bloodPress = 0;
    pthread_mutex_lock(&m_tothCommMutex);
    bloodPress = m_pressBloodMean;
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

  string getEquipAddress() override {
    string address;
    pthread_mutex_lock(&m_tothCommMutex);
    address = m_address;
    setAddressReady(false);
    pthread_mutex_unlock(&m_tothCommMutex);
    return address;
  }

  bool isEquipAddressReady() override {
    bool ready = 0;
    pthread_mutex_lock(&m_tothCommMutex);
    ready = m_addressReady;
    pthread_mutex_unlock(&m_tothCommMutex);
    return ready;
  }

  string getSerialSensor() override {
    string serial;
    pthread_mutex_lock(&m_tothCommMutex);
    serial = m_serial;
    setSerialReady(false);
    pthread_mutex_unlock(&m_tothCommMutex);
    return serial;
  }

  bool isSerialSensorReady() override {
    bool ready = 0;
    pthread_mutex_lock(&m_tothCommMutex);
    ready = m_serialReady;
    pthread_mutex_unlock(&m_tothCommMutex);
    return ready;
  }

  inline void setTempReaded(double value) { m_temp = value; }
  inline void setTempReady(bool tempReady) { m_tempReady = tempReady; }

  //! Set SpO2 value readed.
  inline void setSpO2Readed(int value) { m_spo2 = value; }
  inline void setSpo2Ready(bool spo2Ready) { m_spo2Ready = spo2Ready; }

  inline void setBloodReady(bool bloodPressReady) {
    m_bloodPressReady = bloodPressReady;
  }

  inline void setPressBloodSys(int pressBloodSys) {
    m_pressBloodSys = pressBloodSys;
  }
  inline void setPressBloodDia(int pressBloodDia) {
    m_pressBloodDia = pressBloodDia;
  }
  inline void setPressBloodMean(int pressBloodMean) {
    m_pressBloodMean = pressBloodMean;
  }

  void setAddress(const string &address) { m_address = address; }
  inline void setAddressReady(bool addressReady) {
    m_addressReady = addressReady;
  }

  void setSerial(const string &serial) { m_serial = serial; }
  inline void setSerialReady(bool serialReady) { m_serialReady = serialReady; }

private:
  pthread_t ptid;
  HL7SocketServer *m_hl7Serv;
  HL7Socket *m_client;
  HL7MLLP *m_mllp;
  double m_temp;
  bool m_tempReady;
  int m_spo2;
  bool m_spo2Ready;
  int m_pressBloodSys;
  int m_pressBloodDia;
  int m_pressBloodMean;
  bool m_bloodPressReady;
  bool m_bNextRegister;
  string m_address;
  bool m_addressReady;
  string m_serial;
  bool m_serialReady;

  static pthread_mutex_t m_tothCommMutex;
};

#endif // TOTHMONITOR_H
