cmake_minimum_required(VERSION 3.5)

project(balanceMod LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(PROJECT_VERSION_MAJOR "0")
set(PROJECT_VERSION_MINOR "1")
set(PROJECT_VERSION_PATCH "0")
set(PROJECT_VERSION "${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH}")

include(CheckSymbolExists)
include(CheckIncludeFile)
include(CheckIncludeFiles)
include(CheckCSourceCompiles)
include(CheckTypeSize)
include(CPack)
include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

find_package(PkgConfig)

file(GLOB balanceMod_HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/*.h
    ${CMAKE_CURRENT_SOURCE_DIR}/*.hpp
    )

file(GLOB balanceMod_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
    )

set(COMMON_BASE_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/../CommonBase/healthsensors.cpp)
include_directories(AFTER "../CommonBase")

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

add_executable(balanceMod
    ${balanceMod_SOURCES}
    ${balanceMod_HEADERS}
    ${HL7_COMMON_SRC}
    ${TOTH_COMMON_SRC}
    ${COMMON_BASE_SOURCES}
    )

target_link_libraries(balanceMod ${APPS_LINK_LIBS})
target_link_libraries(balanceMod "-lpthread")
target_link_libraries(balanceMod "-ldl")

set(CMAKE_INSTALL_DEFAULT_DIRECTORY_PERMISSIONS
        OWNER_READ
        OWNER_WRITE
        OWNER_EXECUTE
        GROUP_READ
        GROUP_EXECUTE
        GROUP_WRITE
        WORLD_READ
        WORLD_EXECUTE
        WORLD_WRITE
   )

if (UNIX OR MINGW OR CYGWIN)
    SET(prefix ${CMAKE_INSTALL_PREFIX})
    # exec_prefix is prefix by default and CMake does not have the
    # concept.
    SET(exec_prefix ${CMAKE_INSTALL_PREFIX})
    SET(libdir ${CMAKE_INSTALL_FULL_LIBDIR})
    SET(includedir ${CMAKE_INSTALL_FULL_INCLUDEDIR})
    SET(VERSION ${PROJECT_VERSION})
#    configure_file(json-c.pc.in json-c.pc @ONLY)
#    set(INSTALL_PKGCONFIG_DIR "${CMAKE_INSTALL_LIBDIR}/pkgconfig" CACHE PATH "Installation directory for pkgconfig (.pc) files")
    install(FILES ${PROJECT_BINARY_DIR}/sensorsMod DESTINATION "/home/olamed/balanceMod")
endif ()
