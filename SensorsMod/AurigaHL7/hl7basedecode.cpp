#include "hl7basedecode.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

using namespace HL7_24;
using namespace std;

/**
 * @brief HL7BaseDecode::HL7BaseDecode
 */
HL7BaseDecode::HL7BaseDecode() {
  m_MSH = new HL7_24::MSH;
  // m_OBX = new HL7_24::OBX;
}

/**
 * @brief HL7BaseDecode::~HL7BaseDecode
 */
HL7BaseDecode::~HL7BaseDecode() {
  if (m_MSH != nullptr) {
    delete m_MSH;
  }
  //  if (m_OBX != nullptr) {
  //    delete m_OBX;
  //  }
}

/**
 * @brief HL7BaseDecode::identifyMessage
 * @param data
 */
void HL7BaseDecode::identifyMessage(string &data) {
  string MSHCrop;
  size_t found = data.find("MSH");
  string subStr;
  int delimiter2 = 1;
  if (found == data.npos) {
    // TODO: Set Error Information
    LOG_F(ERROR, "HL7 Message is bad format for this application!!");
    m_eHL7BaseError = HL7BaseError::BadFormat;
  }

  // Find MSH segment and cut it out for analysis
  for (size_t idx = 0; idx < data.length(); ++idx) {
    if (data[found + idx] == 0x0d) {
      data.erase(0, idx);
      break;
    }
    MSHCrop.push_back(data[found + idx]);
  }

  LOG_F(9, "MSH => %s", MSHCrop.c_str());

  string dataField;
  int delimiterCount = 1;
  // Parse the fields
  for (size_t idx = 0; idx < MSHCrop.length(); ++idx) {

    if (MSHCrop[idx] == '|') {
      switch (delimiterCount) {
      case 2:
        m_MSH->getEncodingCharacters()->setData(dataField);
        break;
      case 3:
        for (size_t i = 0; i < dataField.size(); ++i) {
          if (dataField[i] == '^') {
            if (delimiter2 == 1) {
              m_MSH->getMSH_3()->getHD_1()->setData(subStr);
              delimiter2++;
              subStr.clear();
            } else {
              m_MSH->getMSH_3()->getHD_2()->setData(subStr);
              subStr.clear();
            }
          } else {
            subStr.push_back(dataField[i]);
          }
        }
        m_MSH->getMSH_3()->getHD_3()->setData(subStr);
        break;

      case 7:
        m_MSH->getDateTimeOfMessage()->getTimeOfAnEvent()->setData(dataField);
        break;

      case 9:
        subStr.clear();
        delimiter2 = 1;
        for (size_t i = 0; i < dataField.size(); ++i) {
          if (dataField[i] == '^') {
            if (delimiter2 == 1) {
              m_MSH->getMSH_9()->getMSG_1()->setData(subStr);
              delimiter2++;
              subStr.clear();
            } else {
              m_MSH->getMSH_9()->getMSG_2()->setData(subStr);
              subStr.clear();
            }
          } else {
            subStr.push_back(dataField[i]);
          }
        }
        m_MSH->getMSH_9()->getMSG_3()->setData(subStr);
        break;
      case 10:
        m_MSH->getMessageControlID()->setData(dataField);
        break;
      }
      delimiterCount++;
      dataField.clear();
    } else {
      dataField.push_back(MSHCrop[idx]);
    }
  }

  LOG_SCOPE_F(9, "MSH Segment");
  LOG_F(9, "EncodingCharacters: %s", m_MSH->getEncodingCharacters()->getData());
  LOG_F(9, "MessageControlID: %s", m_MSH->getMessageControlID()->getData());
  LOG_F(9, "DateTimeOfMessage: %s",
        m_MSH->getDateTimeOfMessage()->getTimeOfAnEvent()->getData());
  LOG_SCOPE_F(9, "SendingApplication:");
  {
    LOG_F(9, "%s", m_MSH->getMSH_3()->getHD_1()->getData());
    LOG_F(9, "%s", m_MSH->getMSH_3()->getHD_2()->getData());
    LOG_F(9, "%s", m_MSH->getMSH_3()->getHD_3()->getData());
  }
  LOG_SCOPE_F(9, "MessageType:");
  {
    LOG_F(9, "%s", m_MSH->getMSH_9()->getMSG_1()->getData());
    LOG_F(9, "%s", m_MSH->getMSH_9()->getMSG_2()->getData());
    LOG_F(9, "%s", m_MSH->getMSH_9()->getMSG_3()->getData());
  }

  m_eHL7BaseError = HL7BaseError::MessageOk;
}

