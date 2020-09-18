#ifndef NETSNMP_H
#define NETSNMP_H

#include "printerdevice.h"
#include "printerprotocol.h"
#include <net-snmp/net-snmp-config.h>
#include <net-snmp/net-snmp-includes.h>

class NetSNMP {
 public:
  NetSNMP();

  bool openSession(PrinterDevice &);
  void closeSession();
  bool readMIBDescription(PrinterProtocol &);
  bool readMIBStatus(PrinterProtocol &);
  bool readMIBErrors(PrinterProtocol &);
  bool readMIBLifeCount(PrinterProtocol &);
  bool processResponse(void *);
  std::string decodePrinterStatus(long);

 private:
  bool m_sOpen;
  struct snmp_session session, *ss;
  struct snmp_pdu *pdu;
  struct snmp_pdu *response;
  struct tree *mib_tree;

  struct variable_list *vars;
  int status;
};

#endif  // NETSNMP_H
