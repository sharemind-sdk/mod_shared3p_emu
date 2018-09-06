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

#ifndef MOD_SHARED3P_EMU_SYSCALLS_FIXSYSCALLS_H
#define MOD_SHARED3P_EMU_SYSCALLS_FIXSYSCALLS_H

#include "../Shared3pValueTraits.h"
#include "../Protocols/SoftFloatUtility.h"
#include "Common.h"
#include <sharemind/module-apis/api_0x1.h>

namespace sharemind {

template<typename T>
struct __attribute__ ((visibility("internal"))) fix_radix_point {
    static constexpr uint64_t value = 0;
};

template<>
struct __attribute__ ((visibility("internal"))) fix_radix_point<s3p_uint32_t> {
    static constexpr uint64_t value = 16;
};

template<>
struct __attribute__ ((visibility("internal"))) fix_radix_point<s3p_uint64_t> {
    static constexpr uint64_t value = 32;
};

template<typename Uint, typename Float>
typename Float::public_type pubFixToFloat(typename Uint::public_type fix) {
    using PubFloat = typename Float::public_type;
    using PubUint = typename Uint::public_type;
    using PubInt = typename respective_signed_type<Uint>::type::public_type;
    PubInt fixI = *reinterpret_cast<PubInt*>(&fix);
    uint64_t radix = fix_radix_point<Uint>::value;

    sf_fpu_state state = 0;
    PubFloat two = sf_int_to_float(static_cast<PubUint>(2), state);
    PubFloat fixF = sf_int_to_float(fixI, state);
    // 2^-radix
    PubFloat tmp = sf_float_pow(two, static_cast<int>(radix));
    // fix * 2^-radix
    PubFloat res = sf_float_div(fixF, tmp, state).result;

    return res;
}

template<typename Uint, typename Float>
typename Uint::public_type pubFloatToFix(typename Float::public_type x) {
    using PubUint = typename Uint::public_type;
    using PubFloat = typename Float::public_type;
    using PubInt = typename respective_signed_type<Uint>::type::public_type;
    uint64_t radix = fix_radix_point<Uint>::value;

    sf_fpu_state state = 0;
    PubFloat two = sf_int_to_float(static_cast<PubUint>(2), state);
    // round(float * 2^radix)
    PubInt resI =
        sf_float_round(
            sf_float_mul(x, sf_float_pow(two, static_cast<int>(radix)), state).result);

    return *reinterpret_cast<PubUint*>(&resI);
}

template <typename Uint, typename Float>
NAMED_SYSCALL(classify_fix_vec, name, args, num_args, refs, crefs, returnValue, c)
    __attribute__ ((visibility("internal")));

template <typename Uint, typename Float>
NAMED_SYSCALL(declassify_fix_vec, name, args, num_args, refs, crefs, returnValue, c)
    __attribute__ ((visibility("internal")));

template <typename Uint, typename Float>
NAMED_SYSCALL(init_fix_vec, name, args, num_args, refs, crefs, returnValue, c)
    __attribute__ ((visibility("internal")));

} /* namespace sharemind */

#endif /* MOD_SHARED3P_EMU_SYSCALLS_FIXSYSCALLS_H */
