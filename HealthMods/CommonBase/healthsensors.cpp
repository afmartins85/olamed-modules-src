#include "healthsensors.h"

/**
 * @brief HealthSensors::HealthSensors
 */
HealthSensors::HealthSensors() {}

/**
 * @brief HealthSensors::getTemp
 * @return
 */
double HealthSensors::getTemp() { return 0; }

/**
 * @brief HealthSensors::isTempReady
 * @return
 */
bool HealthSensors::isTempReady() { return false; }

/**
 * @brief HealthSensors::getSpO2
 * @return
 */
int HealthSensors::getSpO2() { return 0; }

/**
 * @brief HealthSensors::isSpo2Ready
 * @return
 */
bool HealthSensors::isSpo2Ready() { return false; }

/**
 * @brief HealthSensors::getPressBloodSys
 * @return
 */
int HealthSensors::getPressBloodSys() { return 0; }

/**
 * @brief HealthSensors::getPressBloodDia
 * @return
 */
int HealthSensors::getPressBloodDia() { return 0; }

/**
 * @brief HealthSensors::getPressBloodMean
 * @return
 */
int HealthSensors::getPressBloodMean() { return 0; }

/**
 * @brief HealthSensors::isBlooPressReady
 * @return
 */
bool HealthSensors::isBlooPressReady() { return false; }

/**
 * @brief HealthSensors::isNextRegister
 * @param next
 */
void HealthSensors::isNextRegister(bool next) { (void)next; }

/**
 * @brief HealthSensors::getEquipAddress
 * @return
 */
string HealthSensors::getEquipAddress() { return string(""); }

/**
 * @brief HealthSensors::isEquipAddressReady
 * @return
 */
bool HealthSensors::isEquipAddressReady() { return false; }

/**
 * @brief HealthSensors::getSerialSensor
 * @return
 */
string HealthSensors::getSerialSensor() { return string(""); }

/**
 * @brief HealthSensors::isSerialSensorReady
 * @return
 */
bool HealthSensors::isSerialSensorReady() { return false; }

/**
 * @brief HealthSensors::isSensorOnline
 * @return
 */
bool HealthSensors::isSensorOnline() { return false; }

/**
 * @brief HealthSensors::getBalance
 * @return
 */
double HealthSensors::getBalance() { return 0; }

/**
 * @brief HealthSensors::isBalanceReady
 * @return
 */
bool HealthSensors::isBalanceReady() { return false; }
