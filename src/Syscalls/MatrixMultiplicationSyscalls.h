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

#ifndef MOD_SHARED3P_EMU_SYSCALLS_MATRIXMULTIPLICATIONSYSCALLS_H
#define MOD_SHARED3P_EMU_SYSCALLS_MATRIXMULTIPLICATIONSYSCALLS_H

#include <sharemind/module-apis/api_0x1.h>
#include "Common.h"
#include "../Protocols/MatrixMultiplicationProtocol.h"
#include "../Shared3pPDPI.h"
#include "../Shared3pValueTraits.h"

namespace sharemind {

/**
 * SysCall: mat_mult<T>
 */
template <typename T>
NAMED_SYSCALL(mat_mult, name, args, num_args, refs, crefs, returnValue, c)
{
    VMHandles handles;
    if (!SyscallArgs<4, false, 0, 3>::check(num_args, refs, crefs, returnValue) ||
        !handles.get(c, args))
    {
        return SHAREMIND_MODULE_API_0x1_INVALID_CALL;
    }

    try {
        Shared3pPDPI * const pdpi = static_cast<Shared3pPDPI*>(handles.pdpiHandle);

        void* mat1Handle = args[1].p[0];
        void* mat2Handle = args[2].p[0];
        void* resultHandle = args[3].p[0];

        if (! pdpi->isValidHandle<T>(mat1Handle) ||
            ! pdpi->isValidHandle<T>(mat2Handle) ||
            ! pdpi->isValidHandle<T>(resultHandle)) {
            return SHAREMIND_MODULE_API_0x1_GENERAL_ERROR;
        }

        const ShareVec<T>& mat1 = *static_cast<const ShareVec<T> *>(mat1Handle);
        const ShareVec<T>& mat2 = *static_cast<const ShareVec<T> *>(mat2Handle);
        ShareVec<T>& result = *static_cast<ShareVec<T> *>(resultHandle);

        ImmutableVmVec<s3p_uint64_t> dim1 (crefs[0]);
        ImmutableVmVec<s3p_uint64_t> dim2 (crefs[1]);
        ImmutableVmVec<s3p_uint64_t> dim3 (crefs[2]);

        if (dim1.size() != dim2.size() || dim1.size() != dim3.size())
            return SHAREMIND_MODULE_API_0x1_GENERAL_ERROR;

        uint64_t l1 = 0;
        uint64_t l2 = 0;
        uint64_t l3 = 0;
        for (size_t i = 0; i < dim1.size(); ++ i) {
            l1 += dim1[i] * dim2[i];
            l2 += dim2[i] * dim3[i];
            l3 += dim1[i] * dim3[i];
        }

        if (mat1.size() != l1 || mat2.size() != l2 || result.size() != l3)
            return SHAREMIND_MODULE_API_0x1_GENERAL_ERROR;

        if (! MatrixMultiplicationProtocol().invoke(mat1, mat2, dim1, dim2, dim3, result,
                                                    typename ValueTraits<T>::value_category{}))
            return SHAREMIND_MODULE_API_0x1_GENERAL_ERROR;

        PROFILE_SYSCALL(c, pdpi->modelEvaluator(), name, l1 + l2);

        return SHAREMIND_MODULE_API_0x1_OK;
    } catch (...) {
        return catchModuleApiErrors ();
    }
}

/**
 * SysCall: mat_mult<T>
 */
template <typename T>
NAMED_SYSCALL(fix_mat_mult, name, args, num_args, refs, crefs, returnValue, c)
{
    VMHandles handles;
    if (!SyscallArgs<4, false, 0, 3>::check(num_args, refs, crefs, returnValue) ||
        !handles.get(c, args))
    {
        return SHAREMIND_MODULE_API_0x1_INVALID_CALL;
    }

    try {
        Shared3pPDPI * const pdpi = static_cast<Shared3pPDPI*>(handles.pdpiHandle);

        void* mat1Handle = args[1].p[0];
        void* mat2Handle = args[2].p[0];
        void* resultHandle = args[3].p[0];

        if (! pdpi->isValidHandle<T>(mat1Handle) ||
            ! pdpi->isValidHandle<T>(mat2Handle) ||
            ! pdpi->isValidHandle<T>(resultHandle)) {
            return SHAREMIND_MODULE_API_0x1_GENERAL_ERROR;
        }

        const ShareVec<T>& mat1 = *static_cast<const ShareVec<T> *>(mat1Handle);
        const ShareVec<T>& mat2 = *static_cast<const ShareVec<T> *>(mat2Handle);
        ShareVec<T>& result = *static_cast<ShareVec<T> *>(resultHandle);

        ImmutableVmVec<s3p_uint64_t> dim1 (crefs[0]);
        ImmutableVmVec<s3p_uint64_t> dim2 (crefs[1]);
        ImmutableVmVec<s3p_uint64_t> dim3 (crefs[2]);

        if (dim1.size() != dim2.size() || dim1.size() != dim3.size())
            return SHAREMIND_MODULE_API_0x1_GENERAL_ERROR;

        uint64_t l1 = 0;
        uint64_t l2 = 0;
        uint64_t l3 = 0;
        for (size_t i = 0; i < dim1.size(); ++ i) {
            l1 += dim1[i] * dim2[i];
            l2 += dim2[i] * dim3[i];
            l3 += dim1[i] * dim3[i];
        }

        if (mat1.size() != l1 || mat2.size() != l2 || result.size() != l3)
            return SHAREMIND_MODULE_API_0x1_GENERAL_ERROR;

        if (! FixMatrixMultiplicationProtocol().invoke(mat1, mat2, dim1, dim2, dim3, result))
            return SHAREMIND_MODULE_API_0x1_GENERAL_ERROR;

        PROFILE_SYSCALL(c, pdpi->modelEvaluator(), name, l1 + l2);

        return SHAREMIND_MODULE_API_0x1_OK;
    } catch (...) {
        return catchModuleApiErrors ();
    }
}

/**
 * SysCall: batch_mat_mult<T>
 * Args:
 *     0) uint64[0]     pd index
 *     1) p[0]          arg1 handle
 *     2) p[0]          arg2 handle
 *     3) p[0]          result handle
 * CRefs:
 *     0) crefs[0]      N
 *     1) crefs[0]      H
 *     2) crefs[0]      K
 *     3) crefs[0]      W
 * Precondition:
 *     arg1 has shape (N, H, K), arg2 has shape (N, K, W),
 *     result has shape (N, H, W)
 */
template <typename T>
NAMED_SYSCALL(batch_mat_mult, name, args, num_args, refs, crefs, returnValue, c)
{
    VMHandles handles;
    if (!SyscallArgs<4, false, 0, 4>::check(num_args, refs, crefs, returnValue) ||
        !handles.get(c, args))
    {
        return SHAREMIND_MODULE_API_0x1_INVALID_CALL;
    }

    try {
        Shared3pPDPI * const pdpi = static_cast<Shared3pPDPI*>(handles.pdpiHandle);

        void* arg1Handle = args[1].p[0];
        void* arg2Handle = args[2].p[0];
        void* resultHandle = args[3].p[0];

        if (! pdpi->isValidHandle<T>(arg1Handle) ||
                ! pdpi->isValidHandle<T>(arg2Handle) ||
                ! pdpi->isValidHandle<T>(resultHandle)) {
            return SHAREMIND_MODULE_API_0x1_GENERAL_ERROR;
        }

        // X has shape (N, H, K), Y has shape (N, K, W), Z has shape (N, H, W)
        const ShareVec<T>& X = *static_cast<const ShareVec<T> *>(arg1Handle);
        const ShareVec<T>& Y = *static_cast<const ShareVec<T> *>(arg2Handle);
        ShareVec<T>& result = *static_cast<ShareVec<T> *>(resultHandle);

        ImmutableVmVec<s3p_uint64_t> N (crefs[0]);
        ImmutableVmVec<s3p_uint64_t> H (crefs[1]);
        ImmutableVmVec<s3p_uint64_t> K (crefs[2]);
        ImmutableVmVec<s3p_uint64_t> W (crefs[3]);

        if (N.size() != 1 || H.size() != 1 || K.size() != 1 || W.size() != 1)
            return SHAREMIND_MODULE_API_0x1_GENERAL_ERROR;

        if (! MatrixMultiplicationProtocol().invokeN(
                X, Y, result,
                N[0], H[0], K[0], W[0]))
        {
            return SHAREMIND_MODULE_API_0x1_GENERAL_ERROR;
        }

        PROFILE_SYSCALL(c, pdpi->modelEvaluator(), name, X.size() + Y.size());

        return SHAREMIND_MODULE_API_0x1_OK;
    } catch (...) {
        return catchModuleApiErrors ();
    }
}


/**
 * SysCall: fix_batch_mat_mult<T>
 * Args:
 *     0) uint64[0]     pd index
 *     1) p[0]          arg1 handle
 *     2) p[0]          arg2 handle
 *     3) p[0]          result handle
 * CRefs:
 *     0) crefs[0]      N
 *     1) crefs[0]      H
 *     2) crefs[0]      K
 *     3) crefs[0]      W
 * Precondition:
 *     arg1 has shape (N, H, K), arg2 has shape (N, K, W),
 *     result has shape (N, H, W)
 */
template <typename T>
NAMED_SYSCALL(fix_batch_mat_mult, name, args, num_args, refs, crefs, returnValue, c)
{
    VMHandles handles;
    if (!SyscallArgs<4, false, 0, 4>::check(num_args, refs, crefs, returnValue) ||
        !handles.get(c, args))
    {
        return SHAREMIND_MODULE_API_0x1_INVALID_CALL;
    }

    try {
        Shared3pPDPI * const pdpi = static_cast<Shared3pPDPI*>(handles.pdpiHandle);

        void* arg1Handle = args[1].p[0];
        void* arg2Handle = args[2].p[0];
        void* resultHandle = args[3].p[0];

        if (! pdpi->isValidHandle<T>(arg1Handle) ||
                ! pdpi->isValidHandle<T>(arg2Handle) ||
                ! pdpi->isValidHandle<T>(resultHandle)) {
            return SHAREMIND_MODULE_API_0x1_GENERAL_ERROR;
        }

        // X has shape (N, H, K), Y has shape (N, K, W), Z has shape (N, H, W)
        const ShareVec<T>& X = *static_cast<const ShareVec<T> *>(arg1Handle);
        const ShareVec<T>& Y = *static_cast<const ShareVec<T> *>(arg2Handle);
        ShareVec<T>& result = *static_cast<ShareVec<T> *>(resultHandle);

        ImmutableVmVec<s3p_uint64_t> N (crefs[0]);
        ImmutableVmVec<s3p_uint64_t> H (crefs[1]);
        ImmutableVmVec<s3p_uint64_t> K (crefs[2]);
        ImmutableVmVec<s3p_uint64_t> W (crefs[3]);

        if (N.size() != 1 || H.size() != 1 || K.size() != 1 || W.size() != 1)
            return SHAREMIND_MODULE_API_0x1_GENERAL_ERROR;

        if (! FixMatrixMultiplicationProtocol().invokeN(
                X, Y, result,
                N[0], H[0], K[0], W[0]))
        {
            return SHAREMIND_MODULE_API_0x1_GENERAL_ERROR;
        }

        PROFILE_SYSCALL(c, pdpi->modelEvaluator(), name, X.size() + Y.size());

        return SHAREMIND_MODULE_API_0x1_OK;
    } catch (...) {
        return catchModuleApiErrors ();
    }
}

} /* namespace sharemind */

#endif /* MOD_SHARED3P_EMU_SYSCALLS_MATRIXMULTIPLICATIONSYSCALLS_H */
