#ifndef NETSNMP_H
#define NETSNMP_H

#include <net-snmp/net-snmp-config.h>
#include <net-snmp/net-snmp-includes.h>

/* change the word "define" to "undef" to try the (insecure) SNMPv1 version */
#define DEMO_USE_SNMP_VERSION_3

class NetSNMP {
 public:
  NetSNMP();

  bool openSession();
  void readIodTest();

 private:
  struct snmp_session session, *ss;
  struct snmp_pdu *pdu;
  struct snmp_pdu *response;

  oid anOID[MAX_OID_LEN];
  size_t anOID_len = MAX_OID_LEN;

  struct variable_list *vars;
  int status;

  const char *our_v3_passphrase;
};

#endif  // NETSNMP_H
