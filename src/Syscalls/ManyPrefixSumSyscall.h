/*
 * This file is a part of the Sharemind framework.
 * Copyright (C) Cybernetica AS
 *
 * All rights are reserved. Reproduction in whole or part is prohibited
 * without the written consent of the copyright owner. The usage of this
 * code is subject to the appropriate license agreement.
 */

#ifndef MOD_SHARED3P_EMU_MANYPREFIXSUMSYSCALL_H
#define MOD_SHARED3P_EMU_MANYPREFIXSUMSYSCALL_H

#include "../Protocols/PrefixSumProtocols.h"
#include "Common.h"
#include <sharemind/module-apis/api_0x1.h>

namespace sharemind {

template <typename T, bool Direction>
NAMED_SYSCALL(many_prefix_sum, name, args, num_args, refs, crefs, returnValue, c)
{
    (void) name;

    VMHandles handles;
    if (! SyscallArgs<2, false, 0, 3>::check(num_args, refs, crefs, returnValue) ||
        ! handles.get(c, args)) {
        return SHAREMIND_MODULE_API_0x1_INVALID_CALL;
    }

    try {
        Shared3pPDPI* pdpi = static_cast<Shared3pPDPI*>(handles.pdpiHandle);

        void* inOutHandle = args[1].p[0];
        if (! pdpi->isValidHandle<T>(inOutHandle)) {
            return SHAREMIND_MODULE_API_0x1_GENERAL_ERROR;
        }

        ShareVec<T> & inOut = *static_cast<ShareVec<T>*>(inOutHandle);
        ImmutableVmVec<s3p_uint32_t> start(crefs[0]);
        ImmutableVmVec<s3p_uint32_t> step(crefs[1]);
        ImmutableVmVec<s3p_uint32_t> count(crefs[2]);

        if (! MatPrefixSumProtocol(*pdpi).invoke(inOut, start, step, count, Direction)) {
            return SHAREMIND_MODULE_API_0x1_GENERAL_ERROR;
        }

        return SHAREMIND_MODULE_API_0x1_OK;
    } catch (...) {
        return SHAREMIND_MODULE_API_0x1_GENERAL_ERROR;
    }
}

} /* namespace sharemind { */

#endif // MOD_SHARED3P_EMU_MANYPREFIXSUMSYSCALL_H
