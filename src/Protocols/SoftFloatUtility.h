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

#ifndef MOD_SHARED3P_EMU_PROTOCOLS_SOFTFLOATURILITY_H
#define MOD_SHARED3P_EMU_PROTOCOLS_SOFTFLOATURILITY_H

#include <sharemind/static_assert.h>
#include <sharemind/libsoftfloat/softfloat.h>
#include <sharemind/libsoftfloat_math/sf_abs.h>
#include <sharemind/libsoftfloat_math/sf_ceil.h>
#include <sharemind/libsoftfloat_math/sf_erf.h>
#include <sharemind/libsoftfloat_math/sf_exp.h>
#include <sharemind/libsoftfloat_math/sf_floor.h>
#include <sharemind/libsoftfloat_math/sf_log.h>
#include <sharemind/libsoftfloat_math/sf_sin.h>


namespace sharemind {

sf_result32f sf_val_to_float32(bool a, sf_fpu_state fpu = sf_fpu_state_default)
{ return { a ? 0x3f800000u : 0x0u , fpu }; }
sf_result32f sf_val_to_float32(int8_t a, sf_fpu_state fpu = sf_fpu_state_default)
{ return sf_int32_to_float32(a, fpu); }
sf_result32f sf_val_to_float32(int16_t a, sf_fpu_state fpu = sf_fpu_state_default)
{ return sf_int32_to_float32(a, fpu); }
sf_result32f sf_val_to_float32(int32_t a, sf_fpu_state fpu = sf_fpu_state_default)
{ return sf_int32_to_float32(a, fpu); }
sf_result32f sf_val_to_float32(int64_t a, sf_fpu_state fpu = sf_fpu_state_default)
{ return sf_int64_to_float32(a, fpu); }
sf_result32f sf_val_to_float32(uint8_t a, sf_fpu_state fpu = sf_fpu_state_default)
{ return sf_int32_to_float32(a, fpu); }
sf_result32f sf_val_to_float32(uint16_t a, sf_fpu_state fpu = sf_fpu_state_default)
{ return sf_int32_to_float32(a, fpu); }
sf_result32f sf_val_to_float32(uint32_t a, sf_fpu_state fpu = sf_fpu_state_default)
{ return sf_int64_to_float32(a, fpu); }
sf_result32f sf_val_to_float32(uint64_t a, sf_fpu_state fpu = sf_fpu_state_default)
{
    /// \todo Replace this workaround with a proper solution.
    sf_result32f rv;
    const float f = static_cast<float>(a);
    SHAREMIND_STATIC_ASSERT(sizeof(decltype(rv.result)) == sizeof(float));
    memcpy(&rv.result, &f, sizeof(float));
    rv.fpu_state = fpu;
    return rv;
}

sf_result64f sf_val_to_float64(bool a, sf_fpu_state fpu = sf_fpu_state_default)
{ return { a ? 0x3ff0000000000000u : 0x0u, fpu }; }
sf_result64f sf_val_to_float64(int8_t a, sf_fpu_state fpu = sf_fpu_state_default)
{ return { sf_int32_to_float64(a),  fpu }; }
sf_result64f sf_val_to_float64(int16_t a, sf_fpu_state fpu = sf_fpu_state_default)
{ return { sf_int32_to_float64(a), fpu }; }
sf_result64f sf_val_to_float64(int32_t a, sf_fpu_state fpu = sf_fpu_state_default)
{ return { sf_int32_to_float64(a),  fpu }; }
sf_result64f sf_val_to_float64(int64_t a, sf_fpu_state fpu = sf_fpu_state_default)
{ return sf_int64_to_float64(a, fpu); }
sf_result64f sf_val_to_float64(uint8_t a, sf_fpu_state fpu = sf_fpu_state_default)
{ return { sf_int32_to_float64(a), fpu }; }
sf_result64f sf_val_to_float64(uint16_t a, sf_fpu_state fpu = sf_fpu_state_default)
{ return { sf_int32_to_float64(a), fpu }; }
sf_result64f sf_val_to_float64(uint32_t a, sf_fpu_state fpu = sf_fpu_state_default)
{ return sf_int64_to_float64(a, fpu); }
sf_result64f sf_val_to_float64(uint64_t a, sf_fpu_state fpu = sf_fpu_state_default)
{
    /// \todo Replace this workaround with a proper solution.
    sf_result64f rv;
    const double d = static_cast<double>(a);
    SHAREMIND_STATIC_ASSERT(sizeof(decltype(rv.result)) == sizeof(double));
    memcpy(&rv.result, &d, sizeof(double));
    rv.fpu_state = fpu;
    return rv;
}

template <typename T>
struct sf_result {
    T result;
    sf_fpu_state fpu_state;
};

template <typename T>
sf_result<T> sf_float32_to_val(sf_float32 a, sf_fpu_state fpu = sf_fpu_state_default)
{
    auto rv = sf_float32_to_int32_round_to_zero(a, fpu);
    return { static_cast<T>(rv.result), rv.fpu_state };
}
template <>
sf_result<bool> sf_float32_to_val<bool>(sf_float32 a, sf_fpu_state fpu)
{
    return { (a & 0x7fffffff) != 0, fpu };
}
template <>
sf_result<int64_t> sf_float32_to_val<int64_t>(sf_float32 a, sf_fpu_state fpu)
{
    auto rv = sf_float32_to_int64_round_to_zero(a, fpu);
    return { rv.result, rv.fpu_state };
}
template <>
sf_result<uint32_t> sf_float32_to_val<uint32_t>(sf_float32 a, sf_fpu_state fpu)
{
    auto rv = sf_float32_to_int64_round_to_zero(a, fpu);
    return { static_cast<uint32_t>(rv.result), rv.fpu_state };
}
template <>
sf_result<uint64_t> sf_float32_to_val<uint64_t>(sf_float32 a, sf_fpu_state fpu)
{
    auto rv = sf_float32_to_int64_round_to_zero(a, fpu);
    return { static_cast<uint64_t>(rv.result), rv.fpu_state };
}

template <typename T>
sf_result<T> sf_float64_to_val(sf_float64 a, sf_fpu_state fpu = sf_fpu_state_default)
{
    auto rv = sf_float64_to_int64_round_to_zero(a, fpu);
    return { static_cast<T>(rv.result), rv.fpu_state };
}
template <>
sf_result<bool> sf_float64_to_val<bool>(sf_float64 a, sf_fpu_state fpu)
{
    return { (a & 0x7fffffffffffffff) != 0, fpu };
}
template <>
sf_result<int64_t> sf_float64_to_val(sf_float64 a, sf_fpu_state fpu)
{
    auto rv = sf_float64_to_int64_round_to_zero(a, fpu);
    return { rv.result, rv.fpu_state };
}
template <>
sf_result<uint32_t> sf_float64_to_val(sf_float64 a, sf_fpu_state fpu)
{
    auto rv = sf_float64_to_int64_round_to_zero(a, fpu);
    return { static_cast<uint32_t>(rv.result), fpu };
}
template <>
sf_result<uint64_t> sf_float64_to_val(sf_float64 a, sf_fpu_state fpu)
{
    auto rv = sf_float64_to_int64_round_to_zero(a, fpu);
    return { static_cast<uint64_t>(rv.result), fpu };
}

sf_flag sf_float_sign(sf_float32 a) { return a >> 31; }
sf_flag sf_float_sign(sf_float64 a) { return a >> 63; }
sf_int16 sf_float_exponent(sf_float32 a) { return a >> 23 & 0xff; }
sf_int16 sf_float_exponent(sf_float64 a) { return a >> 52 & 0x7ff; }
sf_int32 sf_float_significand(sf_float32 a) { return a & 0x7fffff; }
sf_int64 sf_float_significand(sf_float64 a) { return a & 0xfffffffffffff; }

sf_float32 sf_float_abs(sf_float32 a) { return sf_float32_abs(a); }
sf_float64 sf_float_abs(sf_float64 a) { return sf_float64_abs(a); }

sf_result32f sf_float_add(sf_float32 a, sf_float32 b,
                          sf_fpu_state fpu = sf_fpu_state_default)
{
    return sf_float32_add(a, b, fpu);
}

sf_result64f sf_float_add(sf_float64 a, sf_float64 b,
                          sf_fpu_state fpu = sf_fpu_state_default)
{
    return sf_float64_add(a, b, fpu);
}

sf_result32f sf_float_ceil(sf_float32 a,
                            sf_fpu_state fpu = sf_fpu_state_default)
{
    return sf_float32_ceil(a, fpu);
}

sf_result64f sf_float_ceil(sf_float64 a,
                            sf_fpu_state fpu = sf_fpu_state_default)
{
    return sf_float64_ceil(a, fpu);
}

sf_result32f sf_float_div(sf_float32 a, sf_float32 b,
                          sf_fpu_state fpu = sf_fpu_state_default)
{
    return sf_float32_div(a, b, fpu);
}

sf_result64f sf_float_div(sf_float64 a, sf_float64 b,
                          sf_fpu_state fpu = sf_fpu_state_default)
{
    return sf_float64_div(a, b, fpu);
}

sf_resultFlag sf_float_eq(sf_float32 a, sf_float32 b,
                          sf_fpu_state fpu = sf_fpu_state_default)
{
    return sf_float32_eq(a, b, fpu);
}

sf_resultFlag sf_float_eq(sf_float64 a, sf_float64 b,
                          sf_fpu_state fpu = sf_fpu_state_default)
{
    return sf_float64_eq(a, b, fpu);
}

sf_result32f sf_float_erf(sf_float32 a,
                          sf_fpu_state fpu = sf_fpu_state_default)
{
    return sf_float32_erf(a, fpu);
}

sf_result64f sf_float_erf(sf_float64 a,
                          sf_fpu_state fpu = sf_fpu_state_default)
{
    return sf_float64_erf(a, fpu);
}

sf_result32f sf_float_exp(sf_float32 a,
                          sf_fpu_state fpu = sf_fpu_state_default)
{
    return sf_float32_exp(a, fpu);
}

sf_result64f sf_float_exp(sf_float64 a,
                          sf_fpu_state fpu = sf_fpu_state_default)
{
    return sf_float64_exp(a, fpu);
}

sf_result32f sf_float_floor(sf_float32 a,
                            sf_fpu_state fpu = sf_fpu_state_default)
{
    return sf_float32_floor(a, fpu);
}

sf_result64f sf_float_floor(sf_float64 a,
                            sf_fpu_state fpu = sf_fpu_state_default)
{
    return sf_float64_floor(a, fpu);
}

sf_result32f sf_float_inv(sf_float32 a,
                          sf_fpu_state fpu = sf_fpu_state_default)
{
    return sf_float32_div(0x3f800000, a, fpu);
}

sf_result64f sf_float_inv(sf_float64 a,
                          sf_fpu_state fpu = sf_fpu_state_default)
{
    return sf_float64_div(0x3ff0000000000000, a, fpu);
}

sf_resultFlag sf_float_le(sf_float32 a, sf_float32 b,
                          sf_fpu_state fpu = sf_fpu_state_default)
{
    return sf_float32_le(a, b, fpu);
}

sf_resultFlag sf_float_le(sf_float64 a, sf_float64 b,
                          sf_fpu_state fpu = sf_fpu_state_default)
{
    return sf_float64_le(a, b, fpu);
}

sf_result32f sf_float_log(sf_float32 a,
                          sf_fpu_state fpu = sf_fpu_state_default)
{
    return sf_float32_log(a, fpu);
}

sf_result64f sf_float_log(sf_float64 a,
                          sf_fpu_state fpu = sf_fpu_state_default)
{
    return sf_float64_log(a, fpu);
}

sf_resultFlag sf_float_lt(sf_float32 a, sf_float32 b,
                          sf_fpu_state fpu = sf_fpu_state_default)
{
    return sf_float32_lt(a, b, fpu);
}

sf_resultFlag sf_float_lt(sf_float64 a, sf_float64 b,
                          sf_fpu_state fpu = sf_fpu_state_default)
{
    return sf_float64_lt(a, b, fpu);
}

sf_result32f sf_float_mul(sf_float32 a, sf_float32 b,
                          sf_fpu_state fpu = sf_fpu_state_default)
{
    return sf_float32_mul(a, b, fpu);
}

sf_result64f sf_float_mul(sf_float64 a, sf_float64 b,
                          sf_fpu_state fpu = sf_fpu_state_default)
{
    return sf_float64_mul(a, b, fpu);
}

sf_float32 sf_float_neg(sf_float32 a) { return sf_float32_neg(a); }
sf_float64 sf_float_neg(sf_float64 a) { return sf_float64_neg(a); }

sf_result32f sf_float_sin(sf_float32 a,
                          sf_fpu_state fpu = sf_fpu_state_default)
{
    return sf_float32_sin(a, fpu);
}

sf_result64f sf_float_sin(sf_float64 a,
                          sf_fpu_state fpu = sf_fpu_state_default)
{
    return sf_float64_sin(a, fpu);
}

sf_result32f sf_float_sub(sf_float32 a, sf_float32 b,
                          sf_fpu_state fpu = sf_fpu_state_default)
{
    return sf_float32_sub(a, b, fpu);
}

sf_result64f sf_float_sub(sf_float64 a, sf_float64 b,
                          sf_fpu_state fpu = sf_fpu_state_default)
{
    return sf_float64_sub(a, b, fpu);
}

sf_result32f sf_float_sqrt(sf_float32 a,
                           sf_fpu_state fpu = sf_fpu_state_default)
{
    return sf_float32_sqrt(a, fpu);
}

sf_result64f sf_float_sqrt(sf_float64 a,
                           sf_fpu_state fpu = sf_fpu_state_default)
{
    return sf_float64_sqrt(a, fpu);
}

sf_result32i sf_float_to_int(sf_float32 a,
                             sf_fpu_state fpu = sf_fpu_state_default)
{
    return sf_float32_to_int32_round_to_zero(a, fpu);
}

sf_result64i sf_float_to_int(sf_float64 a,
                             sf_fpu_state fpu = sf_fpu_state_default)
{
    return sf_float64_to_int64_round_to_zero(a, fpu);
}

} /* namespace sharemind { */

#endif /* MOD_SHARED3P_EMU_PROTOCOLS_SOFTFLOATURILITY_H */
