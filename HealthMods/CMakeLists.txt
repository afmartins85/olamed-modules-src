cmake_minimum_required(VERSION 3.5)

project(HealthMods LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(BalanceMod)
add_subdirectory(SensorsMod)
