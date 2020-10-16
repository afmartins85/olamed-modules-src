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

class TothMonitor : public HealthSensors, public HL7BaseDecode {
public:
  TothMonitor();
  static void *sensorListen(void *arg);

  double getTemp() override { return m_temp; }
  bool isTempReady() override { return m_tempReady; }

  int getSpO2() override { return m_spo2; }
  bool isSpo2Ready() override { return m_spo2Ready; }

  int getBlooPress() override { return m_bloodPress; }
  bool isBlooPressReady() override { return m_bloodPressReady; }

  void setTempReaded(double value) { m_temp = value; }

  void setTempReady(bool tempReady) { m_tempReady = tempReady; }

  //! Set SpO2 value readed.
  void setSpO2Readed(int value) { m_spo2 = value; }

  void setSpo2Ready(bool spo2Ready) { m_spo2Ready = spo2Ready; }

  void setBloodReaded(int value) { m_bloodPress = value; }

  void setBloodReady(bool bloodPressReady) {
    m_bloodPressReady = bloodPressReady;
  }

private:
  pthread_t ptid;
  HL7SocketServer *m_hl7Serv;
  HL7Socket *m_client;
  HL7MLLP *m_mllp;
  double m_temp;
  bool m_tempReady;
  int m_spo2;
  bool m_spo2Ready;
  double m_bloodPress;
  bool m_bloodPressReady;
};

#endif // TOTHMONITOR_H
