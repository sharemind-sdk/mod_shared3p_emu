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

#ifndef MOD_SHARED3P_EMU_SYSCALLS_CORESYSCALLS_H
#define MOD_SHARED3P_EMU_SYSCALLS_CORESYSCALLS_H

#include <sharemind/libmodapi/api_0x1.h>
#include <sharemind/libemulator_protocols/VmVector.h>
#include "Common.h"
#include "../Shared3pPDPI.h"
#include "../ShareVector.h"
#include "../ValueTraits.h"


namespace sharemind {

namespace {

template <typename T>
inline void copy_shares(const share_vec<T> & src,
                        typename value_traits<T>::share_type * dest)
{
    std::copy(src.begin(), src.end(), dest);
}

template <>
inline void copy_shares<s3p_bool_t>(const share_vec<s3p_bool_t> & src,
                                     s3p_bool_t::share_type * dest)
{
    for (size_t i = 0u; i < src.size(); ++i)
        dest[i] = src[i];
}

} /* anonymous namespace */

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
NAMED_SYSCALL(new_vec, name, args, num_args, refs, crefs, returnValue, c)
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
        const size_t vsize = args[1u].uint64[0u];

        share_vec<T> * const vec = new share_vec<T>(vsize);
        pdpi->registerVector(vec);

        returnValue->p[0u] = vec;

        PROFILE_SYSCALL(pdpi->profiler(), pdpi->modelEvaluator(), name, vsize);

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
NAMED_SYSCALL(init_vec, name, args, num_args, refs, crefs, returnValue, c)
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

        share_vec<T> & vec = *static_cast<share_vec<T>*>(args[2u].p[0u]);
        const typename T::public_type init = getStack<T>(args[1u]);
        for (size_t i = 0u; i < vec.size(); ++i)
            vec[i] = init;

        PROFILE_SYSCALL(pdpi->profiler(), pdpi->modelEvaluator(), name,
                        vec.size());

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
NAMED_SYSCALL(set_shares, name, args, num_args, refs, crefs, returnValue, c)
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
        /** \todo the following is a workaround! We are always allocating
             one byte too much (for arrays) as VM does not allow us to allocate
             0 sized memory block. */
        const size_t num_elems = (crefs[0u].size - 1) / sizeof(share_type);

        if (num_args == 2) {
            if (!pdpi->isValidHandle<T>(args[1u].p[0u]))
                return SHAREMIND_MODULE_API_0x1_GENERAL_ERROR;

            share_vec<T> & dest = *static_cast<share_vec<T>*>(args[1u].p[0u]);
            if (dest.size() != num_elems)
                return SHAREMIND_MODULE_API_0x1_GENERAL_ERROR;

            dest.assign(src, src + num_elems);
        }

        if (returnValue)
            returnValue->uint64[0u] = num_elems;

        PROFILE_SYSCALL(pdpi->profiler(), pdpi->modelEvaluator(), name,
                        num_elems);

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
NAMED_SYSCALL(get_shares, name, args, num_args, refs, crefs, returnValue, c)
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
        const share_vec<T> & src = *static_cast<const share_vec<T>*>(args[1u].p[0u]);
        const size_t num_bytes = src.size() * sizeof(share_type);

        if (refs) {
            /** \todo the following is a workaround! We are always allocating
                 one byte too much (for arrays) as VM does not allow us to
                 allocate 0 sized memory block. */
            if (num_bytes != refs[0u].size - 1)
                return SHAREMIND_MODULE_API_0x1_GENERAL_ERROR;

            share_type * const dest = static_cast<share_type *>(refs[0u].pData);
            copy_shares (src, dest);
        }

        if (returnValue)
            returnValue->uint64[0u] = num_bytes;

        PROFILE_SYSCALL(pdpi->profiler(), pdpi->modelEvaluator(), name,
                        src.size());

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
NAMED_SYSCALL(get_type_size, name, args, num_args, refs, crefs, returnValue, c)
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
        Shared3pPDPI * const pdpi = static_cast<Shared3pPDPI*>(handles.pdpiHandle);

        typedef typename value_traits<T>::share_type share_type;
        returnValue->uint64[0u] = sizeof(share_type);

