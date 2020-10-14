#include "hl7basedecode.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

using namespace HL7_24;
using namespace std;

HL7BaseDecode::HL7BaseDecode() { m_MSH = new HL7_24::MSH; }

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
      data.erase(idx);
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
 * @brief HL7BaseDecode::getError
 * @return
 */
HL7BaseError HL7BaseDecode::getError() { return m_eHL7BaseError; }
