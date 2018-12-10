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

#ifndef MOD_SHARED3P_EMU_SYSCALLS_MATRIXSHUFFLINGSYSCALLS_H
#define MOD_SHARED3P_EMU_SYSCALLS_MATRIXSHUFFLINGSYSCALLS_H

#include <sharemind/module-apis/api_0x1.h>
#include "Common.h"
#include "../Protocols/MatrixShufflingProtocol.h"
#include "../Shared3pPDPI.h"
#include "../Shared3pVector.h"

namespace sharemind {

/**
 * SysCall: vector_shuffle<T>
 * Args:
 *      0) uint64[0u]     pd index
 *      1) p[0u]          input/output handle
 * Precondition:
 *      Input/output handle is valid vector of type T.
 */
template <typename T, bool NeedKey, bool InverseShuffle>
NAMED_SYSCALL(vector_shuffle, name, args, num_args, refs, crefs, returnValue, c)
{
    static_assert(!InverseShuffle || NeedKey, "");

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

        ShareVec<T> & vec = * static_cast<ShareVec<T> *>(vectorHandle);

        MatrixShufflingProtocol msp(*pdpi);
        if (NeedKey) {
            void * const randHandle = args[2u].p[0u];
            if (!pdpi->isValidHandle<s3p_uint8_t>(randHandle))
                return SHAREMIND_MODULE_API_0x1_INVALID_CALL;

            const ShareVec<s3p_uint8_t> & rand = * static_cast<ShareVec<s3p_uint8_t> *>(randHandle);
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

        PROFILE_SYSCALL(c, pdpi->modelEvaluator(), name,
                        vec.size());

        return SHAREMIND_MODULE_API_0x1_OK;
    } catch (...) {
        return catchModuleApiErrors ();
    }
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
NAMED_SYSCALL(matrix_shuffle, name, args, num_args, refs, crefs, returnValue, c)
{
    static_assert(!InverseShuffle || NeedKey, "");

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

        ShareVec<s3p_uint8_t> const * rand = nullptr;
        if (NeedKey) {
            void * const randHandle = args[3u].p[0u];
            if (!pdpi->isValidHandle<s3p_uint8_t>(randHandle))
                return SHAREMIND_MODULE_API_0x1_INVALID_CALL;

            rand = static_cast<ShareVec<s3p_uint8_t> *>(randHandle);
            if (rand->size() != 32u) {
                return SHAREMIND_MODULE_API_0x1_INVALID_CALL;
            }
        }

        ShareVec<T> & matrix = * static_cast<ShareVec<T> *>(matrixHandle);
        if (matrix.empty())
            return SHAREMIND_MODULE_API_0x1_OK;

        const uint64_t elementsPerRow = args[2u].uint64[0u];
        if (elementsPerRow <= 0u)
            return SHAREMIND_MODULE_API_0x1_INVALID_CALL;

        if (matrix.size() == elementsPerRow)
            return SHAREMIND_MODULE_API_0x1_OK;

        if (matrix.size() % elementsPerRow != 0u)
            return SHAREMIND_MODULE_API_0x1_INVALID_CALL;

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

        PROFILE_SYSCALL(c, pdpi->modelEvaluator(), name,
                        matrix.size());

        return SHAREMIND_MODULE_API_0x1_OK;
    } catch (...) {
        return catchModuleApiErrors ();
    }
    return SHAREMIND_MODULE_API_0x1_GENERAL_ERROR;
}

} /* namespace sharemind */

#endif /* MOD_SHARED3P_EMU_SYSCALLS_MATRIXSHUFFLINGSYSCALLS_H */
