#include "balanceprotocol.h"
#include "loguru.hpp"
#include <stdio.h>

/**
 * @brief BalanceProtocol::BalanceProtocol
 */
BalanceProtocol::BalanceProtocol() : m_wasSentDisconnected(false) { jobj_actual = json_object_new_object(); }

void BalanceProtocol::create_json_object(struct json_object *jobj, const char *a, void *b, int variable_b_type) {
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
      char temp[10];
      sprintf(temp, "%0.2f", x);
      json_object_object_add(jobj, a, json_object_new_double_s(x, temp));
      break;
    }

    case Type::Int:
      json_object_object_add(jobj, a, json_object_new_int(static_cast<int>(reinterpret_cast<intptr_t>(b))));
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

void BalanceProtocol::string_parse_json_object(const char *str) { m_jobj = json_tokener_parse(str); }

void *BalanceProtocol::value_json_object(const char *key) {
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
      *shazam = json_object_get_double(jobj);  // TODO: rework this "gambiarra"
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
 * @brief BalanceProtocol::DateTime
 */
void BalanceProtocol::DateTime(void) {
  char buf[100];
  time_t now;

  time(&now);
  strftime(buf, sizeof(buf), "%Y-%m-%dT%H:%M:%SZ", gmtime(&now));
  m_datetime = buf;
}

/**
 * @brief BalanceProtocol::prepare_json_oximetry
 */
void BalanceProtocol::prepare_json_balance(double weigth, string address) {
  /* PrintProtocol JSON:
      "type": 16,
      "weigth": "87.00",
      "address": "/dev/tty",
      "connected": true,
      "date": "2020-08-10T20:00:31Z"
  */

  create_json_object(jobj_actual, "type", (void *)16, Type::Int);
  create_json_object(jobj_actual, "weigth", &weigth, Type::Double);
  create_json_object(jobj_actual, "address", (void *)address.c_str(), Type::String);
  create_json_object(jobj_actual, "connected", (void *)(true), Type::Boolean);
  create_json_object(jobj_actual, "date", (void *)date().c_str(), Type::String);

  // Copy jobj_actual to jobj_previous
  setJson_message(json_object_to_json_string_ext(jobj_actual, JSON_C_TO_STRING_SPACED | JSON_C_TO_STRING_PRETTY));
}

/**
 * @brief BalanceProtocol::prepare_json_balanceOff
 */
bool BalanceProtocol::prepare_json_balanceOff() {
  /* PrintProtocol JSON:
      "type": 16,
      "connected": false,
      "date": "2020-08-10T20:00:31Z"
  */

  if (wasSentDisconnected() == false) {
    json_object_object_del(jobj_actual, "weigth");
    json_object_object_del(jobj_actual, "address");

    this->DateTime();

    create_json_object(jobj_actual, "type", (void *)16, Type::Int);
    create_json_object(jobj_actual, "connected", (void *)(false), Type::Boolean);
    create_json_object(jobj_actual, "date", (void *)this->datetime().c_str(), Type::String);

    // Copy jobj_actual to jobj_previous
    setJson_message(json_object_to_json_string_ext(jobj_actual, JSON_C_TO_STRING_SPACED | JSON_C_TO_STRING_PRETTY));
    setWasSentDisconnected(true);
  } else {
    return false;
  }
  return true;
}
