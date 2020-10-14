#ifndef HEALTHSENSORS_H
#define HEALTHSENSORS_H

class HealthSensors {
public:
  HealthSensors();

  virtual int getSpO2() = 0;
};

#endif // HEALTHSENSORS_H
