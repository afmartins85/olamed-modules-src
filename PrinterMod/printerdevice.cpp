#include "printerdevice.h"
#include "loguru.hpp"
#include <chrono>
#include <cups/cups.h>
#include <iostream>
#include <string.h>
#include <string>
#include <sys/wait.h>
#include <thread>
#include <unistd.h>

/**
 * @brief PrinterDevice::PrinterDevice
 */
PrinterDevice::PrinterDevice() : m_ptrIsFound(false) {
  this->m_ipp.insert(0, "ipp://");
  this->m_ippSuffix.insert(0, "/ipp/print");
}

/**
 * @brief PrinterDevice::addNewPrinter
 * @param prt
 */
void PrinterDevice::addNewPrinter(PrinterDiscovery *prt) { this->m_deviceList.insert(prt); }

/**
 * @brief PrinterDevice::findPrinter
 * @return
 */
bool PrinterDevice::findPrinter() {
  PrinterDiscovery discovery;
  char buff[1024];
  FILE *f = avahi_browserExec();

  if (f == nullptr) {
    return false;
  }

  LOG_SCOPE_FUNCTION(INFO);

  int countDelimiter = 0;
  while (fgets(buff, 1024, f)) {
    for (size_t offset = 0; offset < strlen(buff); ++offset) {
      std::string iface;
      std::string proto;
      std::string devName;
      std::string typeName;
      std::string service;

      for (size_t idx = offset; idx < strlen(buff); ++idx) {
        // Ignora a primeira entrada
        if ((buff[idx] != ';') && (!countDelimiter)) {
          continue;
        }

        if ((buff[idx] == ';') || (buff[idx] == '\n')) {
          if (buff[idx] == '\n') {
            countDelimiter = 0;
            offset = strlen(buff);
            discovery.setInterfaces(iface);
            discovery.setProto(proto);
            discovery.setDevName(devName);
            discovery.setDevType(typeName);
            discovery.setService(service);
            break;
          } else {
            countDelimiter++;
          }
        } else {
          switch (countDelimiter) {
            case 1:
              iface.push_back(buff[idx]);
              break;
            case 2:
              proto.push_back(buff[idx]);
              break;
            case 3:
              devName.push_back(buff[idx]);
              break;
            case 4:
              typeName.push_back(buff[idx]);
              break;
            case 5:
              service.push_back(buff[idx]);
              break;
          }
        }
        offset = idx;
      }
      PrinterDiscovery *newPrinter = new PrinterDiscovery(discovery);
      this->addNewPrinter(newPrinter);
      discovery.clear();
    }
  }
  fclose(f);

  auto it = this->m_deviceList.begin();
  for (; it != this->m_deviceList.end(); ++it) {
    if (this->resolveDataHost(*it) == true) {
      this->setPrtDiscovery(*it);
      // Set true if successful discovery printer
      this->setPtrIsFound(true);
      break;
    }
  }

  return true;
}

/**
 * @brief PrinterDevice::avahi_browserExec
 * @return
 */
FILE *PrinterDevice::avahi_browserExec() {
  const char avahi[] = "avahi-browse --all -t -p -v | grep Printer";
  int fd[2];
  int read_fd, write_fd;
  int pid;

  pipe(fd);
  read_fd = fd[0];
  write_fd = fd[1];

  pid = fork();
  if (pid == 0) {
    close(read_fd);
    dup2(write_fd, 1);
    close(write_fd);
    execl("/bin/sh", "sh", "-c", avahi, NULL);
    return nullptr;
  } else {
    close(write_fd);
    return fdopen(read_fd, "r");
  }
}

/**
 * @brief PrinterDevice::avahi_browserExec
 * @param custom_avahi
 * @return
 */
FILE *PrinterDevice::avahi_browserExec(const char *custom_avahi) {
  int fd[2];
  int read_fd, write_fd;
  int pid;

  pipe(fd);
  read_fd = fd[0];
  write_fd = fd[1];

  pid = fork();
  if (pid == 0) {
    close(read_fd);
    dup2(write_fd, 1);
    close(write_fd);
    execl("/bin/sh", "sh", "-c", custom_avahi, NULL);
    return nullptr;
  } else {
    close(write_fd);
    return fdopen(read_fd, "r");
  }
}

/**
 * @brief PrinterDevice::resolveDataHost
 * @param p_ptr
 * @return
 */
