#include "sensorprotocol.h"
#include "loguru.hpp"
#include "sensorprotocol.h"
#include <stdio.h>

/**
 * @brief PrinterProtocol::PrinterProtocol
 */
SensorProtocol::SensorProtocol() { jobj_actual = json_object_new_object(); }

void SensorProtocol::create_json_object(struct json_object *jobj, const char *a,
                                        void *b, int variable_b_type) {
  switch (variable_b_type) {
  case Type::Null:
    break;

  case Type::Boolean:
    json_object_object_add(jobj, a, json_object_new_boolean((bool)b));
    break;

  case Type::Double: {
    // json_object_object_add(jobj, a, json_object_new_double(*((double
    // *)(b))));
    double x = *((double *)b);
    json_object_object_add(jobj, a, json_object_new_double(x));
    break;
  }

  case Type::Int:
    json_object_object_add(
        jobj, a,
        json_object_new_int(static_cast<int>(reinterpret_cast<intptr_t>(b))));
    break;

  case Type::String:
    json_object_object_add(jobj, a, json_object_new_string((const char *)b));
    break;

  case Type::Object:
    break;

  case Type::Array:
    break;
  }
}

void SensorProtocol::string_parse_json_object(const char *str) {
  m_jobj = json_tokener_parse(str);
}

void *SensorProtocol::value_json_object(const char *key) {
  /* PrintProtocol JSON:
  {
    "type": 4,
    "id": "cf046407-ad01-4a61-925f-1d8156370c5c",
    "filename": "file_print",
    "filetype": ".txt",
    "content": "<base64>",
    "date" : "2020-08-01T20:10:10Z"
  }
  */
  void *value = nullptr;
  double *shazam = nullptr;
  enum json_type type;
  struct json_object *jobj;

  jobj = json_object_object_get(m_jobj, key);

  type = json_object_get_type(jobj);

  switch (type) {
  case json_type_null:
    LOG_F(INFO, "json_type_null");
    break;

  case json_type_boolean:
    value = (void *)(json_object_get_boolean(jobj) ? "true" : "false");
    LOG_F(9, "json_type_boolen value: %s", (char *)value);
    break;

  case json_type_double:
    *shazam = json_object_get_double(jobj); // TODO: rework this "gambiarra"
    value = (void *)shazam;
    LOG_F(9, "json_type_double value: %lf", json_object_get_double(jobj));
    break;

  case json_type_int:
    value = reinterpret_cast<void *>(json_object_get_int(jobj));
    LOG_F(9, "json_type_int value: %ld", (uint64_t)value);
    break;

  case json_type_object:
    LOG_F(9, "json_type_object\n");
    // TODO:
    // LOG_F(INFO,"value: %d", json_object_get_int(jobj))
    break;

  case json_type_array:
    LOG_F(9, "json_type_array\n");
    // TODO:
    // LOG_F(INFO,"value: %d", json_object_get_array(jobj))
    // int arraylen = json_object_array_length(jobj);
    // LOG_F(INFO,"Array Length: %d", arraylen);
    // int i;
    // json_object *jvalue;
    // for(i=0; i<arraylen; i++) {
    //  jvalue = json_object_array_get_idx(obj, i);
    //  LOG_F(INFO,"value[%d]: %s\n", i, json_object_get_string(jvalue));
    //}
    break;

  case json_type_string:
    value = (void *)json_object_get_string(jobj);
    LOG_F(9, "json_type_str value: %s", (char *)value);
    break;
  }
  return value;
}

/**
 * @brief SensorProtocol::DateTime
 */
void SensorProtocol::DateTime(void) {
  char buf[100];
  time_t now;

  time(&now);
  strftime(buf, sizeof(buf), "%Y-%m-%dT%H:%M:%SZ", gmtime(&now));
  m_datetime = buf;
}

/**
 * @brief SensorProtocol::prepare_json_oximetry
 */