/**
 * @brief HL7BaseDecode::ObservationOrResult
 * @param data
 */
void HL7BaseDecode::ObservationOrResult(string &data) {
  string OBX;
  vector<string> list;
  size_t found = 0;
  bool isOBXFound = false;

  do {
    found = data.find("OBX");
    if (found != data.npos) {
      // Find OBX segment and cut it out for analysis
      for (size_t idx = 0; idx < (data.length() - found); ++idx) {
        if (data[found + idx] == 0x0d) {
          isOBXFound = true;
          data.erase(found, idx);
          list.push_back(OBX);
          OBX.clear();
          break;
        }
        OBX.push_back(data[found + idx]);
      }
    }
  } while (found != data.npos);

  if (isOBXFound == false) {
    // TODO: Set Error Information
    LOG_F(ERROR, "HL7 Message is bad format for this application!!");
    m_eHL7BaseError = HL7BaseError::BadFormat;
    return;
  }

  string OBXstr;
  for (size_t i = 0; i < list.size(); i++) {
    OBXstr = list[i];

    LOG_F(9, "%s", OBXstr.c_str());

    string dataField;
    int delimiterCount = 1;

    HL7_24::OBX *m_OBX = new HL7_24::OBX;

    for (size_t idx = 0; idx < OBXstr.length(); ++idx) {
      if (OBXstr[idx] == '|') {
        switch (delimiterCount) {
        case 1:
          m_OBX->getSetIDOBX()->setData(dataField);
          break;
        case 2:
          m_OBX->getValueType()->setData(dataField);
          break;
        case 4: { // Scope delimiter
          string subStr;
          int delimiter2 = 1;
          for (size_t i = 0; i < dataField.size(); ++i) {
            if (dataField[i] == '^') {
              if (delimiter2 == 1) {
                m_OBX->getObservationIdentifier()->getIdentifier()->setData(
                    subStr);
                delimiter2++;
                subStr.clear();
              } else {
                m_OBX->getObservationIdentifier()->getText()->setData(subStr);
                subStr.clear();
              }
            } else {
              subStr.push_back(dataField[i]);
            }
          }
          m_OBX->getObservationIdentifier()->getAlternateIdentifier()->setData(
              subStr);
        } // Scope delimiter
        break;
        case 5:
          m_OBX->getObservationSubId()->setData(dataField);
          break;
        case 6:
          m_OBX->getObservationValue()->setData(dataField);
          break;
        case 7: { // Scope delimiter
          string subStr;
          int delimiter2 = 1;
          for (size_t i = 0; i < dataField.size(); ++i) {
            if (dataField[i] == '^') {
              if (delimiter2 == 1) {
                m_OBX->getUnits()->getIdentifier()->setData(subStr);
                delimiter2++;
                subStr.clear();
              } else {
                m_OBX->getUnits()->getText()->setData(subStr);
                subStr.clear();
              }
            } else {
              subStr.push_back(dataField[i]);
            }
          }
          m_OBX->getUnits()->getNameOfCodingSystem()->setData(subStr);
        } // Scope delimiter
        break;
        case 8:
          m_OBX->getReferencesRange()->setData(dataField);
          break;
          //        case 19: { // Scope delimiter
          //          string subStr;
          //          int delimiter2 = 1;
          //          for (size_t i = 0; i < dataField.size(); ++i) {
          //            if (dataField[i] == '^') {
          //              if (delimiter2 == 1) {
          //                m_OBX->getUnits()->getIdentifier()->setData(subStr);
          //                delimiter2++;
          //                subStr.clear();
          //              } else {
          //                m_OBX->getUnits()->getText()->setData(subStr);
          //                subStr.clear();
          //              }
          //            } else {
          //              subStr.push_back(dataField[i]);
          //            }
          //          }
          //          m_OBX->getUnits()->getNameOfCodingSystem()->setData(subStr);
          //        } // Scope delimiter
          //        break;
        }
        delimiterCount++;
        dataField.clear();
      } else {
        dataField.push_back(OBXstr[idx]);
      }
    }
    m_OBXList.push_back(m_OBX);
  }

  LOG_SCOPE_F(9, "OBX Segment");
  for (size_t i = 0; i < m_OBXList.size(); i++) {
    HL7_24::OBX *m_OBX = m_OBXList[i];
    LOG_F(9, "Set ID - OBX: %s", m_OBX->getSetIDOBX()->getData());
    LOG_F(9, "Value Type: %s", m_OBX->getValueType()->getData());
    LOG_F(9, "%s",
          m_OBX->getObservationIdentifier()->getIdentifier()->getData());
    LOG_F(9, "%s", m_OBX->getObservationIdentifier()->getText()->getData());
    LOG_F(9, "Observation Sub-ID: %s", m_OBX->getObservationSubId()->getData());
    LOG_F(9, "Observation Value: %s ", m_OBX->getObservationValue()->getData());
    LOG_F(9, "%s", m_OBX->getUnits()->getIdentifier()->getData());
    LOG_F(9, "%s", m_OBX->getUnits()->getText()->getData());
    LOG_F(9, "%s", m_OBX->getUnits()->getNameOfCodingSystem()->getData());
    LOG_F(9, "References Range: %s", m_OBX->getReferencesRange()->getData());
  }

  m_eHL7BaseError = HL7BaseError::MessageOk;
}