bool PrinterDevice::resolveDataHost(PrinterDiscovery *p_ptr) {
  char buff[1024];
  std::string stat("avahi-browse --all -t -p -r ");

  if (p_ptr->devType().compare("Internet Printer")) {
    return false;
  }

  LOG_F(INFO, "iFace: %s", p_ptr->interfaces().c_str());
  LOG_F(INFO, "protocol: %s", p_ptr->proto().c_str());
  LOG_F(INFO, "device name: %s", p_ptr->devName().c_str());
  LOG_F(INFO, "device type: %s", p_ptr->devType().c_str());
  LOG_F(INFO, "service: %s", p_ptr->service().c_str());

  // Make custom grep
  std::string grep("| grep \";");
  grep.append(p_ptr->devType());
  grep.append("\" | grep rp");
  // Append custom grep statement
  stat.append(grep);

  FILE *f = avahi_browserExec(stat.c_str());
  if (f == nullptr) {
    return false;
  }

  LOG_SCOPE_FUNCTION(INFO);

  int countDelimiter = 0;
  while (fgets(buff, 1024, f)) {
    for (size_t offset = 0; offset < strlen(buff); ++offset) {
      std::string hostName;
      std::string hostIp;
      std::string port;
      std::string features;

      for (size_t idx = offset; idx < strlen(buff); ++idx) {
        // Ignora a primeira entrada
        if ((buff[idx] != ';') && (countDelimiter < 5)) {
          continue;
        }

        if ((buff[idx] == ';') || (buff[idx] == '\n')) {
          if (buff[idx] == '\n') {
            countDelimiter = 0;
            this->setAddress(hostIp);
            this->setHostName(hostName);
            this->setPort(port);
            this->setIPPUrl();
            this->setFeatures(features);
            offset = strlen(buff);
            break;
          } else {
            countDelimiter++;
          }
        } else {
          switch (countDelimiter) {
            case 6:
              hostName.push_back(buff[idx]);
              break;
            case 7:
              hostIp.push_back((buff[idx]));
              break;
            case 8:
              port.push_back(buff[idx]);
              break;
            case 9:
              features.push_back(buff[idx]);
              break;
          }
        }
        offset = idx;
      }
    }
  }

  LOG_F(INFO, "Host Name: %s", this->hostName().c_str());
  LOG_F(INFO, "host Ip: %s", this->address().c_str());
  LOG_F(INFO, "port: %s", this->port().c_str());
  LOG_F(INFO, "ipp: %s", this->ipp().c_str());
  LOG_F(INFO, "features: %s", this->features().c_str());

  return true;
}

/**
 * @brief PrinterDevice::setIPPUrl
 */
void PrinterDevice::setIPPUrl() {
  this->m_ipp.append(this->hostName());
  this->m_ipp.append(":");
  this->m_ipp.append(this->port());
  this->m_ipp.append(this->m_ippSuffix);
}

/**
 * @brief PrinterDevice::isReadyCUPS
 * @return
 */
bool PrinterDevice::isReadyCUPS() {
  char buff[1024];
  std::string queueName(this->prtDiscovery()->devName());
  std::replace(queueName.begin(), queueName.end(), ' ', '_');
  std::string lp_cmd("lpstat -s");

  my_user_data_t user_data = {0, NULL};
  getCUPSDests(0, 0, user_data);

  for (int i = 0; i < user_data.num_dests; ++i) {
    LOG_F(INFO, "Destination available: %s", user_data.dests->name);
    LOG_F(INFO, "Options: %s", user_data.dests->options->name);
    LOG_F(INFO, "Options value: %s", user_data.dests->options->value);
    LOG_F(INFO, "Instance: %s", user_data.dests->instance);
    LOG_F(INFO, "IsDefault %s", (user_data.dests->is_default ? "true" : "false"));
    LOG_F(INFO, "NumOptions %d", user_data.dests->num_options);

    if (user_data.dests->is_default) {
      this->setIsReadyPrinter(true);
      return true;
    }
  }

  FILE *file = this->lpstat(lp_cmd.c_str());
  if (file == nullptr) {
    return false;
  }

  while (fgets(buff, 1024, file)) {
    if (strstr(buff, "No destinations added") != nullptr) {
      LOG_F(INFO, "Printer system default destination %s", queueName.c_str());
      this->setIsReadyPrinter(true);
      return true;
    } else {
      LOG_F(WARNING, "Printer not is ready!!! Setting CUPS...");
      lp_cmd.clear();
      lp_cmd = "lpadmin -p ";
      lp_cmd.append(queueName);
      lp_cmd.append(" -E -v ");
      lp_cmd.append(this->ipp());
      lp_cmd.append(" -m everywhere");
      LOG_F(INFO, "%s", lp_cmd.c_str());
      if (!system(lp_cmd.c_str())) {
        LOG_F(INFO, "Set the default printer: %s", queueName.c_str());
        lp_cmd.clear();
        lp_cmd = "lpoptions -d ";
        lp_cmd.append(queueName);
        LOG_F(INFO, "%s", lp_cmd.c_str());
        if (!system(lp_cmd.c_str())) {
          LOG_F(INFO, "Set Printer Succesfull!!");
          break;
        } else {
          LOG_F(ERROR, "Fail Set Default Printer!!");
          return false;
        }
      }
    }
  }

  this->setIsReadyPrinter(true);
  return true;
}

