#include "dapplication.h"
#include "loguru.hpp"
#include <chrono>
#include <iostream>
#include <thread>
#include <vector>

// Define the static DApplication pointer
DApplication *DApplication::instance_ = nullptr;

pthread_mutex_t printServerMutex = PTHREAD_MUTEX_INITIALIZER;

/**
 * @brief DApplication::DApplication
 */
DApplication::DApplication() {
  this->m_ptr_Protocol = new PrinterProtocol;
  this->m_netSNMP = new NetSNMP;
  this->m_ptrDevice = new PrinterDevice;
  m_ptr_Socket = new Socket;
  this->m_ptr_Socket->setPort(8080);
  this->m_ptr_Socket->setAddress((char *)"127.0.0.1");
  this->m_pathFile = "/usr/local/share/printerMod/printQueue/";
  this->m_lifeCount = 0;
}

/**
 * @brief DApplication::DApplication
 * @param argc
 * @param argv
 */
DApplication::DApplication(int argc, char *argv[]) {
  (void)argc;
  (void)argv;
  this->m_ptr_Protocol = new PrinterProtocol;
  this->m_netSNMP = new NetSNMP;
  this->m_ptrDevice = new PrinterDevice;
  m_ptr_Socket = new Socket;
  this->m_ptr_Socket->setPort(8080);
  this->m_ptr_Socket->setAddress((char *)"127.0.0.1");
  this->m_pathFile = "/usr/local/share/printerMod/printQueue/";
  this->m_lifeCount = 0;
}

/**
 * @brief DApplication::getInstance
 * @return
 */
DApplication *DApplication::getInstance() {
  if (instance_ == nullptr) {
    instance_ = new DApplication();
  }
  return instance_;
}

/**
 * @brief DApplication::getInstance
 * @param argc
 * @param argv
 * @return
 */
DApplication *DApplication::getInstance(int argc, char *argv[]) {
  if (instance_ == nullptr) {
    instance_ = new DApplication(argc, argv);
  }
  return instance_;
}

void *DApplication::printServerListen(void *ptr) {
  pthread_setname_np(pthread_self(), __FUNCTION__);
  Socket *sock = reinterpret_cast<Socket *>(ptr);
  sock->Server();
  pthread_exit(NULL);
}

void DApplication::startServer() {
  pthread_t ptid;
  pthread_create(&ptid, NULL, &printServerListen, &m_ptr_Socket);
  pthread_detach(ptid);
}

/**
 * @brief DApplication::saveFile
 */
void DApplication::saveFile() {
  FILE *fp;
  string file;

  file.insert(0, this->m_pathFile);
  file.append(this->m_ptr_Protocol->id());
  file.append("_");
  file.append(this->m_ptr_Protocol->filename());
  file.append(this->fileExtension(this->m_ptr_Protocol->filetype()));
  std::replace(file.begin(), file.end(), ' ', '-');
  // Insert file in print queue
  this->m_printQueue.insert({this->m_ptr_Protocol->id(), file});

  fp = fopen(file.c_str(), "wb");

  fprintf(fp, "%s", (this->m_ptr_Protocol->base64_decode(this->m_ptr_Protocol->content())).c_str());
  fclose(fp);
}

/**
 * @brief DApplication::fileExtension
 * @return
 */
string DApplication::fileExtension(string s) {
  string extension;

  if (!s.compare(CUPS_FORMAT_JPEG)) {
    extension = ".jpeg";
  } else if (!s.compare(CUPS_FORMAT_PDF)) {
    extension = ".pdf";
  } else if (!s.compare(CUPS_FORMAT_TEXT)) {
    extension = ".txt";
  } else if (!s.compare(CUPS_FORMAT_POSTSCRIPT)) {
    extension = ".postscript";
  } else {
    extension = "";
  }
  return extension;
}

/**
 * @brief DApplication::printFileFromQueue
 */
void DApplication::printFileFromQueue() {
  pthread_mutex_lock(&printServerMutex);

  if (this->m_printQueue.size() > 0) {
    LOG_SCOPE_FUNCTION(INFO);
    LOG_F(INFO, "There are pending print files!!");
    string printSys("lpr ");
    auto it = this->m_printQueue.begin();
    printSys.append(it->second);
    if (!system(printSys.c_str())) {
      LOG_F(INFO, "Document send succeeded.");
    } else {
      LOG_F(ERROR, "Document (%s) send failed.", it->second.c_str());
    }
  }
  pthread_mutex_unlock(&printServerMutex);
}

