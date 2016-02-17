#
# Copyright (C) 2015 Cybernetica
#
# Research/Commercial License Usage
# Licensees holding a valid Research License or Commercial License
# for the Software may use this file according to the written
# agreement between you and Cybernetica.
#
# GNU General Public License Usage
# Alternatively, this file may be used under the terms of the GNU
# General Public License version 3.0 as published by the Free Software
# Foundation and appearing in the file LICENSE.GPL included in the
# packaging of this file.  Please review the following information to
# ensure the GNU General Public License version 3.0 requirements will be
# met: http://www.gnu.org/copyleft/gpl-3.0.html.
#
# For further information, please contact us at sharemind@cyber.ee.
#

CMAKE_MINIMUM_REQUIRED(VERSION 2.8.12)
PROJECT(SHAREMIND_MOD_SHARED3P_EMU CXX)

SET(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/CMakeIncludes/"
                      ${CMAKE_MODULE_PATH})

INCLUDE("${CMAKE_CURRENT_SOURCE_DIR}/config.local" OPTIONAL)
INCLUDE("${CMAKE_CURRENT_BINARY_DIR}/config.local" OPTIONAL)

ADD_COMPILE_OPTIONS(
  "-std=c++11" "-Wall" "-Wextra" "-O2" "-fPIC"
  "$<$<NOT:$<STREQUAL:$<CONFIGURATION>,Release>>:-ggdb>"
  "$<$<NOT:$<STREQUAL:$<CONFIGURATION>,Release>>:-fno-omit-frame-pointer>"
  "$<$<STREQUAL:$<CONFIGURATION>,Release>:-DNDEBUG>"
  "$<$<STREQUAL:$<CONFIGURATION>,Release>:-fomit-frame-pointer>"
)

FIND_PACKAGE(Boost REQUIRED)
FIND_PACKAGE(Cryptopp REQUIRED)
FIND_PACKAGE(LogHard REQUIRED)
FIND_PACKAGE(SharemindCHeaders REQUIRED)
FIND_PACKAGE(SharemindCxxHeaders REQUIRED)
FIND_PACKAGE(SharemindLibEmulatorProtocols REQUIRED)
FIND_PACKAGE(SharemindLibExecutionModelEvaluator REQUIRED)
FIND_PACKAGE(SharemindLibExecutionProfiler REQUIRED)
FIND_PACKAGE(SharemindLibmodapi REQUIRED)
FIND_PACKAGE(SharemindLibsoftfloat REQUIRED)
FIND_PACKAGE(SharemindLibsoftfloatMath REQUIRED)
FIND_PACKAGE(SharemindPdkHeaders REQUIRED)
INCLUDE_DIRECTORIES(
    ${Boost_INCLUDE_DIRS}
    ${CRYPTOPP_INCLUDE_DIRS}
    ${LogHard_INCLUDE_DIRS}
    ${SharemindCHeaders_INCLUDE_DIRS}
    ${SharemindCxxHeaders_INCLUDE_DIRS}
    ${SharemindLibEmulatorProtocols_INCLUDE_DIRS}
    ${SharemindLibExecutionModelEvaluator_INCLUDE_DIRS}
    ${SharemindLibExecutionProfiler_INCLUDE_DIRS}
    ${SharemindLibmodapi_INCLUDE_DIRS}
    ${SharemindLibsoftfloat_INCLUDE_DIRS}
    ${SharemindLibsoftfloatMath_INCLUDE_DIRS}
    ${SharemindPdkHeaders_INCLUDE_DIRS}
)

