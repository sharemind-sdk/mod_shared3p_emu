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

#ifndef MOD_SHARED3P_EMU_SYSCALLS_AESSYSCALLS_H
#define MOD_SHARED3P_EMU_SYSCALLS_AESSYSCALLS_H

#include <sharemind/libmodapi/api_0x1.h>
#include "Common.h"

namespace sharemind {

/**
 * SysCall: aes_xor_uint32_vec
 * Args:
 *      0) uint64[0u]     pd index
 *      1) p[0u]          input text vector handle
 *      2) p[0u]          preexpanded key vector handle
 *      3) p[0u]          handle of the output vector for the output text
 *
 * \pre All handles are valid vectors of type s3p_xor_uint32_t.
 * \pre Both input and output vectors contain a multiple of Nb values, where Nb
 *      is an AES constant from FIPS 197 (with the value 4).
 * \pre The preexpanded key vector contains exactly (Nk * (Nr + 1)) / Nb times
 *      more elements than the input/output vector, where Nk, Nb and Nr are the
 *      AES constants from FIPS 197.
 * \post The input vector is not modified.
 * \post The preexpanded key vector is not modified.
 * \post If successful, the output vector contains the encrypted/decrypted result.
 */
template <class Protocol>
NAMED_SYSCALL(aes_xor_uint32_vec, name, args, num_args, refs, crefs, returnValue, c)
    __attribute__ ((visibility("internal")));

/**
 * SysCall: aes_xor_uint32_vec_expand_key
 * Args:
 *      0) uint64[0u]     pd index
 *      1) p[0u]          input key vector handle
 *      2) p[0u]          output preexpanded key vector handle
 *
 * \pre All handles are valid vectors of type s3p_xor_uint32_t.
 * \pre The input vector contains a multiple of Nb values, where Nb is an AES
 *      constant from FIPS 197 (with the value 4).
 * \pre The output vector contains exactly (Nr + 1) times more elements than the
 *      input vector, where Nr is an AES constant from FIPS 197.
 * \post The input vector is not modified.
 * \post If successful, the output vector contains the expanded key.
 */
template <class Protocol>
NAMED_SYSCALL(aes_xor_uint32_vec_expand_key, name, args, num_args, refs, crefs, returnValue, c)
    __attribute__ ((visibility("internal")));

} /* namespace sharemind */

#endif /* MOD_SHARED3P_EMU_SYSCALLS_AESSYSCALLS_H */
