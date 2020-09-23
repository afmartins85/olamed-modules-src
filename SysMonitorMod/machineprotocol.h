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

    inline int64_t total_disk() { return m_total_disk; }
    inline void setTotal_disk(const int64_t total_disk) { m_total_disk = total_disk; }

    inline int64_t free_disk() { return m_free_disk; }
    inline void setFree_disk(const int64_t free_disk) { m_free_disk = free_disk; }

    inline int64_t total_ram() { return m_total_ram; }
    inline void setTotal_ram(const int64_t total_ram) { m_total_ram = total_ram; }

    inline int64_t free_ram() { return m_free_ram; }
    inline void setFree_ram(const int64_t free_ram) { m_free_ram = free_ram; }

    inline int64_t fan_speed() { return m_fan_speed; }
    inline void setFan_speed(const int64_t fan_speed) { m_fan_speed = fan_speed; }
    
    inline double cpu_temperature() { return m_cpu_temperature; }
    inline void setCPU_temperature(const double cpu_temperature) { m_cpu_temperature = cpu_temperature; }
   
    inline string date() { return m_date; }
    inline void setDate(const string &date) { m_date = date; }

    inline int64_t journal_err() const { return m_journal_err; }
    inline void setJournal_err(int64_t journal_err) { m_journal_err = journal_err; }

    inline string graceful_shutdown() { return m_graceful_shutdown; }
    inline void setGraceful_shutdown(const string &graceful_shutdown) { m_graceful_shutdown = graceful_shutdown; }
    
    inline string json_message() const { return m_json_message; }
    inline void setJson_message(const string &json_message) { m_json_message = json_message; }

    void create_json_object(struct json_object *jobj, const char *a, void *b, int variable_b_type);
    void prepare_json_object(void);
 
  private:
    uint64_t m_type;
    int64_t m_total_disk;
    int64_t m_free_disk;
    int64_t m_total_ram;
    int64_t m_free_ram;
    int64_t m_fan_speed;
    double m_cpu_temperature;
    string m_date;
    int64_t m_journal_err;
    string m_graceful_shutdown;
    string m_json_message;
    struct json_object *jobj_actual;
    struct json_object *jobj_previous;

    enum Type : int { Null, Boolean, Double, Int, String, Object, Array };
};


#endif  // MACHINEPROTOCOL_H

