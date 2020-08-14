#include "printerprotocol.h"

#include <json-c/json.h>
#include <stdio.h>

/**
 * @brief PrinterProtocol::PrinterProtocol
 */
PrinterProtocol::PrinterProtocol() {}

void PrinterProtocol::create_json_object(struct json_object *jobj, const char *a, void *b, int variable_b_type) {
  //  double x = *((double *)b);
  // double x = static_cast<double*>(b);
  // double x = *(reinterpret_cast<double*>(b));
  // double x = static_cast<double>(reinterpret_cast<double*>(b));

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
  struct json_object *jobj;
  jobj = json_object_new_object();

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
  double cyan = cyan_level();

  create_json_object(jobj, "type", (void *)type(), TYPE_INT);
  create_json_object(jobj, "serial", (void *)serial().c_str(), TYPE_STRING);
  create_json_object(jobj, "description", (void *)description().c_str(), TYPE_STRING);
  create_json_object(jobj, "connected", (void *)connected(), TYPE_BOOLEAN);
  create_json_object(jobj, "prints", (void *)prints(), TYPE_INT);
  create_json_object(jobj, "state", (void *)state().c_str(), TYPE_STRING);
  create_json_object(jobj, "error", (void *)error().c_str(), TYPE_STRING);
  create_json_object(jobj, "supply_type", (void *)supply_type().c_str(), TYPE_STRING);
  create_json_object(jobj, "cyan_level", &cyan, TYPE_DOUBLE);
  //  create_json_object(jobj, "magenta_level", (void *)magenta_level(), TYPE_DOUBLE);
  //  create_json_object(jobj, "yellow_level", (void *)yellow_level(), TYPE_DOUBLE);
  //  create_json_object(jobj, "black_level", (void *)black_level(), TYPE_DOUBLE);

  // printf("jobj from str:\n---\n%s\n---\n", json_object_to_json_string_ext(jobj, JSON_C_TO_STRING_SPACED |
  //   JSON_C_TO_STRING_PRETTY));
  cout << "jobj from str:" << endl;
  cout << "---" << endl;
  cout << json_object_to_json_string_ext(jobj, JSON_C_TO_STRING_SPACED | JSON_C_TO_STRING_PRETTY) << endl;
  cout << "---" << endl;
}
