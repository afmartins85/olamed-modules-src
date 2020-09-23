#include "printerprotocol.h"
#include "loguru.hpp"
#include <stdio.h>

static const string base64_chars =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
    "abcdefghijklmnopqrstuvwxyz"
    "0123456789+/";

/**
 * @brief PrinterProtocol::PrinterProtocol
 */
PrinterProtocol::PrinterProtocol()
    : m_type(0), m_cyan_level(0), m_magenta_level(0), m_yellow_level(0), m_black_level(0) {
  jobj_actual = json_object_new_object();
}

void PrinterProtocol::create_json_object(struct json_object *jobj, const char *a, void *b, int variable_b_type) {
  switch (variable_b_type) {
    case Type::Null:
      break;

    case Type::Boolean:
      json_object_object_add(jobj, a, json_object_new_boolean((bool)b));
      break;

    case Type::Double: {
      // json_object_object_add(jobj, a, json_object_new_double(*((double *)(b))));
      double x = *((double *)b);
      json_object_object_add(jobj, a, json_object_new_double(x));
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

void PrinterProtocol::prepare_json_object(void) {
  /* PrintProtocol JSON:

    "type": 2,
    "serial": "7558100415344-106-0",
    "description": "Lexmark Laser, Mono, MX421ADE",
    "connected": "true",
    "prints": 20,
    "state": "idle",
    "error": "no paper",
    "supply_type": "tonner"
    "cyan_level": 100.00
    "magenta_level: 98.89,
    "yellow_level: 88.00,
    "black_level: 23.00,
    "date": "2020-08-10T20:00:31Z"
*/

  if (connected() == true) {
    create_json_object(jobj_actual, "type", (void *)type(), Type::Int);
    create_json_object(jobj_actual, "serial", (void *)serial().c_str(), Type::String);
    create_json_object(jobj_actual, "description", (void *)description().c_str(), Type::String);
    create_json_object(jobj_actual, "connected", (void *)("true"), Type::String);
    create_json_object(jobj_actual, "prints", (void *)prints(), Type::Int);
    create_json_object(jobj_actual, "state", (void *)state().c_str(), Type::String);
    create_json_object(jobj_actual, "error", (void *)error().c_str(), Type::String);
    create_json_object(jobj_actual, "supply_type", (void *)supply_type().c_str(), Type::String);
    create_json_object(jobj_actual, "cyan_level", &m_cyan_level, Type::Double);
    create_json_object(jobj_actual, "magenta_level", &m_magenta_level, Type::Double);
    create_json_object(jobj_actual, "yellow_level", &m_yellow_level, Type::Double);
    create_json_object(jobj_actual, "black_level", &m_black_level, Type::Double);
    create_json_object(jobj_actual, "date", (void *)date().c_str(), Type::String);
  } else {
    json_object_object_del(jobj_actual, "description");
    json_object_object_del(jobj_actual, "serial");
    json_object_object_del(jobj_actual, "prints");
    json_object_object_del(jobj_actual, "state");
    json_object_object_del(jobj_actual, "error");
    json_object_object_del(jobj_actual, "supply_type");
    json_object_object_del(jobj_actual, "cyan_level");
    json_object_object_del(jobj_actual, "magenta_level");
    json_object_object_del(jobj_actual, "yellow_level");
    json_object_object_del(jobj_actual, "black_level");
    create_json_object(jobj_actual, "type", (void *)type(), Type::Int);
    create_json_object(jobj_actual, "connected", (void *)("false"), Type::String);
    create_json_object(jobj_actual, "date", (void *)date().c_str(), Type::String);
  }

  // Copy jobj_actual to jobj_previous
  setJson_message(json_object_to_json_string_ext(jobj_actual, JSON_C_TO_STRING_SPACED | JSON_C_TO_STRING_PRETTY));
}

void PrinterProtocol::string_parse_json_object(char *str) { m_jobj = json_tokener_parse(str); }

void *PrinterProtocol::value_json_object(const char *key) {
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

string PrinterProtocol::base64_decode(string const &encoded_string) {
  int in_len = encoded_string.size();
  int i = 0;
  int j = 0;
  int in_ = 0;
  unsigned char char_array_4[4], char_array_3[3];
  string ret;

  while (in_len-- && (encoded_string[in_] != '=') && is_base64(encoded_string[in_])) {
    char_array_4[i++] = encoded_string[in_];
    in_++;
    if (i == 4) {
      for (i = 0; i < 4; i++) char_array_4[i] = base64_chars.find(char_array_4[i]);

      char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
      char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
      char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];

      for (i = 0; i < 3; i++) ret += char_array_3[i];
      i = 0;
    }
  }

  if (i) {
    for (j = i; j < 4; j++) char_array_4[j] = 0;

    for (j = 0; j < 4; j++) char_array_4[j] = base64_chars.find(char_array_4[j]);

    char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
    char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
    char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];

    for (j = 0; (j < i - 1); j++) ret += char_array_3[j];
  }

  return ret;
}

string PrinterProtocol::base64_encode(unsigned char const *bytes_to_code, unsigned int in_len) {
  string ret;
  int i = 0;
  int j = 0;

  unsigned char char_array_3[3];
  unsigned char char_array_4[4];

  while (in_len--) {
    char_array_3[i++] = *(bytes_to_code++);
    if (i == 3) {
      char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
      char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
      char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
      char_array_4[3] = char_array_3[2] & 0x3f;

      for (i = 0; i < 4; i++) ret += base64_chars[char_array_4[i]];
      i = 0;
    }
  }

  if (i) {
    for (j = i; j < 3; j++) char_array_3[j] = '\0';

    char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
    char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
    char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
    char_array_4[3] = char_array_3[2] & 0x3f;

    for (j = 0; (j < i + 1); j++) ret += base64_chars[char_array_4[j]];

    while ((i++ < 3)) ret += '=';
  }
  return ret;
}

/**
 * @brief PrinterProtocol::DateTime
 */
void PrinterProtocol::DateTime(void) {
  char buf[100];
  time_t now;

  time(&now);
  strftime(buf, sizeof (buf), "%Y-%m-%dT%H:%M:%SZ", gmtime(&now));
  m_datetime = buf;
}
