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

#ifndef MOD_SHARED3P_EMU_SYSCALLS_META_H
#define MOD_SHARED3P_EMU_SYSCALLS_META_H

#include <sharemind/libmodapi/api_0x1.h>
#include <sharemind/VmVector.h>

#include "Common.h"
#include "../Shared3pPDPI.h"


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
NAMED_SYSCALL(binary_vec, name, args, num_args, refs, crefs, returnValue, c)
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

        const ShareVec<T1> & param1 = *static_cast<ShareVec<T1>*>(lhsHandle);
        const ShareVec<T2> & param2 = *static_cast<ShareVec<T2>*>(rhsHandle);
        ShareVec<T3> & result = *static_cast<ShareVec<T3>*>(resultHandle);

        Protocol protocol(*pdpi);
        if (!protocol.invoke(param1, param2, result))
            return SHAREMIND_MODULE_API_0x1_GENERAL_ERROR;

        PROFILE_SYSCALL(pdpi->profiler(), pdpi->modelEvaluator(), name,
                        param1.size());

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
 *      2) p[0]          output handle
 * Precondition:
 *      LHS handle is a vector of type T1.
 *      RHS handle is a vector of type T2.
 *      Output handle is a vector of type T3.
 */
template <typename T1, typename T2, typename T3, typename Protocol>
NAMED_SYSCALL(binary_public_vec, name, args, num_args, refs, crefs, returnValue, c)
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

        const ShareVec<T1> & param1 = *static_cast<ShareVec<T1>*>(lhsHandle);
        const ImmutableVmVec<T2> param2(crefs[0u]);
        ShareVec<T3> & result = *static_cast<ShareVec<T3>*>(resultHandle);

        Protocol protocol(*pdpi);
        if (!protocol.invoke(param1, param2, result))
            return SHAREMIND_MODULE_API_0x1_GENERAL_ERROR;

        PROFILE_SYSCALL(pdpi->profiler(), pdpi->modelEvaluator(), name,
                        param1.size());

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
NAMED_SYSCALL(binary_arith_vec, name, args, num_args, refs, crefs, returnValue, c)
{
    return binary_vec<T, T, T, Protocol>(name, args, num_args, refs, crefs, returnValue, c);
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
NAMED_SYSCALL(binary_arith_public_vec, name, args, num_args, refs, crefs, returnValue, c)
{
    return binary_public_vec<T, T, T, Protocol>(name, args, num_args, refs, crefs, returnValue, c);
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
NAMED_SYSCALL(unary_vec, name, args, num_args, refs, crefs, returnValue, c)
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

        const ShareVec<T>& param = *static_cast<ShareVec<T>*>(paramHandle);
        ShareVec<L>& result = *static_cast<ShareVec<L>*>(resultHandle);

        Protocol protocol(*pdpi);
        if (!protocol.invoke(param, result))
            return SHAREMIND_MODULE_API_0x1_GENERAL_ERROR;

        PROFILE_SYSCALL(pdpi->profiler(), pdpi->modelEvaluator(), name,
                        param.size());

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
NAMED_SYSCALL(unary_arith_vec, name, args, num_args, refs, crefs, returnValue, c)
{
    return unary_vec<T, T, Protocol>(name, args, num_args, refs, crefs, returnValue, c);
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
NAMED_SYSCALL(nullary_vec, name, args, num_args, refs, crefs, returnValue, c)
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

        ShareVec<T> & result = *static_cast<ShareVec<T>*>(resultHandle);

        if (!Protocol(*pdpi).invoke(result))
            return SHAREMIND_MODULE_API_0x1_GENERAL_ERROR;

        PROFILE_SYSCALL(pdpi->profiler(), pdpi->modelEvaluator(), name,
                        result.size());

        return SHAREMIND_MODULE_API_0x1_OK;
    } catch (...) {
        return catchModuleApiErrors();
    }
}

/**
 * SysCall: ternary_vec<T1, T2, T3, T4, Protocol>
 * Args:
 *      0) uint64[0]     pd index
 *      1) p[0]          first parameter handle
 *      2) p[1]          second parameter handle
 *      3) p[1]          third parameter handle
 *      4) p[3]          output handle
 * Precondition:
 *      first parameter handle is a vector of type T1.
 *      second paremeter handle is a vector of type T2.
 *      third paremeter handle is a vector of type T3.
 *      Output handle is a vector of type T4.
 */
template <typename T1, typename T2, typename T3, typename T4, typename Protocol>
NAMED_SYSCALL(ternary_vec, name, args, num_args, refs, crefs, returnValue, c)
{
    VMHandles handles;
    if (!SyscallArgs<5>::check(num_args, refs, crefs, returnValue) ||
            !handles.get(c, args))
    {
        return SHAREMIND_MODULE_API_0x1_INVALID_CALL;
    }

    try {
        Shared3pPDPI * const pdpi = static_cast<Shared3pPDPI*>(handles.pdpiHandle);

        void * const param1Handle = args[1u].p[0u];
        void * const param2Handle = args[2u].p[0u];
        void * const param3Handle = args[3u].p[0u];
        void * const resultHandle = args[4u].p[0u];

        if (!pdpi->isValidHandle<T1>(param1Handle) ||
                !pdpi->isValidHandle<T2>(param2Handle) ||
                !pdpi->isValidHandle<T3>(param3Handle) ||
                !pdpi->isValidHandle<T4>(resultHandle))
        {
            return SHAREMIND_MODULE_API_0x1_GENERAL_ERROR;
        }

        const ShareVec<T1> & param1 = *static_cast<ShareVec<T1>*>(param1Handle);
        const ShareVec<T2> & param2 = *static_cast<ShareVec<T2>*>(param2Handle);
        const ShareVec<T3> & param3 = *static_cast<ShareVec<T3>*>(param3Handle);
        ShareVec<T4> & result = *static_cast<ShareVec<T4>*>(resultHandle);

        Protocol protocol(*pdpi);
        if (!protocol.invoke(param1, param2, param3, result))
            return SHAREMIND_MODULE_API_0x1_GENERAL_ERROR;

        PROFILE_SYSCALL(pdpi->profiler(), pdpi->modelEvaluator(), name,
                        param1.size());

        return SHAREMIND_MODULE_API_0x1_OK;
    } catch (...) {
        return catchModuleApiErrors();
    }
}

} /* namespace sharemind */

#endif /* MOD_SHARED3P_EMU_SYSCALLS_META_H */
