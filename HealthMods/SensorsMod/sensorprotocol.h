#ifndef SENSORPROTOCOL_H
#define SENSORPROTOCOL_H

#include <ctype.h>
#include <iostream>
#include <json-c/json.h>
#include <stdio.h>
#include <string.h>

using namespace std;

class SensorProtocol {
public:
  SensorProtocol();

  inline uint64_t type() { return m_type; }
  inline void setType(const uint64_t type) { m_type = type; }

  inline string serial() const { return m_serial; }
  inline void setSerial(const string &serial) { m_serial = serial; }

  inline bool connected() const { return m_connected; }
  inline void setConnected(bool connected) { m_connected = connected; }

  inline string date() const { return m_date; }
  inline void setDate(const string &date) { m_date = date; }

  inline string json_message() const { return m_json_message; }
  inline void setJson_message(const string &json_message) {
    m_json_message.clear();
    m_json_message = json_message;
  }

  inline string datetime() const { return m_datetime; }
  inline void setDatetime(const string &datetime) { m_datetime = datetime; }

  void create_json_object(struct json_object *jobj, const char *a, void *b,
                          int variable_b_type);
  void string_parse_json_object(const char *str);
  void *value_json_object(const char *key);

  void DateTime();

  void prepare_json_oximetry();
  void prepare_json_BodyTemperature();
  void prepare_json_BloodPressure(int Sys, int Dia, int Mean);

  inline string addr() const { return m_addr; }
  inline void setAddr(const string &addr) { m_addr = addr; }

  inline int spo2() const { return m_spo2; }
  inline void setSpo2(int spo2) { m_spo2 = spo2; }

  inline double temp() const { return m_temp; }
  inline void setTemp(double temp) { m_temp = temp; }

private:
  uint64_t m_type;
  string m_serial;
  string m_datetime;
  bool m_connected;
  string m_date;
  string m_addr;
  int m_spo2;
  double m_temp;

  string m_json_message;

  struct json_object *m_jobj;
  struct json_object *jobj_actual;

  enum Type : int { Null, Boolean, Double, Int, String, Object, Array };
};

#endif // SENSORPROTOCOL_H