void SensorProtocol::prepare_json_oximetry() {
  /* PrintProtocol JSON:
      "type": 7,
      "serial": "7558100415344-106-0",
      "address": "192.168.0.1",
      "connected": "true",
      "oximetry": 0.98,
      "date": "2020-08-10T20:00:31Z"
  */

  if (connected()) {
    create_json_object(jobj_actual, "type", (void *)7, Type::Int);
    create_json_object(jobj_actual, "serial", (void *)serial().c_str(),
                       Type::String);
    create_json_object(jobj_actual, "address", (void *)m_addr.c_str(),
                       Type::String);
    create_json_object(jobj_actual, "connected", (void *)(true), Type::Boolean);
    create_json_object(jobj_actual, "oximetry", &m_spo2, Type::Double);
    create_json_object(jobj_actual, "date", (void *)date().c_str(),
                       Type::String);
  } else {
    json_object_object_del(jobj_actual, "serial");
    create_json_object(jobj_actual, "type", (void *)type(), Type::Int);
    create_json_object(jobj_actual, "connected", (void *)(false),
                       Type::Boolean);
    create_json_object(jobj_actual, "date", (void *)date().c_str(),
                       Type::String);
  }

  // Copy jobj_actual to jobj_previous
  setJson_message(json_object_to_json_string_ext(
      jobj_actual, JSON_C_TO_STRING_SPACED | JSON_C_TO_STRING_PRETTY));
}

/**
 * @brief SensorProtocol::prepare_json_BodyTemperature
 */
void SensorProtocol::prepare_json_BodyTemperature() {
  /* PrintProtocol JSON:
    "type": 8,
    "serial": "7558100415344-106-0",
    "address": "192.168.0.1",
    "connected": "true",
    "temperature": 36.2,
    "date": "2020-08-10T20:00:31Z"
*/

  if (connected()) {
    create_json_object(jobj_actual, "type", (void *)8, Type::Int);
    create_json_object(jobj_actual, "serial", (void *)serial().c_str(),
                       Type::String);
    create_json_object(jobj_actual, "address", (void *)m_addr.c_str(),
                       Type::String);
    create_json_object(jobj_actual, "connected", (void *)(true), Type::Boolean);
    create_json_object(jobj_actual, "temperature", &m_temp, Type::Double);
    create_json_object(jobj_actual, "date", (void *)date().c_str(),
                       Type::String);
  } else {
    json_object_object_del(jobj_actual, "serial");
    create_json_object(jobj_actual, "type", (void *)type(), Type::Int);
    create_json_object(jobj_actual, "connected", (void *)(false),
                       Type::Boolean);
    create_json_object(jobj_actual, "date", (void *)date().c_str(),
                       Type::String);
  }

  // Copy jobj_actual to jobj_previous
  setJson_message(json_object_to_json_string_ext(
      jobj_actual, JSON_C_TO_STRING_SPACED | JSON_C_TO_STRING_PRETTY));
}

/**
 * @brief SensorProtocol::prepare_json_BloodPressure
 */
void SensorProtocol::prepare_json_BloodPressure(int Sys, int Dia, int Mean) {

  /* PrintProtocol JSON:
  "type": 9,
  "serial": "7558100415344-106-0",
  "address": "192.168.0.1",
  "connected": "true",
  "bloodPress": 130|60,
  "date": "2020-08-10T20:00:31Z"
*/

  if (connected()) {
    create_json_object(jobj_actual, "type", (void *)9, Type::Int);
    create_json_object(jobj_actual, "serial", (void *)serial().c_str(),
                       Type::String);
    create_json_object(jobj_actual, "address", (void *)m_addr.c_str(),
                       Type::String);
    create_json_object(jobj_actual, "connected", (void *)(true), Type::Boolean);
    create_json_object(jobj_actual, "systolic", (void *)Sys, Type::Int);
    create_json_object(jobj_actual, "diastolic", (void *)Dia, Type::Int);
    create_json_object(jobj_actual, "mean", (void *)Mean, Type::Int);
    create_json_object(jobj_actual, "date", (void *)date().c_str(),
                       Type::String);
  } else {
    json_object_object_del(jobj_actual, "serial");
    create_json_object(jobj_actual, "type", (void *)type(), Type::Int);
    create_json_object(jobj_actual, "connected", (void *)(false),
                       Type::Boolean);
    create_json_object(jobj_actual, "date", (void *)date().c_str(),
                       Type::String);
  }

  // Copy jobj_actual to jobj_previous
  setJson_message(json_object_to_json_string_ext(
      jobj_actual, JSON_C_TO_STRING_SPACED | JSON_C_TO_STRING_PRETTY));
}
