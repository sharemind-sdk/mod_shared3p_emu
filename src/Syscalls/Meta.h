/*
 * This file is a part of the Sharemind framework.
 * Copyright (C) Cybernetica AS
 *
 * All rights are reserved. Reproduction in whole or part is prohibited
 * without the written consent of the copyright owner. The usage of this
 * code is subject to the appropriate license agreement.
 */

#ifndef MOD_ADDITIVE3P_SYSCALLS_META_H
#define MOD_ADDITIVE3P_SYSCALLS_META_H

#include <sharemind/libmodapi/api_0x1.h>

#include "Common.h"
#include "../Shared3pPDPI.h"
#include "../VMReferences.h"

/**
 * Meta-syscalls for many common cases.
 */


namespace sharemind {

/**
 * SysCall: binary_vec<T1, T2, T3, Protocol>
 * Args:
 *      0) uint64[0]     pd index
 *      1) p[0]          LHS handle
 *      2) p[0]          RHS handle
 *      2) p[0]          output handle
 * Precondition:
 *      LHS handle is a vector of type T1.
 *      RHS handle is a vector of type T2.
 *      Output handle is a vector of type T3.
 */
template <typename T1, typename T2, typename T3, typename Protocol>
SHAREMIND_MODULE_API_0x1_SYSCALL(binary_vec,
                                 args, num_args, refs, crefs,
                                 returnValue, c)
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

        if (!pdpi->isValidHandle<T1>(lhsHandle) ||
                !pdpi->isValidHandle<T2>(rhsHandle) ||
                !pdpi->isValidHandle<T3>(resultHandle))
        {
            return SHAREMIND_MODULE_API_0x1_GENERAL_ERROR;
        }

        const s3p_vec<T1> & param1 = *static_cast<s3p_vec<T1>*>(lhsHandle);
        const s3p_vec<T2> & param2 = *static_cast<s3p_vec<T2>*>(rhsHandle);
        s3p_vec<T3> & result = *static_cast<s3p_vec<T3>*>(resultHandle);

        Protocol protocol(*pdpi);
        if (!protocol.invoke(param1, param2, result, typename value_traits<T1>::value_category()))
            return SHAREMIND_MODULE_API_0x1_GENERAL_ERROR;

        return SHAREMIND_MODULE_API_0x1_OK;
    } catch (...) {
        return catchModuleApiErrors();
    }
}

/**
 * SysCall: binary_public_vec<T1, T2, T3, Protocol>
 * Args:
 *      0) uint64[0]     pd index
 *      1) p[0]          LHS handle
 *      2) p[0]          RHS handle
 *      2) p[0]          output handle
 * Precondition:
 *      LHS handle is a vector of type T1.
 *      RHS handle is a vector of type T2.
 *      Output handle is a vector of type T3.
 */
template <typename T1, typename T2, typename T3, typename Protocol>
SHAREMIND_MODULE_API_0x1_SYSCALL(binary_public_vec,
                                 args, num_args, refs, crefs,
                                 returnValue, c)
{
    VMHandles handles;
    if (!SyscallArgs<3, false, 0u, 1u>::check(num_args, refs, crefs, returnValue) ||
            !handles.get(c, args))
    {
        return SHAREMIND_MODULE_API_0x1_INVALID_CALL;
    }

    try {
        Shared3pPDPI * const pdpi = static_cast<Shared3pPDPI*>(handles.pdpiHandle);

        void * const lhsHandle = args[1u].p[0u];
        void * const resultHandle = args[2u].p[0u];

        if (!pdpi->isValidHandle<T1>(lhsHandle) ||
                !pdpi->isValidHandle<T3>(resultHandle))
        {
            return SHAREMIND_MODULE_API_0x1_GENERAL_ERROR;
        }

        const s3p_vec<T1> & param1 = *static_cast<s3p_vec<T1>*>(lhsHandle);
        const immutable_vm_vec<T2> param2(crefs[0u]);
        s3p_vec<T3> & result = *static_cast<s3p_vec<T3>*>(resultHandle);

        if (param1.size() <= crefs[0u].size / sizeof(T2) ||
                param1.size() != result.size())
            return SHAREMIND_MODULE_API_0x1_INVALID_CALL;

        Protocol protocol(*pdpi);
        if (!protocol.invoke(param1, param2, result, typename value_traits<T1>::value_category()))
            return SHAREMIND_MODULE_API_0x1_GENERAL_ERROR;

        return SHAREMIND_MODULE_API_0x1_OK;
    } catch (...) {
        return catchModuleApiErrors();
    }
}

/**
 * SysCall: binary_arith_vec<T, Protocol>
 * Args:
 *      0) uint64[0]     pd index
 *      1) p[0]          vector handle (lhs)
 *      2) p[0]          vector handle (rhs)
 *      3) p[0]          output handle
 * Precondition:
 *      All handles are valid vectors of type T.
 */
