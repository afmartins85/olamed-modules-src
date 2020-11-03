#ifndef BALANCESENSOR_H
#define BALANCESENSOR_H

#include <healthsensors.h>
#include <pthread.h>

#define MAX_INSIDE_BUFFER_LEN 1024

class BalanceSensor : public HealthSensors {
 public:
  BalanceSensor();
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
  static pthread_mutex_t m_balMutex;

  bool m_isReady;
  double m_weight;

  int setInterfaceAttribsPorts(int fd, int speed, int parity);
  void setBlockingPorts(int fd, int should_block);
  int read_msg_bal(string &data);
};

#endif  // BALANCESENSOR_H
