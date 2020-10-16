#ifndef HL7BASEDECODE_H
#define HL7BASEDECODE_H

#include "../loguru.hpp"
#include "2.4/segment/MSH.h"
#include "2.4/segment/OBX.h"
#include "common/hl7mllp.h"

using namespace std;

#define MDC_TEMPERATURE "MDC_TEMP_BODY"
#define MDC_BLOOD_PRESS "MDC_PRESS_BLD_NONINV_SYS"
#define MDC_SPO2 "MDC_PULS_OXIM_SAT_O2"

enum class HL7BaseError { SendindAckError, BadFormat, MessageOk };

class HL7BaseDecode {
public:
  HL7BaseDecode();
  ~HL7BaseDecode();

  //! Check to message type
  void identifyMessage(string &data);
  //! Get pending result records
  void ObservationOrResult(string &data);
  //! Send acknowledge
  void sendAckMessage(HL7MLLP *mllp);
  //! Process the register received
  void messageProcess();
  // Returns the result of parsing the message
  HL7BaseError getError();

  double getTemperature() const { return m_temperature; }
  void setTemperature(double temperature) { m_temperature = temperature; }

  int getBloodPressure() const { return m_bloodPressure; }
  void setBloodPressure(int bloodPressure) { m_bloodPressure = bloodPressure; }

  bool getIsTemperature() const { return m_isTemperature; }
  void setIsTemperature(bool isTemperature) { m_isTemperature = isTemperature; }

  bool getIsBloodPressure() const { return m_isBloodPressure; }
  void setIsBloodPressure(bool isBloodPressure) {
    m_isBloodPressure = isBloodPressure;
  }

  int getOximeter() const { return m_oximeter; }
  void setOximeter(int oximeter) { m_oximeter = oximeter; }

  bool getIsOximeter() const { return m_isOximeter; }
  void setIsOximeter(bool isOximeter) { m_isOximeter = isOximeter; }

private:
  HL7_24::MSH *m_MSH;
  HL7_24::OBX *m_OBX;
  HL7BaseError m_eHL7BaseError;

  double m_temperature;
  bool m_isTemperature;

  int m_bloodPressure;
  bool m_isBloodPressure;

  int m_oximeter;
  bool m_isOximeter;
};

#endif // HL7BASEDECODE_H
