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

#ifndef MOD_SHARED3P_EMU_SYSCALLS_FIXEDPOINT_H
#define MOD_SHARED3P_EMU_SYSCALLS_FIXEDPOINT_H

#include <sharemind/module-apis/api_0x1.h>
#include "Common.h"
#include "../Shared3pPDPI.h"
#include "../Shared3pVector.h"
#include "../Protocols/SoftFloatUtility.h"
#include "../Protocols/FixedPoint.h"


namespace sharemind {

template <typename F, typename U>
NAMED_SYSCALL(float_to_fix, name, args, num_args, refs, crefs, returnValue, c)
{
    (void) name;

    VMHandles handles;
    if (!SyscallArgs<4u>::check(num_args, refs, crefs, returnValue) ||
        !handles.get(c, args)) {
        return SHAREMIND_MODULE_API_0x1_INVALID_CALL;
    }

    try {
        Shared3pPDPI* pdpi = static_cast<Shared3pPDPI*>(handles.pdpiHandle);
        void* paramHandle = args[1u].p[0u];
        void* resultHandle = args[2u].p[0u];
        uint64_t radix_point = args[3u].uint64[0u];

        if (!pdpi->isValidHandle<F>(paramHandle) ||
            !pdpi->isValidHandle<U>(resultHandle)) {
            return SHAREMIND_MODULE_API_0x1_GENERAL_ERROR;
        }

        const ShareVec<F>& param = *static_cast<ShareVec<F>*>(paramHandle);
        ShareVec<U>& result = *static_cast<ShareVec<U>*>(resultHandle);

        FloatToFixProtocol().invoke(param, result, radix_point);

        return SHAREMIND_MODULE_API_0x1_OK;
    } catch (const std::bad_alloc &) {
        return SHAREMIND_MODULE_API_0x1_OUT_OF_MEMORY;
    } catch (...) {
        return SHAREMIND_MODULE_API_0x1_GENERAL_ERROR;
    }
}

template <typename U, typename F>
NAMED_SYSCALL(fix_to_float, name, args, num_args, refs, crefs, returnValue, c)
{
    (void) name;

    VMHandles handles;
    if (!SyscallArgs<4u>::check(num_args, refs, crefs, returnValue) ||
        !handles.get(c, args)) {
        return SHAREMIND_MODULE_API_0x1_INVALID_CALL;
    }

    try {
        Shared3pPDPI* pdpi = static_cast<Shared3pPDPI *>(handles.pdpiHandle);
        void* paramHandle = args[1u].p[0u];
        void* resultHandle = args[2u].p[0u];
        uint64_t radix_point = args[3u].uint64[0u];

        if (!pdpi->isValidHandle<U>(paramHandle) ||
            !pdpi->isValidHandle<F>(resultHandle)) {
            return SHAREMIND_MODULE_API_0x1_GENERAL_ERROR;
        }

        const ShareVec<U>& param = *static_cast<ShareVec<U>*>(paramHandle);
        ShareVec<F>& result = *static_cast<ShareVec<F>*>(resultHandle);

        FixToFloatProtocol().invoke(param, result, radix_point);

        return SHAREMIND_MODULE_API_0x1_OK;
    } catch (const std::bad_alloc &) {
        return SHAREMIND_MODULE_API_0x1_OUT_OF_MEMORY;
    } catch (...) {
        return SHAREMIND_MODULE_API_0x1_GENERAL_ERROR;
    }
}

} /* namespace sharemind */

#endif // MOD_SHARED3P_EMU_SYSCALLS_FIXEDPOINT_H
