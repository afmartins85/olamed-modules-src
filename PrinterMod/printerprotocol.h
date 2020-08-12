#ifndef PRINTERPROTOCOL_H
#define PRINTERPROTOCOL_H

#include "supplyprinter.h"
#include <ctype.h>
#include <iostream>

using namespace std;

class PrinterProtocol : public SupplyPrinter {
 public:
  PrinterProtocol();

  inline string serial() const { return m_serial; }
  inline void setSerial(const string &serial) { m_serial = serial; }

  inline string description() const { return m_description; }
  inline void setDescription(const string &description) { m_description = description; }

  inline bool connected() const { return m_connected; }
  inline void setConnected(bool connected) { m_connected = connected; }

  inline uint64_t prints() const { return m_prints; }
  inline void setPrints(const uint64_t &prints) { m_prints = prints; }

  inline string state() const { return m_state; }
  inline void setState(const string &state) { m_state = state; }

  inline string error() const { return m_error; }
  inline void setError(const string &error) { m_error = error; }

 private:
  string m_serial;
  string m_description;
  bool m_connected;
  uint64_t m_prints;
  string m_state;
  string m_error;
};

#endif  // PRINTERPROTOCOL_H
