#ifndef HEALTHSENSORS_H
#define HEALTHSENSORS_H

#include <iostream>
using namespace std;

class HealthSensors {
public:
  HealthSensors();

  virtual double getTemp();
  virtual bool isTempReady();
  virtual int getSpO2();
  virtual bool isSpo2Ready();
  virtual int getPressBloodSys();
  virtual int getPressBloodDia();
  virtual int getPressBloodMean();
  virtual bool isBlooPressReady();
  virtual void isNextRegister(bool next);
  virtual string getEquipAddress();
  virtual bool isEquipAddressReady();
  virtual string getSerialSensor();
  virtual bool isSerialSensorReady();
  virtual bool isSensorOnline();

  virtual double getBalance();
  virtual bool isBalanceReady();
};

#endif // HEALTHSENSORS_H
