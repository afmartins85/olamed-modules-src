#
# This module detects if CUPS is installed and determines where the
# include files and libraries are.
#
# This code sets the following variables:
#
# CUPS_DIR      = full path to the import directory
# CUPS_LIBRARIES       = full path to the CUPS libraries
# CUPS_INCLUDE_DIR     = include dir to be used when using the library
# CUPS_FOUND           = set to true if was found successfully
#
# CUPS_ROOT
#   setting this enables search for libraries / headers in this location

# -----------------------------------------------------
# CUPS Import Directories
# -----------------------------------------------------
set (CUPS_ROOT ${CUPS_ROOT} /usr/include)
set (CUPS_ROOT_LIB ${CUPS_ROOT_LIB} /usr/lib/x86_64-linux-gnu)

find_path(CUPS_DIR
  NAMES cups.h
  PATHS ${CUPS_ROOT}/cups
)

message(STATUS "Find CUPS is found " ${CUPS_DIR})
# -----------------------------------------------------
# CUPS Libraries
# -----------------------------------------------------
find_library(CUPS_LIBRARIES
        NAMES cups
        HINTS ${CUPS_ROOT_LIB}
        DOC "The main open CUPS library"
)
message(STATUS "Library " ${CUPS_LIBRARIES})
# -----------------------------------------------------
# CUPS Include Directories
# CUPS_DIR
# -----------------------------------------------------

# -----------------------------------------------------
# handle the QUIETLY and REQUIRED arguments and set CUPS_FOUND to TRUE if
# all listed variables are TRUE
# -----------------------------------------------------
include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(CUPS DEFAULT_MSG CUPS_LIBRARIES CUPS_DIR)
mark_as_advanced(CUPS_DIR CUPS_LIBRARIES)


