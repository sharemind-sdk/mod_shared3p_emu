/*
 * Copyright (C) 2017 Cybernetica
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

#include "SortingSyscalls.h"

#include "../Protocols/SortingProtocol.h"
#include "../Shared3pPDPI.h"
#include "../Shared3pVector.h"

#include <sharemind/VmVector.h>

namespace sharemind {

template<typename T>
NAMED_SYSCALL(stable_sort, name, args, num_args, refs, crefs, returnValue, c)
{
    bool haveBlocks = false;

    if (num_args != 4)
        return SHAREMIND_MODULE_API_0x1_INVALID_CALL;

    if (returnValue)
        return SHAREMIND_MODULE_API_0x1_INVALID_CALL;

    if (!refs || !refs[0].pData || refs[1].pData)
        return SHAREMIND_MODULE_API_0x1_INVALID_CALL;

    if (crefs) {
        haveBlocks = true;
        if (!crefs[0].pData || crefs[1].pData)
            return SHAREMIND_MODULE_API_0x1_INVALID_CALL;
    }

    VMHandles handles;
    if (!handles.get(c, args)) {
        return SHAREMIND_MODULE_API_0x1_INVALID_CALL;
    }

    try {
        Shared3pPDPI * pdpi = static_cast<Shared3pPDPI *>(handles.pdpiHandle);
        void* const vecHandle = args[1].p[0];
        void* const indexHandle = args[2].p[0];
        if (! pdpi->isValidHandle<T>(vecHandle) ||
            ! pdpi->isValidHandle<s3p_xor_uint64_t>(indexHandle)) {
            return SHAREMIND_MODULE_API_0x1_GENERAL_ERROR;
        }

        const auto& vec = *static_cast<const ShareVec<T> *>(vecHandle);
        const auto& indices = *static_cast<const ShareVec<s3p_xor_uint64_t> *>(indexHandle);
        MutableVmVec<s3p_uint64_t> perm (refs[0]);

        if (vec.size() != indices.size() || vec.size() != perm.size()) {
            return SHAREMIND_MODULE_API_0x1_GENERAL_ERROR;
        }

        for (size_t i = 0; i < perm.size(); ++ i) {
            if (perm[i] >= vec.size()) {
                return SHAREMIND_MODULE_API_0x1_GENERAL_ERROR;
            }
        }

        bool ascending = static_cast<bool>(args[3].uint8[0]);

        if (haveBlocks) {
            ImmutableVmVec<s3p_uint64_t> blocks (crefs[0]);
            if (! StableSortingProtocol(*pdpi).invoke(vec, indices, blocks, perm, ascending)) {
                return SHAREMIND_MODULE_API_0x1_GENERAL_ERROR;
            }
        }
        else {
            if (! StableSortingProtocol(*pdpi).invoke(vec, indices, perm, ascending)) {
                return SHAREMIND_MODULE_API_0x1_GENERAL_ERROR;
            }
        }

        PROFILE_SYSCALL(pdpi->profiler(), pdpi->modelEvaluator(), name, vec.size());

        return SHAREMIND_MODULE_API_0x1_OK;
    } catch (...) {
        return catchModuleApiErrors ();
    }
}

/*
 * Explicitly instantiate system calls:
 */

template NAMED_SYSCALL(stable_sort<s3p_xor_uint8_t>,
                       name, args, num_args, refs, crefs, returnValue, c);
template NAMED_SYSCALL(stable_sort<s3p_xor_uint16_t>,
                       name, args, num_args, refs, crefs, returnValue, c);
template NAMED_SYSCALL(stable_sort<s3p_xor_uint32_t>,
                       name, args, num_args, refs, crefs, returnValue, c);
template NAMED_SYSCALL(stable_sort<s3p_xor_uint64_t>,
                       name, args, num_args, refs, crefs, returnValue, c);
template NAMED_SYSCALL(stable_sort<s3p_uint8_t>,
                       name, args, num_args, refs, crefs, returnValue, c);
template NAMED_SYSCALL(stable_sort<s3p_uint16_t>,
                       name, args, num_args, refs, crefs, returnValue, c);
template NAMED_SYSCALL(stable_sort<s3p_uint32_t>,
                       name, args, num_args, refs, crefs, returnValue, c);
template NAMED_SYSCALL(stable_sort<s3p_uint64_t>,
                       name, args, num_args, refs, crefs, returnValue, c);
template NAMED_SYSCALL(stable_sort<s3p_int8_t>,
                       name, args, num_args, refs, crefs, returnValue, c);
template NAMED_SYSCALL(stable_sort<s3p_int16_t>,
                       name, args, num_args, refs, crefs, returnValue, c);
template NAMED_SYSCALL(stable_sort<s3p_int32_t>,
                       name, args, num_args, refs, crefs, returnValue, c);
template NAMED_SYSCALL(stable_sort<s3p_int64_t>,
                       name, args, num_args, refs, crefs, returnValue, c);

} /* namespace sharemind */
