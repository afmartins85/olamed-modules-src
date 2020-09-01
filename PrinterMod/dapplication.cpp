#include "dapplication.h"
#include "loguru.hpp"

// Define the static DApplication pointer
DApplication *DApplication::instance_ = nullptr;

/**
 * @brief DApplication::DApplication
 */
DApplication::DApplication() {
  this->m_ptr_Protocol = new PrinterProtocol;
  this->m_netSNMP = new NetSNMP;
  m_ptr_Socket = new Socket;
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
  m_ptr_Socket = new Socket;
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

/**
 * @brief DApplication::exec
 */
void DApplication::exec() {
  LOG_SCOPE_FUNCTION(INFO);
  LOG_F(INFO, "Printer Daemon Started!! Checking for priner Device...");

  double a, b, c, d;
  a = 100.00;
  b = 98.89;
  c = 88.00;
  d = 23.00;

  if (this->m_netSNMP->openSession() == true) {
    LOG_F(INFO, "net-SNMP open session is done!!!");
    this->m_netSNMP->readIodTest();
  } else {
    LOG_F(ERROR, "Failure in open session net-SNMP");
  }

  this->m_ptr_Protocol->setType(2);
  this->m_ptr_Protocol->setSerial("7558100415344-106-0");
  this->m_ptr_Protocol->setDescription("7558100415344-106-0");
  this->m_ptr_Protocol->setConnected(false);
  this->m_ptr_Protocol->setState("Lexmark Laser, Mono, MX421ADE");
  this->m_ptr_Protocol->setError("no paper");
  this->m_ptr_Protocol->setSupply_type("tonner");
  this->m_ptr_Protocol->setCyan_level(a);
  this->m_ptr_Protocol->setMagenta_level(98.89);
  this->m_ptr_Protocol->setYellow_level(88.00);
  this->m_ptr_Protocol->setBlack_level(23.00);

  //  this->m_ptr_Protocol->setYellow_level(&c);
  //  this->m_ptr_Protocol->setBlack_level(&d);

  this->m_ptr_Protocol->prepare_json_object();

  this->m_ptr_Protocol->prepare_json_object();

  //content: GVsbG8gV29ybGQgISEh -> Hello World !!!
  this->m_ptr_Protocol->string_parse_json_object((char*)"{ \"type\": 4, \"id\": \"cf046407-ad01-4a61-925f-1d8156370c5c\", \"filename\": \"file_print\", \"filetype\": \".txt\", \"content\": \"SGVsbG8gV29ybGQgISEh\", \"date\": \"2020-08-01T20:10:10Z\"}");
  this->m_ptr_Protocol->setType((uint64_t)this->m_ptr_Protocol->value_json_object("type"));
  this->m_ptr_Protocol->setId((char*)this->m_ptr_Protocol->value_json_object("id"));
  this->m_ptr_Protocol->setFilename((char*)this->m_ptr_Protocol->value_json_object("filename"));
  this->m_ptr_Protocol->setFiletype((char*)this->m_ptr_Protocol->value_json_object("filetype"));
  this->m_ptr_Protocol->setContent((char*)this->m_ptr_Protocol->value_json_object("content"));
  this->m_ptr_Protocol->setDate((char*)this->m_ptr_Protocol->value_json_object("date"));
 
  cout << "Testing the Get functions results:" << endl;
  cout << "type: "; cout << this->m_ptr_Protocol->type() << endl;
  cout << "id: "; cout << this->m_ptr_Protocol->id() << endl;
  cout << "filename: "; cout << this->m_ptr_Protocol->filename() << endl;
  cout << "filetype: "; cout << this->m_ptr_Protocol->filetype() << endl;
  cout << "content: "; cout << this->m_ptr_Protocol->content() << endl;
  cout << "date: "; cout << this->m_ptr_Protocol->date() << endl;
  cout << "---" << endl;

  cout << this->m_ptr_Protocol->base64_encode((const unsigned char*)"Hello World !!!", strlen("Hello World !!!")) << endl;
  cout << this->m_ptr_Protocol->base64_decode(this->m_ptr_Protocol->content()) << endl;
  cout << "---" << endl;
  this->m_ptr_Protocol->file_save();
  cout << "---" << endl;



  this->m_ptr_Socket->setPort(8080);
  this->m_ptr_Socket->setAddress((char *)"127.0.0.1");
  cout << "DApplication::exec():" << endl;
  cout << "((this->m_ptr_Protocol->json_message()).c_str()):" << endl;
  cout << ((this->m_ptr_Protocol->json_message()).c_str()) << endl;
  this->m_ptr_Socket->setMessage(const_cast<char*>((this->m_ptr_Protocol->json_message()).c_str()));
  cout << "this->m_ptr_Socket->message():" << endl;
  cout << this->m_ptr_Socket->message() << endl;

  this->m_ptr_Socket->Client();

  while (1) {
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
