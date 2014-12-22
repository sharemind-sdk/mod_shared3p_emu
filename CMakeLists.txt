#
# This file is a part of the Sharemind framework.
# Copyright (C) Cybernetica AS
#
# All rights are reserved. Reproduction in whole or part is prohibited
# without the written consent of the copyright owner. The usage of this
# code is subject to the appropriate license agreement.
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
FIND_PACKAGE(ExprTk REQUIRED)
FIND_PACKAGE(Cryptopp REQUIRED)
FIND_PACKAGE(LogHard REQUIRED)
FIND_PACKAGE(SharemindCxxHeaders REQUIRED)
FIND_PACKAGE(SharemindLibExecutionProfiler REQUIRED)
FIND_PACKAGE(SharemindLibmodapi REQUIRED)
FIND_PACKAGE(SharemindLibsoftfloat REQUIRED)
FIND_PACKAGE(SharemindLibsoftfloatMath REQUIRED)
FIND_PACKAGE(SharemindVmGlobals REQUIRED)
INCLUDE_DIRECTORIES(
    ${Boost_INCLUDE_DIRS}
    ${ExprTk_INCLUDE_DIRS}
    ${CRYPTOPP_INCLUDE_DIRS}
    ${LogHard_INCLUDE_DIRS}
    ${SharemindCxxHeaders_INCLUDE_DIRS}
    ${SharemindLibExecutionProfiler_INCLUDE_DIRS}
    ${SharemindLibmodapi_INCLUDE_DIRS}
    ${SharemindLibsoftfloat_INCLUDE_DIRS}
    ${SharemindLibsoftfloatMath_INCLUDE_DIRS}
    ${SharemindVmGlobals_INCLUDE_DIRS}
)

ADD_DEFINITIONS(
    ${LogHard_DEFINITIONS}
    ${SharemindCxxHeaders_DEFINITIONS}
    ${SharemindLibExecutionProfiler_DEFINITIONS}
    ${SharemindLibmodapi_DEFINITIONS}
    ${SharemindLibsoftfloat_DEFINITIONS}
    ${SharemindLibsoftfloatMath_DEFINITIONS}
    ${SharemindVmGlobals_DEFINITIONS}
    "-Dexprtk_disable_comments"
    "-Dexprtk_disable_break_continue"
    "-Dexprtk_disable_enhanced_features"
    "-D__STDC_LIMIT_MACROS"
    "-D__STDC_CONSTANT_MACROS"
    "-D__STDC_FORMAT_MACROS"
)

SET(SHARED3P_EMU_SOURCES
    "src/Facilities/ExecutionModelEvaluator.cpp"
    "src/mod_shared3p_emu.cpp"
    "src/RandomFacility.cpp"
    "src/Shared3pConfiguration.cpp"
    "src/Shared3pModule.cpp"
    "src/Shared3pPD.cpp"
    "src/Shared3pPDPI.cpp"
    "src/Syscalls/CRCSyscalls.cpp"
)

SET(SHARED3P_EMU_HEADERS
    "src/BitVector.h"
    "src/Facilities/ExecutionModelEvaluator.h"
    "src/Protocols/Binary.h"
    "src/Protocols/Randomize.h"
    "src/Protocols/Ternary.h"
    "src/Protocols/Unary.h"
    "src/RandomFacility.h"
    "src/Shared3pConfiguration.h"
    "src/Shared3pModule.h"
    "src/Shared3pPD.h"
    "src/Shared3pPDPI.h"
    "src/Shared3pValueHeap.h"
    "src/Shared3pValueTraits.h"
    "src/Shared3pVector.h"
    "src/Syscalls/Common.h"
    "src/Syscalls/CRCSyscalls.h"
    "src/Syscalls/Meta.h"
    "src/VMReferences.h"
)

ADD_LIBRARY(sharemind_mod_shared3p_emu MODULE ${SHARED3P_EMU_SOURCES} ${SHARED3P_EMU_HEADERS})
TARGET_LINK_LIBRARIES(sharemind_mod_shared3p_emu
    ${CRYPTOPP_LIBRARIES}
    ${LogHard_LIBRARIES}
    ${SharemindCxxHeaders_LIBRARIES}
    ${SharemindLibExecutionProfiler_LIBRARIES}
    ${SharemindLibsoftfloat_LIBRARIES}
    ${SharemindLibsoftfloatMath_LIBRARIES}
    ${SharemindVmGlobals_LIBRARIES}
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
