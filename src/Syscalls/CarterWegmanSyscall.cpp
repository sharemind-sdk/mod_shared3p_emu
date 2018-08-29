/*
 * Copyright (C) 2018 Cybernetica
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

#include "CarterWegmanSyscall.h"

#include "../Shared3pPDPI.h"
#include "../Shared3pVector.h"

#include <sharemind/VmVector.h>

namespace sharemind {

bool CarterWegman128Protocol(const ShareVec<s3p_xor_uint8_t>& keyVec,
                             const ShareVec<s3p_xor_uint8_t>& dataVec,
                             ShareVec<s3p_xor_uint8_t>& resultVec)
{
    size_t rowLen = keyVec.size() / 128;
    size_t rows = dataVec.size() / rowLen;

    for (size_t row = 0; row < rows; ++row) {
        ShareVec<s3p_xor_uint8_t> hash(16);

        for (size_t i = 0; i < 128; ++i) {
            uint8_t bit = 0;

            for (size_t byteI = 0; byteI < rowLen; ++byteI) {
                uint8_t byte = dataVec[rowLen * row + byteI];
                uint8_t x = byte & keyVec[i * rowLen + byteI];

                // Sum x bits
                for (size_t j = 0; j < 8; ++j) {
                    // Mask other bits
                    uint8_t tmp = x & (static_cast<uint8_t>(1) << static_cast<uint8_t>(j));
                    // Move to the right
                    tmp = tmp >> static_cast<uint8_t>(j);
                    // Xor with bit
                    bit = bit ^ tmp;
                }
            }

            size_t resByte = i / 8;
            size_t resBit = i % 8;
            hash[resByte] = hash[resByte] | (bit << (7 - resBit));
        }

        // Assign hash of this row to resultVec
        for (size_t i = 0; i < 16; ++i) {
            resultVec[row * 16 + i] = hash[i];
        }
    }

    return true;
}

NAMED_SYSCALL(carter_wegman128, name, args, num_args, refs, crefs, returnValue, c)
{
    (void) name;

    if (num_args != 4)
        return SHAREMIND_MODULE_API_0x1_INVALID_CALL;

    if (returnValue)
        return SHAREMIND_MODULE_API_0x1_INVALID_CALL;

    if (refs)
        return SHAREMIND_MODULE_API_0x1_INVALID_CALL;

    if (crefs)
        return SHAREMIND_MODULE_API_0x1_INVALID_CALL;

    VMHandles handles;
    if (!handles.get(c, args)) {
        return SHAREMIND_MODULE_API_0x1_INVALID_CALL;
    }

    try {
        Shared3pPDPI * pdpi = static_cast<Shared3pPDPI *>(handles.pdpiHandle);

        void* const keyHandle = args[1].p[0];
        void* dataHandle = args[2].p[0];
        void* resultHandle = args[3].p[0];

        if (! pdpi->isValidHandle<s3p_xor_uint8_t>(keyHandle) ||
            ! pdpi->isValidHandle<s3p_xor_uint8_t>(dataHandle) ||
            ! pdpi->isValidHandle<s3p_xor_uint8_t>(resultHandle))
        {
            return SHAREMIND_MODULE_API_0x1_GENERAL_ERROR;
        }

        const ShareVec<s3p_xor_uint8_t>& keyVec =
            *static_cast<const ShareVec<s3p_xor_uint8_t> *>(keyHandle);

        const ShareVec<s3p_xor_uint8_t>& dataVec =
            *static_cast<const ShareVec<s3p_xor_uint8_t> *>(dataHandle);

        ShareVec<s3p_xor_uint8_t>& resultVec =
            *static_cast<ShareVec<s3p_xor_uint8_t> *>(resultHandle);

        if (!CarterWegman128Protocol(keyVec, dataVec, resultVec)) {
            return SHAREMIND_MODULE_API_0x1_GENERAL_ERROR;
        }

        return SHAREMIND_MODULE_API_0x1_OK;
    } catch (...) {
        return catchModuleApiErrors ();
    }
}

} /* namespace sharemind */
