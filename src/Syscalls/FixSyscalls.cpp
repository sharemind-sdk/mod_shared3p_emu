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

#include "FixSyscalls.h"

#include "../Shared3pPDPI.h"
#include "../Shared3pVector.h"

#include <sharemind/VmVector.h>

namespace sharemind {

template <typename Uint, typename Float>
NAMED_SYSCALL(classify_fix_vec, name, args, num_args, refs, crefs, returnValue, c)
{
    (void) name;

    if (num_args != 2 || returnValue || refs)
        return SHAREMIND_MODULE_API_0x1_INVALID_CALL;

    if (!crefs || !crefs[0].pData)
        return SHAREMIND_MODULE_API_0x1_INVALID_CALL;

    VMHandles handles;
    if (!handles.get (c, args)) {
        return SHAREMIND_MODULE_API_0x1_INVALID_CALL;
    }

    try {
        Shared3pPDPI * pdpi = static_cast<Shared3pPDPI *> (handles.pdpiHandle);
        void* const destHandle = args[1].p[0];
        if (! pdpi->isValidHandle<Uint> (destHandle)) {
            return SHAREMIND_MODULE_API_0x1_GENERAL_ERROR;
        }

        ShareVec<Uint>& dest = *static_cast<ShareVec<Uint>*> (destHandle);
        ImmutableVmVec<Float> src (crefs[0]);

        if (sizeof (Uint) * dest.size () > crefs[0].size) {
            return SHAREMIND_MODULE_API_0x1_GENERAL_ERROR;
        }

        size_t size = src.size ();
        for (size_t i = 0; i < size; ++i) {
            dest[i] = pubFloatToFix<Uint, Float> (src[i]);
        }

        return SHAREMIND_MODULE_API_0x1_OK;
    } catch (...) {
        return catchModuleApiErrors ();
    }
}

template <typename Uint, typename Float>
NAMED_SYSCALL(declassify_fix_vec, name, args, num_args, refs, crefs, returnValue, c)
{
    (void) name;

    if (num_args != 2 || returnValue || crefs)
        return SHAREMIND_MODULE_API_0x1_INVALID_CALL;

    if (!refs || !refs[0].pData)
        return SHAREMIND_MODULE_API_0x1_INVALID_CALL;

    VMHandles handles;
    if (!handles.get (c, args)) {
        return SHAREMIND_MODULE_API_0x1_INVALID_CALL;
    }

    try {
        Shared3pPDPI * pdpi = static_cast<Shared3pPDPI *> (handles.pdpiHandle);
        MutableVmVec<Float> ref (refs[0]);
        size_t size = ref.size ();

        void* srcHandle = args[1].p[0];
        if (! pdpi->isValidHandle<Uint> (srcHandle)) {
            return SHAREMIND_MODULE_API_0x1_GENERAL_ERROR;
        }

        const ShareVec<Uint>& src = *static_cast<ShareVec<Uint>*> (srcHandle);
        if (refs[0].size < sizeof (Uint) * src.size ()) {
            return SHAREMIND_MODULE_API_0x1_GENERAL_ERROR;
        }

        for (size_t i = 0; i < size; ++i) {
            ref[i] = pubFixToFloat<Uint, Float> (src[i]);
        }

        return SHAREMIND_MODULE_API_0x1_OK;
    } catch (...) {
        return catchModuleApiErrors ();
    }
}

template <typename Uint, typename Float>
NAMED_SYSCALL(init_fix_vec, name, args, num_args, refs, crefs, returnValue, c)
{
    (void) name;

    if (num_args != 3 || returnValue || crefs || refs)
        return SHAREMIND_MODULE_API_0x1_INVALID_CALL;

    VMHandles handles;
    if (!handles.get (c, args)) {
        return SHAREMIND_MODULE_API_0x1_INVALID_CALL;
    }

    try {
        Shared3pPDPI* pdpi = static_cast<Shared3pPDPI *> (handles.pdpiHandle);
        if (! pdpi->isValidHandle<Uint> (args[2].p[0])) {
            return SHAREMIND_MODULE_API_0x1_GENERAL_ERROR;
        }

        ShareVec<Uint>& vec = *static_cast<ShareVec<Uint>*> (args[2].p[0]);

        using PubFloat = typename Float::public_type;
        using PubUint = typename Uint::public_type;
        PubFloat val = getStack<Float> (args[1]);
        PubUint fix = pubFloatToFix<Uint, Float> (val);
        size_t size = vec.size();

        for (size_t i = 0; i < size; ++i) {
            vec[i] = fix;
        }

        return SHAREMIND_MODULE_API_0x1_OK;
    } catch (...) {
        return catchModuleApiErrors ();
    }
}

/*
 * Explicitly instantiate system calls:
 */

template NAMED_SYSCALL((classify_fix_vec<s3p_uint32_t, s3p_float32_t>),
                       name, args, num_args, refs, crefs, returnValue, c);
template NAMED_SYSCALL((classify_fix_vec<s3p_uint64_t, s3p_float64_t>),
                       name, args, num_args, refs, crefs, returnValue, c);
template NAMED_SYSCALL((declassify_fix_vec<s3p_uint32_t, s3p_float32_t>),
                       name, args, num_args, refs, crefs, returnValue, c);
template NAMED_SYSCALL((declassify_fix_vec<s3p_uint64_t, s3p_float64_t>),
                       name, args, num_args, refs, crefs, returnValue, c);
template NAMED_SYSCALL((init_fix_vec<s3p_uint32_t, s3p_float32_t>),
                       name, args, num_args, refs, crefs, returnValue, c);
template NAMED_SYSCALL((init_fix_vec<s3p_uint64_t, s3p_float64_t>),
                       name, args, num_args, refs, crefs, returnValue, c);

} /* namespace sharemind */
