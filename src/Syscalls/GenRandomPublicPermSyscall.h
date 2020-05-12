/*
 * This file is a part of the Sharemind framework.
 * Copyright (C) Cybernetica AS
 *
 * All rights are reserved. Reproduction in whole or part is prohibited
 * without the written consent of the copyright owner. The usage of this
 * code is subject to the appropriate license agreement.
 */

#ifndef MOD_SHARED3P_EMU_GENRANDOMPUBLICPERMSYSCALL_H
#define MOD_SHARED3P_EMU_GENRANDOMPUBLICPERMSYSCALL_H

#include <sharemind/module-apis/api_0x1.h>
#include "Common.h"

namespace sharemind {

NAMED_SYSCALL(gen_random_public_perm, name, args, num_args, refs, crefs, returnValue, c)
{
    (void) name;

    VMHandles handles;
    if (!SyscallArgs<1, false, 1, 0>::check(num_args, refs, crefs, returnValue) ||
        !handles.get(c, args))
    {
        return SHAREMIND_MODULE_API_0x1_INVALID_CALL;
    }

    try {
        MutableVmVec<s3p_uint32_t> ref(refs[0]);
        std::random_device rd;
        std::default_random_engine rng;
        rng.seed(rd());
        std::iota(ref.begin(), ref.end(), 0u);
        std::shuffle(ref.begin(), ref.end(), rng);
        return SHAREMIND_MODULE_API_0x1_OK;
    } catch (...) {
        return catchModuleApiErrors ();
    }
}

} /* namespace sharemind { */

#endif // MOD_SHARED3P_EMU_GENRANDOMPUBLICPERMSYSCALL_H
