#include "tothspo2.h"
#include "../../loguru.hpp"

TothSpO2::TothSpO2() : HealtSensors() {

  // create socket server
  this->m_hl7Serv = new HL7SocketServer(50000);
  // accept client connection
  this->m_client = this->m_hl7Serv->run();
  if (this->m_client) {
    LOG_F(ERROR, "ERROR accepting client connection.\n");
    return;
  }
  // init MLLP object to receive MLLP rotocol messages
  this->m_mllp = new HL7MLLP(this->m_client->getSocket());
}

/**
 * @brief TothSpO2::getSpO2
 */
void TothSpO2::getSpO2() {
  std::string data;
  // read client request
  this->m_mllp->read_msg_mllp(data);
}
