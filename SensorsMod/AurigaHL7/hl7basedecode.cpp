#include "hl7basedecode.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

using namespace HL7_24;
using namespace std;

HL7BaseDecode::HL7BaseDecode() {
    m_MSH = new HL7_24::MSH;
    m_OBX = new HL7_24::OBX;
}

/**
 * @brief HL7BaseDecode::identifyMessage
 * @param data
 */
void HL7BaseDecode::identifyMessage(string &data) {
  string MSHCrop;

  size_t found = data.find("MSH");
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

  string dataField;
  int delimiterCount = 1;
  // Parse the fields
  for (size_t idx = 0; idx < MSHCrop.length(); ++idx) {

    if (MSHCrop[idx] == '|') {
      switch (delimiterCount) {
      case 2:
        printf("%s\n", dataField.c_str());
        m_MSH->getEncodingCharacters()->setData(dataField);
        break;
      case 3:
        printf("%s\n", dataField.c_str());
        string subStr;
        int delimiter2 = 1;
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
        printf("getSendingApplication\n");
        break;
      }
      delimiterCount++;
      dataField.clear();
    } else {
      dataField.push_back(MSHCrop[idx]);
    }
  }

  LOG_SCOPE_F(INFO, "MSH Segment");
  LOG_F(INFO, "EncodingCharacters: %s",
        m_MSH->getEncodingCharacters()->getData());
  LOG_SCOPE_F(INFO, "SendingApplication:");
  {
    LOG_F(INFO, "%s", m_MSH->getMSH_3()->getHD_1()->getData());
    LOG_F(INFO, "%s", m_MSH->getMSH_3()->getHD_2()->getData());
    LOG_F(INFO, "%s", m_MSH->getMSH_3()->getHD_3()->getData());
  }
  m_eHL7BaseError = HL7BaseError::MessageOk;
}

/**
 * @brief HL7BaseDecode::ObservationOrResult
 * @param data
 */
void HL7BaseDecode::ObservationOrResult(string &data) {
    string OBX;
    std::vector<std::string>list;
    size_t found = 0;

    do {
        found = data.find("OBX");
        if (found != data.npos) {
            // Find OBX segment and cut it out for analysis
            for (size_t idx = 0; idx < data.length(); ++idx) {
              if (data[found + idx] == 0x0d) {
                data.erase(0, idx);
                list.push_back(OBX);
                OBX.clear();
                break;
              }
              OBX.push_back(data[found + idx]);
            }
        }
        else {
            // TODO: Set Error Information
            LOG_F(ERROR, "HL7 Message is bad format for this application!!");
            m_eHL7BaseError = HL7BaseError::BadFormat;
        }
    }while(found != data.npos);

    printf("ObservationOrResult: TEST\n");
    printf("data: %s\n",data.c_str());
    printf("list:\n");
    for (size_t i=0; i < list.size(); i++){
        std::cout << list[i] << std::endl;
    }

    string dataField;
    int delimiterCount = 1;

    // Parse the fields : listRowIndex was forced to value 3, just to take de last line od strings in list vector.
    for (size_t listRowIndex = 3; listRowIndex < list.size(); listRowIndex ++) {
        std::string OBXstr = list[listRowIndex];
        for (size_t idx = 0; idx < OBXstr.length(); ++idx) {
            if (OBXstr[idx] == '|') {
                switch (delimiterCount) {
                case 1:
                    printf("%s\n", dataField.c_str());
                    m_OBX->getSetIDOBX()->setData(dataField);
                    break;
                case 2:
                  printf("%s\n", dataField.c_str());
                  m_OBX->getValueType()->setData(dataField);
                  break;
                case 3:
                    {// Scope delimiter
                        printf("%s\n", dataField.c_str());
                        string subStr;
                        int delimiter2 = 1;
                         for (size_t i = 0; i < dataField.size(); ++i) {
                            if (dataField[i] == '^') {
                                if (delimiter2 == 1) {
                                    m_OBX->getObservationIdentifier()->getIdentifier()->setData(subStr);
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
                        m_OBX->getObservationIdentifier()->getAlternateIdentifier()->setData(subStr);
                        printf("getObservationIdentifier\n");
                    }// Scope delimiter
                    break;
                case 4:
                    printf("%s\n", dataField.c_str());
                    m_OBX->getObservationSubId()->setData(dataField);
                    break;
                case 5:
                    printf("%s\n", dataField.c_str());
                    m_OBX->getObservationValue()->setData(dataField);
                    break;
                case 6:
                    {// Scope delimiter
                        printf("%s\n", dataField.c_str());
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
                        printf("getUnits\n");
                    }// Scope delimiter
                    break;
                case 7:
                    printf("%s\n", dataField.c_str());
                    m_OBX->getReferencesRange()->setData(dataField);
                    break;
                }
                delimiterCount++;
                dataField.clear();
            } else {
                dataField.push_back(OBXstr[idx]);
            }
        }
    }

    LOG_SCOPE_F(INFO, "OBX Segment");
    LOG_F(INFO, "Set ID - OBX: %s", m_OBX->getSetIDOBX()->getData());
    LOG_F(INFO, "Value Type: %s", m_OBX->getValueType()->getData());
    LOG_SCOPE_F(INFO, "Observation Identifier:");
    {
        LOG_F(INFO, "%s", m_OBX->getObservationIdentifier()->getIdentifier()->getData());
        LOG_F(INFO, "%s", m_OBX->getObservationIdentifier()->getText()->getData());
        LOG_F(INFO, "%s", m_OBX->getObservationIdentifier()->getNameOfCodingSystem()->getData());
    }
    LOG_F(INFO, "Observation Sub-ID: %s", m_OBX->getObservationSubId()->getData());
    LOG_F(INFO, "Observation Value: %s", m_OBX->getObservationValue()->getData());
    LOG_SCOPE_F(INFO, "Units:");
    {
        LOG_F(INFO, "%s", m_OBX->getUnits()->getIdentifier()->getData());
        LOG_F(INFO, "%s", m_OBX->getUnits()->getText()->getData());
        LOG_F(INFO, "%s", m_OBX->getUnits()->getNameOfCodingSystem()->getData());
    }
    LOG_F(INFO, "References Range: %s",  m_OBX->getReferencesRange()->getData());

    m_eHL7BaseError = HL7BaseError::MessageOk;
}

/**
 * @brief HL7BaseDecode::getError
 * @return
 */
HL7BaseError HL7BaseDecode::getError() { return m_eHL7BaseError; }
