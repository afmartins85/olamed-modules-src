#include "printerprotocol.h"

#include <stdio.h>

/**
 * @brief PrinterProtocol::PrinterProtocol
 */
PrinterProtocol::PrinterProtocol() {

  jobj_actual = json_object_new_object();
  jobj_previous = json_object_new_object();

}

void PrinterProtocol::create_json_object(struct json_object *jobj, const char *a, void *b, int variable_b_type) {

  switch (variable_b_type) {
    case TYPE_NULL:
      break;

    case TYPE_BOOLEAN:
      json_object_object_add(jobj, a, json_object_new_boolean((bool)b));
      break;

    case TYPE_DOUBLE: {
      // json_object_object_add(jobj, a, json_object_new_double(*((double *)(b))));
      double x = *((double *)b);
      json_object_object_add(jobj, a, json_object_new_double(x));
      break;
    }

    case TYPE_INT:
      json_object_object_add(jobj, a, json_object_new_int(static_cast<int>(reinterpret_cast<intptr_t>(b))));
      break;

    case TYPE_STRING:
      json_object_object_add(jobj, a, json_object_new_string((const char *)b));
      break;

    case TYPE_OBJECT:
      break;

    case TYPE_ARRAY:
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

  create_json_object(jobj_actual, "type", (void *)type(), TYPE_INT);
  create_json_object(jobj_actual, "serial", (void *)serial().c_str(), TYPE_STRING);
  create_json_object(jobj_actual, "description", (void *)description().c_str(), TYPE_STRING);
  create_json_object(jobj_actual, "connected", (void *)connected(), TYPE_BOOLEAN);
  create_json_object(jobj_actual, "prints", (void *)prints(), TYPE_INT);
  create_json_object(jobj_actual, "state", (void *)state().c_str(), TYPE_STRING);
  create_json_object(jobj_actual, "error", (void *)error().c_str(), TYPE_STRING);
  create_json_object(jobj_actual, "supply_type", (void *)supply_type().c_str(), TYPE_STRING);
  create_json_object(jobj_actual, "cyan_level", &m_cyan_level, TYPE_DOUBLE);
  create_json_object(jobj_actual, "magenta_level", &m_magenta_level, TYPE_DOUBLE);
  create_json_object(jobj_actual, "yellow_level", &m_yellow_level, TYPE_DOUBLE);
  create_json_object(jobj_actual, "black_level", &m_black_level, TYPE_DOUBLE);

  if(json_object_equal(jobj_actual, jobj_previous) != 1) {

    //Copy jobj_actual to jobj_previous
    jobj_previous = json_tokener_parse(json_object_get_string(jobj_actual));

    cout << "jobj_previous from str:" << endl;
    cout << "---" << endl;
    cout << json_object_to_json_string_ext(jobj_previous, JSON_C_TO_STRING_SPACED | JSON_C_TO_STRING_PRETTY) << endl;
    cout << "---" << endl;

    //cout << "jobj_actual from str:" << endl;
    //cout << "---" << endl;
    //cout << json_object_to_json_string_ext(jobj_actual, JSON_C_TO_STRING_SPACED | JSON_C_TO_STRING_PRETTY) << endl;
    //cout << "---" << endl;

    setJson_message(json_object_to_json_string_ext(jobj_actual, JSON_C_TO_STRING_SPACED | JSON_C_TO_STRING_PRETTY));

    cout << "jobj_actual from str:" << endl;
    cout << "---" << endl;
    cout << m_json_message << endl;
    cout << "---" << endl;
  } else {
    cout << "---" << endl;
    cout << "jobj_actual and job_actual are equals:" << endl;
    cout << "---" << endl;
  }
}
