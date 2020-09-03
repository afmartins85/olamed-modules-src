#include "netsnmp.h"
#include "loguru.hpp"
#include <stdio.h>
#include <string.h>

/**
 * @brief NetSNMP::NetSNMP
 */
NetSNMP::NetSNMP() {
  /*
   * Initialize the SNMP library
   */
  init_snmp("PrinterMod");
}

/**
 * @brief NetSNMP::openSession
 * @return
 */
bool NetSNMP::openSession(PrinterDevice &printer) {
  /*
   * Initialize a "session" that defines who we're going to talk to
   */
  snmp_sess_init(&session); /* set up defaults */
  session.peername = const_cast<char *>(printer.address().c_str());

  /* set the SNMP version number */
  session.version = SNMP_VERSION_2c;

  /* set the SNMPv1 community name used for authentication */
  session.community = reinterpret_cast<u_char *>(const_cast<char *>("public"));
  session.community_len = strlen(reinterpret_cast<char *>(session.community));

  /*
   * Open the session
   */
  ss = snmp_open(&session); /* establish the session */

  if (!ss) {
    snmp_perror("ack");
    snmp_log(LOG_ERR, "something horrible happened!!!\n");
    return false;
  }

  return true;
}

/**
 * @brief NetSNMP::closeSession
 */
void NetSNMP::closeSession() {
  /*
   * Clean up:
   *  1) free the response.
   *  2) close the session.
   */
  if (response) snmp_free_pdu(response);
  snmp_close(ss);
}

/**
 * @brief NetSNMP::readMIBDescription
 * @param prtProto
 */
void NetSNMP::readMIBDescription(PrinterProtocol &prtProto) {
  /*
   * Create the PDU for the data for our request.
   */
  pdu = snmp_pdu_create(SNMP_MSG_GET);
  oid anOID[MAX_OID_LEN];
  size_t anOID_len = MAX_OID_LEN;

  // prtMarkerSuppliesDescription
  read_objid("SNMPv2-SMI::mib-2.43.11.1.1.6.1.1", anOID, &anOID_len);
  snmp_add_null_var(pdu, anOID, anOID_len);
  /*
   * Send the Request out.
   */
  status = snmp_synch_response(ss, pdu, &response);
  std::string suppliesDescription;
  processResponse(&suppliesDescription);
  prtProto.setSupply_type(suppliesDescription);
  LOG_F(INFO, "supply_type: %s", suppliesDescription.c_str());

  // prtGeneralPrinterName
  pdu = snmp_pdu_create(SNMP_MSG_GET);
  //  read_objid("SNMPv2-SMI::mib-2.43.5.1.1.16.1", anOID, &anOID_len);
  read_objid("HOST-RESOURCES-MIB::hrDeviceDescr.1", anOID, &anOID_len);
  snmp_add_null_var(pdu, anOID, anOID_len);

  status = snmp_synch_response(ss, pdu, &response);
  std::string printerModel;
  processResponse(&printerModel);

  // prtGeneralSerialNumber
  pdu = snmp_pdu_create(SNMP_MSG_GET);
  read_objid("SNMPv2-SMI::mib-2.43.5.1.1.17.1", anOID, &anOID_len);
  snmp_add_null_var(pdu, anOID, anOID_len);

  status = snmp_synch_response(ss, pdu, &response);
  std::string printerSerialNumber;
  processResponse(&printerSerialNumber);
  prtProto.setSerial(printerSerialNumber);

  std::string description(printerModel);
  description.append(", ");
  description.append(suppliesDescription);
  description.append(", ");
  description.append(printerSerialNumber);
  prtProto.setDescription(description);
  LOG_F(INFO, "description: %s", description.c_str());
}

/**
 * @brief NetSNMP::readMIBStatus
 * @param prtProto
 */
