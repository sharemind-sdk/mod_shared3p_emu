/*
 * This file is a part of the Sharemind framework.
 * Copyright (C) Cybernetica AS
 *
 * All rights are reserved. Reproduction in whole or part is prohibited
 * without the written consent of the copyright owner. The usage of this
 * code is subject to the appropriate license agreement.
 */

#ifndef MOD_SHARED3P_EMU_SCALARPRODUCTSYSCALL_H
#define MOD_SHARED3P_EMU_SCALARPRODUCTSYSCALL_H

#include "../SecretSharing.h"
#include "../Shared3pValueTraits.h"
#include "Common.h"
#include <sharemind/module-apis/api_0x1.h>

namespace sharemind {

namespace {

template <typename T>
static T constMultShare(const T& v1, const T& v2, numeric_value_tag) {
    return v1 * v2;
}

template <typename T>
static T constMultShare(const T& v1, const T& v2, xored_numeric_value_tag) {
    return v1 & v2;
}

static bool constMultShare(bool v1, bool v2, bool_value_tag) {
    return v1 && v2;
}

} /* namespace { */

template <typename T>
NAMED_SYSCALL(parallel_const_scalar_product, name, args, num_args, refs, crefs, returnValue, c)
{
    (void) name;

    VMHandles handles;
    if (! SyscallArgs<3, false, 0, 1>::check(num_args, refs, crefs, returnValue) ||
        ! handles.get(c, args)) {
        return SHAREMIND_MODULE_API_0x1_INVALID_CALL;
    }

    try {
        Shared3pPDPI* pdpi = static_cast<Shared3pPDPI *>(handles.pdpiHandle);
        void* srcHandle = args[1].p[0];
        void* destHandle = args[2].p[0];
        if (! pdpi->isValidHandle<T>(destHandle) ||
            ! pdpi->isValidHandle<T>(srcHandle))
        {
            return SHAREMIND_MODULE_API_0x1_GENERAL_ERROR;
        }

        ShareVec<T>& dest = *static_cast<ShareVec<T> *>(destHandle);
        const ShareVec<T> & src = *static_cast<const ShareVec<T> * >(srcHandle);
        ImmutableVmVec<T> coeffs(crefs[0]);

        const size_t n = src.size();
        const size_t m = coeffs.size();

        if (dest.size() != n - m + 1) {
            return SHAREMIND_MODULE_API_0x1_INVALID_CALL;
        }

        for (size_t i = 0; i < n - m + 1; ++i) {
            dest[i] = 0;

            for (size_t j = 0; j < m; ++j) {
                auto const mul = constMultShare(
                    src[i + j], coeffs[j], typename ValueTraits<T>::value_category{});
                dest[i] = SecretSharing::combine(
                    dest[i], mul, typename ValueTraits<T>::value_category{});
            }
        }

        return SHAREMIND_MODULE_API_0x1_OK;
    } catch (...) {
        return catchModuleApiErrors ();
    }
}

} /* namespace sharemind { */

#endif // MOD_SHARED3P_EMU_SCALARPRODUCTSYSCALL_H
