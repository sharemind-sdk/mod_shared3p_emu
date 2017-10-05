/*
 * Copyright (C) 2017 Cybernetica
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

#ifndef MOD_SHARED3P_EMU_PROTOCOLS_FIXEDPOINT_H
#define MOD_SHARED3P_EMU_PROTOCOLS_FIXEDPOINT_H

#include <sharemind/uint128_t.h>
#include "../Shared3pPDPI.h"
#include "../Shared3pValueTraits.h"
#include "SoftFloatUtility.h"


namespace sharemind {

template<typename T>
sf_float32 val_to_float(T x, sharemind::float32_numeric_value_tag) {
    return sharemind::sf_val_to_float32(x).result;
}

template<typename T>
sf_float64 val_to_float(T x, sharemind::float64_numeric_value_tag) {
    return sharemind::sf_val_to_float64(x).result;
}

template<typename T>
T float_to_val(sf_float32 x) {
    return sharemind::sf_float32_to_val<T>(x).result;
}

template<typename T>
T float_to_val(sf_float64 x) {
    return sharemind::sf_float64_to_val<T>(x).result;
}

template<typename T>
struct __attribute__ ((visibility("internal"))) Radix {
    static constexpr uint64_t value = 0;
};

template<>
struct __attribute__ ((visibility("internal"))) Radix<uint32_t> {
    static constexpr uint64_t value = 16;
};

template<>
struct __attribute__ ((visibility("internal"))) Radix<uint64_t> {
    static constexpr uint64_t value = 32;
};

template<typename T>
struct __attribute__ ((visibility("internal"))) respective_float_type {
    using type = sharemind::s3p_float32_t;
};

template<>
struct __attribute__ ((visibility("internal"))) respective_float_type<sharemind::s3p_uint32_t> {
    using type = sharemind::s3p_float32_t;
};

template<>
struct __attribute__ ((visibility("internal"))) respective_float_type<sharemind::s3p_uint64_t> {
    using type = sharemind::s3p_float64_t;
};

class __attribute__ ((visibility("internal"))) FloatToFixProtocol {

public: /* Methods: */

    FloatToFixProtocol(const Shared3pPDPI& pdpi) { (void) pdpi; }

    template<typename Float, typename Uint>
    typename std::enable_if<is_unsigned_value_tag<Uint>::value &&
                            is_float_value_tag<Float>::value, bool>::type
    invoke(const ShareVec<Float>& param,
           ShareVec<Uint>& result)
    {
        if (param.size() != result.size())
            return false;

        static constexpr uint64_t radix_point =
            Radix<typename Uint::share_type>::value;
        using Int = typename respective_signed_type<Uint>::type::share_type;
        using SFloat = typename Float::share_type;

        for (size_t i = 0u; i < param.size(); ++i) {
            SFloat x = val_to_float<uint64_t>(
                static_cast<uint64_t>(1u) << radix_point,
                typename Float::value_category());
            Int res = float_to_val<Int>(sf_float_mul(param[i], x).result);
            result[i] = *reinterpret_cast<typename Uint::share_type*>(&res);
        }

        return true;
    }
};

class __attribute__ ((visibility("internal"))) FixToFloatProtocol {

public: /* Methods: */

    FixToFloatProtocol(const Shared3pPDPI& pdpi) { (void) pdpi; }

    template<typename Float, typename Uint>
    typename std::enable_if<is_unsigned_value_tag<Uint>::value &&
                            is_float_value_tag<Float>::value, bool>::type
    invoke(const ShareVec<Uint>& param,
           ShareVec<Float>& result)
    {
        if (param.size() != result.size())
            return false;

        static constexpr uint64_t radix_point =
            Radix<typename Uint::share_type>::value;
        using SFloat = typename Float::share_type;

        for (size_t i = 0u; i < param.size(); ++i) {
            SFloat x = val_to_float<uint64_t>(
                static_cast<uint64_t>(1u) << radix_point,
                typename Float::value_category());
            using Int = typename respective_signed_type<Uint>::type::share_type;
            Int paramInt = *reinterpret_cast<const Int*>(&param[i]);
            result[i] = sf_float_div(
                val_to_float<Int>(paramInt, typename Float::value_category()),
                x).result;
        }

        return true;
    }
};

