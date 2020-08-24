#ifndef MACHINEPROTOCOL_H
#define MACHINEPROTOCOL_H

#include <ctype.h>
#include <iostream>
#include <json-c/json.h>

using namespace std;

class MachineProtocol {

  public:
    MachineProtocol();

    inline uint64_t type() { return m_type; }
    inline void setType(const uint64_t type) { m_type = type; }

    inline uint64_t total_disk() { return m_total_disk; }
    inline void setTotal_disk(const uint64_t total_disk) { m_total_disk = total_disk; }

    inline uint64_t free_disk() { return m_free_disk; }
    inline void setFree_disk(const uint64_t free_disk) { m_free_disk = free_disk; }

    inline uint64_t total_ram() { return m_total_ram; }
    inline void setTotal_ram(const uint64_t total_ram) { m_total_ram = total_ram; }

    inline uint64_t free_ram() { return m_free_ram; }
    inline void setFree_ram(const uint64_t free_ram) { m_free_ram = free_ram; }

    inline string date() { return m_date; }
    inline void setDate(const string &date) { m_date = date; }
    
    inline string json_message() const { return m_json_message; }
    inline void setJson_message(const string &json_message) { m_json_message = json_message; }

    void create_json_object(struct json_object *jobj, const char *a, void *b, int variable_b_type);
    void prepare_json_object(void);
 
  private:
    uint64_t m_type;
    uint64_t m_total_disk;
    uint64_t m_free_disk;
    uint64_t m_total_ram;
    uint64_t m_free_ram;
    string m_date;

    string m_json_message;
    struct json_object *jobj_actual;
    struct json_object *jobj_previous;

    enum Type : int { Null, Boolean, Double, Int, String, Object, Array };
};


#endif  // MACHINEPROTOCOL_H