        PROFILE_SYSCALL(pdpi->profiler(), pdpi->modelEvaluator(), name, 0u);

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
NAMED_SYSCALL(fill_vec, name, args, num_args, refs, crefs, returnValue, c)
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

        share_vec<T> & dest = *static_cast<share_vec<T>*>(destHandle);
        const share_vec<T> & src = *static_cast<share_vec<T>*>(srcHandle);

        for (size_t i = 0; i < dest.size(); ++i)
            dest[i] = src[0u];

        PROFILE_SYSCALL(pdpi->profiler(), pdpi->modelEvaluator(), name,
                        dest.size());

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
NAMED_SYSCALL(assign_vec, name, args, num_args, refs, crefs, returnValue, c)
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

        const share_vec<T> & src = *static_cast<share_vec<T>*>(srcHandle);
        share_vec<T> & dest = *static_cast<share_vec<T>*>(destHandle);

        if (src.size() != dest.size())
            return SHAREMIND_MODULE_API_0x1_INVALID_CALL;

        dest.assign(src);

        PROFILE_SYSCALL(pdpi->profiler(), pdpi->modelEvaluator(), name,
                        dest.size());

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
NAMED_SYSCALL(declassify_vec, name, args, num_args, refs, crefs, returnValue, c)
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

        const share_vec<T> & src = *static_cast<share_vec<T>*>(srcHandle);
        if (refs[0u].size < sizeof(T) * src.size()) {
            return SHAREMIND_MODULE_API_0x1_GENERAL_ERROR;
        }

        for (size_t i = 0u; i < src.size(); ++i)
            dest[i] = src[i];

        PROFILE_SYSCALL(pdpi->profiler(), pdpi->modelEvaluator(), name,
                        src.size());

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
NAMED_SYSCALL(classify_vec, name, args, num_args, refs, crefs, returnValue, c)
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

        share_vec<T> & dest = *static_cast<share_vec<T>*>(destHandle);
        immutable_vm_vec<T> src(crefs[0u]);
        if (sizeof(T) * dest.size() > crefs[0u].size) {
            return SHAREMIND_MODULE_API_0x1_GENERAL_ERROR;
        }

        for (size_t i = 0u; i < dest.size(); ++i)
            dest[i] = src[i];

        PROFILE_SYSCALL(pdpi->profiler(), pdpi->modelEvaluator(), name,
                        src.size());

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
NAMED_SYSCALL(delete_vec, name, args, num_args, refs, crefs, returnValue, c)
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

        share_vec<T> * vec = static_cast<share_vec<T>*>(vecHandle);
        const size_t vsize = vec->size();
        pdpi->freeRegisteredVector(vec);

        PROFILE_SYSCALL(pdpi->profiler(), pdpi->modelEvaluator(), name, vsize);

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
NAMED_SYSCALL(load_vec, name, args, num_args, refs, crefs, returnValue, c)
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

        const share_vec<T> & src = *static_cast<share_vec<T>*>(srcHandle);
        share_vec<T> & dest = *static_cast<share_vec<T>*>(destHandle);

        if (dest.empty() || !(index < src.size())) {
            return SHAREMIND_MODULE_API_0x1_GENERAL_ERROR;
        }

        dest[0u] = src[index];

        PROFILE_SYSCALL(pdpi->profiler(), pdpi->modelEvaluator(), name, 1u);

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
NAMED_SYSCALL(store_vec, name, args, num_args, refs, crefs, returnValue, c)
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

        const share_vec<T> & src = *static_cast<share_vec<T>*>(srcHandle);
        share_vec<T> & dest = *static_cast<share_vec<T>*>(destHandle);

        if (src.empty() || !(index < dest.size())) {
            return SHAREMIND_MODULE_API_0x1_GENERAL_ERROR;
        }

        dest[index] = src[0u];

        PROFILE_SYSCALL(pdpi->profiler(), pdpi->modelEvaluator(), name, 1u);

        return SHAREMIND_MODULE_API_0x1_OK;
    } catch (...) {
        return catchModuleApiErrors ();
    }
}

} /* namespace sharemind */

#endif /* MOD_SHARED3P_EMU_SYSCALLS_CORESYSCALLS_H */