/**
 * @brief PrinterDevice::lpstat
 * @param lp
 * @return
 */
FILE *PrinterDevice::lpstat(const char *lp) {
  int fd[2];
  int read_fd, write_fd;
  int pid;

  pipe(fd);
  read_fd = fd[0];
  write_fd = fd[1];

  pid = fork();
  if (pid == 0) {
    close(read_fd);
    dup2(write_fd, 1);
    close(write_fd);
    execl("/bin/sh", "sh", "-c", lp, NULL);
    return nullptr;
  } else {
    close(write_fd);
    return fdopen(read_fd, "r");
  }
}

/**
 * @brief PrinterDevice::testCUPS
 * @param user_data
 */
void PrinterDevice::testCUPS(my_user_data_t &user_data) {
  const char *model = cupsGetOption("printer-make-and-model", user_data.dests->num_options, user_data.dests->options);
  LOG_F(INFO, "Model %s", model);

  cups_dinfo_t *info = cupsCopyDestInfo(CUPS_HTTP_DEFAULT, user_data.dests);

  //  if (cupsCheckDestSupported(CUPS_HTTP_DEFAULT, user_data.dests, info, CUPS_FINISHINGS, NULL)) {
  //    ipp_attribute_t *finishings = cupsFindDestSupported(CUPS_HTTP_DEFAULT, user_data.dests, info, CUPS_FINISHINGS);
  //    int count = ippGetCount(finishings);

  //    puts("finishings supported:");
  //    for (int i = 0; i < count; i++) printf("  %d\n", ippGetInteger(finishings, i));
  //  } else
  //    puts("finishings not supported.");

  //  ipp_attribute_t *attrs = cupsFindDestSupported(CUPS_HTTP_DEFAULT, user_data.dests, info,
  //  "job-creation-attributes"); int count = ippGetCount(attrs);

  //  for (int i = 0; i < count; i++) puts(ippGetString(attrs, i, NULL));

  //  ipp_attribute_t *ready_finishings = cupsFindDestReady(CUPS_HTTP_DEFAULT, user_data.dests, info, CUPS_FINISHINGS);

  //  if (ready_finishings != NULL) {
  //    int i, count = ippGetCount(ready_finishings);

  //    puts("finishings ready:");
  //    for (i = 0; i < count; i++) printf("  %d\n", ippGetInteger(ready_finishings, i));
  //  } else
  //    puts("no finishings are ready.");

  int job_id = 0;
  int num_options = 0;
  cups_option_t *options = NULL;

  num_options = cupsAddOption(CUPS_COPIES, "1", num_options, &options);
  num_options = cupsAddOption(CUPS_FINISHINGS, CUPS_FINISHINGS_NONE, num_options, &options);
  num_options = cupsAddOption(CUPS_MEDIA, CUPS_MEDIA_A4, num_options, &options);
  num_options = cupsAddOption(CUPS_MEDIA_SOURCE, CUPS_MEDIA_SOURCE_AUTO, num_options, &options);
  num_options = cupsAddOption(CUPS_MEDIA_TYPE, CUPS_MEDIA_TYPE_PLAIN, num_options, &options);
  num_options = cupsAddOption(CUPS_NUMBER_UP, "1", num_options, &options);
  num_options = cupsAddOption(CUPS_ORIENTATION, CUPS_ORIENTATION_PORTRAIT, num_options, &options);
  num_options = cupsAddOption(CUPS_PRINT_COLOR_MODE, CUPS_PRINT_COLOR_MODE_AUTO, num_options, &options);
  num_options = cupsAddOption(CUPS_PRINT_QUALITY, CUPS_PRINT_QUALITY_NORMAL, num_options, &options);
  num_options = cupsAddOption(CUPS_SIDES, CUPS_SIDES_TWO_SIDED_PORTRAIT, num_options, &options);

  if (cupsCreateDestJob(CUPS_HTTP_DEFAULT, user_data.dests, info, &job_id, "Bradesco_28042020_153516.pdf", num_options,
                        options) == IPP_STATUS_OK)
    printf("Created job: %d\n", job_id);
  else
    printf("Unable to create job: %s\n", cupsLastErrorString());

  FILE *fp = fopen("/home/anderson/Downloads/Bradesco_28042020_153516.pdf", "rb");
  size_t bytes;
  char buffer[65536];
  //  std::string fPDF;
  //    while ((bytes = fread(buffer, 1, sizeof(buffer), fp)) > 0)
  //      ;

  //    printf("size %zu\n", bytes);
  //    bytes = strlen(buffer);
  //    printf("%s", buffer);
  //  while (fgets(buffer, 1024, fp)) {
  //    fPDF.append(buffer);
  //  }

  if (cupsStartDestDocument(CUPS_HTTP_DEFAULT, user_data.dests, info, job_id, "Bradesco_28042020_153516.pdf",
                            CUPS_FORMAT_PDF, num_options, NULL, 1) == HTTP_STATUS_CONTINUE) {
    while ((bytes = fread(buffer, 1, sizeof(buffer), fp)) > 0)
      if (cupsWriteRequestData(CUPS_HTTP_DEFAULT, buffer, strlen(buffer)) != HTTP_STATUS_CONTINUE) break;

    //    cupsWriteRequestData(CUPS_HTTP_DEFAULT, fPDF.c_str(), fPDF.size());
    //    std::this_thread::sleep_for(std::chrono::seconds(4));
    if (cupsFinishDestDocument(CUPS_HTTP_DEFAULT, user_data.dests, info) == IPP_STATUS_OK)
      puts("Document send succeeded.");
    else
      printf("Document send failed: %s\n", cupsLastErrorString());
  }

  fclose(fp);
}

