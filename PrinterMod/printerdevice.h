#ifndef PRINTERDEVICE_H
#define PRINTERDEVICE_H

#include "printerdiscovery.h"
#include <cups/cups.h>
#include <set>
#include <stdio.h>
#include <unistd.h>

class PrinterDevice {
 public:
  typedef struct {
    int num_dests;
    cups_dest_t* dests;
  } my_user_data_t;

 public:
  PrinterDevice();

  inline bool ptrIsFound() const { return m_ptrIsFound; }
  inline void setPtrIsFound(bool ptrIsFound) { m_ptrIsFound = ptrIsFound; }

  inline std::string address() const { return m_address; }
  inline void setAddress(const std::string& address) { m_address = address; }

  inline std::string hostName() const { return m_hostName; }
  inline void setHostName(const std::string& hostName) { m_hostName = hostName; }

  inline std::string port() const { return m_port; }
  inline void setPort(const std::string& port) { m_port = port; }

  inline std::string features() const { return m_features; }
  inline void setFeatures(const std::string& features) { m_features = features; }

  inline std::string ipp() const { return m_ipp; }

  inline bool isReadyPrinter() const { return m_isReadyPrinter; }
  inline void setIsReadyPrinter(bool isReadyPrinter) { m_isReadyPrinter = isReadyPrinter; }

  inline PrinterDiscovery* prtDiscovery() const { return m_prtDiscovery; }
  inline void setPrtDiscovery(PrinterDiscovery* prtDiscovery) {
    this->m_prtDiscovery = new PrinterDiscovery;
    this->m_prtDiscovery = prtDiscovery;
  }

  //! Add the data printer found pointed for prt
  void addNewPrinter(PrinterDiscovery* prt);
  //! Find Printer on network local.
  bool findPrinter();
  //! Check for printer CUPs config
  bool isReadyCUPS();

 private:
  bool m_ptrIsFound;
  bool m_isReadyPrinter;
  std::set<PrinterDiscovery*> m_deviceList;
  std::string m_address;
  std::string m_hostName;
  std::string m_port;
  std::string m_features;
  std::string m_ipp;
  std::string m_ippSuffix;
  PrinterDiscovery* m_prtDiscovery;
  //! Exec an external shell command for list printer devices in local network
  FILE* avahi_browserExec();
  FILE* avahi_browserExec(const char* custom_avahi);
  //! Prepare an custom avahi_browser command for resolve the host name and the IP address
  bool resolveDataHost(PrinterDiscovery*);
  //! Format ipp url from printer data found
  void setIPPUrl();
  //! Exec lpstat for check status of CUPS printers
  FILE* lpstat(const char* lp);
  //! CUPS Tests
  void testCUPS(my_user_data_t& user_data);

 protected:
  //! Callback for receive the printers destination
  static int cbPrinterDest(my_user_data_t* user_data, unsigned flags, cups_dest_t* dest);
  //! Return all printers destination available
  int getCUPSDests(cups_ptype_t type, cups_ptype_t mask, cups_dest_t** dests);
  bool getCUPSDests(cups_ptype_t type, cups_ptype_t mask, my_user_data_t& user_data);
};

#endif  // PRINTERDEVICE_H
