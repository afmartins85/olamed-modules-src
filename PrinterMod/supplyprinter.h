#ifndef SUPPLYPRINTER_H
#define SUPPLYPRINTER_H

#include <ctype.h>
#include <iostream>

using namespace std;

class SupplyPrinter {
 public:
  SupplyPrinter();

  inline string supType() const { return m_supType; }
  inline void setSupType(const string &supType) { m_supType = supType; }

  inline uint8_t cyanLevel() const { return m_cyanLevel; }
  inline void setCyanLevel(const uint8_t &cyanLevel) { m_cyanLevel = cyanLevel; }

  inline uint8_t magentaLevel() const { return m_magentaLevel; }
  inline void setMagentaLevel(const uint8_t &magentaLevel) { m_magentaLevel = magentaLevel; }

  inline uint8_t yellowLevel() const { return m_yellowLevel; }
  inline void setYellowLevel(const uint8_t &yellowLevel) { m_yellowLevel = yellowLevel; }

  inline uint8_t blackLevel() const { return m_blackLevel; }
  inline void setBlackLevel(const uint8_t &blackLevel) { m_blackLevel = blackLevel; }

 private:
  string m_supType;
  uint8_t m_cyanLevel;
  uint8_t m_magentaLevel;
  uint8_t m_yellowLevel;
  uint8_t m_blackLevel;
};

#endif  // SUPPLYPRINTER_H
