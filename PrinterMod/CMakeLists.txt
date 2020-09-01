cmake_minimum_required(VERSION 3.5)

project(PrinterMod LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(cmake/Utils.cmake)

#Here we add a Cmake Module abble to find GSOAP on your system, and find it
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")

find_package(NetSNMP)
find_package(CUPS)
find_package(PkgConfig)

file(GLOB_RECURSE PrinterMod_HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/*.h
    ${CMAKE_CURRENT_SOURCE_DIR}/*.hpp
    )

file(GLOB_RECURSE PrinterMod_SOURCES
    ${PrinterMod_SOURCE_DIR}/*.cpp
    )

pkg_check_modules(PC_JSONC json-c)
if (PC_JSONC_FOUND)
        message(STATUS "Found json-c using pkg-config: ${PC_JSONC_PREFIX}")
        message(STATUS " PC_JSONC_INCLUDE_DIRS=${PC_JSONC_INCLUDE_DIRS}")
        message(STATUS " PC_JSONC_LIBRARIES=${PC_JSONC_LIBRARIES}")
        message(STATUS " PC_JSONC_LIBRARY_DIRS=${PC_JSONC_LIBRARY_DIRS}")
        link_directories(${PC_JSONC_LIBRARY_DIRS})
        include_directories(${PC_JSONC_INCLUDE_DIRS})
        # for target_link_libraries(...)
        set(APPS_INCLUDE_DIRS ${PC_JSONC_INCLUDE_DIRS})
        set(APPS_LINK_DIRS ${PC_JSONC_LIBRARY_DIRS})
        set(APPS_LINK_LIBS ${PC_JSONC_LIBRARIES})
else()
        message(STATUS "Using find_package to locate json-c")

        # Note: find_package needs CMAKE_PREFIX_PATH set appropriately.
        # XXX json-c's installed cmake files don't actually set up what's
        #  needed to use find_package() by itself, so we're just using it
        #  to confirm the top of the install location.
        find_package(json-c CONFIG)  # sets json-c_DIR

        # Assume json-c-config.cmake is in lib64/cmake/json-c/
        get_filename_component(json-c_TOP "${json-c_DIR}/../../.." ABSOLUTE)
        get_filename_component(json-c_LIBDIR "${json-c_DIR}/../.." ABSOLUTE)

        message(STATUS " json-c_TOP=${json-c_TOP}")
        message(STATUS " json-c_DIR=${json-c_DIR}")
        message(STATUS " json-c_LIBDIR=${json-c_LIBDIR}")

        link_directories(${json-c_LIBDIR})
        include_directories(${json-c_TOP}/include)
        include_directories(${json-c_TOP}/include/json-c)
        set(APPS_LINK_DIRS "${json-c_LIBDIR}")
        set(APPS_INCLUDE_DIRS "${json-c_TOP}/include;${json-c_TOP}/include/json-c")

        set(APPS_LINK_LIBS json-c)
endif()

if (NETSNMP_FOUND)
    message(STATUS "net-SNMP found")
    include_directories(${NETSNMP_INCLUDE_DIR})
else()
    message(ERROR "Failure on find net-SNMP!!!")
endif()

if (CUPS_FOUND)
    message(STATUS "CUPS found")
    include_directories(${CUPS_DIR})
else()
    message(ERROR "Failure on find CUPS!!!")
endif()

add_executable(PrinterMod ${PrinterMod_SOURCES} ${PrinterMod_HEADERS})
target_link_libraries(PrinterMod ${APPS_LINK_LIBS})
target_link_libraries(PrinterMod ${NETSNMP_LIBRARY})
target_link_libraries(PrinterMod ${CUPS_LIBRARIES})
target_link_libraries(PrinterMod "-lpthread")
target_link_libraries(PrinterMod "-ldl")
