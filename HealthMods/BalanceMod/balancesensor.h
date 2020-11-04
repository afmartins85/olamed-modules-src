#ifndef BALANCESENSOR_H
#define BALANCESENSOR_H

#include <healthsensors.h>
#include <pthread.h>
#include <string>
#include <vector>

#define MAX_INSIDE_BUFFER_LEN 1024

class BalanceSensor : public HealthSensors {
 public:
  BalanceSensor();
  ~BalanceSensor();
  static void *sensorListen(void *arg);

  double getBalance() override;
  bool isBalanceReady() override;
  bool isSensorOnline() override;
  string getEquipAddress() override;

  inline void setIsReady(bool isReady) { m_isReady = isReady; }

 private:
  pthread_t ptid;
  string m_device;
  int m_ttyPort;
  vector<string> m_queue;
  static pthread_mutex_t m_balMutex;

  bool m_isReady;
  double m_weight;
  bool m_tRunning;

  int setInterfaceAttribsPorts(int fd, int speed, int parity);
  void setBlockingPorts(int fd, int should_block);
  int read_msg_bal(string &data);
  bool extractValidFrame(string &data);
};

#endif  // BALANCESENSOR_H
