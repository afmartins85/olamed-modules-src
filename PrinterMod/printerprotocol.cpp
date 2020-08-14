#include "printerprotocol.h"

#include <json-c/json.h>
#include <stdio.h>

/**
 * @brief PrinterProtocol::PrinterProtocol
 */
PrinterProtocol::PrinterProtocol() {}

void PrinterProtocol::create_json_object(struct json_object *jobj, const char *a, void *b, int variable_b_type) {
  switch (variable_b_type) {
    case TYPE_NULL:
      break;

    case TYPE_BOOLEAN:
      break;

    case TYPE_DOUBLE:
      break;

    case TYPE_INT:
      json_object_object_add(jobj, a, json_object_new_int(*((int *)b)));
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

  create_json_object(jobj, "serial", (void *)serial().c_str(), TYPE_STRING);

  // printf("jobj from str:\n---\n%s\n---\n", json_object_to_json_string_ext(jobj, JSON_C_TO_STRING_SPACED |
  //   JSON_C_TO_STRING_PRETTY));
  cout << "jobj from str:" << endl;
  cout << "---" << endl;
  cout << json_object_to_json_string_ext(jobj, JSON_C_TO_STRING_SPACED | JSON_C_TO_STRING_PRETTY) << endl;
  cout << "---" << endl;
}
