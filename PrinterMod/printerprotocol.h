#ifndef PRINTERPROTOCOL_H
#define PRINTERPROTOCOL_H

#include "supplyprinter.h"
#include <ctype.h>
#include <iostream>
#include <json-c/json.h>
#include <stdio.h>
#include <string.h>

using namespace std;

class PrinterProtocol : public SupplyPrinter {
 public:
  PrinterProtocol();

  inline uint64_t type() { return m_type; }
  inline void setType(const uint64_t type) {
    //    if (type != m_type) {
    //      create_json_object(jobj_actual, "type", (void *)type, Type::Int);
    //    } else {
    //      json_object_object_del(jobj_actual, "type");
    //    }
    m_type = type;
  }

  inline string serial() const { return m_serial; }
  inline void setSerial(const string &serial) {
    //    if (serial.compare(m_serial)) {
    //      create_json_object(jobj_actual, "serial", (void *)serial.c_str(), Type::String);
    //    } else {
    //      json_object_object_del(jobj_actual, "serial");
    //    }
    m_serial = serial;
  }

  inline string description() const { return m_description; }
  inline void setDescription(const string &description) {
    //    if (description.compare(m_description)) {
    //      create_json_object(jobj_actual, "description", (void *)description.c_str(), Type::String);
    //    } else {
    //      json_object_object_del(jobj_actual, "description");
    //    }
    m_description = description;
  }

  inline bool connected() const { return m_connected; }
  inline void setConnected(bool connected) {
    //    create_json_object(jobj_actual, "connected", (void *)((connected ? "true" : "false")), Type::String);
    m_connected = connected;
  }

  inline uint64_t prints() const { return m_prints; }
  inline void setPrints(const uint64_t &prints) {
    //    create_json_object(jobj_actual, "prints", (void *)prints, Type::Int);
    m_prints = prints;
  }

  inline string state() const { return m_state; }
  inline void setState(const string &state) {
    //    create_json_object(jobj_actual, "state", (void *)state.c_str(), Type::String);
    m_state = state;
  }

  inline string error() const { return m_error; }
  inline void setError(const string &error) {
    //    if (error.compare(m_error)) {
    //      create_json_object(jobj_actual, "error", (void *)error.c_str(), Type::String);
    //    } else {
    //      json_object_object_del(jobj_actual, "error");
    //    }
    m_error = error;
  }

  inline string supply_type() const { return m_supply_type; }
  inline void setSupply_type(const string &supply_type) {
    //    if (supply_type.compare(m_supply_type)) {
    //      create_json_object(jobj_actual, "supply_type", (void *)supply_type.c_str(), Type::String);
    //    } else {
    //      json_object_object_del(jobj_actual, "supply_type");
    //    }
    m_supply_type = supply_type;
  }

  inline double cyan_level() { return m_cyan_level; }
  inline void setCyan_level(double cyan_level) {
    //    if (cyan_level != m_cyan_level) {
    //      create_json_object(jobj_actual, "cyan_level", &m_cyan_level, Type::Double);
    //    } else {
    //      json_object_object_del(jobj_actual, "cyan_level");
    //    }
    m_cyan_level = cyan_level;
  }

  inline double magenta_level() const { return m_magenta_level; }
  inline void setMagenta_level(const double &magenta_level) {
    //    if (magenta_level != m_magenta_level) {
    //      create_json_object(jobj_actual, "magenta_level", &m_magenta_level, Type::Double);
    //    } else {
    //      json_object_object_del(jobj_actual, "magenta_level");
    //    }
    m_magenta_level = magenta_level;
  }

  inline double yellow_level() const { return m_yellow_level; }
  inline void setYellow_level(const double &yellow_level) {
    //    if (yellow_level != m_yellow_level) {
    //      create_json_object(jobj_actual, "yellow_level", &m_yellow_level, Type::Double);
    //    } else {
    //      json_object_object_del(jobj_actual, "yellow_level");
    //    }
    m_yellow_level = yellow_level;
  }

  inline double black_level() const { return m_black_level; }
  inline void setBlack_level(const double &black_level) {
    //    if (black_level != m_black_level) {
    //      create_json_object(jobj_actual, "black_level", &m_black_level, Type::Double);
    //    } else {
    //      json_object_object_del(jobj_actual, "black_level");
    //    }
    m_black_level = black_level;
  }

  inline string json_message() const { return m_json_message; }
  inline void setJson_message(const string &json_message) {
    m_json_message.clear();
    m_json_message = json_message;
  }

  inline string id() const { return m_id; }
  inline void setId(const string &id) { m_id = id; }

  inline string filename() const { return m_filename; }
  inline void setFilename(const string &filename) { m_filename = filename; }

  inline string filetype() const { return m_filetype; }
  inline void setFiletype(const string &filetype) { m_filetype = filetype; }

  inline string content() const { return m_content; }
  inline void setContent(const string &content) { m_content = content; }

  inline string date() const { return m_date; }
  inline void setDate(const string &date) {
    //    create_json_object(jobj_actual, "date", (void *)date.c_str(), Type::String);
    m_date = date;
  }

  inline string datetime() const { return m_datetime; }
  inline void setDatetime(const string &datetime) { m_datetime = datetime; }

  void create_json_object(struct json_object *jobj, const char *a, void *b, int variable_b_type);
  void prepare_json_object(void);
  void string_parse_json_object(char *str);
  void *value_json_object(const char *key);

  static string base64_decode(string const &encoded_string);
  string base64_encode(unsigned char const *bytes_to_code, unsigned int in_len);

  void DateTime();

 private:
  uint64_t m_type;
  string m_serial;
  string m_description;
  bool m_connected;
  uint64_t m_prints;
  string m_state;
  string m_error;
  string m_supply_type;
  string m_datetime;

  double m_cyan_level;
  double m_magenta_level;
  double m_yellow_level;
  double m_black_level;

  // int m_type;
  string m_id;
  string m_filename;
  string m_filetype;
  string m_content;
  string m_date;

  string m_json_message;

  struct json_object *m_jobj;
  struct json_object *jobj_actual;

  enum Type : int { Null, Boolean, Double, Int, String, Object, Array };

  static inline bool is_base64(unsigned char c) { return (isalnum(c) || (c == '+') || (c == '/')); }
};

#endif  // PRINTERPROTOCOL_H
