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

CMAKE_MINIMUM_REQUIRED(VERSION 3.0)
PROJECT(SharemindModShared3pEmu VERSION 1.4.0 LANGUAGES CXX)

INCLUDE("${CMAKE_CURRENT_SOURCE_DIR}/config.local" OPTIONAL)
INCLUDE("${CMAKE_CURRENT_BINARY_DIR}/config.local" OPTIONAL)

FIND_PACKAGE(SharemindCMakeHelpers 1.6 REQUIRED)

SET(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/CMakeIncludes/"
                      ${CMAKE_MODULE_PATH})


FIND_PACKAGE(Boost 1.62 COMPONENTS filesystem REQUIRED)
FIND_PACKAGE(Cryptopp REQUIRED)
FIND_PACKAGE(LogHard 0.6.0 REQUIRED)
FIND_PACKAGE(SharemindCHeaders 1.3.0 REQUIRED)
FIND_PACKAGE(SharemindCxxHeaders 0.9.0 REQUIRED)
FIND_PACKAGE(SharemindLibConfiguration 0.6.0 REQUIRED)
FIND_PACKAGE(SharemindLibEmulatorProtocols 0.3.0 REQUIRED)
FIND_PACKAGE(SharemindLibExecutionModelEvaluator 0.4.0 REQUIRED)
FIND_PACKAGE(SharemindLibExecutionProfiler 0.5.0 REQUIRED)
FIND_PACKAGE(SharemindLibSoftfloat 0.3.0 REQUIRED)
FIND_PACKAGE(SharemindLibSoftfloatMath 0.2.0 REQUIRED)
FIND_PACKAGE(SharemindModuleApis 1.1.0 REQUIRED)
FIND_PACKAGE(SharemindPdkHeaders 0.6.0 REQUIRED)


FILE(GLOB_RECURSE SharemindModShared3pEmu_SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")
FILE(GLOB_RECURSE SharemindModShared3pEmu_HEADERS
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.h")
SharemindAddSharedLibrary(ModShared3pEmu
    MODULE
    OUTPUT_NAME "sharemind_mod_shared3p_emu"
    SOURCES
        ${SharemindModShared3pEmu_SOURCES}
        ${SharemindModShared3pEmu_HEADERS}
)
TARGET_INCLUDE_DIRECTORIES(ModShared3pEmu PRIVATE "${CRYPTOPP_INCLUDE_DIR}")
IF(SHAREMIND_NETWORK_STATISTICS)
    TARGET_COMPILE_DEFINITIONS(ModShared3pEmu
        PRIVATE "SHAREMIND_NETWORK_STATISTICS_ENABLE")
ENDIF()
TARGET_LINK_LIBRARIES(ModShared3pEmu
    PRIVATE
        Boost::boost
        Boost::filesystem
        ${CRYPTOPP_LIBRARIES}
        LogHard::LogHard
        Sharemind::CHeaders
        Sharemind::CxxHeaders
        Sharemind::LibConfiguration
        Sharemind::LibEmulatorProtocols
        Sharemind::LibExecutionModelEvaluator
        Sharemind::LibExecutionProfiler
        Sharemind::LibSoftfloat
        Sharemind::LibSoftfloatMath
        Sharemind::ModuleApis
        Sharemind::PdkHeaders
    )

# Configuration files:
INSTALL(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/packaging/configs/sharemind/"
        DESTINATION "/etc/sharemind/"
        COMPONENT "lib"
        EXCLUDE_FROM_ALL)

# Packaging:
SharemindSetupPackaging()
SET(BV
    "${Boost_MAJOR_VERSION}.${Boost_MINOR_VERSION}.${Boost_SUBMINOR_VERSION}"
)

SharemindAddComponentPackage("lib"
    NAME "libsharemind-mod-shared3p-emu"
    DESCRIPTION "Sharemind Shared3p emulation module"
    DEB_SECTION "libs"
    DEB_DEPENDS
        "libboost-filesystem${BV}"
        "libcrypto++6"
        "| libcrypto++8"
        "libloghard (>= 0.5.0)"
        "libsharemind-configuration (>= 0.5.0)"
        "libsharemind-executionmodelevaluator (>= 0.3.0)"
        "libsharemind-executionprofiler (>= 0.4.0)"
        "libsharemind-softfloat (>= 0.3.0)"
        "libsharemind-softfloat-math (>= 0.2.0)"
        "libstdc++6 (>= 4.8.0)"
        "libc6 (>= 2.19)"
    DEB_CONFLICTS
        "libsharemind-mod-shared3p"
        "libsharemind-mod-shared3pdev"
    DEB_EXTRA_CONTROL_FILES
        "${CMAKE_CURRENT_SOURCE_DIR}/packaging/debian/conffiles"

)
SharemindAddComponentPackage("debug"
    NAME "libsharemind-mod-shared3p-emu-dbg"
    DESCRIPTION "Sharemind Shared3p emulation module debug symbols"
    DEB_SECTION "debug"
    DEB_DEPENDS
        "libsharemind-mod-shared3p-emu (= ${SharemindModShared3pEmu_DEB_lib_PACKAGE_VERSION})"
)
SharemindPackagingFinalize()
