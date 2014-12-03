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

/// \todo internal


namespace sharemind {

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

} /* namespace sharemind { */

#endif /* MOD_SHARED3P_EMU_PROTOCOLS_SOFTFLOATURILITY_H */
