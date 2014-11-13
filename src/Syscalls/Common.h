/*
 * This file is a part of the Sharemind framework.
 * Copyright (C) Cybernetica AS
 *
 * All rights are reserved. Reproduction in whole or part is prohibited
 * without the written consent of the copyright owner. The usage of this
 * code is subject to the appropriate license agreement.
 */

#ifndef MOD_ADDITIVE3P_SYSCALLS_COMMON_H
#define MOD_ADDITIVE3P_SYSCALLS_COMMON_H

#include "../Shared3pValueTraits.h"

#include <inttypes.h>
#include <sharemind/libmodapi/api_0x1.h>
#include <cstdio>

namespace sharemind {

/**
 * Template to get stack value given shared value type.
 */
template <typename T> inline typename T::public_type getStack (const SharemindCodeBlock& arg);

template <> inline uint8_t getStack<s3p_bool_t>(const SharemindCodeBlock& arg) { return arg.uint8[0]; }
template <> inline uint8_t getStack<s3p_uint8_t>(const SharemindCodeBlock& arg) { return arg.uint8[0]; }
template <> inline uint16_t getStack<s3p_uint16_t>(const SharemindCodeBlock& arg) { return arg.uint16[0]; }
template <> inline uint32_t getStack<s3p_uint32_t>(const SharemindCodeBlock& arg) { return arg.uint32[0]; }
template <> inline uint64_t getStack<s3p_uint64_t>(const SharemindCodeBlock& arg) { return arg.uint64[0]; }
template <> inline int8_t getStack<s3p_int8_t>(const SharemindCodeBlock& arg) { return arg.int8[0]; }
template <> inline int16_t getStack<s3p_int16_t>(const SharemindCodeBlock& arg) { return arg.int16[0]; }
template <> inline int32_t getStack<s3p_int32_t>(const SharemindCodeBlock& arg) { return arg.int32[0]; }
template <> inline int64_t getStack<s3p_int64_t>(const SharemindCodeBlock& arg) { return arg.int64[0]; }
template <> inline uint8_t getStack<s3p_xor_uint8_t>(const SharemindCodeBlock& arg) { return arg.uint8[0]; }
template <> inline uint16_t getStack<s3p_xor_uint16_t>(const SharemindCodeBlock& arg) { return arg.uint16[0]; }
template <> inline uint32_t getStack<s3p_xor_uint32_t>(const SharemindCodeBlock& arg) { return arg.uint32[0]; }
template <> inline uint64_t getStack<s3p_xor_uint64_t>(const SharemindCodeBlock& arg) { return arg.uint64[0]; }
template <> inline sf_float32 getStack<s3p_float32_t>(const SharemindCodeBlock& arg) { return arg.float32[0]; }
template <> inline sf_float64 getStack<s3p_float64_t>(const SharemindCodeBlock& arg) { return arg.float64[0]; }
template <> inline uint32_t getStack<s3p_fix32_t>(const SharemindCodeBlock& arg) { return arg.uint32[0]; }
template <> inline uint64_t getStack<s3p_fix64_t>(const SharemindCodeBlock& arg) { return arg.uint64[0]; }

/**
 * Basic syscall parameter verification.
 */
template < const size_t NumArgs
         , const bool   NeedReturnValue = false
         , const size_t NumRefs = 0u
         , const size_t NumCRefs = 0u
         >
struct __attribute__ ((visibility("internal"))) SyscallArgs {
    template <class T>
    static inline size_t countRefs(const T * refs) {
        if (!refs)
            return 0u;

        assert(refs->pData);
        size_t r = 1u;
        while ((++refs)->pData)
            r++;
        return r;
    }

    static inline bool check(const size_t num_args,
                             const SharemindModuleApi0x1Reference * const refs,
                             const SharemindModuleApi0x1CReference * const crefs,
                             const SharemindCodeBlock * const returnValue)
    {
        if (num_args != NumArgs)
            return false;

        if (NeedReturnValue && !returnValue)
            return false;

        if (countRefs(refs) != NumRefs)
            return false;

        if (countRefs(crefs) != NumCRefs)
            return false;

        return true;
    }
};

/**
 * Virtual machine handle representation. Validates that the handle is correct.
 */
class __attribute__ ((visibility("internal"))) VMHandles: public SharemindModuleApi0x1PdpiInfo {
public: /* Methods: */
    VMHandles () {
        pdpiHandle = 0;
        pdHandle = 0;
        pdkIndex = 0u;
        moduleHandle = 0;
    }


    inline bool get (SharemindModuleApi0x1SyscallContext* c, SharemindCodeBlock* args, size_t index = 0) {
        assert (c != 0 && args != 0);

        const size_t SHARED3P_INDEX = 0u;

        const SharemindModuleApi0x1PdpiInfo * pdpiInfo = (*(c->get_pdpi_info))(c, args[index].uint64[0]);
        if (!pdpiInfo) {
            fprintf (stderr, "get_pd_process_instance_handle (%" PRIu64 ")\n", args[index].uint64[0]);
            return false;
        }

        if (pdpiInfo->pdkIndex != SHARED3P_INDEX       // or wrong pdk is returned
            || pdpiInfo->moduleHandle != c->moduleHandle) // or module handle pointers mismatch
            return false;

        assert(pdpiInfo->pdpiHandle);

        SharemindModuleApi0x1PdpiInfo::operator = (*pdpiInfo);

        return true;
    }
};

/**
 * Centralized exception handling.
 */
inline SharemindModuleApi0x1Error catchModuleApiErrors() noexcept
        __attribute__ ((visibility("internal")));

inline SharemindModuleApi0x1Error catchModuleApiErrors() noexcept {
    try {
        if (const auto eptr = std::current_exception()) {
            std::rethrow_exception(eptr);
        } else {
            return SHAREMIND_MODULE_API_0x1_GENERAL_ERROR;
        }
    } catch (const std::bad_alloc &) {
        return SHAREMIND_MODULE_API_0x1_OUT_OF_MEMORY;
    } catch (...) {
        return SHAREMIND_MODULE_API_0x1_GENERAL_ERROR;
    }
}


} /* namespace sharemind */

#endif /* MOD_ADDITIVE3P_SYSCALLS_COMMON_H */
