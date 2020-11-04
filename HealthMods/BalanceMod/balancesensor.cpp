#include "balancesensor.h"
#include "loguru.hpp"
#include <chrono>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <termios.h>
#include <unistd.h>

pthread_mutex_t BalanceSensor::m_balMutex(PTHREAD_MUTEX_INITIALIZER);

typedef chrono::seconds seconds;
typedef chrono::high_resolution_clock Clock;

BalanceSensor::BalanceSensor() : m_device("/dev/ttyBALANCE"), m_ttyPort(-1), m_isReady(false), m_weight(0) {
  m_queue.clear();

  pthread_create(&ptid, NULL, &BalanceSensor::sensorListen, this);
  pthread_detach(ptid);
}

/**
 * @brief BalanceSensor::~BalanceSensor
 */
BalanceSensor::~BalanceSensor() {
  close(m_ttyPort);
  pthread_join(ptid, nullptr);
}

/**
 * @brief BalanceSensor::sensorListen
 * @param arg
 * @return
 */
void *BalanceSensor::sensorListen(void *arg) {
  string data;
  bool ctrlLogPtr = false;
  //  Clock::time_point t0;
  //  time_t timestamp = 0;

  pthread_setname_np(pthread_self(), __FUNCTION__);
  BalanceSensor *balance = reinterpret_cast<BalanceSensor *>(arg);

  while (true) {
    pthread_mutex_lock(&m_balMutex);
    int ret = balance->read_msg_bal(data);
    if ((ret <= 0) && (data.empty())) {
      if (ret == -1) {
        if (ctrlLogPtr == false) {
          LOG_F(WARNING, "Device is not ready!\n");
          ctrlLogPtr = true;
        }
      }
    } else {
      ctrlLogPtr = false;
      LOG_F(INFO, "Data length %lu", data.length());
      for (size_t i = 0; i < data.length(); ++i) {
        printf("%02X ", data[i]);
      }
      printf("\n");
    }

    pthread_mutex_unlock(&m_balMutex);
    usleep(10);
  }

  pthread_exit(nullptr);
}

/**
 * @brief BalanceSensor::getBalance
 * @return
 */
double BalanceSensor::getBalance() {
  double weight = 0;
  pthread_mutex_lock(&m_balMutex);
  weight = m_weight;
  setIsReady(false);
  pthread_mutex_unlock(&m_balMutex);
  return weight;
}

/**
 * @brief BalanceSensor::isBalanceReady
 * @return
 */
bool BalanceSensor::isBalanceReady() {
  bool isready = false;
  pthread_mutex_lock(&m_balMutex);
  isready = m_isReady;
  pthread_mutex_unlock(&m_balMutex);
  return isready;
}

/**
 * @brief BalanceSensor::isSensorOnline
 * @return
 */
bool BalanceSensor::isSensorOnline() {
  pthread_mutex_lock(&m_balMutex);
  if (m_ttyPort < 0) {
    m_ttyPort = open(m_device.c_str(), O_RDWR | O_NOCTTY | O_SYNC);
    if (m_ttyPort < 0) {
      LOG_F(WARNING, "Device %s is not available!", m_device.c_str());
      pthread_mutex_unlock(&m_balMutex);
      return false;
    }
    setInterfaceAttribsPorts(m_ttyPort, B19200, 0);
    setBlockingPorts(m_ttyPort, 0);
  }
  pthread_mutex_unlock(&m_balMutex);
  return true;
}

/**
 * @brief BalanceSensor::getEquipAddress
 * @return
 */
string BalanceSensor::getEquipAddress() {
  string device("");
  pthread_mutex_lock(&m_balMutex);
  device = m_device;
  pthread_mutex_unlock(&m_balMutex);
  return device;
}

/**
 * @brief BalanceSensor::setInterfaceAttribsPorts
 * @param fd
 * @param speed
 * @param parity
 * @return
 */