void NetSNMP::readMIBStatus(PrinterProtocol &prtProto) {
  // HOST-RESOURCES-MIB::hrPrinterStatus
  pdu = snmp_pdu_create(SNMP_MSG_GET);
  oid anOID[MAX_OID_LEN];
  size_t anOID_len = MAX_OID_LEN;
  read_objid("HOST-RESOURCES-MIB::hrPrinterStatus.1", anOID, &anOID_len);
  snmp_add_null_var(pdu, anOID, anOID_len);

  status = snmp_synch_response(ss, pdu, &response);
  long printerStatus;
  processResponse(&printerStatus);
  prtProto.setState(this->decodePrinterStatus(printerStatus));
  LOG_F(INFO, "status: %s(%ld)", this->decodePrinterStatus(printerStatus).c_str(), printerStatus);
}

/**
 * @brief NetSNMP::readMIBErrors
 * @param prtProto
 */
void NetSNMP::readMIBErrors(PrinterProtocol &prtProto) {
  //  HOST-RESOURCES-MIB::hrPrinterDetectedErrorState
  pdu = snmp_pdu_create(SNMP_MSG_GET);
  oid anOID[MAX_OID_LEN];
  size_t anOID_len = MAX_OID_LEN;
  read_objid("HOST-RESOURCES-MIB::hrPrinterDetectedErrorState.1", anOID, &anOID_len);
  snmp_add_null_var(pdu, anOID, anOID_len);

  status = snmp_synch_response(ss, pdu, &response);
  std::string printerError;
  processResponse(&printerError);
  prtProto.setError(printerError);
  LOG_F(INFO, "error: %s", printerError.c_str());
}

/**
 * @brief NetSNMP::readMIBLifeCount
 * @param prtProto
 */
void NetSNMP::readMIBLifeCount(PrinterProtocol &prtProto) {
  u_long lifeCount;
  //  HOST-RESOURCES-MIB::hrPrinterDetectedErrorState
  pdu = snmp_pdu_create(SNMP_MSG_GET);
  oid anOID[MAX_OID_LEN];
  size_t anOID_len = MAX_OID_LEN;
  read_objid("SNMPv2-SMI::mib-2.43.10.2.1.4.1.1", anOID, &anOID_len);
  snmp_add_null_var(pdu, anOID, anOID_len);
  status = snmp_synch_response(ss, pdu, &response);
  processResponse(&lifeCount);
  prtProto.setPrints(lifeCount);
  LOG_F(INFO, "Life Count: %ld", lifeCount);
}

/**
 * @brief NetSNMP::processResponse
 * @param ptr
 * @return
 */
bool NetSNMP::processResponse(void *ptr) {
  if (status == STAT_SUCCESS && response->errstat == SNMP_ERR_NOERROR) {
    /* manipulate the information ourselves */
    for (vars = response->variables; vars; vars = vars->next_variable) {
      int count = 1;
      if (vars->type == ASN_OCTET_STR) {
        char *sp = (char *)malloc(1 + vars->val_len);
        printf("vars_len %zu", vars->val_len);
        memcpy(sp, vars->val.string, vars->val_len);
        sp[vars->val_len] = '\0';
        static_cast<std::string *>(ptr)->insert(0, sp);
        free(sp);
      } else if (vars->type == ASN_INTEGER) {
        *static_cast<long *>(ptr) = *vars->val.integer;
      } else if ((vars->type & ASN_APPLICATION) == ASN_APPLICATION) {
        *static_cast<u_long *>(ptr) = vars->val.counter64->high;
      } else {
        LOG_F(WARNING, "value #%d Data type (%d) is not supported! Ack!\n", count++, vars->type);
        return false;
      }
    }
  } else {
    /*
     * FAILURE: print what went wrong!
     */
    if (status == STAT_SUCCESS) {
      LOG_F(ERROR, "Error in packet\nReason: %s\n", snmp_errstring(response->errstat));
    } else {
      snmp_sess_perror("snmpget", ss);
    }
    return false;
  }
  return true;
}

/**
 * @brief NetSNMP::decodePrinterStatus
 * @return
 */
string NetSNMP::decodePrinterStatus(long code) {
  std::string status;
  switch (code) {
    case 1:
      status.insert(0, "other");
      break;
    case 2:
      status.insert(0, "unknown");
      break;
    case 3:
      status.insert(0, "idle");
      break;
    case 4:
      status.insert(0, "printing");
      break;
    case 5:
      status.insert(0, "warmup");
      break;
  }
  return status;
}
