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

  int getSpO2() override;
  //! Set SpO2 value readed.
  void setSpO2Readed(int value);

private:
  pthread_t ptid;
  HL7SocketServer *m_hl7Serv;
  HL7Socket *m_client;
  HL7MLLP *m_mllp;

  int m_spo2;
};

#endif // TOTHMONITOR_H
