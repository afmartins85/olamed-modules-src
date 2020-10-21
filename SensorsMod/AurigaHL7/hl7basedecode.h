#ifndef HL7BASEDECODE_H
#define HL7BASEDECODE_H

#include "../loguru.hpp"
#include "2.4/segment/MSH.h"
#include "2.4/segment/OBX.h"
#include "common/hl7mllp.h"

using namespace std;

#define MDC_TEMPERATURE "MDC_TEMP_BODY"
#define MDC_BLOOD_PRESS_SYS "MDC_PRESS_BLD_NONINV_SYS"
#define MDC_BLOOD_PRESS_DIA "MDC_PRESS_BLD_NONINV_DIA"
#define MDC_BLOOD_PRESS_MEAN "MDC_PRESS_BLD_NONINV_MEAN"
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
  //! Returns the result of parsing the message
  HL7BaseError getError();
  //! Remove all OBX registers
  void clearOBXList();

  double getTemperature() const { return m_temperature; }
  void setTemperature(double temperature) { m_temperature = temperature; }

  bool getIsTemperature() const { return m_isTemperature; }
  void setIsTemperature(bool isTemperature) { m_isTemperature = isTemperature; }

  bool getIsBloodPressure() const { return m_isPressBlood; }
  void setIsBloodPressure(bool isBloodPressure) {
    m_isPressBlood = isBloodPressure;
  }

  int getOximeter() const { return m_oximeter; }
  void setOximeter(double oximeter) { m_oximeter = oximeter; }

  bool getIsOximeter() const { return m_isOximeter; }
  void setIsOximeter(bool isOximeter) { m_isOximeter = isOximeter; }

  int getPressBloodSys() const { return m_pressBloodSys; }
  void setPressBloodSys(int pressBloodSys) { m_pressBloodSys = pressBloodSys; }

  int getPressBloodDia() const { return m_pressBloodDia; }
  void setPressBloodDia(int pressBloodDia) { m_pressBloodDia = pressBloodDia; }

  int getPressBloodMean() const { return m_pressBloodMean; }
  void setPressBloodMean(int pressBloodMean) {
    m_pressBloodMean = pressBloodMean;
  }

private:
  HL7_24::MSH *m_MSH;
  // HL7_24::OBX *m_OBX;
  vector<HL7_24::OBX *> m_OBXList;
  HL7BaseError m_eHL7BaseError;

  double m_temperature;
  bool m_isTemperature;

  int m_pressBloodSys;
  int m_pressBloodDia;
  int m_pressBloodMean;
  bool m_isPressBlood;

  double m_oximeter;
  bool m_isOximeter;
};

#endif // HL7BASEDECODE_H
