#ifndef HEALTHSENSORS_H
#define HEALTHSENSORS_H

class HealthSensors {
public:
  HealthSensors();

  virtual double getTemp() = 0;
  virtual bool isTempReady() = 0;
  virtual int getSpO2() = 0;
  virtual bool isSpo2Ready() = 0;
  virtual int getBlooPress() = 0;
  virtual bool isBlooPressReady() = 0;
};

#endif // HEALTHSENSORS_H