ADD_DEFINITIONS(
    ${LogHard_DEFINITIONS}
    ${SharemindCHeaders_DEFINITIONS}
    ${SharemindCxxHeaders_DEFINITIONS}
    ${SharemindLibEmulatorProtocols_DEFINITIONS}
    ${SharemindLibExecutionModelEvaluator_DEFINITIONS}
    ${SharemindLibExecutionProfiler_DEFINITIONS}
    ${SharemindLibmodapi_DEFINITIONS}
    ${SharemindLibsoftfloat_DEFINITIONS}
    ${SharemindLibsoftfloatMath_DEFINITIONS}
    ${SharemindPdkHeaders_DEFINITIONS}
    "-D__STDC_LIMIT_MACROS"
    "-D__STDC_CONSTANT_MACROS"
    "-D__STDC_FORMAT_MACROS"
)

IF(SHAREMIND_NETWORK_STATISTICS)
    ADD_DEFINITIONS("-DSHAREMIND_NETWORK_STATISTICS_ENABLE")
ENDIF(SHAREMIND_NETWORK_STATISTICS)

SET(SHARED3P_EMU_SOURCES
    "src/mod_shared3p_emu.cpp"
    "src/Shared3pConfiguration.cpp"
    "src/Shared3pModule.cpp"
    "src/Shared3pPD.cpp"
    "src/Shared3pPDPI.cpp"
    "src/Syscalls/AESSyscalls.cpp"
    "src/Syscalls/CRCSyscalls.cpp"
)

SET(SHARED3P_EMU_HEADERS
    "src/Facilities/CxxRandomEngine.h"
    "src/Protocols/AESProtocol.h"
    "src/Protocols/Binary.h"
    "src/Protocols/CRCProtocol.h"
    "src/Protocols/MatrixMultiplicationProtocol.h"
    "src/Protocols/MatrixShufflingProtocol.h"
    "src/Protocols/SoftFloatUtility.h"
    "src/Protocols/Unary.h"
    "src/Shared3pConfiguration.h"
    "src/Shared3pModule.h"
    "src/Shared3pPD.h"
    "src/Shared3pPDPI.h"    
    "src/Shared3pValueTraits.h"
    "src/Shared3pVector.h"
    "src/Syscalls/AESSyscalls.h"
    "src/Syscalls/BaseSyscalls.h"
    "src/Syscalls/Common.h"
    "src/Syscalls/CRCSyscalls.h"
    "src/Syscalls/MatrixMultiplicationSyscalls.h"
    "src/Syscalls/MatrixShufflingSyscalls.h"
    "src/Syscalls/Meta.h"
)

ADD_LIBRARY(sharemind_mod_shared3p_emu MODULE ${SHARED3P_EMU_SOURCES} ${SHARED3P_EMU_HEADERS})
TARGET_LINK_LIBRARIES(sharemind_mod_shared3p_emu
    ${CRYPTOPP_LIBRARIES}
    ${LogHard_LIBRARIES}
    ${SharemindCHeaders_LIBRARIES}
    ${SharemindCxxHeaders_LIBRARIES}
    ${SharemindLibExecutionModelEvaluator_LIBRARIES}
    ${SharemindLibExecutionProfiler_LIBRARIES}
    ${SharemindLibsoftfloat_LIBRARIES}
    ${SharemindLibsoftfloatMath_LIBRARIES}
    ${SharemindPdkHeaders_LIBRARIES}
)

INSTALL(TARGETS sharemind_mod_shared3p_emu LIBRARY DESTINATION "lib")

SET(SHAREMIND_MOD_SHARED3P_EMU_DEBUG_FILES
    "debug_files/shared3p_emu.cfg"
    "debug_files/shared3p_emu-models.cfg"
)
ADD_CUSTOM_TARGET("include_SHAREMIND_MOD_SHARED3P_EMU_DEBUG_FILES_in_IDE" SOURCES
                  ${SHAREMIND_MOD_SHARED3P_EMU_DEBUG_FILES})

IF(NOT (CMAKE_BUILD_TYPE STREQUAL "Release"))
    INSTALL(FILES ${SHAREMIND_MOD_SHARED3P_EMU_DEBUG_FILES}
            DESTINATION "bin")
ENDIF()
