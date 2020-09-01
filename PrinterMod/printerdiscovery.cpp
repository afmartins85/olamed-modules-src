#include "printerdiscovery.h"
#include "loguru.hpp"
#include <algorithm>
#include <iostream>
#include <unistd.h>

PrinterDiscovery::PrinterDiscovery() {}

/**
 * @brief PrinterDiscovery::PrinterDiscovery
 */
PrinterDiscovery::PrinterDiscovery(PrinterDiscovery &prt) { this->operator=(prt); }

/**
 * @brief PrinterDiscovery::operator =
 * @param p
 * @return
 */
PrinterDiscovery &PrinterDiscovery::operator=(PrinterDiscovery &p) {
  this->m_interfaces.insert(0, p.interfaces());
  this->m_proto.insert(0, p.proto());
  this->m_devName.insert(0, p.devName());
  this->m_devType.insert(0, p.devType());
  this->m_service.insert(0, p.service());
  return *this;
}

/**
 * @brief PrinterDiscovery::clear
 */
void PrinterDiscovery::clear() {
  this->m_interfaces.clear();
  this->m_proto.clear();
  this->m_devName.clear();
  this->m_devType.clear();
  this->m_service.clear();
}

/**
 * @brief PrinterDiscovery::spaceToDec
 * @param s
 */
void PrinterDiscovery::spaceToDec(std::string &s) {
  for (size_t i = 0; i < s.size(); ++i) {
    if (s[i] == ' ') {
      s.insert(i, "\032");
      i = 0;
    }
  }
}

/**
 * @brief PrinterDiscovery::spaceTransform
 * @param s
 */
void PrinterDiscovery::decToSpace(std::string &s) {
  for (size_t i = 0; i < s.size(); ++i) {
    if (s[i] == '\\') {
      s.replace(i, 4, 1, ' ');
      i = 0;
    }
  }
}
