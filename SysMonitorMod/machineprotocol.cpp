#include "machineprotocol.h"

#include <stdio.h>

MachineProtocol::MachineProtocol() {
  jobj_actual = json_object_new_object();
  jobj_previous = json_object_new_object();
}

void MachineProtocol::create_json_object(struct json_object *jobj, const char *a, void *b, int variable_b_type) {
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
      //json_object_object_add(jobj, a, json_object_new_int(static_cast<int>(reinterpret_cast<intptr_t>(b))));
      json_object_object_add(jobj, a, json_object_new_int64(static_cast<int64_t>(reinterpret_cast<intptr_t>(b))));
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



void MachineProtocol::prepare_json_object(void) {
  /* PrintProtocol JSON:

    "type": 3,
    "total_disk": 32212238227,
    "free_disk": 1073741824,
    "total_ram": 4294967296,
    "free_ram": 1073741824,
    "cpu_usage": 23.45,
    "cpu_temperature": 80.09,
    "motherboard_temperature": 56.89,
    "date": "2020-08-10T20:00:31Z"
*/
  
  create_json_object(jobj_actual, "type", (void *)type(), Type::Int); 
  create_json_object(jobj_actual, "total_disk", (void *)total_disk(), Type::Int); 
  create_json_object(jobj_actual, "free_disk", (void *)free_disk(), Type::Int); 
  create_json_object(jobj_actual, "total_ram", (void *)total_ram(), Type::Int); 
  create_json_object(jobj_actual, "free_ram", (void *)free_ram(), Type::Int); 
  create_json_object(jobj_actual, "fan_speed", (void *)fan_speed(), Type::Int); 
  create_json_object(jobj_actual, "cpu_temperature", (void *)&m_cpu_temperature, Type::Double); 
  create_json_object(jobj_actual, "journal_err", (void *)journal_err(), Type::Int); 
  create_json_object(jobj_actual, "date", (void *)date().c_str(), Type::String); 

  //TO DO
  //create_json_object(jobj_actual, "cpu_usage", &m_cpu_usage, Type::Double);
  //create_json_object(jobj_actual, "motherboard_temperature", &m_motherboard_temperature, Type::Double);

  if (json_object_equal(jobj_actual, jobj_previous) != 1) {
    // Copy jobj_actual to jobj_previous
    jobj_previous = json_tokener_parse(json_object_get_string(jobj_actual));
    
    cout << "jobj_previous from str:" << endl;
    cout << "---" << endl;
    cout << json_object_to_json_string_ext(jobj_previous, JSON_C_TO_STRING_SPACED | JSON_C_TO_STRING_PRETTY) << endl;
    cout << "---" << endl;

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

