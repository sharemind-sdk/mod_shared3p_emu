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

#include <sharemind/libmodapi/api_0x1.h>
#include "Common.h"
#include "../Protocols/MatrixMultiplicationProtocol.h"
#include "../Shared3pPDPI.h"
#include "../Shared3pVector.h"

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

        if (! MatrixMultiplicationProtocol().invoke(mat1, mat2, dim1, dim2, dim3, result))
            return SHAREMIND_MODULE_API_0x1_GENERAL_ERROR;

        PROFILE_SYSCALL(pdpi->profiler(), pdpi->modelEvaluator(), name, l1 + l2);

        return SHAREMIND_MODULE_API_0x1_OK;
    } catch (...) {
        return catchModuleApiErrors ();
    }
}

} /* namespace sharemind */

#endif /* MOD_SHARED3P_EMU_SYSCALLS_MATRIXMULTIPLICATIONSYSCALLS_H */