template <typename T, typename Protocol>
SHAREMIND_MODULE_API_0x1_SYSCALL(binary_arith_vec,
                                 args, num_args, refs, crefs,
                                 returnValue, c)
{
    return binary_vec<T, T, T, Protocol>(args, num_args, refs, crefs, returnValue, c);
}

/**
 * SysCall: binary_arith_public_vec<T, Protocol>
 * Args:
 *      0) uint64[0]     pd index
 *      1) p[0]          vector handle (lhs)
 *      2) p[0]          vector handle (rhs)
 *      3) p[0]          output handle
 * Precondition:
 *      All handles are valid vectors of type T.
 */
template <typename T, typename Protocol>
SHAREMIND_MODULE_API_0x1_SYSCALL(binary_arith_public_vec,
                                 args, num_args, refs, crefs,
                                 returnValue, c)
{
    return binary_public_vec<T, T, T, Protocol>(args, num_args, refs, crefs, returnValue, c);
}

/**
 * SysCall: unary_vec<T, L, Protocol>
 * Args:
 *      0) uint64[0]     pd index
 *      1) p[0]          input handle
 *      2) p[0]          output handle
 * Precondition:
 *      Input handle is a vector of type T.
 *      Output handle is a vector of type L.
 */
template <typename T, typename L, typename Protocol>
SHAREMIND_MODULE_API_0x1_SYSCALL(unary_vec,
                                 args, num_args, refs, crefs,
                                 returnValue, c)
{
    VMHandles handles;
    if (!SyscallArgs<3>::check(num_args, refs, crefs, returnValue) ||
            !handles.get(c, args))
    {
        return SHAREMIND_MODULE_API_0x1_INVALID_CALL;
    }

    try {
        Shared3pPDPI * const pdpi = static_cast<Shared3pPDPI *>(handles.pdpiHandle);

        void * const paramHandle = args[1u].p[0u];
        void * const resultHandle = args[2u].p[0u];

        if (!pdpi->isValidHandle<T>(paramHandle) ||
                !pdpi->isValidHandle<L>(resultHandle))
        {
            return SHAREMIND_MODULE_API_0x1_GENERAL_ERROR;
        }

        const s3p_vec<T>& param = *static_cast<s3p_vec<T>*>(paramHandle);
        s3p_vec<L>& result = *static_cast<s3p_vec<L>*>(resultHandle);

        Protocol protocol(*pdpi);
        if (!protocol.invoke(param, result, typename value_traits<T>::value_category()))
            return SHAREMIND_MODULE_API_0x1_GENERAL_ERROR;

        return SHAREMIND_MODULE_API_0x1_OK;
    } catch (...) {
        return catchModuleApiErrors();
    }
}

/**
 * SysCall: unary_arith_vec<T, Protocol>
 * Args:
 *      0) uint64[0]     pd index
 *      1) p[0]          input handle
 *      2) p[0]          output handle
 * Precondition:
 *      Both handles are valid vectors of type T.
 */
template <typename T, typename Protocol>
SHAREMIND_MODULE_API_0x1_SYSCALL(unary_arith_vec, args, num_args, refs, crefs, returnValue, c)
{
    return unary_vec<T, T, Protocol>(args, num_args, refs, crefs, returnValue, c);
}

/**
 * SysCall: nullary_vec<T, Protocol>
 * Args:
 *      0) uint64[0]     pd index
 *      1) p[0]          output handle
 * Precondition:
 *      Output handle is valid vectors of type T.
 */
template <typename T, typename Protocol>
SHAREMIND_MODULE_API_0x1_SYSCALL(nullary_vec,
                                 args, num_args, refs, crefs,
                                 returnValue, c)
{
    VMHandles handles;
    if (!SyscallArgs<2>::check(num_args, refs, crefs, returnValue) ||
            !handles.get(c, args))
    {
        return SHAREMIND_MODULE_API_0x1_INVALID_CALL;
    }

    try {
        Shared3pPDPI * const pdpi = static_cast<Shared3pPDPI*>(handles.pdpiHandle);

        void * const resultHandle = args[1u].p[0u];

        if (!pdpi->isValidHandle<T>(resultHandle)) {
            return SHAREMIND_MODULE_API_0x1_GENERAL_ERROR;
        }

        s3p_vec<T> & result = *static_cast<s3p_vec<T>*>(resultHandle);

        if (!Protocol(*pdpi).invoke(result))
            return SHAREMIND_MODULE_API_0x1_GENERAL_ERROR;

        return SHAREMIND_MODULE_API_0x1_OK;
    } catch (...) {
        return catchModuleApiErrors();
    }
}

} /* namespace sharemind */

#endif /* MOD_ADDITIVE3P_SYSCALLS_META_H */
