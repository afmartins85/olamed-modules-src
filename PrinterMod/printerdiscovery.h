#ifndef PRINTERDISCOVERY_H
#define PRINTERDISCOVERY_H

#include <algorithm>
#include <iostream>
#include <string>

class PrinterDiscovery {
 public:
  PrinterDiscovery();
  PrinterDiscovery(PrinterDiscovery &);

  inline std::string interfaces() const { return m_interfaces; }
  inline void setInterfaces(const std::string &interfaces) {
    this->decToSpace(const_cast<std::string &>(interfaces));
    m_interfaces = interfaces;
  }

  inline std::string proto() const { return m_proto; }
  inline void setProto(const std::string &proto) {
    this->decToSpace(const_cast<std::string &>(proto));
    m_proto = proto;
  }

  inline std::string devName() const { return m_devName; }
  inline void setDevName(const std::string &devName) {
    this->decToSpace(const_cast<std::string &>(devName));
    m_devName = devName;
  }

  inline std::string devType() const { return m_devType; }
  inline void setDevType(const std::string &devType) {
    this->decToSpace(const_cast<std::string &>(devType));
    m_devType = devType;
  }

  inline std::string service() const { return m_service; }
  inline void setService(const std::string &service) {
    this->decToSpace(const_cast<std::string &>(service));
    m_service = service;
  }

  PrinterDiscovery &operator=(PrinterDiscovery &p);

  //! Clean all datas
  void clear();
  //! Convert string to the character space of the decimal representation
  void spaceToDec(std::string &s);
  //! Convert the character space of the decimal representation to string
  void decToSpace(std::string &s);

 private:
  std::string m_interfaces;
  std::string m_proto;
  std::string m_devName;
  std::string m_devType;
  std::string m_service;
};

#endif  // PRINTERDISCOVERY_H
