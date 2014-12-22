/*
 * This file is a part of the Sharemind framework.
 * Copyright (C) Cybernetica AS
 *
 * All rights are reserved. Reproduction in whole or part is prohibited
 * without the written consent of the copyright owner. The usage of this
 * code is subject to the appropriate license agreement.
 */

#ifndef MOD_SHARED3P_EMU_PROTOCOLS_SOFTFLOATURILITY_H
#define MOD_SHARED3P_EMU_PROTOCOLS_SOFTFLOATURILITY_H

#include <sharemind/3rdparty/libsoftfloat/softfloat.h>
#include <sharemind/3rdparty/libsoftfloat_math/sf_abs.h>
#include <sharemind/3rdparty/libsoftfloat_math/sf_ceil.h>
#include <sharemind/3rdparty/libsoftfloat_math/sf_erf.h>
#include <sharemind/3rdparty/libsoftfloat_math/sf_exp.h>
#include <sharemind/3rdparty/libsoftfloat_math/sf_floor.h>
#include <sharemind/3rdparty/libsoftfloat_math/sf_log.h>
#include <sharemind/3rdparty/libsoftfloat_math/sf_sin.h>

/// \todo internal


namespace sharemind {

sf_flag sf_float_sign(sf_float32 a) { return a >> 31; }
sf_flag sf_float_sign(sf_float64 a) { return a >> 63; }
sf_int16 sf_float_exponent(sf_float32 a) { return a >> 23 & 0xff; }
sf_int16 sf_float_exponent(sf_float64 a) { return a >> 52 & 0x7ff; }
sf_int32 sf_float_significand(sf_float32 a) { return a & 0x7fffff; }
sf_int64 sf_float_significand(sf_float64 a) { return a & 0xfffffffffffff; }

sf_float32 sf_float_abs(sf_float32 a) { return sf_float32_abs(a); }
sf_float64 sf_float_abs(sf_float64 a) { return sf_float64_abs(a); }

sf_result32f sf_float_add(sf_float32 a, sf_float32 b,
                          sf_fpu_state fpuState = sf_fpu_state_default)
{
    return sf_float32_add(a, b, fpuState);
}

sf_result64f sf_float_add(sf_float64 a, sf_float64 b,
                          sf_fpu_state fpuState = sf_fpu_state_default)
{
    return sf_float64_add(a, b, fpuState);
}

sf_result32f sf_float_ceil(sf_float32 a,
                            sf_fpu_state fpuState = sf_fpu_state_default)
{
    return sf_float32_ceil(a, fpuState);
}

sf_result64f sf_float_ceil(sf_float64 a,
                            sf_fpu_state fpuState = sf_fpu_state_default)
{
    return sf_float64_ceil(a, fpuState);
}

sf_result32f sf_float_div(sf_float32 a, sf_float32 b,
                          sf_fpu_state fpuState = sf_fpu_state_default)
{
    return sf_float32_div(a, b, fpuState);
}

sf_result64f sf_float_div(sf_float64 a, sf_float64 b,
                          sf_fpu_state fpuState = sf_fpu_state_default)
{
    return sf_float64_div(a, b, fpuState);
}

sf_resultFlag sf_float_eq(sf_float32 a, sf_float32 b,
                          sf_fpu_state fpuState = sf_fpu_state_default)
{
    return sf_float32_eq(a, b, fpuState);
}

sf_resultFlag sf_float_eq(sf_float64 a, sf_float64 b,
                          sf_fpu_state fpuState = sf_fpu_state_default)
{
    return sf_float64_eq(a, b, fpuState);
}

sf_result32f sf_float_erf(sf_float32 a,
                          sf_fpu_state fpuState = sf_fpu_state_default)
{
    return sf_float32_erf(a, fpuState);
}

sf_result64f sf_float_erf(sf_float64 a,
                          sf_fpu_state fpuState = sf_fpu_state_default)
{
    return sf_float64_erf(a, fpuState);
}

sf_result32f sf_float_exp(sf_float32 a,
                          sf_fpu_state fpuState = sf_fpu_state_default)
{
    return sf_float32_exp(a, fpuState);
}

sf_result64f sf_float_exp(sf_float64 a,
                          sf_fpu_state fpuState = sf_fpu_state_default)
{
    return sf_float64_exp(a, fpuState);
}

sf_result32f sf_float_floor(sf_float32 a,
                            sf_fpu_state fpuState = sf_fpu_state_default)
{
    return sf_float32_floor(a, fpuState);
}

sf_result64f sf_float_floor(sf_float64 a,
                            sf_fpu_state fpuState = sf_fpu_state_default)
{
    return sf_float64_floor(a, fpuState);
}

sf_resultFlag sf_float_le(sf_float32 a, sf_float32 b,
                          sf_fpu_state fpuState = sf_fpu_state_default)
{
    return sf_float32_le(a, b, fpuState);
}

sf_resultFlag sf_float_le(sf_float64 a, sf_float64 b,
                          sf_fpu_state fpuState = sf_fpu_state_default)
{
    return sf_float64_le(a, b, fpuState);
}

sf_result32f sf_float_log(sf_float32 a,
                          sf_fpu_state fpuState = sf_fpu_state_default)
{
    return sf_float32_log(a, fpuState);
}

sf_result64f sf_float_log(sf_float64 a,
                          sf_fpu_state fpuState = sf_fpu_state_default)
{
    return sf_float64_log(a, fpuState);
}

sf_resultFlag sf_float_lt(sf_float32 a, sf_float32 b,
                          sf_fpu_state fpuState = sf_fpu_state_default)
{
    return sf_float32_lt(a, b, fpuState);
}

sf_resultFlag sf_float_lt(sf_float64 a, sf_float64 b,
                          sf_fpu_state fpuState = sf_fpu_state_default)
{
    return sf_float64_lt(a, b, fpuState);
}

sf_result32f sf_float_mul(sf_float32 a, sf_float32 b,
                          sf_fpu_state fpuState = sf_fpu_state_default)
{
    return sf_float32_mul(a, b, fpuState);
}

sf_result64f sf_float_mul(sf_float64 a, sf_float64 b,
                          sf_fpu_state fpuState = sf_fpu_state_default)
{
    return sf_float64_mul(a, b, fpuState);
}

sf_result32f sf_float_sin(sf_float32 a,
                          sf_fpu_state fpuState = sf_fpu_state_default)
{
    return sf_float32_sin(a, fpuState);
}

sf_result64f sf_float_sin(sf_float64 a,
                          sf_fpu_state fpuState = sf_fpu_state_default)
{
    return sf_float64_sin(a, fpuState);
}

sf_result32f sf_float_sub(sf_float32 a, sf_float32 b,
                          sf_fpu_state fpuState = sf_fpu_state_default)
{
    return sf_float32_sub(a, b, fpuState);
}

sf_result64f sf_float_sub(sf_float64 a, sf_float64 b,
                          sf_fpu_state fpuState = sf_fpu_state_default)
{
    return sf_float64_sub(a, b, fpuState);
}

sf_result32f sf_float_sqrt(sf_float32 a,
                           sf_fpu_state fpuState = sf_fpu_state_default)
{
    return sf_float32_sqrt(a, fpuState);
}

sf_result64f sf_float_sqrt(sf_float64 a,
                           sf_fpu_state fpuState = sf_fpu_state_default)
{
    return sf_float64_sqrt(a, fpuState);
}

sf_result32i sf_float_to_int(sf_float32 a,
                             sf_fpu_state fpuState = sf_fpu_state_default)
{
    return sf_float32_to_int32(a, fpuState);
}

sf_result64i sf_float_to_int(sf_float64 a,
                             sf_fpu_state fpuState = sf_fpu_state_default)
{
    return sf_float64_to_int64(a, fpuState);
}

} /* namespace sharemind { */

#endif /* MOD_SHARED3P_EMU_PROTOCOLS_SOFTFLOATURILITY_H */
