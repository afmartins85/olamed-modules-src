#ifndef HEALTHSENSORS_H
#define HEALTHSENSORS_H

#include <iostream>
using namespace std;

class HealthSensors {
public:
  HealthSensors();

  virtual double getTemp() = 0;
  virtual bool isTempReady() = 0;
  virtual double getSpO2() = 0;
  virtual bool isSpo2Ready() = 0;
  virtual int getPressBloodSys() = 0;
  virtual int getPressBloodDia() = 0;
  virtual int getPressBloodMean() = 0;
  virtual bool isBlooPressReady() = 0;
  virtual void isNextRegister(bool next) = 0;
  virtual string getEquipAddress() = 0;
  virtual bool isEquipAddressReady() = 0;
};

#endif // HEALTHSENSORS_H
