/*
 * This file is a part of the Sharemind framework.
 * Copyright (C) Cybernetica AS
 *
 * All rights are reserved. Reproduction in whole or part is prohibited
 * without the written consent of the copyright owner. The usage of this
 * code is subject to the appropriate license agreement.
 */

#include <boost/static_assert.hpp>
#include <cassert>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <inttypes.h>
#include <limits>
#include <new>
#include <sharemind/3rdparty/libsoftfloat/softfloat.h>
#include <sharemind/compiler-support/GccIsNothrowDestructible.h>
#include <sharemind/ExecutionProfiler.h>
#include <sharemind/libmodapi/api_0x1.h>
#include <sharemind/miner/libpd.h>
#include <sharemind/miner/libprocessfacility.h>
#include <sharemind/Random/IRandom.h>
#include <sharemind/uint128_t.h>
#include <vector>
#include <type_traits>
#include "Facilities/ExecutionModelEvaluator.h"
#include "Shared3pModule.h"
#include "Shared3pPDPI.h"
#include "Syscalls/Common.h"
#include "Syscalls/CRCSyscalls.h"
#include "Syscalls/Meta.h"
#include "Protocols/unary.h"
#include "VMReferences.h"


namespace {

using namespace sharemind;

template <typename T>
inline void copy_shares(const s3p_vec<T> & src,
                        typename value_traits<T>::share_type * dest)
{
    std::copy(src.begin(), src.end(), dest);
}

template <>
inline void copy_shares<s3p_bool_t>(const s3p_vec<s3p_bool_t> & src,
                                     s3p_bool_t::share_type * dest)
{
    for (size_t i = 0u; i < src.size(); ++i)
        dest[i] = src[i];
}

/**
 * SysCall: new_vec<T>
 * Stack:
 *      0) uint64[0u]     pd index
 *      1) uint64[0u]     vector size
 * RetVal:
 *      0) p[0u]          vector handle or 0 if allocation failed
 * Precondition:
 *      Enough resources are available to allocate vector of given size.
 * Postcondition:
 *      Return value is set to valid handle to args[1u].uint64[0u] sized vector of type T.
 * Effect:
 *      Allocates memory for the vector.
 */
template <typename T>
SHAREMIND_MODULE_API_0x1_SYSCALL(new_vec,
                                 args, num_args, refs, crefs,
                                 returnValue, c)
{
    VMHandles handles;
    if (!SyscallArgs<2, true>::check(num_args, refs, crefs, returnValue) ||
        !handles.get(c, args)) {
        return SHAREMIND_MODULE_API_0x1_INVALID_CALL;
    }

    // Check if the size result would overflow
    if (std::numeric_limits<size_t>::max() / sizeof(T) < args[1u].uint64[0u]) {
        returnValue->p[0u] = 0;
        return SHAREMIND_MODULE_API_0x1_INVALID_CALL;
    }

    try {
        Shared3pPDPI * const pdpi = static_cast<Shared3pPDPI*>(handles.pdpiHandle);

        s3p_vec<T> * const vec = new s3p_vec<T>(args[1u].uint64[0u]);
        pdpi->registerVector(vec);

        returnValue->p[0u] = vec;

        return SHAREMIND_MODULE_API_0x1_OK;
    } catch (...) {
        return catchModuleApiErrors ();
    }
}

/**
 * SysCall: init_vec<T>
 * Stack:
 *      0) uint64[0u]     pd index
 *      1) uint64[0u]     the value to initialize vector elements with
 *      2) p[0u]          destination handle
 * Precondition:
 *      Destination handle is valid vector of type T.
 * Postcondition:
 *      Destination vector is initialized to shares of second stack position.
 * Effect:
 *      Networks communication is performed to share the value.
 */
template <typename T>
SHAREMIND_MODULE_API_0x1_SYSCALL(init_vec,
                                 args, num_args, refs, crefs,
                                 returnValue, c)
{
    VMHandles handles;
    if (!SyscallArgs<3>::check(num_args, refs, crefs, returnValue) ||
        !handles.get(c, args)) {
        return SHAREMIND_MODULE_API_0x1_INVALID_CALL; // Signal that the call was invalid.
    }

    try {
        Shared3pPDPI * const pdpi = static_cast<Shared3pPDPI*>(handles.pdpiHandle);
        if (!pdpi->isValidHandle<T>(args[2u].p[0u]))
            return SHAREMIND_MODULE_API_0x1_GENERAL_ERROR;

        s3p_vec<T> & vec = *static_cast<s3p_vec<T>*>(args[2u].p[0u]);
        const typename T::public_type init = getStack<T>(args[1u]);
        for (size_t i = 0u; i < vec.size(); ++i)
            vec[i] = init;

        return SHAREMIND_MODULE_API_0x1_OK;
    } catch (...) {
        return catchModuleApiErrors ();
    }
}

/**
 * SysCall: set_shares<T>
 * Stack:
 *      0) uint64[0u]     pd index
 *      1) p[0u]          destination handle (optional)
 * CRef: memory to copy the shares from
 * Returns size of the private vector (optional)
 */
template<typename T>
SHAREMIND_MODULE_API_0x1_SYSCALL(set_shares,
                                 args, num_args, refs, crefs,
                                 returnValue, c)
{
    if (!crefs || crefs[1u].pData)
        return SHAREMIND_MODULE_API_0x1_INVALID_CALL;

    if (refs)
        return SHAREMIND_MODULE_API_0x1_INVALID_CALL;

    if (num_args != 1 && num_args != 2)
        return SHAREMIND_MODULE_API_0x1_INVALID_CALL;

    VMHandles handles;
    if (!handles.get(c, args))
        return SHAREMIND_MODULE_API_0x1_INVALID_CALL;

    try {
        Shared3pPDPI * const pdpi = static_cast<Shared3pPDPI*>(handles.pdpiHandle);

        typedef typename value_traits<T>::share_type share_type;
        const share_type* src = static_cast<const share_type*>(crefs[0u].pData);
        // TODO: the following is a workaround!We are always allocating one
        // byte too much as VM does not allow us to allocate 0 sized memory block.
        const size_t num_elems = (crefs[0u].size - 1) / sizeof(share_type);

        if (num_args == 2) {
            if (!pdpi->isValidHandle<T>(args[1u].p[0u]))
                return SHAREMIND_MODULE_API_0x1_GENERAL_ERROR;

            s3p_vec<T> & dest = *static_cast<s3p_vec<T>*>(args[1u].p[0u]);
            if (dest.size() != num_elems)
                return SHAREMIND_MODULE_API_0x1_GENERAL_ERROR;

            dest.assign(src, src + num_elems);
        }

        if (returnValue)
            returnValue->uint64[0u] = num_elems;

        return SHAREMIND_MODULE_API_0x1_OK;
    } catch (...) {
        return catchModuleApiErrors ();
    }
}

/**
 * SysCall: get_shares<T>
 * Stack:
 *      0) uint64[0u]     pd index
 *      1) p[0u]          source handle
 * Ref: memory to copy the shares to (optional)
 * Returns the size of the bytes to write (optional)
 */
template<typename T>
SHAREMIND_MODULE_API_0x1_SYSCALL(get_shares,
                                 args, num_args, refs, crefs,
                                 returnValue, c)
{
    if (refs && refs[1u].pData)
        return SHAREMIND_MODULE_API_0x1_INVALID_CALL;

    if (crefs)
        return SHAREMIND_MODULE_API_0x1_INVALID_CALL;

    if (num_args != 2)
        return SHAREMIND_MODULE_API_0x1_INVALID_CALL;

    VMHandles handles;
    if (!handles.get(c, args))
        return SHAREMIND_MODULE_API_0x1_INVALID_CALL;

    try {
        Shared3pPDPI * const pdpi = static_cast<Shared3pPDPI*>(handles.pdpiHandle);

        if (!pdpi->isValidHandle<T>(args[1u].p[0u]))
            return SHAREMIND_MODULE_API_0x1_GENERAL_ERROR;

        typedef typename value_traits<T>::share_type share_type;
        const s3p_vec<T> & src = *static_cast<const s3p_vec<T>*>(args[1u].p[0u]);
        const size_t num_bytes = src.size() * sizeof(share_type);

        if (refs) {
            // TODO: the following is a workaround!We are always allocating one
            // byte too much as VM does not allow us to allocate 0 sized memory block.
            if (num_bytes != refs[0u].size - 1)
                return SHAREMIND_MODULE_API_0x1_GENERAL_ERROR;

            /// \todo
            share_type * dest = static_cast<share_type*>(refs[0u].pData);
            copy_shares (src, dest);
        }

        if (returnValue)
            returnValue->uint64[0u] = num_bytes;

        return SHAREMIND_MODULE_API_0x1_OK;
    } catch (...) {
        return catchModuleApiErrors ();
    }
}

/**
 * SysCall: get_type_size<T>
 * Stack:
 *      0) uint64[0u]     pd index
 * Ref: memory to copy the shares to (optional)
 * Returns the size of the bytes to write (optional)
 */
template<typename T>
SHAREMIND_MODULE_API_0x1_SYSCALL(get_type_size,
                                 args, num_args, refs, crefs,
                                 returnValue, c)
{
    if (refs)
        return SHAREMIND_MODULE_API_0x1_INVALID_CALL;

    if (crefs)
        return SHAREMIND_MODULE_API_0x1_INVALID_CALL;

    if (num_args != 1)
        return SHAREMIND_MODULE_API_0x1_INVALID_CALL;

    if (!returnValue)
        return SHAREMIND_MODULE_API_0x1_INVALID_CALL;

    VMHandles handles;
    if (!handles.get(c, args))
        return SHAREMIND_MODULE_API_0x1_INVALID_CALL;

    try {
        typedef typename value_traits<T>::share_type share_type;
        returnValue->uint64[0u] = sizeof(share_type);

        return SHAREMIND_MODULE_API_0x1_OK;
    } catch (...) {
        return catchModuleApiErrors ();
    }
}

/**
 * SysCall: fill_vec
 * Stack:
 *      0) uint64[0u]     pd index
 *      1) p[0u]          argument handle
 *      2) p[0u]          output handle
 * Precondition:
 *      Both handles point to valid vectors of type T.
 *      Argument vector is not empty.
 * Postcondition:
 *      All elements of the output vector are equal to the first element of argument vector.
 */
template <typename T>
SHAREMIND_MODULE_API_0x1_SYSCALL(fill_vec,
                                 args, num_args, refs, crefs,
                                 returnValue, c)
{
    VMHandles handles;
    if (!SyscallArgs<3>::check(num_args, refs, crefs, returnValue) ||
        !handles.get(c, args)) {
        return SHAREMIND_MODULE_API_0x1_INVALID_CALL; // Signal that the call was invalid.
    }

    try {
        void * const srcHandle = args[1u].p[0u];
        void * const destHandle = args[2u].p[0u];
        Shared3pPDPI * const pdpi = static_cast<Shared3pPDPI*>(handles.pdpiHandle);

        if (!pdpi->isValidHandle<T>(srcHandle) ||
            !pdpi->isValidHandle<T>(destHandle)) {
            return SHAREMIND_MODULE_API_0x1_GENERAL_ERROR;
        }

        s3p_vec<T> & dest = *static_cast<s3p_vec<T>*>(destHandle);
        const s3p_vec<T> & src = *static_cast<s3p_vec<T>*>(srcHandle);

        for (size_t i = 0; i < dest.size(); ++i)
            dest[i] = src[0u];

        return SHAREMIND_MODULE_API_0x1_OK;
    } catch (...) {
        return catchModuleApiErrors ();
    }
}

/**
 * SysCall: assign_vec
 * Stack:
 *     0) uint64[0u]     pd index
 *     1) p[0u]          argument vector handle
 *     2) p[0u]          output vector handle
 * Precondition:
 *     Both handles point to valid uint32 vector handles.
 *     Both vectors have same length.
 * Postcondition:
 *     Output vector contains same elements in the same order as argument vector.
 * Effect:
 *     No reclassification is performed.
 */
template <typename T>
SHAREMIND_MODULE_API_0x1_SYSCALL(assign_vec,
                                 args, num_args, refs, crefs,
                                 returnValue, c)
{
    VMHandles handles;
    if (!SyscallArgs<3>::check(num_args, refs, crefs, returnValue) ||
        !handles.get(c, args)) {
        return SHAREMIND_MODULE_API_0x1_INVALID_CALL;
    }

    try {
        void * const srcHandle = args[1u].p[0u];
        void * const destHandle = args[2u].p[0u];

        Shared3pPDPI * const pdpi = static_cast<Shared3pPDPI*>(handles.pdpiHandle);

        if (!pdpi->isValidHandle<T>(srcHandle) ||
            !pdpi->isValidHandle<T>(destHandle)) {
            return SHAREMIND_MODULE_API_0x1_GENERAL_ERROR;
        }

        const s3p_vec<T> & src = *static_cast<s3p_vec<T>*>(srcHandle);
        s3p_vec<T> & dest = *static_cast<s3p_vec<T>*>(destHandle);

        if (src.size() != dest.size())
            return SHAREMIND_MODULE_API_0x1_INVALID_CALL;

        dest.assign(src);

        return SHAREMIND_MODULE_API_0x1_OK;
    } catch (...) {
        return catchModuleApiErrors ();
    }
}

/**
 * SysCall: declassify_vec<T>
 * Stack:
 *     0) uint64[0u]     pd index
 *     1) p[0u]          argument vector handle
 * Refs:
 *     0) refs[0u]       reference to destination data
 * Precondition:
 *     The argument vector points to valid vector of type T.
 *     Destination data can contain at least all of the input data.
 * Postcondition:
 *     Data denoted by the destination vector contains all declassified input data.
 */
template <typename T>
SHAREMIND_MODULE_API_0x1_SYSCALL(declassify_vec,
                                 args, num_args, refs, crefs,
                                 returnValue, c)
{
    VMHandles handles;
    if (!SyscallArgs<2, false, 1, 0>::check(num_args, refs, crefs, returnValue) ||
        !handles.get(c, args)) {
        return SHAREMIND_MODULE_API_0x1_INVALID_CALL; // Signal that the call was invalid.
    }

    try {
        Shared3pPDPI * const pdpi = static_cast<Shared3pPDPI*>(handles.pdpiHandle);
        mutable_vm_vec<T> dest(refs[0u]);

        void * const srcHandle = args[1u].p[0u];
        if (!pdpi->isValidHandle<T>(srcHandle)) {
            return SHAREMIND_MODULE_API_0x1_GENERAL_ERROR;
        }

        const s3p_vec<T> & src = *static_cast<s3p_vec<T>*>(srcHandle);
        if (refs[0u].size < sizeof(T) * src.size()) {
            return SHAREMIND_MODULE_API_0x1_GENERAL_ERROR;
        }

        for (size_t i = 0u; i < src.size(); ++i)
            dest[i] = src[i];

        return SHAREMIND_MODULE_API_0x1_OK;
    } catch (...) {
        return catchModuleApiErrors ();
    }
}

/**
 * SysCall: classify_vec
 * Args:
 *     0) uint64[0u]     pd index
 *     1) p[0u]          handle to destination vector
 * CRefs:
 *     0) crefs[0u]      reference to source data
 * Precondition:
 *     The destination handle is valid vector or type T.
 *     The source data contains no less data that destination vector is able to contain.
 * Postcondition:
 *     The destination vector contains all of the data, referred to by the source reference, as shares.
 */
template <typename T>
SHAREMIND_MODULE_API_0x1_SYSCALL(classify_vec,
                                 args, num_args, refs, crefs,
                                 returnValue, c)
{
    VMHandles handles;
    if (!SyscallArgs<2, false, 0, 1>::check(num_args, refs, crefs, returnValue) ||
        !handles.get(c, args)) {
        return SHAREMIND_MODULE_API_0x1_INVALID_CALL; // Signal that the call was invalid.
    }

    try {
        Shared3pPDPI * const pdpi = static_cast<Shared3pPDPI*>(handles.pdpiHandle);

        void * const destHandle = args[1u].p[0u];
        if (!pdpi->isValidHandle<T>(destHandle)) {
            return SHAREMIND_MODULE_API_0x1_GENERAL_ERROR;
        }

        s3p_vec<T> & dest = *static_cast<s3p_vec<T>*>(destHandle);
        immutable_vm_vec<T> src(crefs[0u]);
        if (sizeof(T) * dest.size() > crefs[0u].size) {
            return SHAREMIND_MODULE_API_0x1_GENERAL_ERROR;
        }

        for (size_t i = 0u; i < src.size(); ++i)
            dest[i] = src[i];

        return SHAREMIND_MODULE_API_0x1_OK;
    } catch (...) {
        return catchModuleApiErrors ();
    }
}

/**
 * SysCall: delete_vec<T>
 * Args:
 *      0) uint64[0u]     pd index
 *      1) p[0u]          vector handle
 * Precondition:
 *      The handle points to valid vector of type T.
 * Postcondition:
 *      The handle no longer points to a vector.
 * Effect:
 *      The vector pointer to by the handle is freed.
 */
template <typename T>
SHAREMIND_MODULE_API_0x1_SYSCALL(delete_vec,
                                 args, num_args, refs, crefs,
                                 returnValue, c)
{
    VMHandles handles;
    if (!SyscallArgs<2>::check(num_args, refs, crefs, returnValue) ||
        !handles.get(c, args)) {
        return SHAREMIND_MODULE_API_0x1_INVALID_CALL; // Signal that the call was invalid.
    }

    try {
        Shared3pPDPI * const pdpi = static_cast<Shared3pPDPI*>(handles.pdpiHandle);

        void * const vecHandle = args[1u].p[0u];
        if (!pdpi->isValidHandle<T>(vecHandle)) {
            return SHAREMIND_MODULE_API_0x1_GENERAL_ERROR;
        }

        s3p_vec<T> * vec = static_cast<s3p_vec<T>*>(vecHandle);
        pdpi->freeRegisteredVector(vec);

        return SHAREMIND_MODULE_API_0x1_OK;
    } catch (...) {
        return catchModuleApiErrors ();
    }
}

/**
 * SysCall: load_vec<T>
 * Args:
 *     0) uint64[0u]     pd index
 *     1) p[0u]          argument vector handle
 *     2) uint64[0u]     index
 *     3) p[0u]          destination vector handle
 * Precondition:
 *     Both handles point to valid vectors of type T.
 *     Destination vector is not empty.
 *     The size of argument vector is greater than the supplied index.
 * Postcondition:
 *     The first position of output vector contains the share of argument vector from the position denoted by the index.
 * Effect:
 *     No reclassification is performed.
 */
template <typename T>
SHAREMIND_MODULE_API_0x1_SYSCALL(load_vec,
                                 args, num_args, refs, crefs,
                                 returnValue, c)
{
    VMHandles handles;
    if (!SyscallArgs<4>::check(num_args, refs, crefs, returnValue) ||
        !handles.get(c, args)) {
        return SHAREMIND_MODULE_API_0x1_INVALID_CALL; // Signal that the call was invalid.
    }

    try {
        Shared3pPDPI * const pdpi = static_cast<Shared3pPDPI*>(handles.pdpiHandle);

        void * const srcHandle = args[1u].p[0u];
        void * const destHandle = args[3u].p[0u];
        uint64_t index = args[2u].uint64[0u];

        if (!pdpi->isValidHandle<T>(srcHandle) ||
            !pdpi->isValidHandle<T>(destHandle)) {
            return SHAREMIND_MODULE_API_0x1_GENERAL_ERROR;
        }

        const s3p_vec<T> & src = *static_cast<s3p_vec<T>*>(srcHandle);
        s3p_vec<T> & dest = *static_cast<s3p_vec<T>*>(destHandle);

        if (dest.empty() || !(index < src.size())) {
            return SHAREMIND_MODULE_API_0x1_GENERAL_ERROR;
        }

        dest[0u] = src[index];

        return SHAREMIND_MODULE_API_0x1_OK;
    } catch (...) {
        return catchModuleApiErrors ();
    }
}

/**
 * SysCall: store_vec<T>
 * Args:
 *     0) uint64[0u]     pd index
 *     1) p[0u]          source vector handle
 *     2) uint64[0u]     index
 *     3) p[0u]          destination vector handle
 * Precondition:
 *     Both handles point to valid vectors of type T.
 *     Source vector is not empty.
 *     The lenght of destination vector is greater than the supplied index.
 * Postcondition:
 *     The position denoted by the index of destination vector contains the first share of the source vector.
 * Effect:
 *     No reclassification is performed.
 */
template <typename T>
SHAREMIND_MODULE_API_0x1_SYSCALL(store_vec,
                                 args, num_args, refs, crefs,
                                 returnValue, c)
{
    VMHandles handles;
    if (!SyscallArgs<4>::check(num_args, refs, crefs, returnValue) ||
        !handles.get(c, args)) {
        return SHAREMIND_MODULE_API_0x1_INVALID_CALL; // Signal that the call was invalid.
    }

    try {
        Shared3pPDPI * const pdpi = static_cast<Shared3pPDPI*>(handles.pdpiHandle);

        void * const srcHandle = args[1u].p[0u];
        void * const destHandle = args[3u].p[0u];
        uint64_t index = args[2u].uint64[0u];

        if (!pdpi->isValidHandle<T>(srcHandle) ||
            !pdpi->isValidHandle<T>(destHandle)) {
            return SHAREMIND_MODULE_API_0x1_GENERAL_ERROR;
        }

        const s3p_vec<T> & src = *static_cast<s3p_vec<T>*>(srcHandle);
        s3p_vec<T> & dest = *static_cast<s3p_vec<T>*>(destHandle);

        if (src.empty() || !(index < dest.size())) {
            return SHAREMIND_MODULE_API_0x1_GENERAL_ERROR;
        }

        dest[index] = src[0u];

        return SHAREMIND_MODULE_API_0x1_OK;
    } catch (...) {
        return catchModuleApiErrors ();
    }
}


/**
 * SysCall: mulc_vec<T, Protocol>
 * Args:
 *      0) uint64[0u]     pd index
 *      1) p[0u]          argument handle
 *      2) p[0u]          result handle
 *      crefs[0u]         public argument vector
 * Precondition:
 *      All handles are valid vectors of type T.
 *      All vectors are of equal length.
 *      Public reference contains no less elements than share vectors.
 *      Protocol is a suitable protocol for performing the division on T.
 * Postcondition:
 *      Output vector contains shares of multiplication of argument vectors.
 */
template <typename T, typename Protocol>
NAMED_SYSCALL(mulc_vec, name, args, num_args, refs, crefs, returnValue, c)
{
    VMHandles handles;
    if (!SyscallArgs<3, false, 0, 1>::check(num_args, refs, crefs, returnValue) ||
        !handles.get(c, args)) {
        return SHAREMIND_MODULE_API_0x1_INVALID_CALL; // Signal that the call was invalid.
    }

    try {
        Shared3pPDPI * const pdpi = static_cast<Shared3pPDPI*>(handles.pdpiHandle);

        void * const param1Handle = args[1u].p[0u];
        void * const resultHandle = args[2u].p[0u];

        if (!pdpi->isValidHandle<T>(param1Handle) ||
            !pdpi->isValidHandle<T>(resultHandle)) {
            return SHAREMIND_MODULE_API_0x1_GENERAL_ERROR;
        }

        const s3p_vec<T> & param1 = *static_cast<s3p_vec<T>*>(param1Handle);
        const immutable_vm_vec<T> param2(crefs[0u]);
        s3p_vec<T> & result = *static_cast<s3p_vec<T>*>(resultHandle);

        if (param1.size() <= crefs[0u].size / sizeof(T) ||
                param1.size() != result.size())
            return SHAREMIND_MODULE_API_0x1_INVALID_CALL;

        for (size_t i = 0u; i < param1.size(); ++i)
            result[i] = param1[i] * param2[i];

        PROFILE_SYSCALL(pdpi->profiler(), pdpi->modelEvaluator(), name,
                        param1.size());

        return SHAREMIND_MODULE_API_0x1_OK;
    } catch (...) {
        return catchModuleApiErrors ();
    }
}

/**
 * SysCall: divc_vec<T, Protocol>
 * Args:
 *      0) uint64[0u]     pd index
 *      1) p[0u]          argument handle
 *      2) p[0u]          result handle
 *      crefs[0u]         public argument vector
 * Precondition:
 *      All handles are valid vectors of type T.
 *      All vectors are of equal length.
 *      Public reference contains no less elements than share vectors.
 *      Protocol is a suitable protocol for performing the division on T.
 * Postcondition:
 *      Output vector contains shares of division of argument vectors.
 */
template <typename T, typename Protocol>
NAMED_SYSCALL(divc_vec, name, args, num_args, refs, crefs, returnValue, c)
{
    VMHandles handles;
    if (!SyscallArgs<3u, false, 0u, 1u>::check(num_args, refs, crefs, returnValue) ||
        !handles.get(c, args)) {
        return SHAREMIND_MODULE_API_0x1_INVALID_CALL; // Signal that the call was invalid.
    }

    try {
        Shared3pPDPI * const pdpi = static_cast<Shared3pPDPI*>(handles.pdpiHandle);

        void * const param1Handle = args[1u].p[0u];
        void * const resultHandle = args[2u].p[0u];

        if (!pdpi->isValidHandle<T>(param1Handle) ||
            !pdpi->isValidHandle<T>(resultHandle)) {
            return SHAREMIND_MODULE_API_0x1_GENERAL_ERROR;
        }

        const s3p_vec<T> & param1 = *static_cast<s3p_vec<T>*>(param1Handle);
        const immutable_vm_vec<T> param2(crefs[0u]);
        s3p_vec<T> & result = *static_cast<s3p_vec<T>*>(resultHandle);

        if (param1.size() <= crefs[0u].size / sizeof(T) ||
                param1.size() != result.size())
            return SHAREMIND_MODULE_API_0x1_INVALID_CALL;

        for (size_t i = 0u; i < param1.size(); ++i) {
            if (param2[i] == 0u)
                return SHAREMIND_MODULE_API_0x1_GENERAL_ERROR;
            result[i] = param1[i] / param2[i];
        }

        PROFILE_SYSCALL(pdpi->profiler(), pdpi->modelEvaluator(), name,
                        param1.size());

        return SHAREMIND_MODULE_API_0x1_OK;
    } catch (...) {
        return catchModuleApiErrors ();
    }
}

/**
 * SysCall: modc_vec<T>
 * Args:
 *      0) uint64[0u]     pd index
 *      1) p[0u]          argument handle
 *      2) p[0u]          result handle
 *      crefs[0u]         public argument vector
 * Precondition:
 *      All handles are valid vectors of type T.
 *      All vectors are of equal length.
 *      Public reference contains no less elements than share vectors.
 * Postcondition:
 *      Output vector contains shares of modular division of argument vectors.
 */
template <typename T>
NAMED_SYSCALL(modc_vec, name, args, num_args, refs, crefs, returnValue, c)
{
    VMHandles handles;
    if (!SyscallArgs<3u, false, 0u, 1u>::check(num_args, refs, crefs, returnValue) ||
        !handles.get(c, args)) {
        return SHAREMIND_MODULE_API_0x1_INVALID_CALL; // Signal that the call was invalid.
    }

    try {
        Shared3pPDPI * const pdpi = static_cast<Shared3pPDPI*>(handles.pdpiHandle);

        void * const param1Handle = args[1u].p[0u];
        void * const resultHandle = args[2u].p[0u];

        if (!pdpi->isValidHandle<T>(param1Handle) ||
            !pdpi->isValidHandle<T>(resultHandle)) {
            return SHAREMIND_MODULE_API_0x1_GENERAL_ERROR;
        }

        const s3p_vec<T> & param1 = *static_cast<s3p_vec<T>*>(param1Handle);
        const immutable_vm_vec<T> param2 (crefs[0u]);
        s3p_vec<T> & result = *static_cast<s3p_vec<T>*>(resultHandle);

        if (param1.size() <= crefs[0u].size / sizeof(T) ||
                param1.size() != result.size())
            return SHAREMIND_MODULE_API_0x1_INVALID_CALL;

        for (size_t i = 0u; i < param1.size(); ++i) {
            if (param2[i] == 0u)
                return SHAREMIND_MODULE_API_0x1_GENERAL_ERROR;
            result[i] = param1[i] % param2[i];
        }

        PROFILE_SYSCALL(pdpi->profiler(), pdpi->modelEvaluator(), name,
                        param1.size());

        return SHAREMIND_MODULE_API_0x1_OK;
    } catch (...) {
        return catchModuleApiErrors ();
    }
}

struct ModeLT {
    template <typename A, typename B>
    static inline bool invoke(const A & a, const B & b) noexcept
    { return a < b; }
};

struct ModeLE {
    template <typename A, typename B>
    static inline bool invoke(const A & a, const B & b) noexcept
    { return a <= b; }
};

/**
 * SysCall: compare_vec<T, Protocol, flipParams>
 * Args:
 *      0) uint64[0u]     pd index
 *      1) p[0u]          input handle (lhs)
 *      2) p[0u]          input handle (rhs)
 *      3) p[0u]          output handle
 * Precondition:
 *      All input handles are valid vectors of type T.
 *      All vectors are of equal length.
 */
template <typename T, typename Mode, bool flipParams>
NAMED_SYSCALL(compare_vec, name, args, num_args, refs, crefs, returnValue, c)
{
    VMHandles handles;
    if (!SyscallArgs<4>::check(num_args, refs, crefs, returnValue) ||
        !handles.get(c, args)) {
        return SHAREMIND_MODULE_API_0x1_INVALID_CALL;
    }

    try {
        Shared3pPDPI * const pdpi = static_cast<Shared3pPDPI*>(handles.pdpiHandle);

        void * const param1Handle = args[flipParams ? 2u : 1u].p[0u];
        void * const param2Handle = args[flipParams ? 1u : 2u].p[0u];
        void * const resultHandle = args[3u].p[0u];

        if (!pdpi->isValidHandle<T>(param1Handle) ||
            !pdpi->isValidHandle<T>(param2Handle) ||
            !pdpi->isValidHandle<s3p_bool_t>(resultHandle)) {
            return SHAREMIND_MODULE_API_0x1_GENERAL_ERROR;
        }

        const s3p_vec<T> & param1 = *static_cast<s3p_vec<T>*>(param1Handle);
        const s3p_vec<T> & param2 = *static_cast<s3p_vec<T>*>(param2Handle);
        s3p_vec<s3p_bool_t> & result = *static_cast<s3p_vec<s3p_bool_t>*>(resultHandle);

        if (param1.size() != param2.size() ||
                param1.size() != result.size())
            return SHAREMIND_MODULE_API_0x1_INVALID_CALL;

        /// \todo figure out something better
        for (size_t i = 0u; i < param1.size(); ++i)
            result[i] = Mode::invoke(param1[i], param2[i]);

        PROFILE_SYSCALL(pdpi->profiler(), pdpi->modelEvaluator(), name,
                        param1.size());

        return SHAREMIND_MODULE_API_0x1_OK;
    } catch (...) {
        return catchModuleApiErrors ();
    }
}

/**
 * SysCall: eq_vec<T>
 * Args:
 *      0) uint64[0u]     pd index
 *      1) p[0u]          input handle (lhs)
 *      2) p[0u]          input handle (rhs)
 *      3) p[0u]          output handle
 * Precondition:
 *      Both input handles are valid vectors of type T.
 *      Output handle is valid vector of type bool.
 *      All vectors are of equal length.
 * Postcondition:
 *      At every position the output vector contains shares of booleans denoting equality of inputs at those positions.
 */
template <typename T>
NAMED_SYSCALL(eq_vec, name, args, num_args, refs, crefs, returnValue, c)
{
    VMHandles handles;
    if (!SyscallArgs<4>::check(num_args, refs, crefs, returnValue) ||
            !handles.get(c, args))
    {
        return SHAREMIND_MODULE_API_0x1_INVALID_CALL;
    }

    try {
        Shared3pPDPI * const pdpi = static_cast<Shared3pPDPI*>(handles.pdpiHandle);

        void * const lhsHandle = args[1u].p[0u];
        void * const rhsHandle = args[2u].p[0u];
        void * const resultHandle = args[3u].p[0u];

        if (!pdpi->isValidHandle<T>(lhsHandle) ||
                !pdpi->isValidHandle<T>(rhsHandle) ||
                !pdpi->isValidHandle<s3p_bool_t>(resultHandle))
        {
            return SHAREMIND_MODULE_API_0x1_GENERAL_ERROR;
        }

        const s3p_vec<T> & param1 = *static_cast<s3p_vec<T>*>(lhsHandle);
        const s3p_vec<T> & param2 = *static_cast<s3p_vec<T>*>(rhsHandle);
        s3p_vec<s3p_bool_t> & result = *static_cast<s3p_vec<s3p_bool_t>*>(resultHandle);

        if (param1.size() != param2.size() ||
                param1.size() != result.size())
            return SHAREMIND_MODULE_API_0x1_INVALID_CALL;

        for (size_t i = 0u; i < param1.size(); ++i)
            result[i] = param1[i] == param2[i];

        PROFILE_SYSCALL(pdpi->profiler(), pdpi->modelEvaluator(), name,
                        param1.size());

        return SHAREMIND_MODULE_API_0x1_OK;
    } catch (...) {
        return catchModuleApiErrors ();
    }
}

/**
 * SysCall: eq_bool_vec<T>
 * Args:
 *      0) uint64[0u]     pd index
 *      1) p[0u]          input handle (lhs)
 *      2) p[0u]          input handle (rhs)
 *      3) p[0u]          output handle
 * Precondition:
 *      Both input handles are valid vectors of type T.
 *      Output handle is valid vector of type bool.
 *      All vectors are of equal length.
 * Postcondition:
 *      At every position the output vector contains shares of booleans denoting equality of inputs at those positions.
 */
template <typename T>
NAMED_SYSCALL(eq_bool_vec, name, args, num_args, refs, crefs, returnValue, c)
{
    return eq_vec<s3p_bool_t>(name, args, num_args, refs, crefs, returnValue, c);
}

/**
 * SysCall: binary_private_public_vec<T, public_type, Protocol>
 * Args:
 *      0) uint64[0u]     pd index
 *      1) p[0u]          vector handle (lhs)
 *      2) p[0u]          output handle
 *      crefs[0u]         rhs vector
 * Precondition:
 *      All handles are valid vectors of type T.
 *      All vectors are of equal length.
 */
template <typename T, typename public_type, typename Protocol>
SHAREMIND_MODULE_API_0x1_SYSCALL(binary_private_public_vec,
                                 args, num_args, refs, crefs,
                                 returnValue, c)
{
    VMHandles handles;
    if (!SyscallArgs<3, false, 0, 1>::check(num_args, refs, crefs, returnValue) ||
        !handles.get(c, args)) {
        return SHAREMIND_MODULE_API_0x1_INVALID_CALL;
    }

    try {
        Shared3pPDPI * const pdpi = static_cast<Shared3pPDPI*>(handles.pdpiHandle);

        void * const paramHandle = args[1u].p[0u];
        void * const resultHandle = args[2u].p[0u];
        if (!pdpi->isValidHandle<T>(paramHandle) ||
            !pdpi->isValidHandle<T>(resultHandle)) {
            return SHAREMIND_MODULE_API_0x1_GENERAL_ERROR;
        }

        const s3p_vec<T> & param1 = *static_cast<s3p_vec<T>*>(paramHandle);
        const public_type * param2 = static_cast<const public_type*>(crefs[0u].pData);
        s3p_vec<T> & result = *static_cast<s3p_vec<T>*>(resultHandle);

        if (crefs[0u].size / sizeof(public_type) < param1.size()) {
            return SHAREMIND_MODULE_API_0x1_GENERAL_ERROR;
        }

        Protocol protocol(*pdpi);
        if (!protocol.invoke(param1, param2, param2 + param1.size(), result,
                typename value_traits<T>::value_category())) {
            return SHAREMIND_MODULE_API_0x1_GENERAL_ERROR;
        }

        return SHAREMIND_MODULE_API_0x1_OK;
    } catch (...) {
        return catchModuleApiErrors ();
    }
}

/**
 * SysCall: oblivious_choice_vec<C, T, Protocol>
 * Args:
 *      0) uint64[0u]     pd index
 *      1) p[0u]          vector handle (condition)
 *      2) p[0u]          vector handle (first result)
 *      3) p[0u]          vector handle (second result)
 *      4) p[0u]          output handle
 * Precondition:
 *      Condition handle is a valid vector of type C.
 *      Both result handles are valid vectors of type T.
 *      All vectors are of equal length.
 */
template <typename C, typename T, typename Protocol>
SHAREMIND_MODULE_API_0x1_SYSCALL(oblivious_choice_vec,
                                 args, num_args, refs, crefs,
                                 returnValue, c)
{
    VMHandles handles;
    if (!SyscallArgs<5>::check(num_args, refs, crefs, returnValue) ||
        !handles.get(c, args)) {
        return SHAREMIND_MODULE_API_0x1_INVALID_CALL;
    }

    try {
        Shared3pPDPI * const pdpi = static_cast<Shared3pPDPI*>(handles.pdpiHandle);

        void * const param1Handle = args[1u].p[0u];
        void * const param2Handle = args[2u].p[0u];
        void * const param3Handle = args[3u].p[0u];
        void * const resultHandle = args[4u].p[0u];

        if (!pdpi->isValidHandle<C>(param1Handle) ||
                !pdpi->isValidHandle<T>(param2Handle) ||
                !pdpi->isValidHandle<T>(param3Handle) ||
                !pdpi->isValidHandle<T>(resultHandle))
        {
            return SHAREMIND_MODULE_API_0x1_GENERAL_ERROR;
        }

        const s3p_vec<C> & param1 = *static_cast<s3p_vec<C>*>(param1Handle);
        const s3p_vec<T> & param2 = *static_cast<s3p_vec<T>*>(param2Handle);
        const s3p_vec<T> & param3 = *static_cast<s3p_vec<T>*>(param3Handle);
        s3p_vec<T> & result = *static_cast<s3p_vec<T>*>(resultHandle);

        Protocol obliviousChoiceProtocol(*pdpi);
        if (!obliviousChoiceProtocol.invoke(param1, param2, param3, result,
                typename value_traits<T>::value_category()))
            return SHAREMIND_MODULE_API_0x1_GENERAL_ERROR;

        return SHAREMIND_MODULE_API_0x1_OK;
    } catch (...) {
        return catchModuleApiErrors ();
    }
}

/**
 * SysCall: vector_shuffle<T>
 * Args:
 *      0) uint64[0u]     pd index
 *      1) p[0u]          input/output handle
 * Precondition:
 *      Input/output handle is valid vector of type T.
 */
template <typename T, bool NeedKey, bool InverseShuffle>
SHAREMIND_MODULE_API_0x1_SYSCALL(vector_shuffle,
                                 args, num_args, refs, crefs,
                                 returnValue, c)
{
    /*
    BOOST_STATIC_ASSERT(!InverseShuffle || NeedKey);

    VMHandles handles;
    if (!SyscallArgs<NeedKey ? 3u : 2u>::check(num_args, refs, crefs, returnValue) ||
        !handles.get(c, args))
    {
        return SHAREMIND_MODULE_API_0x1_INVALID_CALL;
    }

    try {
        Shared3pPDPI * const pdpi = static_cast<Shared3pPDPI*>(handles.pdpiHandle);

        void * const vectorHandle = args[1u].p[0u];
        if (!pdpi->isValidHandle<T>(vectorHandle))
            return SHAREMIND_MODULE_API_0x1_INVALID_CALL;

        s3p_vec<T> & vec = * static_cast<s3p_vec<T> *>(vectorHandle);

        SCOPED_SECTION_VM (pdpi->profiler(), sid,
                           "PROTOCOL_VECTOR_SHUFFLE", vec.size());

        MatrixShufflingProtocol msp(*pdpi);
        if (NeedKey) {
            void * const randHandle = args[2u].p[0u];
            if (!pdpi->isValidHandle<s3p_uint8_t>(randHandle))
                return SHAREMIND_MODULE_API_0x1_INVALID_CALL;

            const s3p_vec<s3p_uint8_t> & rand = * static_cast<s3p_vec<s3p_uint8_t> *>(randHandle);
            if (rand.size() != 32u) {
                return SHAREMIND_MODULE_API_0x1_INVALID_CALL;
            }

            if (vec.size() <= 1u)
                return SHAREMIND_MODULE_API_0x1_OK;

            if (InverseShuffle)
                msp.invokeInverse(vec, 1, rand);
            else
                msp.invoke(vec, 1, rand);
        }
        else {

            if (vec.size() <= 1u)
                return SHAREMIND_MODULE_API_0x1_OK;

            msp.invoke(vec,  1);
        }

        return SHAREMIND_MODULE_API_0x1_OK;
    } catch (...) {
        return catchModuleApiErrors ();
    }
    */
    return SHAREMIND_MODULE_API_0x1_GENERAL_ERROR;
}

/**
 * SysCall: matrix_shuffle<T>
 * Args:
 *      0) uint64[0u]     pd index
 *      1) p[0u]          input/output handle
 *      2) uint64[0u]     number of elements in row
 * Preconditions:
 *      Input/output handle is valid vector of type T.
 *      Number of elements in the matrix is a multiple of the number of elements in a row.
 */
template <typename T, bool NeedKey, bool InverseShuffle>
SHAREMIND_MODULE_API_0x1_SYSCALL(matrix_shuffle,
                                 args, num_args, refs, crefs,
                                 returnValue, c)
{
    /*
    BOOST_STATIC_ASSERT(!InverseShuffle || NeedKey);

    VMHandles handles;
    if (!SyscallArgs<NeedKey ? 4 : 3>::check(num_args, refs, crefs, returnValue) ||
        !handles.get(c, args))
    {
        return SHAREMIND_MODULE_API_0x1_INVALID_CALL;
    }

    try {
        Shared3pPDPI * const pdpi = static_cast<Shared3pPDPI*>(handles.pdpiHandle);
        void * const matrixHandle = args[1u].p[0u];
        if (!pdpi->isValidHandle<T>(matrixHandle))
            return SHAREMIND_MODULE_API_0x1_GENERAL_ERROR;

        const s3p_vec<s3p_uint8_t>* rand = 0;
        if (NeedKey) {
            void * const randHandle = args[3u].p[0u];
            if (!pdpi->isValidHandle<s3p_uint8_t>(randHandle))
                return SHAREMIND_MODULE_API_0x1_INVALID_CALL;

            rand = static_cast<s3p_vec<s3p_uint8_t> *>(randHandle);
            if (rand->size() != 32u) {
                return SHAREMIND_MODULE_API_0x1_INVALID_CALL;
            }
        }

        s3p_vec<T> & matrix = * static_cast<s3p_vec<T> *>(matrixHandle);
        if (matrix.empty())
            return SHAREMIND_MODULE_API_0x1_OK;

        const uint64_t elementsPerRow = args[2u].uint64[0u];
        if (elementsPerRow <= 0u)
            return SHAREMIND_MODULE_API_0x1_INVALID_CALL;

        if (matrix.size() == elementsPerRow)
            return SHAREMIND_MODULE_API_0x1_OK;

        if (matrix.size() % elementsPerRow != 0u)
            return SHAREMIND_MODULE_API_0x1_INVALID_CALL;

        SCOPED_SECTION_VM (pdpi->profiler(), sid,
                           "PROTOCOL_MATRIX_SHUFFLE", matrix.size());

        MatrixShufflingProtocol msp(*pdpi);
        if (NeedKey) {
            if (InverseShuffle)
              msp.invokeInverse(matrix, elementsPerRow, *rand);
            else
              msp.invoke(matrix, elementsPerRow, *rand);
        }
        else {
            msp.invoke(matrix, elementsPerRow);
        }

        return SHAREMIND_MODULE_API_0x1_OK;
    } catch (...) {
        return catchModuleApiErrors ();
    }
    */
    return SHAREMIND_MODULE_API_0x1_GENERAL_ERROR;
}

/**
 * SysCall: aes_xor_uint32_vec
 * Args:
 *      0) uint64[0u]     pd index
 *      1) p[0u]          input text vector handle
 *      2) p[0u]          preexpanded key vector handle
 *      3) p[0u]          handle of the output vector for the output text
 *
 * \pre All handles are valid vectors of type s3p_xor_uint32_t.
 * \pre Both input and output vectors contain a multiple of Nb values, where Nb
 *      is an AES constant from FIPS 197 (with the value 4).
 * \pre The preexpanded key vector contains exactly (Nk * (Nr + 1)) / Nb times
 *      more elements than the input/output vector, where Nk, Nb and Nr are the
 *      AES constants from FIPS 197.
 * \post The input vector is not modified.
 * \post The preexpanded key vector is not modified.
 * \post If successful, the output vector contains the encrypted/decrypted result.
 */
template <class Protocol>
SHAREMIND_MODULE_API_0x1_SYSCALL(aes_xor_uint32_vec,
                                 args, num_args, refs, crefs,
                                 returnValue, c)
{
    /*
    VMHandles handles;
    if (!SyscallArgs<4>::check(num_args, refs, crefs, returnValue) ||
        !handles.get(c, args))
    {
        return SHAREMIND_MODULE_API_0x1_INVALID_CALL;
    }

    try {
        Shared3pPDPI * const pdpi = static_cast<Shared3pPDPI*>(handles.pdpiHandle);

        void * const inputVecHandle = args[1u].p[0u];
        void * const keyVecHandle = args[2u].p[0u];
        void * const outputVecHandle = args[3u].p[0u];

        if (!pdpi->isValidHandle<s3p_xor_uint32_t>(inputVecHandle) ||
            !pdpi->isValidHandle<s3p_xor_uint32_t>(keyVecHandle) ||
            !pdpi->isValidHandle<s3p_xor_uint32_t>(outputVecHandle))
        {
            return SHAREMIND_MODULE_API_0x1_GENERAL_ERROR;
        }

        const s3p_vec<s3p_xor_uint32_t> & inputVec = *static_cast<s3p_vec<s3p_xor_uint32_t> *>(inputVecHandle);
        const s3p_vec<s3p_xor_uint32_t> & keyVec = *static_cast<s3p_vec<s3p_xor_uint32_t> *>(keyVecHandle);
        s3p_vec<s3p_xor_uint32_t> & outputVec = *static_cast<s3p_vec<s3p_xor_uint32_t> *>(outputVecHandle);

        // Check whether input and output vectors have proper size:
        if (inputVec.empty()
            || (inputVec.size() % Protocol::Nb) != 0u
            || inputVec.size() != outputVec.size()
            || (keyVec.size() % (Protocol::Nb * (Protocol::Nr + 1u))) != 0u
            || (inputVec.size() / Protocol::Nb) != (keyVec.size() / (Protocol::Nb * (Protocol::Nr + 1u))))
        {
            return SHAREMIND_MODULE_API_0x1_INVALID_CALL;
        }

        SCOPED_SECTION_VM (pdpi->profiler(), sid,
                           "PROTOCOL_AES_ENCRYPTION", inputVec.size());
        Protocol(*pdpi).processWithExpandedKey(inputVec, keyVec, outputVec);
        return SHAREMIND_MODULE_API_0x1_OK;
    } catch (...) {
        return catchModuleApiErrors ();
    }
    */
    return SHAREMIND_MODULE_API_0x1_GENERAL_ERROR;
}

/**
 * SysCall: aes_xor_uint32_vec_expand_key
 * Args:
 *      0) uint64[0u]     pd index
 *      1) p[0u]          input key vector handle
 *      2) p[0u]          output preexpanded key vector handle
 *
 * \pre All handles are valid vectors of type s3p_xor_uint32_t.
 * \pre The input vector contains a multiple of Nb values, where Nb is an AES
 *      constant from FIPS 197 (with the value 4).
 * \pre The output vector contains exactly (Nr + 1) times more elements than the
 *      input vector, where Nr is an AES constant from FIPS 197.
 * \post The input vector is not modified.
 * \post If successful, the output vector contains the expanded key.
 */
template <class Protocol>
SHAREMIND_MODULE_API_0x1_SYSCALL(aes_xor_uint32_vec_expand_key,
                                 args, num_args, refs, crefs,
                                 returnValue, c)
{
    /*
    VMHandles handles;
    if (!SyscallArgs<3>::check(num_args, refs, crefs, returnValue) ||
        !handles.get(c, args))
    {
        return SHAREMIND_MODULE_API_0x1_INVALID_CALL;
    }

    try {
        Shared3pPDPI * const pdpi = static_cast<Shared3pPDPI*>(handles.pdpiHandle);

        void * const inputVecHandle = args[1u].p[0u];
        void * const outputVecHandle = args[2u].p[0u];

        if (!pdpi->isValidHandle<s3p_xor_uint32_t>(inputVecHandle) ||
            !pdpi->isValidHandle<s3p_xor_uint32_t>(outputVecHandle))
        {
            return SHAREMIND_MODULE_API_0x1_GENERAL_ERROR;
        }

        const s3p_vec<s3p_xor_uint32_t> & inputVec = *static_cast<s3p_vec<s3p_xor_uint32_t> *>(inputVecHandle);
        s3p_vec<s3p_xor_uint32_t> & outputVec = *static_cast<s3p_vec<s3p_xor_uint32_t> *>(outputVecHandle);

        // Check whether input and output vectors have proper size:
        if (inputVec.empty()
            || (inputVec.size() % Protocol::Nk) != 0u
            || (outputVec.size() % (Protocol::Nb * (Protocol::Nr + 1u))) != 0u
            || ((inputVec.size() / Protocol::Nk) != (outputVec.size() / (Protocol::Nb * (Protocol::Nr + 1u)))))
        {
            return SHAREMIND_MODULE_API_0x1_INVALID_CALL;
        }

        SCOPED_SECTION_VM (pdpi->profiler(), sid,
                           "PROTOCOL_AES_KEY_EXPANSION", inputVec.size());

        Protocol(*pdpi).expandAesKey(inputVec, outputVec);
        return SHAREMIND_MODULE_API_0x1_OK;
    } catch (...) {
        return catchModuleApiErrors ();
    }
    */
    return SHAREMIND_MODULE_API_0x1_GENERAL_ERROR;
}

/**
 * SysCall: aes_test
 * Args:
 *      None
 * Precondition:
 *      None
 * Postcondition:
 *      Returns whether the call was successful
 */
template <class Protocol>
SHAREMIND_MODULE_API_0x1_SYSCALL(aes_test,
                                 args, num_args, refs, crefs,
                                 returnValue, c)
{
    /*
    VMHandles handles;
    if (!SyscallArgs<3u>::check(num_args, refs, crefs, returnValue) ||
        !handles.get(c, args))
    {
        return SHAREMIND_MODULE_API_0x1_INVALID_CALL;
    }

    try {
        Shared3pPDPI * const pdpi = static_cast<Shared3pPDPI*>(handles.pdpiHandle);

        Protocol aesProtocol(*pdpi);
        BOOST_STATIC_ASSERT(sizeof(uint64_t) <= sizeof(size_t));
        const size_t numberOfIterations = args[1u].uint64[0u];
        const bool result = numberOfIterations > 0u
                            ? aesProtocol.test(numberOfIterations, args[2u].uint64[0u])
                            : true;
        if (returnValue)
            returnValue->uint64[0u] = result;
        return SHAREMIND_MODULE_API_0x1_OK;
    } catch (...) {
        return catchModuleApiErrors ();
    }
    */
    return SHAREMIND_MODULE_API_0x1_GENERAL_ERROR;
}

SHAREMIND_MODULE_API_0x1_SYSCALL(get_domain_name,
                                 args, num_args, refs, crefs,
                                 returnValue, c)
{
    if (!SyscallArgs<1u, true, 0u, 0u>::check(num_args, refs, crefs, returnValue))
        return SHAREMIND_MODULE_API_0x1_INVALID_CALL;

    VMHandles handles;
    if (!handles.get(c, args))
        return SHAREMIND_MODULE_API_0x1_INVALID_CALL;

    try {
        Shared3pPDPI * const pdpi = static_cast<Shared3pPDPI*>(handles.pdpiHandle);
        const std::string & pdName = pdpi->pdName();

        const uint64_t mem_size = pdName.size() + 1u;
        const uint64_t mem_hndl = (* c->publicAlloc)(c, mem_size);
        char * const mem_ptr = static_cast<char *>((* c->publicMemPtrData)(c, mem_hndl));
        strncpy(mem_ptr, pdName.c_str(), mem_size);
        returnValue[0u].uint64[0u] = mem_hndl;

        return SHAREMIND_MODULE_API_0x1_OK;
    } catch (...) {
        return catchModuleApiErrors ();
    }
}

} // anonymous namespace

