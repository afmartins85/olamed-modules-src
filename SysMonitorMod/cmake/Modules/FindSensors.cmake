#
# This module detects if SENSORS is installed and determines where the
# include files and libraries are.
#
# This code sets the following variables:
#
# SENSORS_DIR      = full path to the import directory
# SENSORS_LIBRARIES       = full path to the SENSORS libraries
# SENSORS_INCLUDE_DIR     = include dir to be used when using the library
# SENSORS_FOUND           = set to true if was found successfully
#
# SENSORS_ROOT
#   setting this enables search for libraries / headers in this location

# -----------------------------------------------------
# SENSORS Import Directories
# -----------------------------------------------------
set (SENSORS_ROOT ${SENSORS_ROOT} /usr/include)
set (SENSORS_ROOT_LIB ${SENSORS_ROOT_LIB} /usr/lib/x86_64-linux-gnu)

find_path(SENSORS_DIR
  NAMES sensors.h
  PATHS ${SENSORS_ROOT}/sensors
)

message(STATUS "Find SENSORS is found " ${SENSORS_DIR})
# -----------------------------------------------------
# SENSORS Libraries
# -----------------------------------------------------
find_library(SENSORS_LIBRARIES
        NAMES sensors
	HINTS ${SENSORS_ROOT_LIB}
	DOC "The main open SENSORS library"
)
message(STATUS "Library " ${SENSORS_LIBRARIES})
# -----------------------------------------------------
# SENSORS Include Directories
# SENSORS_DIR
# -----------------------------------------------------

# -----------------------------------------------------
# handle the QUIETLY and REQUIRED arguments and set SENSORS_FOUND to TRUE if
# all listed variables are TRUE
# -----------------------------------------------------
include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(Sensors DEFAULT_MSG SENSORS_LIBRARIES SENSORS_DIR)
mark_as_advanced(SENSORS_DIR SENSORS_LIBRARIES)


