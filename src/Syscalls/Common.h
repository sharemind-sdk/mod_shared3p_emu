/*
 * Copyright (C) 2015 Cybernetica
 *
 * Research/Commercial License Usage
 * Licensees holding a valid Research License or Commercial License
 * for the Software may use this file according to the written
 * agreement between you and Cybernetica.
 *
 * GNU General Public License Usage
 * Alternatively, this file may be used under the terms of the GNU
 * General Public License version 3.0 as published by the Free Software
 * Foundation and appearing in the file LICENSE.GPL included in the
 * packaging of this file.  Please review the following information to
 * ensure the GNU General Public License version 3.0 requirements will be
 * met: http://www.gnu.org/copyleft/gpl-3.0.html.
 *
 * For further information, please contact us at sharemind@cyber.ee.
 */

#ifndef MOD_SHARED3P_EMU_SYSCALLS_COMMON_H
#define MOD_SHARED3P_EMU_SYSCALLS_COMMON_H

#include <inttypes.h>
#include <sharemind/compiler-support/GccPR55015.h>
#include <sharemind/Concat.h>
#include <sharemind/ExecutionModelEvaluator.h>
#include <sharemind/ExecutionProfiler.h>
#include <sharemind/libmodapi/api_0x1.h>
#include <sharemind/SyscallsCommon.h>
#include <sstream>
#include "../Shared3pValueTraits.h"

namespace sharemind {

/**
 * \todo maintain unique indexes for all PDKs
 */
static constexpr size_t SHARED3P_EMU_INDEX = 0u;

using VMHandles = PdpiVmHandles<SHARED3P_EMU_INDEX>;

/**
 * Template to get stack value given shared value type.
 */
template <typename T>
inline typename T::public_type getStack(const SharemindCodeBlock & arg);

template <>
inline uint8_t getStack<s3p_bool_t>(const SharemindCodeBlock & arg)
{ return arg.uint8[0]; }
template <>
inline uint8_t getStack<s3p_uint8_t>(const SharemindCodeBlock & arg)
{ return arg.uint8[0]; }
template <>
inline uint16_t getStack<s3p_uint16_t>(const SharemindCodeBlock & arg)
{ return arg.uint16[0]; }
template <>
inline uint32_t getStack<s3p_uint32_t>(const SharemindCodeBlock & arg)
{ return arg.uint32[0]; }
template <>
inline uint64_t getStack<s3p_uint64_t>(const SharemindCodeBlock & arg)
{ return arg.uint64[0]; }
template <>
inline int8_t getStack<s3p_int8_t>(const SharemindCodeBlock & arg)
{ return arg.int8[0]; }
template <>
inline int16_t getStack<s3p_int16_t>(const SharemindCodeBlock & arg)
{ return arg.int16[0]; }
template <>
inline int32_t getStack<s3p_int32_t>(const SharemindCodeBlock & arg)
{ return arg.int32[0]; }
template <>
inline int64_t getStack<s3p_int64_t>(const SharemindCodeBlock & arg)
{ return arg.int64[0]; }
template <>
inline uint8_t getStack<s3p_xor_uint8_t>(const SharemindCodeBlock & arg)
{ return arg.uint8[0]; }
template <>
inline uint16_t getStack<s3p_xor_uint16_t>(const SharemindCodeBlock & arg)
{ return arg.uint16[0]; }
template <>
inline uint32_t getStack<s3p_xor_uint32_t>(const SharemindCodeBlock & arg)
{ return arg.uint32[0]; }
template <>
inline uint64_t getStack<s3p_xor_uint64_t>(const SharemindCodeBlock & arg)
{ return arg.uint64[0]; }
template <>
inline sf_float32 getStack<s3p_float32_t>(const SharemindCodeBlock & arg)
{ return arg.float32[0]; }
template <>
inline sf_float64 getStack<s3p_float64_t>(const SharemindCodeBlock & arg)
{ return arg.float64[0]; }

/**
 * Macros for defining named syscalls and their wrappers
 */
#define NAMED_SYSCALL(fname,name,args,argc,refs,crefs,retVal,c) \
    SharemindModuleApi0x1Error fname( \
            const char * name, \
            SharemindCodeBlock * args, \
            size_t argc, \
            const SharemindModuleApi0x1Reference * refs, \
            const SharemindModuleApi0x1CReference * crefs, \
            SharemindCodeBlock * retVal, \
            SharemindModuleApi0x1SyscallContext * c)

#if SHAREMIND_GCCPR55015
#define NAMED_SYSCALL_WRAPPER(name,...) \
    SharemindModuleApi0x1Error name( \
        SharemindCodeBlock * args, \
        size_t argc, \
        const SharemindModuleApi0x1Reference * refs, \
        const SharemindModuleApi0x1CReference * crefs, \
        SharemindCodeBlock * retVal, \
        SharemindModuleApi0x1SyscallContext * c) \
    { \
        return __VA_ARGS__(("shared3p::" #name), args, argc, refs, crefs, retVal, c); \
    }

#define NAMED_SYSCALL_DEFINITION(signature,fptr) \
  { (signature), &(fptr) }
#else /* SHAREMIND_GCCPR55015 */
#define NAMED_SYSCALL_WRAPPER(name,...) \
    auto name = [](SharemindCodeBlock * args, \
       size_t argc, \
       const SharemindModuleApi0x1Reference * refs, \
       const SharemindModuleApi0x1CReference * crefs, \
       SharemindCodeBlock * retVal, \
       SharemindModuleApi0x1SyscallContext * c) \
            -> SharemindModuleApi0x1Error \
    { \
        return __VA_ARGS__(("shared3p::" #name), args, argc, refs, crefs, retVal, c); \
    };

#define NAMED_SYSCALL_DEFINITION(signature,fptr) \
  { (signature), (fptr) }
#endif /* SHAREMIND_GCCPR55015 */


/**
 * Macros for profiling syscalls
 */
/// \todo evaluate() returns double. Make sure we can cast it to UsTime.
#ifdef SHAREMIND_NETWORK_STATISTICS_ENABLE
#define PROFILE_SYSCALL(profiler,evaluator,name,parameter) \
    do { \
        static const uint32_t sectionTypeId = \
            (profiler).newSectionType((name)); \
        sharemind::ExecutionModelEvaluator::Model * const timeModel = \
            (evaluator).model("TimeModel", (name)); \
        if (timeModel) \
            (profiler).addSection(sectionTypeId, (parameter), 0u, \
                    timeModel->evaluate((parameter)), \
                    sharemind::MinerNetworkStatistics(), \
                    sharemind::MinerNetworkStatistics()); \
    } while (false)
#else
#define PROFILE_SYSCALL(profiler,evaluator,name,parameter) \
    do { \
        static const uint32_t sectionTypeId = \
            (profiler).newSectionType((name)); \
        sharemind::ExecutionModelEvaluator::Model * const timeModel = \
            (evaluator).model("TimeModel", (name)); \
        if (timeModel) \
            (profiler).addSection(sectionTypeId, (parameter), 0u, \
                    timeModel->evaluate((parameter))); \
    } while (false)
#endif

} /* namespace sharemind */

#endif /* MOD_SHARED3P_EMU_SYSCALLS_COMMON_H */