extern "C" {

SHAREMIND_MODULE_API_MODULE_INFO("shared3p",
                                 0x00010000,   /* Version 0.1.0.0 */
                                 0x1);         /* Support API version 1. */

/* Should I change the name of this function? */
SHAREMIND_MODULE_API_0x1_INITIALIZER(c) __attribute__ ((visibility("default")));
SHAREMIND_MODULE_API_0x1_INITIALIZER(c) {
    assert(c);

    /*
     Initialize the module handle
    */
    try {
        c->moduleHandle = new sharemind::Shared3pModule();
        return SHAREMIND_MODULE_API_0x1_OK;
    } catch (...) {
        return catchModuleApiErrors ();
    }
}

/* Should I change the name of this function? */
SHAREMIND_MODULE_API_0x1_DEINITIALIZER(c) __attribute__ ((visibility("default")));
SHAREMIND_MODULE_API_0x1_DEINITIALIZER(c) {
    assert(c);
    assert(c->moduleHandle);

    try {
        delete static_cast<sharemind::Shared3pModule *>(c->moduleHandle);
        #ifndef NDEBUG
        c->moduleHandle = nullptr; // Not needed, but may help debugging.
        #endif
    } catch (...) {
        /// \todo log message or noexcept module destructor
    }
}

SHAREMIND_MODULE_API_0x1_SYSCALL_DEFINITIONS(

  /**
   *  Booleans
   */

   // Variable management
    { "shared3p::new_bool_vec", &new_vec<s3p_bool_t> }
  , { "shared3p::init_bool_vec", &init_vec<s3p_bool_t> }
  , { "shared3p::set_shares_bool_vec",  &set_shares<s3p_bool_t> }
  , { "shared3p::get_shares_bool_vec",  &get_shares<s3p_bool_t> }
  , { "shared3p::fill_bool_vec", &fill_vec<s3p_bool_t> }
  , { "shared3p::assign_bool_vec", &assign_vec<s3p_bool_t> }
  , { "shared3p::delete_bool_vec", &delete_vec<s3p_bool_t> }
  , { "shared3p::load_bool_vec", &load_vec<s3p_bool_t> }
  , { "shared3p::store_bool_vec", &store_vec<s3p_bool_t>  }
  , { "shared3p::classify_bool_vec", &classify_vec<s3p_bool_t> }
  , { "shared3p::declassify_bool_vec", &declassify_vec<s3p_bool_t> }
  , { "shared3p::get_type_size_bool", &get_type_size<s3p_bool_t> }

/*
   // Bit logic
  , { "shared3p::not_bool_vec", &unary_arith_vec<s3p_bool_t, NotProtocol> }
  , { "shared3p::and_bool_vec", &binary_arith_vec<s3p_bool_t, ConjunctionProtocol> }
  , { "shared3p::or_bool_vec", &binary_arith_vec<s3p_bool_t, DisjunctionProtocol> }
  , { "shared3p::xor_bool_vec", &binary_arith_vec<s3p_bool_t, XorProtocol> }
 */

   // Arithmetic
  , { "shared3p::sum_bool_vec", &unary_vec<s3p_bool_t, s3p_uint64_t, SumProtocol> }

  /*
   // Comparisons
  , { "shared3p::eq_bool_vec", &eq_bool_vec<s3p_bool_t> }

   */
   // Casting
  , { "shared3p::conv_bool_to_uint8_vec",  &unary_vec<s3p_bool_t, s3p_uint8_t, Conversion> }
  , { "shared3p::conv_bool_to_uint16_vec", &unary_vec<s3p_bool_t, s3p_uint16_t, Conversion> }
  , { "shared3p::conv_bool_to_uint32_vec", &unary_vec<s3p_bool_t, s3p_uint32_t, Conversion> }
  , { "shared3p::conv_bool_to_uint64_vec", &unary_vec<s3p_bool_t, s3p_uint64_t, Conversion> }
  , { "shared3p::conv_bool_to_int8_vec",  &unary_vec<s3p_bool_t, s3p_int8_t, Conversion> }
  , { "shared3p::conv_bool_to_int16_vec", &unary_vec<s3p_bool_t, s3p_int16_t, Conversion> }
  , { "shared3p::conv_bool_to_int32_vec", &unary_vec<s3p_bool_t, s3p_int32_t, Conversion> }
  , { "shared3p::conv_bool_to_int64_vec", &unary_vec<s3p_bool_t, s3p_int64_t, Conversion> }
  /*
  , { "shared3p::conv_bool_to_float32_vec", &unary_vec<s3p_bool_t, s3p_float32_t, ConversionToFloatProtocol> }
  , { "shared3p::conv_bool_to_float64_vec", &unary_vec<s3p_bool_t, s3p_float64_t, ConversionToFloatProtocol> }

   // Utilities
  , { "shared3p::randomize_bool_vec",  &nullary_vec<s3p_bool_t, RandomizeProtocol> }
*/

   // Database functions
   /*, { "shared3p::vecshuf_bool_vec", &vector_shuffle<s3p_bool_t, false, false> }
   , { "shared3p::vecshufkey_bool_vec", &vector_shuffle<s3p_bool_t, true, false> }
   , { "shared3p::matshuf_bool_vec", &matrix_shuffle<s3p_bool_t, false, false> }
   , { "shared3p::matshufkey_boo_vec", &matrix_shuffle<s3p_bool_t, true, false> }*/

  /**
   *  Additively shared unsigned integers
   */

   // Variable management
  , { "shared3p::new_uint8_vec",  &new_vec<s3p_uint8_t> }
  , { "shared3p::new_uint16_vec", &new_vec<s3p_uint16_t> }
  , { "shared3p::new_uint32_vec", &new_vec<s3p_uint32_t> }
  , { "shared3p::new_uint64_vec", &new_vec<s3p_uint64_t> }
  , { "shared3p::init_uint8_vec",  &init_vec<s3p_uint8_t> }
  , { "shared3p::init_uint16_vec", &init_vec<s3p_uint16_t> }
  , { "shared3p::init_uint32_vec", &init_vec<s3p_uint32_t> }
  , { "shared3p::init_uint64_vec", &init_vec<s3p_uint64_t> }
  , { "shared3p::set_shares_uint8_vec",  &set_shares<s3p_uint8_t> }
  , { "shared3p::set_shares_uint16_vec", &set_shares<s3p_uint16_t> }
  , { "shared3p::set_shares_uint32_vec", &set_shares<s3p_uint32_t> }
  , { "shared3p::set_shares_uint64_vec", &set_shares<s3p_uint64_t> }
  , { "shared3p::get_shares_uint8_vec",  &get_shares<s3p_uint8_t> }
  , { "shared3p::get_shares_uint16_vec", &get_shares<s3p_uint16_t> }
  , { "shared3p::get_shares_uint32_vec", &get_shares<s3p_uint32_t> }
  , { "shared3p::get_shares_uint64_vec", &get_shares<s3p_uint64_t> }
  , { "shared3p::fill_uint8_vec",  &fill_vec<s3p_uint8_t> }
  , { "shared3p::fill_uint16_vec", &fill_vec<s3p_uint16_t> }
  , { "shared3p::fill_uint32_vec", &fill_vec<s3p_uint32_t> }
  , { "shared3p::fill_uint64_vec", &fill_vec<s3p_uint64_t> }
  , { "shared3p::assign_uint8_vec",  &assign_vec<s3p_uint8_t> }
  , { "shared3p::assign_uint16_vec", &assign_vec<s3p_uint16_t> }
  , { "shared3p::assign_uint32_vec", &assign_vec<s3p_uint32_t> }
  , { "shared3p::assign_uint64_vec", &assign_vec<s3p_uint64_t> }
  , { "shared3p::delete_uint8_vec",  &delete_vec<s3p_uint8_t> }
  , { "shared3p::delete_uint16_vec", &delete_vec<s3p_uint16_t> }
  , { "shared3p::delete_uint32_vec", &delete_vec<s3p_uint32_t> }
  , { "shared3p::delete_uint64_vec", &delete_vec<s3p_uint64_t> }
  , { "shared3p::load_uint8_vec",  &load_vec<s3p_uint8_t> }
  , { "shared3p::load_uint16_vec", &load_vec<s3p_uint16_t> }
  , { "shared3p::load_uint32_vec", &load_vec<s3p_uint32_t> }
  , { "shared3p::load_uint64_vec", &load_vec<s3p_uint64_t> }
  , { "shared3p::store_uint8_vec",  &store_vec<s3p_uint8_t> }
  , { "shared3p::store_uint16_vec", &store_vec<s3p_uint16_t> }
  , { "shared3p::store_uint32_vec", &store_vec<s3p_uint32_t> }
  , { "shared3p::store_uint64_vec", &store_vec<s3p_uint64_t> }
  , { "shared3p::classify_uint8_vec",  &classify_vec<s3p_uint8_t> }
  , { "shared3p::classify_uint16_vec", &classify_vec<s3p_uint16_t> }
  , { "shared3p::classify_uint32_vec", &classify_vec<s3p_uint32_t> }
  , { "shared3p::classify_uint64_vec", &classify_vec<s3p_uint64_t> }
  , { "shared3p::declassify_uint8_vec",  &declassify_vec<s3p_uint8_t> }
  , { "shared3p::declassify_uint16_vec", &declassify_vec<s3p_uint16_t> }
  , { "shared3p::declassify_uint32_vec", &declassify_vec<s3p_uint32_t> }
  , { "shared3p::declassify_uint64_vec", &declassify_vec<s3p_uint64_t> }
  , { "shared3p::get_type_size_uint8", &get_type_size<s3p_uint8_t> }
  , { "shared3p::get_type_size_uint16", &get_type_size<s3p_uint16_t> }
  , { "shared3p::get_type_size_uint32", &get_type_size<s3p_uint32_t> }
  , { "shared3p::get_type_size_uint64", &get_type_size<s3p_uint64_t> }

/*
    // Unsigned integer arithmetic
  , { "shared3p::add_uint8_vec",  &binary_arith_vec<s3p_uint8_t , AdditionProtocol> }
  , { "shared3p::add_uint16_vec", &binary_arith_vec<s3p_uint16_t, AdditionProtocol> }
  , { "shared3p::add_uint32_vec", &binary_arith_vec<s3p_uint32_t, AdditionProtocol> }
  , { "shared3p::add_uint64_vec", &binary_arith_vec<s3p_uint64_t, AdditionProtocol> }
*/
  , { "shared3p::sum_uint8_vec",  &unary_arith_vec<s3p_uint8_t, SumProtocol> }
  , { "shared3p::sum_uint16_vec", &unary_arith_vec<s3p_uint16_t, SumProtocol> }
  , { "shared3p::sum_uint32_vec", &unary_arith_vec<s3p_uint32_t, SumProtocol> }
  , { "shared3p::sum_uint64_vec", &unary_arith_vec<s3p_uint64_t, SumProtocol> }
/*
  , { "shared3p::product_uint8_vec",  &unary_arith_vec<s3p_uint8_t, ProductProtocol> }
  , { "shared3p::product_uint16_vec", &unary_arith_vec<s3p_uint16_t, ProductProtocol> }
  , { "shared3p::product_uint32_vec", &unary_arith_vec<s3p_uint32_t, ProductProtocol> }
  , { "shared3p::product_uint64_vec", &unary_arith_vec<s3p_uint64_t, ProductProtocol> }
  , { "shared3p::neg_uint8_vec",  &unary_arith_vec<s3p_uint8_t, NegProtocol> }
  , { "shared3p::neg_uint16_vec", &unary_arith_vec<s3p_uint16_t, NegProtocol> }
  , { "shared3p::neg_uint32_vec", &unary_arith_vec<s3p_uint32_t, NegProtocol> }
  , { "shared3p::neg_uint64_vec", &unary_arith_vec<s3p_uint64_t, NegProtocol> }
  , { "shared3p::sub_uint8_vec",  &binary_arith_vec<s3p_uint8_t , SubtractionProtocol> }
  , { "shared3p::sub_uint16_vec", &binary_arith_vec<s3p_uint16_t, SubtractionProtocol> }
  , { "shared3p::sub_uint32_vec", &binary_arith_vec<s3p_uint32_t, SubtractionProtocol> }
  , { "shared3p::sub_uint64_vec", &binary_arith_vec<s3p_uint64_t, SubtractionProtocol> }
  , { "shared3p::mul_uint8_vec",  &binary_arith_vec<s3p_uint8_t , MultiplicationProtocol> }
  , { "shared3p::mul_uint16_vec", &binary_arith_vec<s3p_uint16_t, MultiplicationProtocol> }
  , { "shared3p::mul_uint32_vec", &binary_arith_vec<s3p_uint32_t, MultiplicationProtocol> }
  , { "shared3p::mul_uint64_vec", &binary_arith_vec<s3p_uint64_t, MultiplicationProtocol> }
//  , { "shared3p::div_uint8_vec",  &binary_arith_vec<s3p_uint8_t, DivisionProtocol <ModeDiv> > }
//  , { "shared3p::div_uint16_vec", &binary_arith_vec<s3p_uint16_t, DivisionProtocol <ModeDiv> > }
  , { "shared3p::div_uint32_vec", &binary_arith_vec<s3p_uint32_t, DivisionProtocol <ModeDiv> > }
//  , { "shared3p::div_uint64_vec", &binary_arith_vec<s3p_uint64_t, DivisionProtocol <ModeDiv> > }
//  , { "shared3p::mod_uint8_vec",  &binary_arith_vec<s3p_uint8_t, DivisionProtocol <ModeMod> > }
//  , { "shared3p::mod_uint16_vec", &binary_arith_vec<s3p_uint16_t, DivisionProtocol <ModeMod> > }
  , { "shared3p::mod_uint32_vec", &binary_arith_vec<s3p_uint32_t, DivisionProtocol <ModeMod> > }
//  , { "shared3p::mod_uint64_vec", &binary_arith_vec<s3p_uint64_t, DivisionProtocol <ModeMod> > }
  , { "shared3p::mulc_uint8_vec",  &mulc_vec<s3p_uint8_t, PublicMultiplicationProtocol> }
  , { "shared3p::mulc_uint16_vec", &mulc_vec<s3p_uint16_t, PublicMultiplicationProtocol> }
  , { "shared3p::mulc_uint32_vec", &mulc_vec<s3p_uint32_t, PublicMultiplicationProtocol> }
  , { "shared3p::mulc_uint64_vec", &mulc_vec<s3p_uint64_t, PublicMultiplicationProtocol> }
  , { "shared3p::divc_uint8_vec",  &divc_vec<s3p_uint8_t, PublicDivisionProtocol <ModeDiv> > }
  , { "shared3p::divc_uint16_vec", &divc_vec<s3p_uint16_t, PublicDivisionProtocol <ModeDiv> > }
  , { "shared3p::divc_uint32_vec", &divc_vec<s3p_uint32_t, PublicDivisionProtocol <ModeDiv> > }
  , { "shared3p::divc_uint64_vec", &divc_vec<s3p_uint64_t, PublicDivisionProtocol <ModeDiv> > }
  , { "shared3p::modc_uint8_vec",  &modc_vec<s3p_uint8_t> }
  , { "shared3p::modc_uint16_vec", &modc_vec<s3p_uint16_t> }
  , { "shared3p::modc_uint32_vec", &modc_vec<s3p_uint32_t> }
  , { "shared3p::modc_uint64_vec", &modc_vec<s3p_uint64_t> }
*/

    // Bit-level operations
  /*
  , { "shared3p::shiftlc_uint8_vec", &shiftlc_vec<s3p_uint8_t> }
  , { "shared3p::shiftlc_uint16_vec", &shiftlc_vec<s3p_uint16_t> }
  , { "shared3p::shiftlc_uint32_vec", &shiftlc_vec<s3p_uint32_t> }
  , { "shared3p::shiftlc_uint64_vec", &shiftlc_vec<s3p_uint64_t> }
  , { "shared3p::shiftrc_uint8_vec", &shiftrc_vec<s3p_uint8_t> }
  , { "shared3p::shiftrc_uint16_vec", &shiftrc_vec<s3p_uint16_t> }
  , { "shared3p::shiftrc_uint32_vec", &shiftrc_vec<s3p_uint32_t> }
  , { "shared3p::shiftrc_uint64_vec", &shiftrc_vec<s3p_uint64_t> }
  , { "shared3p::bitx_uint8_vec", &shared3p_bitx_uint8_vec }
  , { "shared3p::bitx_uint16_vec", &shared3p_bitx_uint16_vec }
  , { "shared3p::bitx_uint32_vec", &shared3p_bitx_uint32_vec }
  , { "shared3p::bitx_uint64_vec", &shared3p_bitx_uint64_vec }
  */

    // Comparisons
  , { "shared3p::eq_uint8_vec",   NAMED_SYSCALL_WRAPPER("eq_uint8_vec", eq_vec<s3p_uint8_t>) }
  , { "shared3p::eq_uint16_vec",  NAMED_SYSCALL_WRAPPER("eq_uint16_vec", eq_vec<s3p_uint16_t>) }
  , { "shared3p::eq_uint32_vec",  NAMED_SYSCALL_WRAPPER("eq_uint32_vec", eq_vec<s3p_uint32_t>) }
  , { "shared3p::eq_uint64_vec",  NAMED_SYSCALL_WRAPPER("eq_uint64_vec", eq_vec<s3p_uint64_t>) }
  , { "shared3p::gt_uint8_vec",   NAMED_SYSCALL_WRAPPER("gt_uint8_vec", compare_vec<s3p_uint8_t , ModeLT,  true>) }
  , { "shared3p::gt_uint16_vec",  NAMED_SYSCALL_WRAPPER("gt_uint16_vec", compare_vec<s3p_uint16_t, ModeLT,  true>) }
  , { "shared3p::gt_uint32_vec",  NAMED_SYSCALL_WRAPPER("gt_uint32_vec", compare_vec<s3p_uint32_t, ModeLT,  true>) }
  , { "shared3p::gt_uint64_vec",  NAMED_SYSCALL_WRAPPER("gt_uint64_vec", compare_vec<s3p_uint64_t, ModeLT,  true>) }
  , { "shared3p::gte_uint8_vec",  NAMED_SYSCALL_WRAPPER("gte_uint8_vec", compare_vec<s3p_uint8_t , ModeLE,  true>) }
  , { "shared3p::gte_uint16_vec", NAMED_SYSCALL_WRAPPER("gte_uint16_vec", compare_vec<s3p_uint16_t, ModeLE,  true>) }
  , { "shared3p::gte_uint32_vec", NAMED_SYSCALL_WRAPPER("gte_uint32_vec", compare_vec<s3p_uint32_t, ModeLE,  true>) }
  , { "shared3p::gte_uint64_vec", NAMED_SYSCALL_WRAPPER("gte_uint64_vec", compare_vec<s3p_uint64_t, ModeLE,  true>) }
  , { "shared3p::lt_uint8_vec",   NAMED_SYSCALL_WRAPPER("lt_uint8_vec", compare_vec<s3p_uint8_t , ModeLT,  false>) }
  , { "shared3p::lt_uint16_vec",  NAMED_SYSCALL_WRAPPER("lt_uint16_vec", compare_vec<s3p_uint16_t, ModeLT,  false>) }
  , { "shared3p::lt_uint32_vec",  NAMED_SYSCALL_WRAPPER("lt_uint32_vec", compare_vec<s3p_uint32_t, ModeLT,  false>) }
  , { "shared3p::lt_uint64_vec",  NAMED_SYSCALL_WRAPPER("lt_uint64_vec", compare_vec<s3p_uint64_t, ModeLT,  false>) }
  , { "shared3p::lte_uint8_vec",  NAMED_SYSCALL_WRAPPER("lte_uint8_vec", compare_vec<s3p_uint8_t , ModeLE,  false>) }
  , { "shared3p::lte_uint16_vec", NAMED_SYSCALL_WRAPPER("lte_uint16_vec", compare_vec<s3p_uint16_t, ModeLE,  false>) }
  , { "shared3p::lte_uint32_vec", NAMED_SYSCALL_WRAPPER("lte_uint32_vec", compare_vec<s3p_uint32_t, ModeLE,  false>) }
  , { "shared3p::lte_uint64_vec", NAMED_SYSCALL_WRAPPER("lte_uint64_vec", compare_vec<s3p_uint64_t, ModeLE,  false>) }

 // Casting
 // , { "shared3p::conv_float32_to_bool_vec", &unary_vec<s3p_float32_t, s3p_bool_t, Conversion> }
 // , { "shared3p::conv_float32_to_float64_vec", &unary_vec<s3p_float32_t, s3p_float64_t, Conversion> }
 // , { "shared3p::conv_float64_to_bool_vec", &unary_vec<s3p_float64_t, s3p_bool_t, Conversion> }
  , { "shared3p::conv_uint8_to_bool_vec",  &unary_vec<s3p_uint8_t, s3p_bool_t, Conversion> }
  , { "shared3p::conv_uint16_to_bool_vec", &unary_vec<s3p_uint16_t, s3p_bool_t, Conversion> }
  , { "shared3p::conv_uint32_to_bool_vec", &unary_vec<s3p_uint32_t, s3p_bool_t, Conversion> }
  , { "shared3p::conv_uint64_to_bool_vec", &unary_vec<s3p_uint64_t, s3p_bool_t, Conversion> }
  , { "shared3p::conv_uint8_to_int8_vec",  &unary_vec<s3p_uint8_t, s3p_int8_t, Conversion> }
  , { "shared3p::conv_uint8_to_int16_vec",  &unary_vec<s3p_uint8_t, s3p_int16_t, Conversion> }
  , { "shared3p::conv_uint8_to_int32_vec",  &unary_vec<s3p_uint8_t, s3p_int32_t, Conversion> }
  , { "shared3p::conv_uint8_to_int64_vec",  &unary_vec<s3p_uint8_t, s3p_int64_t, Conversion> }
  , { "shared3p::conv_uint16_to_int8_vec",  &unary_vec<s3p_uint16_t, s3p_int8_t, Conversion> }
  , { "shared3p::conv_uint16_to_int16_vec",  &unary_vec<s3p_uint16_t, s3p_int16_t, Conversion> }
  , { "shared3p::conv_uint16_to_int32_vec",  &unary_vec<s3p_uint16_t, s3p_int32_t, Conversion> }
  , { "shared3p::conv_uint16_to_int64_vec",  &unary_vec<s3p_uint16_t, s3p_int64_t, Conversion> }
  , { "shared3p::conv_uint32_to_int8_vec", &unary_vec<s3p_uint32_t, s3p_int8_t, Conversion> }
  , { "shared3p::conv_uint32_to_int16_vec", &unary_vec<s3p_uint32_t, s3p_int16_t, Conversion> }
  , { "shared3p::conv_uint32_to_int32_vec", &unary_vec<s3p_uint32_t, s3p_int32_t, Conversion> }
  , { "shared3p::conv_uint32_to_int64_vec", &unary_vec<s3p_uint32_t, s3p_int64_t, Conversion> }
  , { "shared3p::conv_uint64_to_int8_vec",  &unary_vec<s3p_uint64_t, s3p_int8_t, Conversion> }
  , { "shared3p::conv_uint64_to_int16_vec",  &unary_vec<s3p_uint64_t, s3p_int16_t, Conversion> }
  , { "shared3p::conv_uint64_to_int32_vec",  &unary_vec<s3p_uint64_t, s3p_int32_t, Conversion> }
  , { "shared3p::conv_uint64_to_int64_vec",  &unary_vec<s3p_uint64_t, s3p_int64_t, Conversion> }

  , { "shared3p::conv_uint16_to_uint8_vec",  &unary_vec<s3p_uint16_t, s3p_uint8_t, Conversion> }
  , { "shared3p::conv_uint32_to_uint8_vec",  &unary_vec<s3p_uint32_t, s3p_uint8_t, Conversion> }
  , { "shared3p::conv_uint32_to_uint16_vec", &unary_vec<s3p_uint32_t, s3p_uint16_t, Conversion> }
  , { "shared3p::conv_uint64_to_uint8_vec",  &unary_vec<s3p_uint64_t, s3p_uint8_t, Conversion> }
  , { "shared3p::conv_uint64_to_uint16_vec", &unary_vec<s3p_uint64_t, s3p_uint16_t, Conversion> }
  , { "shared3p::conv_uint64_to_uint32_vec", &unary_vec<s3p_uint64_t, s3p_uint32_t, Conversion> }
  , { "shared3p::conv_uint8_to_uint16_vec",  &unary_vec<s3p_uint8_t, s3p_uint16_t, Conversion> }
  , { "shared3p::conv_uint8_to_uint32_vec",  &unary_vec<s3p_uint8_t, s3p_uint32_t, Conversion> }
  , { "shared3p::conv_uint8_to_uint64_vec",  &unary_vec<s3p_uint8_t, s3p_uint64_t, Conversion> }
  , { "shared3p::conv_uint16_to_uint32_vec", &unary_vec<s3p_uint16_t, s3p_uint32_t, Conversion> }
  , { "shared3p::conv_uint16_to_uint64_vec", &unary_vec<s3p_uint16_t, s3p_uint64_t, Conversion> }
  , { "shared3p::conv_uint32_to_uint64_vec", &unary_vec<s3p_uint32_t, s3p_uint64_t, Conversion> }
  /*
  , { "shared3p::conv_uint8_to_float32_vec", &unary_vec<s3p_uint8_t, s3p_float32_t,ConversionToFloatProtocol> }
  , { "shared3p::conv_uint8_to_float64_vec", &unary_vec<s3p_uint8_t, s3p_float64_t,ConversionToFloatProtocol> }
  , { "shared3p::conv_uint16_to_float32_vec", &unary_vec<s3p_uint16_t, s3p_float32_t,ConversionToFloatProtocol> }
  , { "shared3p::conv_uint16_to_float64_vec", &unary_vec<s3p_uint16_t, s3p_float64_t,ConversionToFloatProtocol> }
  , { "shared3p::conv_uint32_to_float32_vec", &unary_vec<s3p_uint32_t, s3p_float32_t,ConversionToFloatProtocol> }
  , { "shared3p::conv_uint32_to_float64_vec", &unary_vec<s3p_uint32_t, s3p_float64_t, ConversionToFloatProtocol> }
  // This syscall is disabled until we can add precision to it.
  // See ConversionToFloatProtocol.h
//, { "shared3p::conv_uint64_to_float32_vec", &unary_vec<s3p_uint64_t, s3p_float32_t, ConversionToFloatProtocol> }
  , { "shared3p::conv_uint64_to_float64_vec", &unary_vec<s3p_uint64_t, s3p_float64_t, ConversionToFloatProtocol> }

  , { "shared3p::bit_extract_xor_uint8_vec", &unary_vec<s3p_xor_uint8_t, s3p_bool_t, BitExtractionProtocol> }
  , { "shared3p::bit_extract_xor_uint16_vec", &unary_vec<s3p_xor_uint16_t, s3p_bool_t, BitExtractionProtocol> }
  , { "shared3p::bit_extract_xor_uint32_vec", &unary_vec<s3p_xor_uint32_t, s3p_bool_t, BitExtractionProtocol> }
  , { "shared3p::bit_extract_xor_uint64_vec", &unary_vec<s3p_xor_uint64_t, s3p_bool_t, BitExtractionProtocol> }

    // Utilities
  , { "shared3p::randomize_uint8_vec",  &nullary_vec<s3p_uint8_t, RandomizeProtocol> }
  , { "shared3p::randomize_uint16_vec", &nullary_vec<s3p_uint16_t, RandomizeProtocol> }
  , { "shared3p::randomize_uint32_vec", &nullary_vec<s3p_uint32_t, RandomizeProtocol> }
  , { "shared3p::randomize_uint64_vec", &nullary_vec<s3p_uint64_t, RandomizeProtocol> }

    // Special functions
  , { "shared3p::min_uint8_vec",  &binary_arith_vec<s3p_uint8_t, MinimumMaximumProtocol<ModeMin> > }
  , { "shared3p::min_uint16_vec", &binary_arith_vec<s3p_uint16_t, MinimumMaximumProtocol<ModeMin> > }
  , { "shared3p::min_uint32_vec", &binary_arith_vec<s3p_uint32_t, MinimumMaximumProtocol<ModeMin> > }
  , { "shared3p::min_uint64_vec", &binary_arith_vec<s3p_uint64_t, MinimumMaximumProtocol<ModeMin> > }
  , { "shared3p::min_float32_vec",  &binary_arith_vec<s3p_float32_t, MinimumMaximumProtocol<ModeMin> > }
  , { "shared3p::min_float64_vec",  &binary_arith_vec<s3p_float64_t, MinimumMaximumProtocol<ModeMin> > }
  , { "shared3p::max_uint8_vec",  &binary_arith_vec<s3p_uint8_t, MinimumMaximumProtocol<ModeMax> > }
  , { "shared3p::max_uint16_vec", &binary_arith_vec<s3p_uint16_t, MinimumMaximumProtocol<ModeMax> > }
  , { "shared3p::max_uint32_vec", &binary_arith_vec<s3p_uint32_t, MinimumMaximumProtocol<ModeMax> > }
  , { "shared3p::max_uint64_vec", &binary_arith_vec<s3p_uint64_t, MinimumMaximumProtocol<ModeMax> > }
  , { "shared3p::max_float32_vec",  &binary_arith_vec<s3p_float32_t, MinimumMaximumProtocol<ModeMin> > }
  , { "shared3p::max_float64_vec",  &binary_arith_vec<s3p_float64_t, MinimumMaximumProtocol<ModeMin> > }


  , { "shared3p::vecmin_uint8_vec",  &unary_arith_vec<s3p_uint8_t, MinimumMaximumProtocol<ModeMin> > }
  , { "shared3p::vecmin_uint16_vec", &unary_arith_vec<s3p_uint16_t, MinimumMaximumProtocol<ModeMin> > }
  , { "shared3p::vecmin_uint32_vec", &unary_arith_vec<s3p_uint32_t, MinimumMaximumProtocol<ModeMin> > }
  , { "shared3p::vecmin_uint64_vec", &unary_arith_vec<s3p_uint64_t, MinimumMaximumProtocol<ModeMin> > }
  , { "shared3p::vecmin_float32_vec", &unary_arith_vec<s3p_float32_t, MinimumMaximumProtocol<ModeMin> > }
  , { "shared3p::vecmin_float64_vec", &unary_arith_vec<s3p_float64_t, MinimumMaximumProtocol<ModeMin> > }
  , { "shared3p::vecmax_uint8_vec",  &unary_arith_vec<s3p_uint8_t, MinimumMaximumProtocol<ModeMax> > }
  , { "shared3p::vecmax_uint16_vec", &unary_arith_vec<s3p_uint16_t, MinimumMaximumProtocol<ModeMax> > }
  , { "shared3p::vecmax_uint32_vec", &unary_arith_vec<s3p_uint32_t, MinimumMaximumProtocol<ModeMax> > }
  , { "shared3p::vecmax_uint64_vec", &unary_arith_vec<s3p_uint64_t, MinimumMaximumProtocol<ModeMax> > }
  , { "shared3p::vecmax_float32_vec", &unary_arith_vec<s3p_float32_t, MinimumMaximumProtocol<ModeMin> > }
  , { "shared3p::vecmax_float64_vec", &unary_arith_vec<s3p_float64_t, MinimumMaximumProtocol<ModeMin> > }

  // Database functions
  , { "shared3p::vecshuf_uint8_vec", &vector_shuffle<s3p_uint8_t, false, false> }
  , { "shared3p::vecshuf_uint16_vec", &vector_shuffle<s3p_uint16_t, false, false> }
  , { "shared3p::vecshuf_uint32_vec", &vector_shuffle<s3p_uint32_t, false, false> }
  , { "shared3p::vecshuf_uint64_vec", &vector_shuffle<s3p_uint64_t, false, false> }
  , { "shared3p::vecshufkey_uint8_vec", &vector_shuffle<s3p_uint8_t, true, false> }
  , { "shared3p::vecshufkey_uint16_vec", &vector_shuffle<s3p_uint16_t, true, false> }
  , { "shared3p::vecshufkey_uint32_vec", &vector_shuffle<s3p_uint32_t, true, false> }
  , { "shared3p::vecshufkey_uint64_vec", &vector_shuffle<s3p_uint64_t, true, false> }
  , { "shared3p::vecshufinv_uint8_vec", &vector_shuffle<s3p_uint8_t, true, true> }
  , { "shared3p::vecshufinv_uint16_vec", &vector_shuffle<s3p_uint16_t, true, true> }
  , { "shared3p::vecshufinv_uint32_vec", &vector_shuffle<s3p_uint32_t, true, true> }
  , { "shared3p::vecshufinv_uint64_vec", &vector_shuffle<s3p_uint64_t, true, true> }
  , { "shared3p::matshuf_uint8_vec", &matrix_shuffle<s3p_uint8_t, false, false> }
  , { "shared3p::matshuf_uint16_vec", &matrix_shuffle<s3p_uint16_t, false, false> }
  , { "shared3p::matshuf_uint32_vec", &matrix_shuffle<s3p_uint32_t, false, false> }
  , { "shared3p::matshuf_uint64_vec", &matrix_shuffle<s3p_uint64_t, false, false> }
  , { "shared3p::matshufkey_uint8_vec", &matrix_shuffle<s3p_uint8_t, true, false> }
  , { "shared3p::matshufkey_uint16_vec", &matrix_shuffle<s3p_uint16_t, true, false> }
  , { "shared3p::matshufkey_uint32_vec", &matrix_shuffle<s3p_uint32_t, true, false> }
  , { "shared3p::matshufkey_uint64_vec", &matrix_shuffle<s3p_uint64_t, true, false> }
  , { "shared3p::matshufinv_uint8_vec", &matrix_shuffle<s3p_uint8_t, true, true> }
  , { "shared3p::matshufinv_uint16_vec", &matrix_shuffle<s3p_uint16_t, true, true> }
  , { "shared3p::matshufinv_uint32_vec", &matrix_shuffle<s3p_uint32_t, true, true> }
  , { "shared3p::matshufinv_uint64_vec", &matrix_shuffle<s3p_uint64_t, true, true> }
*/

  /**
   *  Additively shared signed integers
   */

    // Variable management
  , { "shared3p::new_int8_vec",  &new_vec<s3p_int8_t> }
  , { "shared3p::new_int16_vec", &new_vec<s3p_int16_t> }
  , { "shared3p::new_int32_vec", &new_vec<s3p_int32_t> }
  , { "shared3p::new_int64_vec", &new_vec<s3p_int64_t> }
  , { "shared3p::init_int8_vec",  &init_vec<s3p_int8_t> }
  , { "shared3p::init_int16_vec", &init_vec<s3p_int16_t> }
  , { "shared3p::init_int32_vec", &init_vec<s3p_int32_t> }
  , { "shared3p::init_int64_vec", &init_vec<s3p_int64_t> }
  , { "shared3p::set_shares_int8_vec",  &set_shares<s3p_int8_t> }
  , { "shared3p::set_shares_int16_vec", &set_shares<s3p_int16_t> }
  , { "shared3p::set_shares_int32_vec", &set_shares<s3p_int32_t> }
  , { "shared3p::set_shares_int64_vec", &set_shares<s3p_int64_t> }
  , { "shared3p::get_shares_int8_vec",  &get_shares<s3p_int8_t> }
  , { "shared3p::get_shares_int16_vec", &get_shares<s3p_int16_t> }
  , { "shared3p::get_shares_int32_vec", &get_shares<s3p_int32_t> }
  , { "shared3p::get_shares_int64_vec", &get_shares<s3p_int64_t> }
  , { "shared3p::fill_int8_vec",  &fill_vec<s3p_int8_t> }
  , { "shared3p::fill_int16_vec", &fill_vec<s3p_int16_t> }
  , { "shared3p::fill_int32_vec", &fill_vec<s3p_int32_t> }
  , { "shared3p::fill_int64_vec", &fill_vec<s3p_int64_t> }
  , { "shared3p::assign_int8_vec",  &assign_vec<s3p_int8_t> }
  , { "shared3p::assign_int16_vec", &assign_vec<s3p_int16_t> }
  , { "shared3p::assign_int32_vec", &assign_vec<s3p_int32_t> }
  , { "shared3p::assign_int64_vec", &assign_vec<s3p_int64_t> }
  , { "shared3p::delete_int8_vec",  &delete_vec<s3p_int8_t> }
  , { "shared3p::delete_int16_vec", &delete_vec<s3p_int16_t> }
  , { "shared3p::delete_int32_vec", &delete_vec<s3p_int32_t> }
  , { "shared3p::delete_int64_vec", &delete_vec<s3p_int64_t> }
  , { "shared3p::load_int8_vec",  &load_vec<s3p_int8_t> }
  , { "shared3p::load_int16_vec", &load_vec<s3p_int16_t> }
  , { "shared3p::load_int32_vec", &load_vec<s3p_int32_t> }
  , { "shared3p::load_int64_vec", &load_vec<s3p_int64_t> }
  , { "shared3p::store_int8_vec",  &store_vec<s3p_int8_t> }
  , { "shared3p::store_int16_vec", &store_vec<s3p_int16_t> }
  , { "shared3p::store_int32_vec", &store_vec<s3p_int32_t> }
  , { "shared3p::store_int64_vec", &store_vec<s3p_int64_t> }
  , { "shared3p::classify_int8_vec",  &classify_vec<s3p_int8_t> }
  , { "shared3p::classify_int16_vec", &classify_vec<s3p_int16_t> }
  , { "shared3p::classify_int32_vec", &classify_vec<s3p_int32_t> }
  , { "shared3p::classify_int64_vec", &classify_vec<s3p_int64_t> }
  , { "shared3p::declassify_int8_vec",  &declassify_vec<s3p_int8_t> }
  , { "shared3p::declassify_int16_vec", &declassify_vec<s3p_int16_t> }
  , { "shared3p::declassify_int32_vec", &declassify_vec<s3p_int32_t> }
  , { "shared3p::declassify_int64_vec", &declassify_vec<s3p_int64_t> }
  , { "shared3p::get_type_size_int8", &get_type_size<s3p_uint8_t> }
  , { "shared3p::get_type_size_int16", &get_type_size<s3p_uint16_t> }
  , { "shared3p::get_type_size_int32", &get_type_size<s3p_uint32_t> }
  , { "shared3p::get_type_size_int64", &get_type_size<s3p_uint64_t> }

/*
  , { "shared3p::add_int8_vec",  &binary_arith_vec<s3p_int8_t , AdditionProtocol> }
  , { "shared3p::add_int16_vec", &binary_arith_vec<s3p_int16_t, AdditionProtocol> }
  , { "shared3p::add_int32_vec", &binary_arith_vec<s3p_int32_t, AdditionProtocol> }
  , { "shared3p::add_int64_vec", &binary_arith_vec<s3p_int64_t, AdditionProtocol> }
*/
  , { "shared3p::sum_int8_vec",  &unary_arith_vec<s3p_int8_t, SumProtocol> }
  , { "shared3p::sum_int16_vec", &unary_arith_vec<s3p_int16_t, SumProtocol> }
  , { "shared3p::sum_int32_vec", &unary_arith_vec<s3p_int32_t, SumProtocol> }
  , { "shared3p::sum_int64_vec", &unary_arith_vec<s3p_int64_t, SumProtocol> }
/*
  , { "shared3p::product_int8_vec",  &unary_arith_vec<s3p_int8_t, ProductProtocol> }
  , { "shared3p::product_int16_vec", &unary_arith_vec<s3p_int16_t, ProductProtocol> }
  , { "shared3p::product_int32_vec", &unary_arith_vec<s3p_int32_t, ProductProtocol> }
  , { "shared3p::product_int64_vec", &unary_arith_vec<s3p_int64_t, ProductProtocol> }
  , { "shared3p::neg_int8_vec",  &unary_arith_vec<s3p_int8_t, NegProtocol> }
  , { "shared3p::neg_int16_vec", &unary_arith_vec<s3p_int16_t, NegProtocol> }
  , { "shared3p::neg_int32_vec", &unary_arith_vec<s3p_int32_t, NegProtocol> }
  , { "shared3p::neg_int64_vec", &unary_arith_vec<s3p_int64_t, NegProtocol> }
  , { "shared3p::sub_int8_vec",  &binary_arith_vec<s3p_int8_t , SubtractionProtocol> }
  , { "shared3p::sub_int16_vec", &binary_arith_vec<s3p_int16_t, SubtractionProtocol> }
  , { "shared3p::sub_int32_vec", &binary_arith_vec<s3p_int32_t, SubtractionProtocol> }
  , { "shared3p::sub_int64_vec", &binary_arith_vec<s3p_int64_t, SubtractionProtocol> }
  , { "shared3p::mul_int8_vec",  &binary_arith_vec<s3p_int8_t , MultiplicationProtocol> }
  , { "shared3p::mul_int16_vec", &binary_arith_vec<s3p_int16_t, MultiplicationProtocol> }
  , { "shared3p::mul_int32_vec", &binary_arith_vec<s3p_int32_t, MultiplicationProtocol> }
  , { "shared3p::mul_int64_vec", &binary_arith_vec<s3p_int64_t, MultiplicationProtocol> }
  , { "shared3p::mulc_int8_vec",  &mulc_vec<s3p_int8_t, PublicMultiplicationProtocol> }
  , { "shared3p::mulc_int16_vec", &mulc_vec<s3p_int16_t, PublicMultiplicationProtocol> }
  , { "shared3p::mulc_int32_vec", &mulc_vec<s3p_int32_t, PublicMultiplicationProtocol> }
  , { "shared3p::mulc_int64_vec", &mulc_vec<s3p_int64_t, PublicMultiplicationProtocol> }
*/

  // Comparisons
  , { "shared3p::eq_int8_vec",   NAMED_SYSCALL_WRAPPER("eq_int8_vec", eq_vec<s3p_int8_t>) }
  , { "shared3p::eq_int16_vec",  NAMED_SYSCALL_WRAPPER("eq_int16_vec", eq_vec<s3p_int16_t>) }
  , { "shared3p::eq_int32_vec",  NAMED_SYSCALL_WRAPPER("eq_int32_vec", eq_vec<s3p_int32_t>) }
  , { "shared3p::eq_int64_vec",  NAMED_SYSCALL_WRAPPER("eq_int64_vec", eq_vec<s3p_int64_t>) }
  , { "shared3p::gt_int8_vec",   NAMED_SYSCALL_WRAPPER("gt_int8_vec", compare_vec<s3p_int8_t , ModeLT,  true>) }
  , { "shared3p::gt_int16_vec",  NAMED_SYSCALL_WRAPPER("gt_int16_vec", compare_vec<s3p_int16_t, ModeLT,  true>) }
  , { "shared3p::gt_int32_vec",  NAMED_SYSCALL_WRAPPER("gt_int32_vec", compare_vec<s3p_int32_t, ModeLT,  true>) }
  , { "shared3p::gt_int64_vec",  NAMED_SYSCALL_WRAPPER("gt_int64_vec", compare_vec<s3p_int64_t, ModeLT,  true>) }
  , { "shared3p::gte_int8_vec",  NAMED_SYSCALL_WRAPPER("gte_int8_vec", compare_vec<s3p_int8_t , ModeLE,  true>) }
  , { "shared3p::gte_int16_vec", NAMED_SYSCALL_WRAPPER("gte_int16_vec", compare_vec<s3p_int16_t, ModeLE,  true>) }
  , { "shared3p::gte_int32_vec", NAMED_SYSCALL_WRAPPER("gte_int32_vec", compare_vec<s3p_int32_t, ModeLE,  true>) }
  , { "shared3p::gte_int64_vec", NAMED_SYSCALL_WRAPPER("gte_int64_vec", compare_vec<s3p_int64_t, ModeLE,  true>) }
  , { "shared3p::lt_int8_vec",   NAMED_SYSCALL_WRAPPER("lt_int8_vec", compare_vec<s3p_int8_t , ModeLT,  false>) }
  , { "shared3p::lt_int16_vec",  NAMED_SYSCALL_WRAPPER("lt_int16_vec", compare_vec<s3p_int16_t, ModeLT,  false>) }
  , { "shared3p::lt_int32_vec",  NAMED_SYSCALL_WRAPPER("lt_int32_vec", compare_vec<s3p_int32_t, ModeLT,  false>) }
  , { "shared3p::lt_int64_vec",  NAMED_SYSCALL_WRAPPER("lt_int64_vec", compare_vec<s3p_int64_t, ModeLT,  false>) }
  , { "shared3p::lte_int8_vec",  NAMED_SYSCALL_WRAPPER("lte_int8_vec", compare_vec<s3p_int8_t , ModeLE,  false>) }
  , { "shared3p::lte_int16_vec", NAMED_SYSCALL_WRAPPER("lte_int16_vec", compare_vec<s3p_int16_t, ModeLE,  false>) }
  , { "shared3p::lte_int32_vec", NAMED_SYSCALL_WRAPPER("lte_int32_vec", compare_vec<s3p_int32_t, ModeLE,  false>) }
  , { "shared3p::lte_int64_vec", NAMED_SYSCALL_WRAPPER("lte_int64_vec", compare_vec<s3p_int64_t, ModeLE,  false>) }

  // Casting
  , { "shared3p::conv_int8_to_bool_vec",  &unary_vec<s3p_int8_t, s3p_bool_t, Conversion> }
  , { "shared3p::conv_int16_to_bool_vec", &unary_vec<s3p_int16_t, s3p_bool_t, Conversion> }
  , { "shared3p::conv_int32_to_bool_vec", &unary_vec<s3p_int32_t, s3p_bool_t, Conversion> }
  , { "shared3p::conv_int64_to_bool_vec", &unary_vec<s3p_int64_t, s3p_bool_t, Conversion> }
  , { "shared3p::conv_int8_to_uint8_vec",  &unary_vec<s3p_int8_t, s3p_uint8_t, Conversion> }
  , { "shared3p::conv_int8_to_uint16_vec",  &unary_vec<s3p_int8_t, s3p_uint16_t, Conversion> }
  , { "shared3p::conv_int8_to_uint32_vec",  &unary_vec<s3p_int8_t, s3p_uint32_t, Conversion> }
  , { "shared3p::conv_int8_to_uint64_vec",  &unary_vec<s3p_int8_t, s3p_uint64_t, Conversion> }
  , { "shared3p::conv_int16_to_uint8_vec",  &unary_vec<s3p_int16_t, s3p_uint8_t, Conversion> }
  , { "shared3p::conv_int16_to_uint16_vec",  &unary_vec<s3p_int16_t, s3p_uint16_t, Conversion> }
  , { "shared3p::conv_int16_to_uint32_vec",  &unary_vec<s3p_int16_t, s3p_uint32_t, Conversion> }
  , { "shared3p::conv_int16_to_uint64_vec",  &unary_vec<s3p_int16_t, s3p_uint64_t, Conversion> }
  , { "shared3p::conv_int32_to_uint8_vec", &unary_vec<s3p_int32_t, s3p_uint8_t, Conversion> }
  , { "shared3p::conv_int32_to_uint16_vec", &unary_vec<s3p_int32_t, s3p_uint16_t, Conversion> }
  , { "shared3p::conv_int32_to_uint32_vec", &unary_vec<s3p_int32_t, s3p_uint32_t, Conversion> }
  , { "shared3p::conv_int32_to_uint64_vec", &unary_vec<s3p_int32_t, s3p_uint64_t, Conversion> }
  , { "shared3p::conv_int64_to_uint8_vec",  &unary_vec<s3p_int64_t, s3p_uint8_t, Conversion> }
  , { "shared3p::conv_int64_to_uint16_vec",  &unary_vec<s3p_int64_t, s3p_uint16_t, Conversion> }
  , { "shared3p::conv_int64_to_uint32_vec",  &unary_vec<s3p_int64_t, s3p_uint32_t, Conversion> }
  , { "shared3p::conv_int64_to_uint64_vec",  &unary_vec<s3p_int64_t, s3p_uint64_t, Conversion> }

/*
  , { "shared3p::conv_int16_to_int8_vec",  &unary_vec<s3p_int16_t, s3p_int8_t, ConvertDownProtocol> }
  , { "shared3p::conv_int32_to_int8_vec",  &unary_vec<s3p_int32_t, s3p_int8_t, ConvertDownProtocol> }
  , { "shared3p::conv_int32_to_int16_vec", &unary_vec<s3p_int32_t, s3p_int16_t, ConvertDownProtocol> }
  , { "shared3p::conv_int64_to_int8_vec",  &unary_vec<s3p_int64_t, s3p_int8_t, ConvertDownProtocol> }
  , { "shared3p::conv_int64_to_int16_vec", &unary_vec<s3p_int64_t, s3p_int16_t, ConvertDownProtocol> }
  , { "shared3p::conv_int64_to_int32_vec", &unary_vec<s3p_int64_t, s3p_int32_t, ConvertDownProtocol> }

  , { "shared3p::conv_int8_to_int16_vec",  &unary_vec<s3p_int8_t, s3p_int16_t, ConvertUpProtocol> }
  , { "shared3p::conv_int8_to_int32_vec",  &unary_vec<s3p_int8_t, s3p_int32_t, ConvertUpProtocol> }
  , { "shared3p::conv_int8_to_int64_vec",  &unary_vec<s3p_int8_t, s3p_int64_t, ConvertUpProtocol> }
  , { "shared3p::conv_int16_to_int32_vec", &unary_vec<s3p_int16_t, s3p_int32_t, ConvertUpProtocol> }
  , { "shared3p::conv_int16_to_int64_vec", &unary_vec<s3p_int16_t, s3p_int64_t, ConvertUpProtocol> }
  , { "shared3p::conv_int32_to_int64_vec", &unary_vec<s3p_int32_t, s3p_int64_t, ConvertUpProtocol> }

  , { "shared3p::conv_int8_to_float32_vec", &unary_vec<s3p_int8_t, s3p_float32_t,ConversionToFloatProtocol> }
  , { "shared3p::conv_int8_to_float64_vec", &unary_vec<s3p_int8_t, s3p_float64_t,ConversionToFloatProtocol> }
  , { "shared3p::conv_int16_to_float32_vec", &unary_vec<s3p_int16_t, s3p_float32_t,ConversionToFloatProtocol> }
  , { "shared3p::conv_int16_to_float64_vec", &unary_vec<s3p_int16_t, s3p_float64_t,ConversionToFloatProtocol> }
  , { "shared3p::conv_int32_to_float32_vec", &unary_vec<s3p_int32_t, s3p_float32_t,ConversionToFloatProtocol> }
  , { "shared3p::conv_int32_to_float64_vec", &unary_vec<s3p_int32_t, s3p_float64_t, ConversionToFloatProtocol> }
  // This syscall is disabled until we can add precision to it.
  // See ConversionToFloatProtocol.h
  //, { "shared3p::conv_uint64_to_float32_vec", &unary_vec<s3p_uint64_t, s3p_float32_t, ConversionToFloatProtocol> }
  , { "shared3p::conv_int64_to_float64_vec", &unary_vec<s3p_int64_t, s3p_float64_t, ConversionToFloatProtocol> }

  // Utilities
  , { "shared3p::randomize_int8_vec",  &nullary_vec<s3p_int8_t, RandomizeProtocol> }
  , { "shared3p::randomize_int16_vec", &nullary_vec<s3p_int16_t, RandomizeProtocol> }
  , { "shared3p::randomize_int32_vec", &nullary_vec<s3p_int32_t, RandomizeProtocol> }
  , { "shared3p::randomize_int64_vec", &nullary_vec<s3p_int64_t, RandomizeProtocol> }

  // Special functions
  , { "shared3p::sign_int8_vec", &unary_arith_vec<s3p_int8_t, SignProtocol> }
  , { "shared3p::sign_int16_vec", &unary_arith_vec<s3p_int16_t, SignProtocol> }
  , { "shared3p::sign_int32_vec", &unary_arith_vec<s3p_int32_t, SignProtocol> }
  , { "shared3p::sign_int64_vec", &unary_arith_vec<s3p_int64_t, SignProtocol> }
  , { "shared3p::abs_int8_vec", &unary_vec<s3p_int8_t, s3p_uint8_t, AbsoluteValueProtocol> }
  , { "shared3p::abs_int16_vec", &unary_vec<s3p_int16_t, s3p_uint16_t, AbsoluteValueProtocol> }
  , { "shared3p::abs_int32_vec", &unary_vec<s3p_int32_t, s3p_uint32_t, AbsoluteValueProtocol> }
  , { "shared3p::abs_int64_vec", &unary_vec<s3p_int64_t, s3p_uint64_t, AbsoluteValueProtocol> }

  // Database functions
  , { "shared3p::vecshuf_int8_vec", &vector_shuffle<s3p_int8_t, false, false> }
  , { "shared3p::vecshuf_int16_vec", &vector_shuffle<s3p_int16_t, false, false> }
  , { "shared3p::vecshuf_int32_vec", &vector_shuffle<s3p_int32_t, false, false> }
  , { "shared3p::vecshuf_int64_vec", &vector_shuffle<s3p_int64_t, false, false> }
  , { "shared3p::vecshufkey_int8_vec", &vector_shuffle<s3p_int8_t, true, false> }
  , { "shared3p::vecshufkey_int16_vec", &vector_shuffle<s3p_int16_t, true, false> }
  , { "shared3p::vecshufkey_int32_vec", &vector_shuffle<s3p_int32_t, true, false> }
  , { "shared3p::vecshufkey_int64_vec", &vector_shuffle<s3p_int64_t, true, false> }
  , { "shared3p::vecshufinv_int8_vec", &vector_shuffle<s3p_int8_t, true, true> }
  , { "shared3p::vecshufinv_int16_vec", &vector_shuffle<s3p_int16_t, true, true> }
  , { "shared3p::vecshufinv_int32_vec", &vector_shuffle<s3p_int32_t, true, true> }
  , { "shared3p::vecshufinv_int64_vec", &vector_shuffle<s3p_int64_t, true, true> }
  , { "shared3p::matshuf_int8_vec", &matrix_shuffle<s3p_int8_t, false, false> }
  , { "shared3p::matshuf_int16_vec", &matrix_shuffle<s3p_int16_t, false, false> }
  , { "shared3p::matshuf_int32_vec", &matrix_shuffle<s3p_int32_t, false, false> }
  , { "shared3p::matshuf_int64_vec", &matrix_shuffle<s3p_int64_t, false, false> }
  , { "shared3p::matshufkey_int8_vec", &matrix_shuffle<s3p_int8_t, true, false> }
  , { "shared3p::matshufkey_int16_vec", &matrix_shuffle<s3p_int16_t, true, false> }
  , { "shared3p::matshufkey_int32_vec", &matrix_shuffle<s3p_int32_t, true, false> }
  , { "shared3p::matshufkey_int64_vec", &matrix_shuffle<s3p_int64_t, true, false> }
  , { "shared3p::matshufinv_int8_vec", &matrix_shuffle<s3p_int8_t, true, true> }
  , { "shared3p::matshufinv_int16_vec", &matrix_shuffle<s3p_int16_t, true, true> }
  , { "shared3p::matshufinv_int32_vec", &matrix_shuffle<s3p_int32_t, true, true> }
  , { "shared3p::matshufinv_int64_vec", &matrix_shuffle<s3p_int64_t, true, true> }
*/

  /**
   *  Bitwise shared unsigned integers
   */

   // Variable management
  , { "shared3p::new_xor_uint8_vec",  &new_vec<s3p_xor_uint8_t> }
  , { "shared3p::new_xor_uint16_vec", &new_vec<s3p_xor_uint16_t> }
  , { "shared3p::new_xor_uint32_vec", &new_vec<s3p_xor_uint32_t> }
  , { "shared3p::new_xor_uint64_vec", &new_vec<s3p_xor_uint64_t> }
  , { "shared3p::init_xor_uint8_vec",  &init_vec<s3p_xor_uint8_t> }
  , { "shared3p::init_xor_uint16_vec", &init_vec<s3p_xor_uint16_t> }
  , { "shared3p::init_xor_uint32_vec", &init_vec<s3p_xor_uint32_t> }
  , { "shared3p::init_xor_uint64_vec", &init_vec<s3p_xor_uint64_t> }
  , { "shared3p::set_shares_xor_uint8_vec",  &set_shares<s3p_xor_uint8_t> }
  , { "shared3p::set_shares_xor_uint16_vec", &set_shares<s3p_xor_uint16_t> }
  , { "shared3p::set_shares_xor_uint32_vec", &set_shares<s3p_xor_uint32_t> }
  , { "shared3p::set_shares_xor_uint64_vec", &set_shares<s3p_xor_uint64_t> }
  , { "shared3p::get_shares_xor_uint8_vec",  &get_shares<s3p_xor_uint8_t> }
  , { "shared3p::get_shares_xor_uint16_vec", &get_shares<s3p_xor_uint16_t> }
  , { "shared3p::get_shares_xor_uint32_vec", &get_shares<s3p_xor_uint32_t> }
  , { "shared3p::get_shares_xor_uint64_vec", &get_shares<s3p_xor_uint64_t> }
  , { "shared3p::fill_xor_uint8_vec",  &fill_vec<s3p_xor_uint8_t> }
  , { "shared3p::fill_xor_uint16_vec", &fill_vec<s3p_xor_uint16_t> }
  , { "shared3p::fill_xor_uint32_vec", &fill_vec<s3p_xor_uint32_t> }
  , { "shared3p::fill_xor_uint64_vec", &fill_vec<s3p_xor_uint64_t> }
  , { "shared3p::assign_xor_uint8_vec",  &assign_vec<s3p_xor_uint8_t> }
  , { "shared3p::assign_xor_uint16_vec", &assign_vec<s3p_xor_uint16_t> }
  , { "shared3p::assign_xor_uint32_vec", &assign_vec<s3p_xor_uint32_t> }
  , { "shared3p::assign_xor_uint64_vec", &assign_vec<s3p_xor_uint64_t> }
  , { "shared3p::delete_xor_uint8_vec",  &delete_vec<s3p_xor_uint8_t> }
  , { "shared3p::delete_xor_uint16_vec", &delete_vec<s3p_xor_uint16_t> }
  , { "shared3p::delete_xor_uint32_vec", &delete_vec<s3p_xor_uint32_t> }
  , { "shared3p::delete_xor_uint64_vec", &delete_vec<s3p_xor_uint64_t> }
  , { "shared3p::load_xor_uint8_vec",  &load_vec<s3p_xor_uint8_t> }
  , { "shared3p::load_xor_uint16_vec", &load_vec<s3p_xor_uint16_t> }
  , { "shared3p::load_xor_uint32_vec", &load_vec<s3p_xor_uint32_t> }
  , { "shared3p::load_xor_uint64_vec", &load_vec<s3p_xor_uint64_t> }
  , { "shared3p::store_xor_uint8_vec",  &store_vec<s3p_xor_uint8_t> }
  , { "shared3p::store_xor_uint16_vec", &store_vec<s3p_xor_uint16_t> }
  , { "shared3p::store_xor_uint32_vec", &store_vec<s3p_xor_uint32_t> }
  , { "shared3p::store_xor_uint64_vec", &store_vec<s3p_xor_uint64_t> }
  , { "shared3p::classify_xor_uint8_vec",  &classify_vec<s3p_xor_uint8_t> }
  , { "shared3p::classify_xor_uint16_vec", &classify_vec<s3p_xor_uint16_t> }
  , { "shared3p::classify_xor_uint32_vec", &classify_vec<s3p_xor_uint32_t> }
  , { "shared3p::classify_xor_uint64_vec", &classify_vec<s3p_xor_uint64_t> }
  , { "shared3p::declassify_xor_uint8_vec",  &declassify_vec<s3p_xor_uint8_t> }
  , { "shared3p::declassify_xor_uint16_vec", &declassify_vec<s3p_xor_uint16_t> }
  , { "shared3p::declassify_xor_uint32_vec", &declassify_vec<s3p_xor_uint32_t> }
  , { "shared3p::declassify_xor_uint64_vec", &declassify_vec<s3p_xor_uint64_t> }
  , { "shared3p::get_type_size_xor_uint8", &get_type_size<s3p_xor_uint8_t> }
  , { "shared3p::get_type_size_xor_uint16", &get_type_size<s3p_xor_uint16_t> }
  , { "shared3p::get_type_size_xor_uint32", &get_type_size<s3p_xor_uint32_t> }
  , { "shared3p::get_type_size_xor_uint64", &get_type_size<s3p_xor_uint64_t> }

/*
  , { "shared3p::reshare_xor_uint8_to_uint8_vec", &unary_vec<s3p_xor_uint8_t, s3p_uint8_t, ReshareProtocol> }
  , { "shared3p::reshare_xor_uint16_to_uint16_vec", &unary_vec<s3p_xor_uint16_t, s3p_uint16_t, ReshareProtocol> }
  , { "shared3p::reshare_xor_uint32_to_uint32_vec", &unary_vec<s3p_xor_uint32_t, s3p_uint32_t, ReshareProtocol> }
  , { "shared3p::reshare_xor_uint64_to_uint64_vec", &unary_vec<s3p_xor_uint64_t, s3p_uint64_t, ReshareProtocol> }
  , { "shared3p::reshare_uint8_to_xor_uint8_vec", &unary_vec<s3p_uint8_t, s3p_xor_uint8_t, ReshareProtocol> }
  , { "shared3p::reshare_uint16_to_xor_uint16_vec", &unary_vec<s3p_uint16_t, s3p_xor_uint16_t, ReshareProtocol> }
  , { "shared3p::reshare_uint32_to_xor_uint32_vec", &unary_vec<s3p_uint32_t, s3p_xor_uint32_t, ReshareProtocol> }
  , { "shared3p::reshare_uint64_to_xor_uint64_vec", &unary_vec<s3p_uint64_t, s3p_xor_uint64_t, ReshareProtocol> }

    // Bitwise operations on xor shared data
  , { "shared3p::xor_xor_uint8_vec",  &binary_arith_vec<s3p_xor_uint8_t , BitwiseXorProtocol> }
  , { "shared3p::xor_xor_uint16_vec", &binary_arith_vec<s3p_xor_uint16_t, BitwiseXorProtocol> }
  , { "shared3p::xor_xor_uint32_vec", &binary_arith_vec<s3p_xor_uint32_t, BitwiseXorProtocol> }
  , { "shared3p::xor_xor_uint64_vec", &binary_arith_vec<s3p_xor_uint64_t, BitwiseXorProtocol> }
  , { "shared3p::and_xor_uint8_vec",  &binary_arith_vec<s3p_xor_uint8_t , BitwiseAndProtocol> }
  , { "shared3p::and_xor_uint16_vec", &binary_arith_vec<s3p_xor_uint16_t, BitwiseAndProtocol> }
  , { "shared3p::and_xor_uint32_vec", &binary_arith_vec<s3p_xor_uint32_t, BitwiseAndProtocol> }
  , { "shared3p::and_xor_uint64_vec", &binary_arith_vec<s3p_xor_uint64_t, BitwiseAndProtocol> }
  , { "shared3p::or_xor_uint8_vec",  &binary_arith_vec<s3p_xor_uint8_t , BitwiseOrProtocol> }
  , { "shared3p::or_xor_uint16_vec", &binary_arith_vec<s3p_xor_uint16_t, BitwiseOrProtocol> }
  , { "shared3p::or_xor_uint32_vec", &binary_arith_vec<s3p_xor_uint32_t, BitwiseOrProtocol> }
  , { "shared3p::or_xor_uint64_vec", &binary_arith_vec<s3p_xor_uint64_t, BitwiseOrProtocol> }
  , { "shared3p::inv_xor_uint8_vec",  &unary_arith_vec<s3p_xor_uint8_t,  BitwiseInvProtocol> }
  , { "shared3p::inv_xor_uint16_vec", &unary_arith_vec<s3p_xor_uint16_t, BitwiseInvProtocol> }
  , { "shared3p::inv_xor_uint32_vec", &unary_arith_vec<s3p_xor_uint32_t, BitwiseInvProtocol> }
  , { "shared3p::inv_xor_uint64_vec", &unary_arith_vec<s3p_xor_uint64_t, BitwiseInvProtocol> }
*/

   // Comparisons
  , { "shared3p::eq_xor_uint8_vec",   NAMED_SYSCALL_WRAPPER("eq_xor_uint8_vec", eq_vec<s3p_xor_uint8_t>) }
  , { "shared3p::eq_xor_uint16_vec",  NAMED_SYSCALL_WRAPPER("eq_xor_uint16_vec", eq_vec<s3p_xor_uint16_t>) }
  , { "shared3p::eq_xor_uint32_vec",  NAMED_SYSCALL_WRAPPER("eq_xor_uint32_vec", eq_vec<s3p_xor_uint32_t>) }
  , { "shared3p::eq_xor_uint64_vec",  NAMED_SYSCALL_WRAPPER("eq_xor_uint64_vec", eq_vec<s3p_xor_uint64_t>) }
  , { "shared3p::gt_xor_uint8_vec",   NAMED_SYSCALL_WRAPPER("gt_xor_uint8_vec", compare_vec<s3p_xor_uint8_t,  ModeLT, true>) }
  , { "shared3p::gt_xor_uint16_vec",  NAMED_SYSCALL_WRAPPER("gt_xor_uint16_vec", compare_vec<s3p_xor_uint16_t, ModeLT, true>) }
  , { "shared3p::gt_xor_uint32_vec",  NAMED_SYSCALL_WRAPPER("gt_xor_uint32_vec", compare_vec<s3p_xor_uint32_t, ModeLT, true>) }
  , { "shared3p::gt_xor_uint64_vec",  NAMED_SYSCALL_WRAPPER("gt_xor_uint64_vec", compare_vec<s3p_xor_uint64_t, ModeLT, true>) }
  , { "shared3p::gte_xor_uint8_vec",  NAMED_SYSCALL_WRAPPER("gte_xor_uint8_vec", compare_vec<s3p_xor_uint8_t,  ModeLE, true>) }
  , { "shared3p::gte_xor_uint16_vec", NAMED_SYSCALL_WRAPPER("gte_xor_uint16_vec", compare_vec<s3p_xor_uint16_t, ModeLE, true>) }
  , { "shared3p::gte_xor_uint32_vec", NAMED_SYSCALL_WRAPPER("gte_xor_uint32_vec", compare_vec<s3p_xor_uint32_t, ModeLE, true>) }
  , { "shared3p::gte_xor_uint64_vec", NAMED_SYSCALL_WRAPPER("gte_xor_uint64_vec", compare_vec<s3p_xor_uint64_t, ModeLE, true>) }
  , { "shared3p::lt_xor_uint8_vec",   NAMED_SYSCALL_WRAPPER("lt_xor_uint8_vec", compare_vec<s3p_xor_uint8_t,  ModeLT, false>) }
  , { "shared3p::lt_xor_uint16_vec",  NAMED_SYSCALL_WRAPPER("lt_xor_uint16_vec", compare_vec<s3p_xor_uint16_t, ModeLT, false>) }
  , { "shared3p::lt_xor_uint32_vec",  NAMED_SYSCALL_WRAPPER("lt_xor_uint32_vec", compare_vec<s3p_xor_uint32_t, ModeLT, false>) }
  , { "shared3p::lt_xor_uint64_vec",  NAMED_SYSCALL_WRAPPER("lt_xor_uint64_vec", compare_vec<s3p_xor_uint64_t, ModeLT, false>) }
  , { "shared3p::lte_xor_uint8_vec",  NAMED_SYSCALL_WRAPPER("lte_xor_uint8_vec", compare_vec<s3p_xor_uint8_t,  ModeLE, false>) }
  , { "shared3p::lte_xor_uint16_vec", NAMED_SYSCALL_WRAPPER("lte_xor_uint16_vec", compare_vec<s3p_xor_uint16_t, ModeLE, false>) }
  , { "shared3p::lte_xor_uint32_vec", NAMED_SYSCALL_WRAPPER("lte_xor_uint32_vec", compare_vec<s3p_xor_uint32_t, ModeLE, false>) }
  , { "shared3p::lte_xor_uint64_vec", NAMED_SYSCALL_WRAPPER("lte_xor_uint64_vec", compare_vec<s3p_xor_uint64_t, ModeLE, false>) }

  // Casting
  , { "shared3p::conv_bool_to_xor_uint8_vec",  &unary_vec<s3p_bool_t, s3p_xor_uint8_t, Conversion> }
  , { "shared3p::conv_bool_to_xor_uint16_vec", &unary_vec<s3p_bool_t, s3p_xor_uint16_t, Conversion> }
  , { "shared3p::conv_bool_to_xor_uint32_vec", &unary_vec<s3p_bool_t, s3p_xor_uint32_t, Conversion> }
  , { "shared3p::conv_bool_to_xor_uint64_vec", &unary_vec<s3p_bool_t, s3p_xor_uint64_t, Conversion> }
  , { "shared3p::conv_xor_uint64_to_xor_uint32_vec", &unary_vec<s3p_xor_uint64_t, s3p_xor_uint32_t, Conversion> }
  , { "shared3p::conv_xor_uint64_to_xor_uint16_vec", &unary_vec<s3p_xor_uint64_t, s3p_xor_uint16_t, Conversion> }
  , { "shared3p::conv_xor_uint64_to_xor_uint8_vec", &unary_vec<s3p_xor_uint64_t, s3p_xor_uint8_t, Conversion> }
  , { "shared3p::conv_xor_uint32_to_xor_uint16_vec", &unary_vec<s3p_xor_uint64_t, s3p_xor_uint32_t, Conversion> }
  , { "shared3p::conv_xor_uint32_to_xor_uint8_vec", &unary_vec<s3p_xor_uint64_t, s3p_xor_uint32_t, Conversion> }
  , { "shared3p::conv_xor_uint16_to_xor_uint8_vec", &unary_vec<s3p_xor_uint64_t, s3p_xor_uint32_t, Conversion> }
  , { "shared3p::conv_xor_uint8_to_xor_uint64_vec", &unary_vec<s3p_xor_uint8_t, s3p_xor_uint64_t, Conversion> }
  , { "shared3p::conv_xor_uint8_to_xor_uint32_vec", &unary_vec<s3p_xor_uint8_t, s3p_xor_uint32_t, Conversion> }
  , { "shared3p::conv_xor_uint8_to_xor_uint16_vec", &unary_vec<s3p_xor_uint8_t, s3p_xor_uint16_t, Conversion> }
  , { "shared3p::conv_xor_uint16_to_xor_uint64_vec", &unary_vec<s3p_xor_uint16_t, s3p_xor_uint64_t, Conversion> }
  , { "shared3p::conv_xor_uint16_to_xor_uint32_vec", &unary_vec<s3p_xor_uint16_t, s3p_xor_uint32_t, Conversion> }
  , { "shared3p::conv_xor_uint32_to_xor_uint64_vec", &unary_vec<s3p_xor_uint32_t, s3p_xor_uint64_t, Conversion> }

/*
   // Utilities
  , { "shared3p::randomize_xor_uint8_vec",  &nullary_vec<s3p_xor_uint8_t, RandomizeProtocol> }
  , { "shared3p::randomize_xor_uint16_vec", &nullary_vec<s3p_xor_uint16_t, RandomizeProtocol> }
  , { "shared3p::randomize_xor_uint32_vec", &nullary_vec<s3p_xor_uint32_t, RandomizeProtocol> }
  , { "shared3p::randomize_xor_uint64_vec", &nullary_vec<s3p_xor_uint64_t, RandomizeProtocol> }
  , { "shared3p::shift_left_xor_uint8_vec", &binary_private_public_vec<s3p_xor_uint8_t, int64_t, LeftShiftProtocol> }
  , { "shared3p::shift_left_xor_uint16_vec", &binary_private_public_vec<s3p_xor_uint16_t, int64_t, LeftShiftProtocol> }
  , { "shared3p::shift_left_xor_uint32_vec", &binary_private_public_vec<s3p_xor_uint32_t, int64_t, LeftShiftProtocol> }
  , { "shared3p::shift_left_xor_uint64_vec", &binary_private_public_vec<s3p_xor_uint64_t, int64_t, LeftShiftProtocol> }
  , { "shared3p::rotate_left_xor_uint8_vec", &binary_private_public_vec<s3p_xor_uint8_t, int64_t, LeftRotationProtocol> }
  , { "shared3p::rotate_left_xor_uint16_vec", &binary_private_public_vec<s3p_xor_uint16_t, int64_t, LeftRotationProtocol> }
  , { "shared3p::rotate_left_xor_uint32_vec", &binary_private_public_vec<s3p_xor_uint32_t, int64_t, LeftRotationProtocol> }
  , { "shared3p::rotate_left_xor_uint64_vec", &binary_private_public_vec<s3p_xor_uint64_t, int64_t, LeftRotationProtocol> }

   // Special functions
  , { "shared3p::choose_xor_uint8_vec", &oblivious_choice_vec<s3p_bool_t, s3p_xor_uint8_t, ObliviousChoiceProtocol> }
  , { "shared3p::choose_xor_uint16_vec", &oblivious_choice_vec<s3p_bool_t, s3p_xor_uint16_t, ObliviousChoiceProtocol> }
  , { "shared3p::choose_xor_uint32_vec", &oblivious_choice_vec<s3p_bool_t, s3p_xor_uint32_t, ObliviousChoiceProtocol> }
  , { "shared3p::choose_xor_uint64_vec", &oblivious_choice_vec<s3p_bool_t, s3p_xor_uint64_t, ObliviousChoiceProtocol> }

  , { "shared3p::min_xor_uint8_vec",  &binary_arith_vec<s3p_xor_uint8_t, MinimumMaximumProtocol<ModeMin> > }
  , { "shared3p::min_xor_uint16_vec", &binary_arith_vec<s3p_xor_uint16_t, MinimumMaximumProtocol<ModeMin> > }
  , { "shared3p::min_xor_uint32_vec", &binary_arith_vec<s3p_xor_uint32_t, MinimumMaximumProtocol<ModeMin> > }
  , { "shared3p::min_xor_uint64_vec", &binary_arith_vec<s3p_xor_uint64_t, MinimumMaximumProtocol<ModeMin> > }
  , { "shared3p::max_xor_uint8_vec",  &binary_arith_vec<s3p_xor_uint8_t, MinimumMaximumProtocol<ModeMax> > }
  , { "shared3p::max_xor_uint16_vec", &binary_arith_vec<s3p_xor_uint16_t, MinimumMaximumProtocol<ModeMax> > }
  , { "shared3p::max_xor_uint32_vec", &binary_arith_vec<s3p_xor_uint32_t, MinimumMaximumProtocol<ModeMax> > }
  , { "shared3p::max_xor_uint64_vec", &binary_arith_vec<s3p_xor_uint64_t, MinimumMaximumProtocol<ModeMax> > }

  , { "shared3p::vecmin_xor_uint8_vec",  &unary_arith_vec<s3p_xor_uint8_t, MinimumMaximumProtocol<ModeMin> > }
  , { "shared3p::vecmin_xor_uint16_vec", &unary_arith_vec<s3p_xor_uint16_t, MinimumMaximumProtocol<ModeMin> > }
  , { "shared3p::vecmin_xor_uint32_vec", &unary_arith_vec<s3p_xor_uint32_t, MinimumMaximumProtocol<ModeMin> > }
  , { "shared3p::vecmin_xor_uint64_vec", &unary_arith_vec<s3p_xor_uint64_t, MinimumMaximumProtocol<ModeMin> > }
  , { "shared3p::vecmax_xor_uint8_vec",  &unary_arith_vec<s3p_xor_uint8_t, MinimumMaximumProtocol<ModeMax> > }
  , { "shared3p::vecmax_xor_uint16_vec", &unary_arith_vec<s3p_xor_uint16_t, MinimumMaximumProtocol<ModeMax> > }
  , { "shared3p::vecmax_xor_uint32_vec", &unary_arith_vec<s3p_xor_uint32_t, MinimumMaximumProtocol<ModeMax> > }
  , { "shared3p::vecmax_xor_uint64_vec", &unary_arith_vec<s3p_xor_uint64_t, MinimumMaximumProtocol<ModeMax> > }

  , { "shared3p::msnzb_xor_uint8_vec", unary_arith_vec<s3p_xor_uint8_t, MostSignificantNonZeroBitProtocol> }
  , { "shared3p::aes128_xor_uint32_vec", &aes_xor_uint32_vec<Aes128Protocol> }
//  , { "shared3p::aes192_xor_uint32_vec", &aes_xor_uint32_vec<Aes192Protocol> }
//  , { "shared3p::aes256_xor_uint32_vec", &aes_xor_uint32_vec<Aes256Protocol> }
  , { "shared3p::aes128_xor_uint32_vec_expand_key", &aes_xor_uint32_vec_expand_key<Aes128Protocol> }
//  , { "shared3p::aes192_xor_uint32_vec_expand_key", &aes_xor_uint32_vec_expand_key<Aes192Protocol> }
//  , { "shared3p::aes256_xor_uint32_vec_expand_key", &aes_xor_uint32_vec_expand_key<Aes256Protocol> }
  , { "shared3p::aes128_test", &aes_test<Aes128Protocol> }
  , { "shared3p::crc16_xor_vec", &crc_xor_vec<CRCMode16> }
  , { "shared3p::crc32_xor_vec", &crc_xor_vec<CRCMode32> }

   // Database functions
  , { "shared3p::vecshuf_xor_uint8_vec", &vector_shuffle<s3p_xor_uint8_t, false, false> }
  , { "shared3p::vecshuf_xor_uint16_vec", &vector_shuffle<s3p_xor_uint16_t, false, false> }
  , { "shared3p::vecshuf_xor_uint32_vec", &vector_shuffle<s3p_xor_uint32_t, false, false> }
  , { "shared3p::vecshuf_xor_uint64_vec", &vector_shuffle<s3p_xor_uint64_t, false, false> }
  , { "shared3p::vecshufkey_xor_uint8_vec", &vector_shuffle<s3p_xor_uint8_t, true, false> }
  , { "shared3p::vecshufkey_xor_uint16_vec", &vector_shuffle<s3p_xor_uint16_t, true, false> }
  , { "shared3p::vecshufkey_xor_uint32_vec", &vector_shuffle<s3p_xor_uint32_t, true, false> }
  , { "shared3p::vecshufkey_xor_uint64_vec", &vector_shuffle<s3p_xor_uint64_t, true, false> }
  , { "shared3p::vecshufinv_xor_uint8_vec", &vector_shuffle<s3p_xor_uint8_t, true, true> }
  , { "shared3p::vecshufinv_xor_uint16_vec", &vector_shuffle<s3p_xor_uint16_t, true, true> }
  , { "shared3p::vecshufinv_xor_uint32_vec", &vector_shuffle<s3p_xor_uint32_t, true, true> }
  , { "shared3p::vecshufinv_xor_uint64_vec", &vector_shuffle<s3p_xor_uint64_t, true, true> }
  , { "shared3p::matshuf_xor_uint8_vec", &matrix_shuffle<s3p_xor_uint8_t, false, false> }
  , { "shared3p::matshuf_xor_uint16_vec", &matrix_shuffle<s3p_xor_uint16_t, false, false> }
  , { "shared3p::matshuf_xor_uint32_vec", &matrix_shuffle<s3p_xor_uint32_t, false, false> }
  , { "shared3p::matshuf_xor_uint64_vec", &matrix_shuffle<s3p_xor_uint64_t, false, false> }
  , { "shared3p::matshufkey_xor_uint8_vec", &matrix_shuffle<s3p_xor_uint8_t, true, false> }
  , { "shared3p::matshufkey_xor_uint16_vec", &matrix_shuffle<s3p_xor_uint16_t, true, false> }
  , { "shared3p::matshufkey_xor_uint32_vec", &matrix_shuffle<s3p_xor_uint32_t, true, false> }
  , { "shared3p::matshufkey_xor_uint64_vec", &matrix_shuffle<s3p_xor_uint64_t, true, false> }
  , { "shared3p::matshufinv_xor_uint8_vec", &matrix_shuffle<s3p_xor_uint8_t, true, true> }
  , { "shared3p::matshufinv_xor_uint16_vec", &matrix_shuffle<s3p_xor_uint16_t, true, true> }
  , { "shared3p::matshufinv_xor_uint32_vec", &matrix_shuffle<s3p_xor_uint32_t, true, true> }
  , { "shared3p::matshufinv_xor_uint64_vec", &matrix_shuffle<s3p_xor_uint64_t, true, true> }
*/

  /**
   *  Floating point numbers
   */

   // Variable management
/*
  , { "shared3p::new_float32_vec", &new_vec<s3p_float32_t> }
  , { "shared3p::new_float64_vec", &new_vec<s3p_float64_t> }
  , { "shared3p::init_float32_vec", &init_vec<s3p_float32_t> }
  , { "shared3p::init_float64_vec", &init_vec<s3p_float64_t> }
  , { "shared3p::set_shares_float32_vec", &set_shares<s3p_float32_t> }
  , { "shared3p::set_shares_float64_vec", &set_shares<s3p_float64_t> }
  , { "shared3p::get_shares_float32_vec", &get_shares<s3p_float32_t> }
  , { "shared3p::get_shares_float64_vec", &get_shares<s3p_float64_t> }
  , { "shared3p::fill_float32_vec", &fill_vec<s3p_float32_t> }
  , { "shared3p::fill_float64_vec", &fill_vec<s3p_float64_t> }
  , { "shared3p::assign_float32_vec", &assign_vec<s3p_float32_t> }
  , { "shared3p::assign_float64_vec", &assign_vec<s3p_float64_t> }
  , { "shared3p::delete_float32_vec", &delete_vec<s3p_float32_t> }
  , { "shared3p::delete_float64_vec", &delete_vec<s3p_float64_t> }
  , { "shared3p::load_float32_vec", &load_vec<s3p_float32_t> }
  , { "shared3p::load_float64_vec", &load_vec<s3p_float64_t> }
  , { "shared3p::store_float32_vec", &store_vec<s3p_float32_t> }
  , { "shared3p::store_float64_vec", &store_vec<s3p_float64_t> }
  , { "shared3p::classify_float32_vec", &classify_vec<s3p_float32_t> }
  , { "shared3p::classify_float64_vec", &classify_vec<s3p_float64_t> }
  , { "shared3p::declassify_float32_vec", &declassify_vec<s3p_float32_t> }
  , { "shared3p::declassify_float64_vec", &declassify_vec<s3p_float64_t> }
  , { "shared3p::get_type_size_float32", &get_type_size<s3p_float32_t> }
  , { "shared3p::get_type_size_float64", &get_type_size<s3p_float64_t> }

    // Floating point arithmetic
  , { "shared3p::add_float32_vec", &binary_arith_vec<s3p_float32_t, FloatAdditionProtocol> }
  , { "shared3p::add_float64_vec", &binary_arith_vec<s3p_float64_t, FloatAdditionProtocol> }
  , { "shared3p::sum_float32_vec",  &unary_arith_vec<s3p_float32_t, FloatSummationProtocol> }
  , { "shared3p::sum_float64_vec",  &unary_arith_vec<s3p_float64_t, FloatSummationProtocol> }
  , { "shared3p::neg_float32_vec",  &unary_arith_vec<s3p_float32_t, FloatNegationProtocol> }
  , { "shared3p::neg_float64_vec",  &unary_arith_vec<s3p_float64_t, FloatNegationProtocol> }
  , { "shared3p::sub_float32_vec", &binary_arith_vec<s3p_float32_t, FloatSubtractionProtocol> }
  , { "shared3p::sub_float64_vec", &binary_arith_vec<s3p_float64_t, FloatSubtractionProtocol> }
  , { "shared3p::mul_float32_vec", &binary_arith_vec<s3p_float32_t, FloatMultiplicationProtocol> }
  , { "shared3p::mul_float64_vec", &binary_arith_vec<s3p_float64_t, FloatMultiplicationProtocol> }
  , { "shared3p::inv_float32_vec",  &unary_arith_vec<s3p_float32_t, FloatInverseProtocol> }
  , { "shared3p::inv_float64_vec",  &unary_arith_vec<s3p_float64_t, FloatInverseProtocol> }
  , { "shared3p::div_float32_vec", &binary_arith_vec<s3p_float32_t, FloatDivisionProtocol> }
  , { "shared3p::div_float64_vec", &binary_arith_vec<s3p_float64_t, FloatDivisionProtocol> }
  , { "shared3p::mulc_float32_vec", &mulc_vec<s3p_float32_t, FloatPublicMultiplicationProtocol> }
  , { "shared3p::mulc_float64_vec", &mulc_vec<s3p_float64_t, FloatPublicMultiplicationProtocol> }
  , { "shared3p::divc_float32_vec", &divc_vec<s3p_float32_t, FloatPublicDivisionProtocol> }
  , { "shared3p::divc_float64_vec", &divc_vec<s3p_float64_t, FloatPublicDivisionProtocol> }

    // Floating point comparisons
  , { "shared3p::isnegligible_float32_vec", &unary_vec<s3p_float32_t, s3p_bool_t, FloatIsNegligibleProtocol> }
  , { "shared3p::isnegligible_float64_vec", &unary_vec<s3p_float64_t, s3p_bool_t, FloatIsNegligibleProtocol> }
  , { "shared3p::eq_float32_vec", &binary_vec<s3p_float32_t, s3p_float32_t, s3p_bool_t, FloatEqualityProtocol> }
  , { "shared3p::eq_float64_vec", &binary_vec<s3p_float64_t, s3p_float64_t, s3p_bool_t, FloatEqualityProtocol> }
*/
    /*, { "shared3p::max_float32_vec", &max_vec<s3p_float32_t> }
      , { "shared3p::min_float32_vec", &min_vec<s3p_float32_t> }*/
/*
  , { "shared3p::lt_float32_vec", &compare_vec<s3p_float32_t, FloatComparisonProtocol<ModeLT>, false> }
  , { "shared3p::lte_float32_vec", &compare_vec<s3p_float32_t, FloatComparisonProtocol<ModeLE>, false> }
  , { "shared3p::gt_float32_vec", &compare_vec<s3p_float32_t, FloatComparisonProtocol<ModeLT>, true> }
  , { "shared3p::gte_float32_vec", &compare_vec<s3p_float32_t, FloatComparisonProtocol<ModeLE>, true> }
  , { "shared3p::lt_float64_vec", &compare_vec<s3p_float64_t, FloatComparisonProtocol<ModeLT>, false> }
  , { "shared3p::lte_float64_vec", &compare_vec<s3p_float64_t, FloatComparisonProtocol<ModeLE>, false> }
  , { "shared3p::gt_float64_vec", &compare_vec<s3p_float64_t, FloatComparisonProtocol<ModeLT>, true> }
  , { "shared3p::gte_float64_vec", &compare_vec<s3p_float64_t, FloatComparisonProtocol<ModeLE>, true> }

   // Casting floating point numbers
  , { "shared3p::ceiling_float32_vec", &unary_vec<s3p_float32_t, s3p_int32_t, FloatCeilingProtocol> }
  , { "shared3p::ceiling_float64_vec", &unary_vec<s3p_float64_t, s3p_int64_t, FloatCeilingProtocol> }
  , { "shared3p::floor_float32_vec", &unary_vec<s3p_float32_t, s3p_int32_t, FloatFloorProtocol> }
  , { "shared3p::floor_float64_vec", &unary_vec<s3p_float64_t, s3p_int64_t, FloatFloorProtocol> }

  // Floating point absolute value
  , { "shared3p::abs_float32_vec", &unary_vec<s3p_float32_t, s3p_float32_t, FloatAbsoluteValueProtocol> }
  , { "shared3p::abs_float64_vec", &unary_vec<s3p_float64_t, s3p_float64_t, FloatAbsoluteValueProtocol> }

  // Special functions
  , { "shared3p::choose_float32_vec", &oblivious_choice_vec<s3p_bool_t, s3p_float32_t, ObliviousChoiceProtocol> }
  , { "shared3p::choose_float64_vec", &oblivious_choice_vec<s3p_bool_t, s3p_float64_t, ObliviousChoiceProtocol> }
  , { "shared3p::erf_float32_vec", &unary_arith_vec<s3p_float32_t, FloatErrorFunctionProtocol> }
  , { "shared3p::erf_float64_vec", &unary_arith_vec<s3p_float64_t, FloatErrorFunctionProtocol> }
  , { "shared3p::exp_float32_vec", &unary_arith_vec<s3p_float32_t, FloatPowerOfEProtocol> }
  , { "shared3p::exp_float64_vec", &unary_arith_vec<s3p_float64_t, FloatPowerOfEProtocol> }
  , { "shared3p::ln_float32_vec", &unary_arith_vec<s3p_float32_t, FloatNaturalLogarithmProtocol> }
  , { "shared3p::ln_float64_vec", &unary_arith_vec<s3p_float64_t, FloatNaturalLogarithmProtocol> }
  , { "shared3p::sin_float32_vec", &unary_arith_vec<s3p_float32_t, FloatSineProtocol> }
  , { "shared3p::sin_float64_vec", &unary_arith_vec<s3p_float64_t, FloatSineProtocol> }
  , { "shared3p::sqrt_float32_vec", &unary_arith_vec<s3p_float32_t, FloatSquareRootProtocol> }
  , { "shared3p::sqrt_float64_vec", &unary_arith_vec<s3p_float64_t, FloatSquareRootProtocol> }

  // Database functions
  , { "shared3p::vecshuf_float32_vec", &vector_shuffle<s3p_float32_t, false, false> }
  , { "shared3p::vecshuf_float64_vec", &vector_shuffle<s3p_float64_t, false, false> }
  , { "shared3p::vecshufkey_float32_vec", &vector_shuffle<s3p_float32_t, true, false> }
  , { "shared3p::vecshufkey_float64_vec", &vector_shuffle<s3p_float64_t, true, false> }
  , { "shared3p::vecshufinv_float32_vec", &vector_shuffle<s3p_float32_t, true, true> }
  , { "shared3p::vecshufinv_float64_vec", &vector_shuffle<s3p_float64_t, true, true> }
  , { "shared3p::matshuf_float32_vec", &matrix_shuffle<s3p_float32_t, false, false> }
  , { "shared3p::matshuf_float64_vec", &matrix_shuffle<s3p_float64_t, false, false> }
  , { "shared3p::matshufkey_float32_vec", &matrix_shuffle<s3p_float32_t, true, false> }
  , { "shared3p::matshufkey_float64_vec", &matrix_shuffle<s3p_float64_t, true, false> }
  , { "shared3p::matshufinv_float32_vec", &matrix_shuffle<s3p_float32_t, true, true> }
  , { "shared3p::matshufinv_float64_vec", &matrix_shuffle<s3p_float64_t, true, true> }
*/

  /**
   *  Other functions
   */

  , { "shared3p::get_domain_name", get_domain_name }
);


SHAREMIND_MODULE_API_0x1_PD_STARTUP(shared3p_emu_startup, w) __attribute__ ((visibility("hidden")));
SHAREMIND_MODULE_API_0x1_PD_STARTUP(shared3p_emu_startup, w) {
    assert(w);
    assert(w->moduleHandle);
    assert(w->conf);
    assert(w->conf->pd_name);
    assert(w->conf->pd_conf_string);

    sharemind::Shared3pModule * const m =
        static_cast<sharemind::Shared3pModule*>(w->moduleHandle);

    try {
        w->pdHandle = new Shared3pPD(w->conf->pd_name,
                                     w->conf->pd_conf_string,
                                     *m);
        return SHAREMIND_MODULE_API_0x1_OK;
    } catch (const Shared3pPD::ConfigurationException & e) {
        m->logger().error() << "Error on protection domain '"
            << w->conf->pd_name << "' startup: " << e.what();
        return SHAREMIND_MODULE_API_0x1_INVALID_PD_CONFIGURATION;
    } catch (...) {
        return catchModuleApiErrors ();
    }
}

SHAREMIND_MODULE_API_0x1_PD_SHUTDOWN(shared3p_emu_shutdown, w) __attribute__ ((visibility("hidden")));
SHAREMIND_MODULE_API_0x1_PD_SHUTDOWN(shared3p_emu_shutdown, w) {
    assert(w);
    assert(w->pdHandle);
    assert(w->moduleHandle);

    try {
        delete static_cast<sharemind::Shared3pPD *>(w->pdHandle);
        #ifndef NDEBUG
        w->pdHandle = nullptr; // Not needed, but may help debugging.
        #endif
    } catch (...) {
        /// \todo log message or noexcept PD destructor
    }
}

SHAREMIND_MODULE_API_0x1_PDPI_STARTUP(shared3p_emu_PDPI_startup, w) __attribute__ ((visibility("hidden")));
SHAREMIND_MODULE_API_0x1_PDPI_STARTUP(shared3p_emu_PDPI_startup, w) {
    assert(w);
    assert(w->pdHandle);

    try {
        sharemind::Shared3pPD * const pd =
            static_cast<sharemind::Shared3pPD*>(w->pdHandle);
        w->pdProcessHandle = new Shared3pPDPI(*pd);
        return SHAREMIND_MODULE_API_0x1_OK;
    } catch (...) {
        return catchModuleApiErrors ();
    }
}

SHAREMIND_MODULE_API_0x1_PDPI_SHUTDOWN(shared3p_emu_PDPI_shutdown, w) __attribute__ ((visibility("hidden")));
SHAREMIND_MODULE_API_0x1_PDPI_SHUTDOWN(shared3p_emu_PDPI_shutdown, w) {
    assert(w);
    assert(w->pdHandle);
    assert(w->pdProcessHandle);

    static_assert(std::is_nothrow_destructible<sharemind::Shared3pPDPI>::value,
                "~Shared3pPDPI() not noexcept!");
    delete static_cast<sharemind::Shared3pPDPI *>(w->pdProcessHandle);
    #ifndef NDEBUG
    w->pdProcessHandle = nullptr; // Not needed, but may help debugging.
    #endif
}

SHAREMIND_MODULE_API_0x1_PDK_DEFINITIONS(
    {
        "shared3p",
        &shared3p_emu_startup,
        &shared3p_emu_shutdown,
        &shared3p_emu_PDPI_startup,
        &shared3p_emu_PDPI_shutdown
    }
);

} // extern "C" {
