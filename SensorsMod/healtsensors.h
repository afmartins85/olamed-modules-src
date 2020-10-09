#ifndef HEALTSENSORS_H
#define HEALTSENSORS_H

class HealtSensors {
public:
  HealtSensors();

  virtual void getSpO2() = 0;
};

#endif // HEALTSENSORS_H