/**
 * @brief HL7BaseDecode::sendAckMessage
 */
void HL7BaseDecode::sendAckMessage(HL7MLLP *mllp) {
  string message("MSH|");
  message.append(m_MSH->getEncodingCharacters()->getData());
  message.append("|");
  message.append(m_MSH->getMSH_3()->getHD_1()->getData());
  message.append("^");
  message.append(m_MSH->getMSH_3()->getHD_2()->getData());
  message.append("^");
  message.append(m_MSH->getMSH_3()->getHD_3()->getData());
  message.append("|1.0|TOTH_SMART_CHECK|Hospital|");
  message.append(m_MSH->getDateTimeOfMessage()->getTimeOfAnEvent()->getData());
  message.append("||");
  message.append(m_MSH->getMSH_9()->getMSG_1()->getData());
  message.append("^");
  message.append(m_MSH->getMSH_9()->getMSG_2()->getData());
  message.append("^");
  message.append(m_MSH->getMSH_9()->getMSG_3()->getData());
  message.append("|");
  message.append(m_MSH->getMessageControlID()->getData());
  message.append("|P|2.6|||AL|NE||UNICOD3855 E UTF-8|||IHE_PCD_001^IHE "
                 "PCD^1.3.6.1.4.1.19376.1.6.1.1.1^ISO\n");
  message.append("MSA|AA|");
  message.append(m_MSH->getMessageControlID()->getData());

  size_t wrCount = mllp->send_msg_mllp(message);
  if (wrCount == message.length()) {
    m_eHL7BaseError = HL7BaseError::MessageOk;
  } else {
    m_eHL7BaseError = HL7BaseError::SendindAckError;
  }
}

/**
 * @brief HL7BaseDecode::messageProcess
 */
void HL7BaseDecode::messageProcess() {
  string pressBloodSYS(MDC_BLOOD_PRESS_SYS);
  string pressBloodDIA(MDC_BLOOD_PRESS_DIA);
  string pressBloodMEAN(MDC_BLOOD_PRESS_MEAN);
  string temperature(MDC_TEMPERATURE);
  string spo2(MDC_SPO2);

  for (size_t i = 0; i < m_OBXList.size(); ++i) {
    HL7_24::OBX *pOBX = m_OBXList[i];

    if (!pressBloodSYS.compare(
            pOBX->getObservationIdentifier()->getText()->getData())) {
      setPressBldSys(atoi(pOBX->getObservationValue()->getData()));
    } else if (!pressBloodDIA.compare(
                   pOBX->getObservationIdentifier()->getText()->getData())) {
      setPressBldDia(atoi(pOBX->getObservationValue()->getData()));
    } else if (!pressBloodMEAN.compare(
                   pOBX->getObservationIdentifier()->getText()->getData())) {
      setIsBloodPressure(true);
      setPressBldMean(atoi(pOBX->getObservationValue()->getData()));
    } else if (!temperature.compare(
                   pOBX->getObservationIdentifier()->getText()->getData())) {
      setIsTemperature(true);
      setTemperature(atof(pOBX->getObservationValue()->getData()));
    } else if (!spo2.compare(
                   pOBX->getObservationIdentifier()->getText()->getData())) {
      setIsOximeter(true);
      setOximeter(atof(pOBX->getObservationValue()->getData()) / 100);
    }
  }

  this->clearOBXList();
}

/**
 * @brief HL7BaseDecode::getError
 * @return
 */
HL7BaseError HL7BaseDecode::getError() { return m_eHL7BaseError; }

/**
 * @brief HL7BaseDecode::clearOBXList
 */
void HL7BaseDecode::clearOBXList() {
  for (size_t i = 0; i < m_OBXList.size(); ++i) {
    HL7_24::OBX *pOBX = m_OBXList[i];
    delete pOBX;
  }
  m_OBXList.clear();
}
