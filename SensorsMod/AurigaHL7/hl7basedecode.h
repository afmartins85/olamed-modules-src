#ifndef HL7BASEDECODE_H
#define HL7BASEDECODE_H

#include "../loguru.hpp"
#include "2.4/segment/MSH.h"

using namespace std;

enum class HL7BaseError { BadFormat, MessageOk };

class HL7BaseDecode {
public:
  HL7BaseDecode();

  // Check to message type
  void identifyMessage(string &data);
  // Returns the result of parsing the message
  HL7BaseError getError();

private:
  HL7_24::MSH *m_MSH;
  HL7BaseError m_eHL7BaseError;
};

#endif // HL7BASEDECODE_H
