#ifndef PRINTERPROTOCOL_H
#define PRINTERPROTOCOL_H

#include "supplyprinter.h"
#include <ctype.h>
#include <iostream>
#include <json-c/json.h>

using namespace std;

class PrinterProtocol : public SupplyPrinter {
 public:
  PrinterProtocol();

  inline uint64_t type() { return m_type; }
  inline void setType(const uint64_t type) { m_type = type; }

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

  inline string supply_type() const { return m_supply_type; }
  inline void setSupply_type(const string &supply_type) { m_supply_type = supply_type; }

  inline double cyan_level() { return m_cyan_level; }
  inline void setCyan_level(double cyan_level) { m_cyan_level = cyan_level; }

  inline double magenta_level() const { return m_magenta_level; }
  inline void setMagenta_level(const double &magenta_level) { m_magenta_level = magenta_level; }

  inline double yellow_level() const { return m_yellow_level; }
  inline void setYellow_level(const double &yellow_level) { m_yellow_level = yellow_level; }

  inline double black_level() const { return m_black_level; }
  inline void setBlack_level(const double &black_level) { m_black_level = black_level; }

  inline string json_message() const { return m_json_message; }
  inline void setJson_message (const string &json_message) { m_json_message = json_message; }
  
  void create_json_object(struct json_object *jobj, const char *a, void *b, int variable_b_type);
  void prepare_json_object(void);

 private:
  uint64_t m_type;
  string m_serial;
  string m_description;
  bool m_connected;
  uint64_t m_prints;
  string m_state;
  string m_error;
  string m_supply_type;

  double m_cyan_level;
  double m_magenta_level;
  double m_yellow_level;
  double m_black_level;

  string m_json_message;

  struct json_object *jobj_actual;
  struct json_object *jobj_previous;

  enum { TYPE_NULL, TYPE_BOOLEAN, TYPE_DOUBLE, TYPE_INT, TYPE_STRING, TYPE_OBJECT, TYPE_ARRAY };
};

#endif  // PRINTERPROTOCOL_H