/**
 * @brief DApplication::exec
 */
void DApplication::exec() {
  int tryComm = MAX_TRY_CON;

  LOG_SCOPE_FUNCTION(INFO);
  LOG_F(INFO, "Printer Daemon Started!! Checking for priner Device...");

  this->m_ptrDevice->findPrinter();
  // Set type protocol for response status
  this->m_ptr_Protocol->setType(2);

  // Check initial printer description from SNMP
  if (this->m_ptrDevice->ptrIsFound() == true) {
    // Check if printer is ready for print
    if (this->m_ptrDevice->isReadyPrinter() == false) {
      this->m_ptrDevice->isReadyCUPS();
    }

    // Set true for printer connected.
    this->m_ptr_Protocol->setConnected(true);
    if (this->m_netSNMP->openSession(*this->m_ptrDevice) == true) {
      LOG_F(INFO, "net-SNMP open session is done!!!");
      this->m_netSNMP->readMIBDescription(*this->m_ptr_Protocol);
      this->m_netSNMP->closeSession();
    } else {
      LOG_F(ERROR, "Failure in open session net-SNMP");
    }
  }

  // Start print server
  startServer();

  while (1) {
    // Check system information at 5 seconds
    std::this_thread::sleep_for(std::chrono::seconds(5));

    if (this->m_ptrDevice->ptrIsFound() == true) {
      if (this->m_netSNMP->openSession(*this->m_ptrDevice) == true) {
        this->m_netSNMP->readMIBLifeCount(*this->m_ptr_Protocol);
        this->m_netSNMP->readMIBStatus(*this->m_ptr_Protocol);
        this->m_netSNMP->readMIBErrors(*this->m_ptr_Protocol);
        this->m_netSNMP->closeSession();
        tryComm = MAX_TRY_CON;  // Reset counter

        //        this->m_ptr_Protocol->prepare_json_object();
        //        this->m_ptr_Protocol->prepare_json_object();
        //        this->m_ptr_Socket->setMessage(const_cast<char *>((this->m_ptr_Protocol->json_message()).c_str()));
        //        this->m_ptr_Socket->Client();
      } else {
        tryComm--;
        LOG_F(ERROR, "Failure in open session net-SNMP");
        if (tryComm <= 0) {
          this->m_ptr_Protocol->setConnected(false);
        }
      }

      // Check for print queue
      this->printFileFromQueue();
    } else {
      LOG_F(WARNING, "No Printer device available!!!");
      this->m_ptr_Protocol->setConnected(false);
    }
  }
}

/**
 * @brief DApplication::parseMessageReceive
 */
void DApplication::parseMessageReceive(char *message) {
  pthread_mutex_lock(&printServerMutex);
  DApplication *app = getInstance();
  LOG_SCOPE_FUNCTION(INFO);
  if (app != nullptr) {
    app->m_ptr_Protocol->string_parse_json_object(message);
    int type = static_cast<int>(reinterpret_cast<intptr_t>(app->m_ptr_Protocol->value_json_object("type")));
    if (type == 4) {
      app->m_ptr_Protocol->setId(reinterpret_cast<char *>(app->m_ptr_Protocol->value_json_object("id")));
      app->m_ptr_Protocol->setFilename(reinterpret_cast<char *>(app->m_ptr_Protocol->value_json_object("filename")));
      app->m_ptr_Protocol->setFiletype(reinterpret_cast<char *>(app->m_ptr_Protocol->value_json_object("filetype")));
      app->m_ptr_Protocol->setContent(reinterpret_cast<char *>(app->m_ptr_Protocol->value_json_object("content")));
      app->m_ptr_Protocol->setDate(reinterpret_cast<char *>(app->m_ptr_Protocol->value_json_object("date")));
      app->saveFile();
    } else {
      LOG_F(WARNING, "Command (%d) not supported!!", type);
    }
  } else {
    LOG_F(ERROR, "App context is null!!!");
  }

  pthread_mutex_unlock(&printServerMutex);
}

/**
 * @brief DApplication::operator =
 * @return
 */
DApplication &DApplication::operator=(const DApplication &d) {
  this->instance_ = d.instance_;
  return *this;
}
