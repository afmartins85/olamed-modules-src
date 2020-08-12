cmake_minimum_required(VERSION 3.5)

project(PrinterMod LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

file(GLOB_RECURSE PrinterMod_HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/*.h
    ${CMAKE_CURRENT_SOURCE_DIR}/*.hpp
    )

file(GLOB_RECURSE PrinterMod_SOURCES
    ${PrinterMod_SOURCE_DIR}/*.cpp
    )

add_executable(PrinterMod ${PrinterMod_SOURCES} ${PrinterMod_HEADERS})
