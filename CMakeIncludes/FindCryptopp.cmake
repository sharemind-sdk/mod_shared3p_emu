################################################################################
#
# CMake script for finding Crypto++.
# The default CMake search process is used to locate files.
#
# This script creates the following variables:
#  CRYPTOPP_FOUND: Boolean that indicates if the package was found
#  CRYPTOPP_INCLUDE_DIRS: Paths to the necessary header files
#  CRYPTOPP_LIBRARIES: Package libraries
#
################################################################################

# Find headers and libraries
FIND_PATH(
    CRYPTOPP_INCLUDE_DIR
    NAMES
        cryptopp/cryptlib.h
    HINTS
        $ENV{CRYPTOPP_ROOT}
        ${CRYPTOPP_ROOT}
    PATHS
        /usr/local
        /usr
        /opt/local
    PATH_SUFFIXES
        include
)

FIND_LIBRARY(
    CRYPTOPP_LIBRARY
    NAMES
        cryptopp
        crypto++
    HINTS
        $ENV{CRYPTOPP_ROOT}
        ${CRYPTOPP_ROOT}
    PATHS
        /usr/local
        /usr
        /opt/local
    PATH_SUFFIXES
        lib
)

# check Cryptopp version:
SET(CRYPTOPP_MINIMUM_VERSION 550)
SET(CRYPTOPP_VERSION_STR 0)
FUNCTION(CRYPTOPP_PARSE_VERSION headerFile)
    FILE(STRINGS "${headerFile}" CRYPTOPP_VERSION_ROW
        REGEX "^[ \t]*#define[ \t]+CRYPTOPP_VERSION[ \t]+[0-9]+")

    IF(CRYPTOPP_VERSION_ROW)
        STRING(REGEX MATCH "[0-9]+" CRYPTOPP_VERSION_STR "${CRYPTOPP_VERSION_ROW}")
        SET(CRYPTOPP_VERSION_STR "${CRYPTOPP_VERSION_STR}" PARENT_SCOPE)
    ENDIF(CRYPTOPP_VERSION_ROW)
ENDFUNCTION()
IF(CRYPTOPP_INCLUDE_DIR)
    IF(EXISTS "${CRYPTOPP_INCLUDE_DIR}/cryptopp/config_ver.h")
        CRYPTOPP_PARSE_VERSION("${CRYPTOPP_INCLUDE_DIR}/cryptopp/config_ver.h")
    ELSEIF(EXISTS "${CRYPTOPP_INCLUDE_DIR}/cryptopp/config.h")
        CRYPTOPP_PARSE_VERSION("${CRYPTOPP_INCLUDE_DIR}/cryptopp/config.h")
    ENDIF()
ENDIF()

SET(CRYPTOPP_VERSION_OK 1)
IF(${CRYPTOPP_VERSION_STR} LESS ${CRYPTOPP_MINIMUM_VERSION})
    SET(CRYPTOPP_VERSION_OK 0)
    MESSAGE(STATUS "Found Cryptopp version ${CRYPTOPP_VERSION_STR}. Version ${CRYPTOPP_MINIMUM_VERSION} required")
ENDIF(${CRYPTOPP_VERSION_STR} LESS ${CRYPTOPP_MINIMUM_VERSION})

# Set CRYPTOPP_FOUND honoring the QUIET and REQUIRED arguments
INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(
    Cryptopp
    "Could NOT find Cryptopp ${CRYPTOPP_MINIMUM_VERSION}+"
    CRYPTOPP_LIBRARY CRYPTOPP_INCLUDE_DIR CRYPTOPP_VERSION_OK)

# Output variables
IF(CRYPTOPP_FOUND)
    # Include dirs
    SET(CRYPTOPP_INCLUDE_DIRS ${CRYPTOPP_INCLUDE_DIR})

    # Libraries
    SET(CRYPTOPP_LIBRARIES ${CRYPTOPP_LIBRARY})
ENDIF(CRYPTOPP_FOUND)

# Advanced options for not cluttering the cmake UIs:
MARK_AS_ADVANCED(CRYPTOPP_INCLUDE_DIR CRYPTOPP_LIBRARY CRYPTOPP_VERSION_OK)