/**
 * @brief PrinterDevice::cbPrinterDest
 * @param user_data
 * @param flags
 * @param dest
 * @return
 */
int PrinterDevice::cbPrinterDest(PrinterDevice::my_user_data_t *user_data, unsigned flags, cups_dest_t *dest) {
  if (flags & CUPS_DEST_FLAGS_REMOVED) {
    /*
     * Remove destination from array...
     */
    user_data->num_dests = cupsRemoveDest(dest->name, dest->instance, user_data->num_dests, &(user_data->dests));
  } else {
    /*
     * Add destination to array...
     */
    user_data->num_dests = cupsCopyDest(dest, user_data->num_dests, &(user_data->dests));
  }

  return (1);
}

/**
 * @brief PrinterDevice::getCUPSDests
 * @param type
 * @param mask
 * @param dests
 * @return
 */
int PrinterDevice::getCUPSDests(cups_ptype_t type, cups_ptype_t mask, cups_dest_t **dests) {
  my_user_data_t user_data = {0, NULL};

  if (!cupsEnumDests(CUPS_DEST_FLAGS_NONE, 1000, NULL, type, mask, (cups_dest_cb_t)cbPrinterDest, &user_data)) {
    /*
     * An error occurred, free all of the destinations and
     * return...
     */
    cupsFreeDests(user_data.num_dests, user_data.dests);

    *dests = NULL;

    return (0);
  }

  /*
   * Return the destination array...
   */
  *dests = user_data.dests;
  return (user_data.num_dests);
}

/**
 * @brief PrinterDevice::getCUPSDests
 * @param type
 * @param mask
 * @param user_data
 * @return
 */
bool PrinterDevice::getCUPSDests(cups_ptype_t type, cups_ptype_t mask, my_user_data_t &user_data) {
  if (!cupsEnumDests(CUPS_DEST_FLAGS_NONE, 1000, NULL, type, mask, (cups_dest_cb_t)cbPrinterDest, &user_data)) {
    /*
     * An error occurred, free all of the destinations and
     * return...
     */
    cupsFreeDests(user_data.num_dests, user_data.dests);
    return false;
  }

  return true;
}