class __attribute__ ((visibility("internal"))) FixMultiplicationProtocol {

private: /* Methods: */

    template<typename DoubleUint, typename Uint>
    DoubleUint promote(Uint x) {
        DoubleUint res = static_cast<DoubleUint>(
            *reinterpret_cast<const Uint*>(&x));

        static constexpr size_t nbits = sizeof(Uint) * 8;

        if (x >> (nbits - 1u)) {
            res ^= (~static_cast<DoubleUint>(0u) << nbits);
        }

        return res;
    }

    uint64_t demote(sharemind::uint128_t x) {
        return x.lower();
    }

    uint32_t demote(uint64_t x) {
        return static_cast<uint32_t>(x);
    }

public: /* Methods: */

    FixMultiplicationProtocol(const Shared3pPDPI& pdpi) { (void) pdpi; }

    template<typename T>
    typename std::enable_if<is_unsigned_value_tag<T>::value, bool>::type
    invoke(const ShareVec<T>& param1,
           const ShareVec<T>& param2,
           ShareVec<T>& result)
    {
        if (param1.size() != result.size() || param2.size() != result.size())
            return false;

        static constexpr uint64_t radix_point =
            Radix<typename T::share_type>::value;

        using Uint = typename T::share_type;
        using DoubleUint = typename double_size<T>::type::share_type;

        for (size_t i = 0u; i < param1.size(); ++i) {
            DoubleUint a = promote<DoubleUint, Uint>(param1[i]);
            DoubleUint b = promote<DoubleUint, Uint>(param2[i]);
            result[i] = demote((a * b) >> radix_point);
        }

        return true;
    }
};

class __attribute__ ((visibility("internal"))) FixInverseProtocol {

public: /* Methods: */

    FixInverseProtocol(const Shared3pPDPI& pdpi)
        : m_pdpi(pdpi) {}

    template<typename Uint>
    typename std::enable_if<is_unsigned_value_tag<Uint>::value, bool>::type
    invoke(const ShareVec<Uint>& param,
           ShareVec<Uint>& result)
    {
        if (param.size() != result.size())
            return false;

        using Float = typename respective_float_type<Uint>::type;

        ShareVec<Float> floats(param.size());

        FixToFloatProtocol(m_pdpi).invoke(param, floats);

        for (size_t i = 0u; i < param.size(); ++i) {
            floats[i] = sf_float_inv(floats[i]).result;
        }

        FloatToFixProtocol(m_pdpi).invoke(floats, result);

        return true;
    }

private: /* Fields: */

    const Shared3pPDPI& m_pdpi;
};

class __attribute__ ((visibility("internal"))) FixSquareRootProtocol {

public: /* Methods: */

    FixSquareRootProtocol(const Shared3pPDPI& pdpi)
        : m_pdpi(pdpi) {}

    template<typename Uint>
    typename std::enable_if<is_unsigned_value_tag<Uint>::value, bool>::type
    invoke(const ShareVec<Uint>& param,
           ShareVec<Uint>& result)
    {
        if (param.size() != result.size())
            return false;

        using Float = typename respective_float_type<Uint>::type;

        ShareVec<Float> floats(param.size());

        FixToFloatProtocol(m_pdpi).invoke(param, floats);

        for (size_t i = 0u; i < param.size(); ++i) {
            floats[i] = sf_float_sqrt(floats[i]).result;
        }

        FloatToFixProtocol(m_pdpi).invoke(floats, result);

        return true;
    }

private: /* Fields: */

    const Shared3pPDPI& m_pdpi;

};

} /* namespace sharemind { */

#endif /* MOD_SHARED3P_EMU_PROTOCOLS_FIXEDPOINT_H */
