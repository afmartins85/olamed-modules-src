#include "dapplication.h"
#include "loguru.hpp"
#include <chrono>
#include <iostream>
#include <thread>
#include <vector>

// Define the static DApplication pointer
DApplication *DApplication::instance_ = nullptr;

/**
 * @brief DApplication::DApplication
 */
DApplication::DApplication() {
  this->m_ptr_Protocol = new PrinterProtocol;
  this->m_netSNMP = new NetSNMP;
  this->m_ptrDevice = new PrinterDevice;
  m_ptr_Socket = new Socket;
  this->m_ptr_Socket->setPort(8080);
  //  this->m_ptr_Socket->setAddress((char *)"10.8.0.2");
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
  //  this->m_ptr_Socket->setAddress((char *)"10.8.0.2");
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
 * @brief DApplication::exec
 */
void DApplication::exec() {
  int tryComm = MAX_TRY_CON;
  PrinterDevice::printer_data_t user_data = {0, NULL};

  LOG_SCOPE_FUNCTION(INFO);
  LOG_F(INFO, "Printer Daemon Started!! Checking for priner Device...");

  this->m_ptrDevice->findPrinter();
  // Set type protocol for response status
  this->m_ptr_Protocol->setType(2);

  // Check initial printer description from SNMP
  if (this->m_ptrDevice->ptrIsFound() == true) {
    // Check if printer is ready for print
    if (this->m_ptrDevice->isReadyPrinter() == false) {
      this->m_ptrDevice->isReadyCUPS(&user_data);
    }

    // Set true for printer connected.
    this->m_ptr_Protocol->setConnected(true);
  }

  while (true) {
    if (this->m_ptrDevice->ptrIsFound() == true) {
      if (this->m_netSNMP->openSession(*this->m_ptrDevice) == true) {
        if ((this->m_netSNMP->readMIBDescription(*this->m_ptr_Protocol) == false) ||
            (this->m_netSNMP->readMIBLifeCount(*this->m_ptr_Protocol) == false) ||
            (this->m_netSNMP->readMIBStatus(*this->m_ptr_Protocol) == false) ||
            (this->m_netSNMP->readMIBErrors(*this->m_ptr_Protocol) == false)) {
          if (tryComm <= 0) {
            LOG_F(ERROR, "Printer Device is offline!!");
            this->m_ptr_Protocol->setConnected(false);
            this->m_netSNMP->closeSession();
          } else {
            LOG_F(WARNING, "net-SNMP Get: Timeout!!");
            tryComm--;
            // Wait 2 seconds for try again
            std::this_thread::sleep_for(std::chrono::seconds(2));
            continue;
          }
        } else {
          tryComm = MAX_TRY_CON;  // Reset counter
          this->m_ptr_Protocol->setConnected(true);
        }
        this->m_ptr_Protocol->DateTime();
        this->m_ptr_Protocol->setDate(this->m_ptr_Protocol->datetime());
        this->m_ptr_Protocol->prepare_json_object();
        this->m_ptr_Socket->setMessage(const_cast<char *>((this->m_ptr_Protocol->json_message()).c_str()));
        this->m_ptr_Socket->clientSendMessage();
      } else {
        LOG_F(ERROR, "Failure in open session net-SNMP");
      }

      // Check for print queue
      this->m_ptrDevice->printFileFromQueue(&user_data);
    } else {
      LOG_F(WARNING, "No Printer device available!!!");
      this->m_ptr_Protocol->setConnected(false);
    }

    // Machine states for connection handle
    switch (this->m_ptr_Socket->ConStatus()) {
      case Socket::Disconnected:
        if (this->m_ptr_Socket->clientConnect() == true) {
          this->m_ptr_Socket->changeConStatus(Socket::Connected);
          LOG_F(INFO, "Successfully connected to the Sync Module!!");
        } else {
          // Check system information at 5 seconds
          std::this_thread::sleep_for(std::chrono::seconds(5));
        }
        break;
      case Socket::Connected:
        // Check for sockect received data
        Socket::CConnectionState retSel = this->m_ptr_Socket->clientSelect();
        if (retSel == Socket::DataAvailable) {
          this->m_ptr_Socket->clientReadMessage();
        } else if (retSel == Socket::LostConnection) {
          LOG_F(WARNING, "Lost connection with Sync Module!!");
          this->m_ptr_Socket->changeConStatus(Socket::Disconnected);
        } else if ((retSel == Socket::ForceClose) || (retSel == Socket::Timeout)) {
          LOG_F(WARNING, "Force Close Connection!!");
          this->m_ptr_Socket->clientClose();
          this->m_ptr_Socket->changeConStatus(Socket::Disconnected);
        }
        break;
    }
  }
}

/**
 * @brief DApplication::parseMessageReceive
 */
void DApplication::parseMessageReceive(char *message) {
  DApplication *app = getInstance();
  LOG_SCOPE_FUNCTION(INFO);
  if (app != nullptr) {
    pthread_mutex_t ctxMutex = app->m_ptrDevice->getPrintServerMutex();
    pthread_mutex_lock(&ctxMutex);
    app->m_ptr_Protocol->string_parse_json_object(message);
    int type = static_cast<int>(reinterpret_cast<intptr_t>(app->m_ptr_Protocol->value_json_object("type")));
    if (type == 4) {
      app->m_ptr_Protocol->setId(reinterpret_cast<char *>(app->m_ptr_Protocol->value_json_object("id")));
      app->m_ptr_Protocol->setFilename(reinterpret_cast<char *>(app->m_ptr_Protocol->value_json_object("filename")));
      app->m_ptr_Protocol->setFiletype(reinterpret_cast<char *>(app->m_ptr_Protocol->value_json_object("filetype")));
      app->m_ptr_Protocol->setContent(reinterpret_cast<char *>(app->m_ptr_Protocol->value_json_object("content")));
      app->m_ptr_Protocol->setDate(reinterpret_cast<char *>(app->m_ptr_Protocol->value_json_object("date")));
      app->m_ptrDevice->saveFile(app->m_pathFile, app->m_ptr_Protocol->filename(), app->m_ptr_Protocol->id(),
                                 app->m_ptr_Protocol->content());
    } else {
      LOG_F(WARNING, "Command (%d) not supported!!", type);
    }
    pthread_mutex_unlock(&ctxMutex);
  } else {
    LOG_F(ERROR, "App context is null!!!");
  }
}

/**
 * @brief DApplication::operator =
 * @return
 */
DApplication &DApplication::operator=(const DApplication &d) {
  this->instance_ = d.instance_;
  return *this;
}