int BalanceSensor::setInterfaceAttribsPorts(int fd, int speed, int parity) {
  struct termios tty;
  memset(&tty, 0, sizeof tty);
  if (tcgetattr(fd, &tty) != 0) {
    LOG_F(ERROR, "error %d from tcgetattr", errno);
    return -1;
  }

  cfsetospeed(&tty, speed);
  cfsetispeed(&tty, speed);

  tty.c_cflag = (tty.c_cflag & ~CSIZE) | CS8;  // 8-bit chars
  // disable IGNBRK for mismatched speed tests; otherwise receive break
  // as \000 chars
  tty.c_iflag &= ~IGNBRK;  // ignore break signal
  tty.c_lflag = 0;         // no signaling chars, no echo,
  // no canonical processing
  tty.c_oflag = 0;      // no remapping, no delays
  tty.c_cc[VMIN] = 0;   // read doesn't block
  tty.c_cc[VTIME] = 5;  // 0.5 seconds read timeout

  tty.c_iflag &= ~(IXON | IXOFF | IXANY);  // shut off xon/xoff ctrl

  tty.c_cflag |= (CLOCAL | CREAD);  // ignore modem controls,
  // enable reading
  tty.c_cflag &= ~(PARENB | PARODD);  // shut off parity
  tty.c_cflag |= parity;
  tty.c_cflag &= ~CSTOPB;
  tty.c_cflag &= ~CRTSCTS;

  if (tcsetattr(fd, TCSANOW, &tty) != 0) {
    LOG_F(ERROR, "error %d from tcsetattr", errno);
    return -1;
  }
  return 0;
}

/**
 * @brief BalanceSensor::setBlockingPorts
 * @param fd
 * @param should_block
 */
void BalanceSensor::setBlockingPorts(int fd, int should_block) {
  struct termios tty;
  memset(&tty, 0, sizeof tty);
  if (tcgetattr(fd, &tty) != 0) {
    LOG_F(ERROR, "error %d from tggetattr", errno);
    return;
  }

  tty.c_cc[VMIN] = should_block ? 1 : 0;
  tty.c_cc[VTIME] = 1;  // 0.5 seconds read timeout

  if (tcsetattr(fd, TCSANOW, &tty) != 0) LOG_F(ERROR, "error %d setting term attributes", errno);
}

/**
 * @brief BalanceSensor::read_msg_bal
 * @param data
 * @return
 */
int BalanceSensor::read_msg_bal(string &data) {
  char buffer[MAX_INSIDE_BUFFER_LEN + 1];
  const size_t max_len = MAX_INSIDE_BUFFER_LEN;

  bzero(buffer, max_len + 1);
  if (m_ttyPort > 0) {
    do {
      usleep(10);
      int n = read(m_ttyPort, buffer, max_len);
      LOG_F(INFO, "nBytes %d", n);
      if (n == -1) {
        return n;
      }

      data = buffer;
      while (n > 0) {
        bzero(buffer, max_len + 1);
        if (0 != ioctl(m_ttyPort, FIONREAD, (int *)&n)) {
          return -1;
        }
        if (n != 0) {
          n = read(m_ttyPort, buffer, max_len);
          if (n == -1) return n;
          data += buffer;
          LOG_F(INFO, "nBytes %d", n);
        }
      }
    } while (extractValidFrame(data) == false);

    LOG_F(INFO, "%zu frames in queue", m_queue.size());
    auto frame = m_queue.begin();
    for (; frame != m_queue.end(); ++frame) {
      for (size_t j = 0; j < frame->size(); ++j) {
        printf("%02X ", frame.base()->at(j));
      }
      printf("\n");
    }
    //    m_queue.clear();

    return data.length();
  }

  return -1;
}

/**
 * @brief BalanceSensor::extractValidFrame
 * @param data
 * @return
 */
bool BalanceSensor::extractValidFrame(string &data) {
  string frame;
  int startFrame = -1;
  int endFrame = -1;
  //  bool isEndBuffer = false;

  if (data.empty() == true) {
    return false;
  }

  for (size_t i = 0; i < data.size(); ++i) {
    if (data[i] == 0x02) {
      startFrame = i;
    } else if (data[i] == 0x03) {
      endFrame = i;
      break;
    }
  }

  if ((startFrame >= 0) && (endFrame >= 0)) {
    frame = data.substr(startFrame, (endFrame - startFrame));
  } else {
    frame.append(data);
  }

  if (!m_queue.empty()) {
    auto lastFrame = m_queue.end();
    if (lastFrame->size() >= 7) {
      if (frame[0] == 0x02) {
        printf("NOVO FRAME\n");
        m_queue.push_back(frame);
      }
    } else {
      printf("CAI AQUI frame[%02X]\n", frame[0]);
      lastFrame->append(frame);
      if (lastFrame->at(lastFrame->size() - 1) == 0x03) {
        return true;
      }
    }
  } else {
    if (frame[0] == 0x02) {
      m_queue.push_back(frame);
    }
  }

  data.erase(0, (!endFrame ? 1 : endFrame));
  if (data.size() > 0) {
    return extractValidFrame(data);
  }
  return false;
}
