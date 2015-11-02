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

#include "AESSyscalls.h"

#include "Common.h"
#include "../Protocols/AESProtocol.h"
#include "../Shared3pPDPI.h"
#include "../Shared3pVector.h"


namespace sharemind {

template <class Protocol>
NAMED_SYSCALL(aes_xor_uint32_vec, name, args, num_args, refs, crefs, returnValue, c)
{
    VMHandles handles;
    if (!SyscallArgs<4>::check(num_args, refs, crefs, returnValue) ||
        !handles.get(c, args))
    {
        return SHAREMIND_MODULE_API_0x1_INVALID_CALL;
    }

    try {
        Shared3pPDPI * const pdpi = static_cast<Shared3pPDPI*>(handles.pdpiHandle);

        void * const inputVecHandle = args[1u].p[0u];
        void * const keyVecHandle = args[2u].p[0u];
        void * const outputVecHandle = args[3u].p[0u];

        if (!pdpi->isValidHandle<s3p_xor_uint32_t>(inputVecHandle) ||
            !pdpi->isValidHandle<s3p_xor_uint32_t>(keyVecHandle) ||
            !pdpi->isValidHandle<s3p_xor_uint32_t>(outputVecHandle))
        {
            return SHAREMIND_MODULE_API_0x1_GENERAL_ERROR;
        }

        const ShareVec<s3p_xor_uint32_t> & inputVec = *static_cast<ShareVec<s3p_xor_uint32_t> *>(inputVecHandle);
        const ShareVec<s3p_xor_uint32_t> & keyVec = *static_cast<ShareVec<s3p_xor_uint32_t> *>(keyVecHandle);
        ShareVec<s3p_xor_uint32_t> & outputVec = *static_cast<ShareVec<s3p_xor_uint32_t> *>(outputVecHandle);

        // Check whether input and output vectors have proper size:
        if (inputVec.empty()
            || (inputVec.size() % Protocol::Nb) != 0u
            || inputVec.size() != outputVec.size()
            || (keyVec.size() % (Protocol::Nb * (Protocol::Nr + 1u))) != 0u
            || (inputVec.size() / Protocol::Nb) != (keyVec.size() / (Protocol::Nb * (Protocol::Nr + 1u))))
        {
            return SHAREMIND_MODULE_API_0x1_INVALID_CALL;
        }

        Protocol().processWithExpandedKey(inputVec, keyVec, outputVec);

        PROFILE_SYSCALL(pdpi->profiler(), pdpi->modelEvaluator(), name,
                        inputVec.size());

        return SHAREMIND_MODULE_API_0x1_OK;
    } catch (...) {
        return catchModuleApiErrors ();
    }
    return SHAREMIND_MODULE_API_0x1_GENERAL_ERROR;
}

template <class Protocol>
NAMED_SYSCALL(aes_xor_uint32_vec_expand_key, name, args, num_args, refs, crefs, returnValue, c)
{
    VMHandles handles;
    if (!SyscallArgs<3>::check(num_args, refs, crefs, returnValue) ||
        !handles.get(c, args))
    {
        return SHAREMIND_MODULE_API_0x1_INVALID_CALL;
    }

    try {
        Shared3pPDPI * const pdpi = static_cast<Shared3pPDPI*>(handles.pdpiHandle);

        void * const inputVecHandle = args[1u].p[0u];
        void * const outputVecHandle = args[2u].p[0u];

        if (!pdpi->isValidHandle<s3p_xor_uint32_t>(inputVecHandle) ||
            !pdpi->isValidHandle<s3p_xor_uint32_t>(outputVecHandle))
        {
            return SHAREMIND_MODULE_API_0x1_GENERAL_ERROR;
        }

        const ShareVec<s3p_xor_uint32_t> & inputVec = *static_cast<ShareVec<s3p_xor_uint32_t> *>(inputVecHandle);
        ShareVec<s3p_xor_uint32_t> & outputVec = *static_cast<ShareVec<s3p_xor_uint32_t> *>(outputVecHandle);

        // Check whether input and output vectors have proper size:
        if (inputVec.empty()
            || (inputVec.size() % Protocol::Nk) != 0u
            || (outputVec.size() % (Protocol::Nb * (Protocol::Nr + 1u))) != 0u
            || ((inputVec.size() / Protocol::Nk) != (outputVec.size() / (Protocol::Nb * (Protocol::Nr + 1u)))))
        {
            return SHAREMIND_MODULE_API_0x1_INVALID_CALL;
        }

        Protocol().expandAesKey(inputVec, outputVec);

        PROFILE_SYSCALL(pdpi->profiler(), pdpi->modelEvaluator(), name,
                        inputVec.size());

        return SHAREMIND_MODULE_API_0x1_OK;
    } catch (...) {
        return catchModuleApiErrors ();
    }
    return SHAREMIND_MODULE_API_0x1_GENERAL_ERROR;
}

/*
 * Explicitly instantiate system calls:
 */

template NAMED_SYSCALL(aes_xor_uint32_vec<Aes128Protocol>,
                       name, args, num_args, refs, crefs, returnValue, c);
template NAMED_SYSCALL(aes_xor_uint32_vec<Aes192Protocol>,
                       name, args, num_args, refs, crefs, returnValue, c);
template NAMED_SYSCALL(aes_xor_uint32_vec<Aes256Protocol>,
                       name, args, num_args, refs, crefs, returnValue, c);

template NAMED_SYSCALL(aes_xor_uint32_vec_expand_key<Aes128Protocol>,
                       name, args, num_args, refs, crefs, returnValue, c);
template NAMED_SYSCALL(aes_xor_uint32_vec_expand_key<Aes192Protocol>,
                       name, args, num_args, refs, crefs, returnValue, c);
template NAMED_SYSCALL(aes_xor_uint32_vec_expand_key<Aes256Protocol>,
                       name, args, num_args, refs, crefs, returnValue, c);

} /* namespace sharemind */
