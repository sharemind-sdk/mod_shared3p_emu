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

#ifndef MOD_SHARED3P_EMU_PROTOCOLS_UNARY_H
#define MOD_SHARED3P_EMU_PROTOCOLS_UNARY_H

#include <algorithm>
#include "../Shared3pValueTraits.h"


namespace sharemind {

class __attribute__ ((visibility("internal"))) AbsoluteValueProtocol {
public: /* Methods: */

    AbsoluteValueProtocol(Shared3pPDPI & pdpi) { (void) pdpi; }

    template <typename T>
    bool invoke(const s3p_vec<T> & param,
                s3p_vec<typename respective_unsigned_type<T>::type> & result,
                numeric_value_tag)
    {
        if (param.size() != result.size())
            return false;

        for (size_t i = 0u; i < param.size(); ++i)
            result[i] = std::abs(param[i]);

        return true;
    }

    template <typename T>
    bool invoke(const s3p_vec<T> & param, s3p_vec<T> & result,
                float_numeric_value_tag)
    {
        if (param.size() != result.size())
            return false;

        for (size_t i = 0u; i < param.size(); ++i)
            result[i] = sf_float_abs(param[i]);

        return true;
    }

}; /* class AbsoluteValueProtocol { */

class __attribute__ ((visibility("internal"))) BitwiseInvProtocol {
public: /* Methods: */

    BitwiseInvProtocol(Shared3pPDPI & pdpi) { (void) pdpi; }

    template <typename T>
    bool invoke(const s3p_vec<T> & param, s3p_vec<T> & result,
                xored_numeric_value_tag)
    {
        if (param.size() != result.size())
            return false;

        for (size_t i = 0u; i < param.size(); ++i)
            result[i] = ~param[i];

        return true;
    }

}; /* BitwiseInvProtocol { */

class __attribute__ ((visibility("internal"))) BitExtractionProtocol {
public: /* Methods: */

    BitExtractionProtocol(Shared3pPDPI & pdpi) { (void) pdpi; }

    template <typename T>
    bool invoke(const s3p_vec<T> & param, s3p_vec<s3p_bool_t> & result,
                any_value_tag)
    {
        SHAREMIND_STATIC_ASSERT(T::num_of_bits > 0u); /// \todo
        if (result.size () % T::num_of_bits != 0u)
            return false;

        if (param.size () != result.size () / T::num_of_bits)
            return false;

        result.assignBits (param);
        return true;
    }

}; /* BitExtractionProtocol { */

class __attribute__ ((visibility("internal"))) ConversionProtocol {
public: /* Methods: */

    ConversionProtocol(Shared3pPDPI & pdpi) { (void) pdpi; }

    template <typename T, typename U>
    bool invoke(const s3p_vec<T> & param, s3p_vec<U> & result,
                any_value_tag)
    {
        if (param.size() != result.size())
            return false;

        for (size_t i = 0u; i < param.size(); ++i)
            result[i] = param[i];

        return true;
    }

    template <typename T>
    bool invoke(const s3p_vec<T> & param, s3p_vec<s3p_float32_t> & result,
                any_value_tag)
    {
        if (param.size() != result.size())
            return false;

        for (size_t i = 0u; i < param.size(); ++i)
            result[i] = sf_val_to_float32(param[i]).result;

        return true;
    }

    template <typename T>
    bool invoke(const s3p_vec<s3p_float32_t> & param, s3p_vec<T> & result,
                float32_numeric_value_tag)
    {
        if (param.size() != result.size())
            return false;

        for (size_t i = 0u; i < param.size(); ++i)
            result[i] = sf_float32_to_val<typename s3p_vec<T>::value_type>(param[i]).result;

        return true;
    }

    bool invoke(const s3p_vec<s3p_float32_t> & param,
                s3p_vec<s3p_float64_t> & result,
                float32_numeric_value_tag)
    {
        if (param.size() != result.size())
            return false;

        for (size_t i = 0u; i < param.size(); ++i)
            result[i] = sf_float32_to_float64(param[i], sf_fpu_state_default).result;

        return true;
    }

    template <typename T>
    bool invoke(const s3p_vec<T> & param, s3p_vec<s3p_float64_t> & result,
                any_value_tag)
    {
        if (param.size() != result.size())
            return false;

        for (size_t i = 0u; i < param.size(); ++i)
            result[i] = sf_val_to_float64(param[i]).result;

        return true;
    }

    template <typename T>
    bool invoke(const s3p_vec<s3p_float64_t> & param, s3p_vec<T> & result,
                float64_numeric_value_tag)
    {
        if (param.size() != result.size())
            return false;

        for (size_t i = 0u; i < param.size(); ++i)
            result[i] = sf_float64_to_val<typename s3p_vec<T>::value_type>(param[i]).result;

        return true;
    }

    bool invoke(const s3p_vec<s3p_float64_t> & param,
                s3p_vec<s3p_float32_t> & result,
                float64_numeric_value_tag)
    {
        if (param.size() != result.size())
            return false;

        for (size_t i = 0u; i < param.size(); ++i)
            result[i] = sf_float64_to_float32(param[i], sf_fpu_state_default).result;

        return true;
    }

}; /* class ConversionProtocol { */

class __attribute__ ((visibility("internal"))) FloatCeilingProtocol {
public: /* Methods: */

    FloatCeilingProtocol(Shared3pPDPI & pdpi) { (void) pdpi; }

    template <typename T, typename U>
    bool invoke(const s3p_vec<T> & param, s3p_vec<U> & result,
                float_numeric_value_tag)
    {
        if (param.size() != result.size())
            return false;

        for (size_t i = 0u; i < param.size(); ++i) {
            const auto rv = sf_float_ceil(param[i]);
            const auto iv = sf_float_to_int(rv.result);
            result[i] = iv.result;
        }

        return true;
    }
};

class __attribute__ ((visibility("internal"))) FloatErrorFunctionProtocol {
public: /* Methods: */

    FloatErrorFunctionProtocol(Shared3pPDPI & pdpi) { (void) pdpi; }

    template <typename T>
    bool invoke(const s3p_vec<T> & param, s3p_vec<T> & result,
                float_numeric_value_tag)
    {
        if (param.size() != result.size())
            return false;

        for (size_t i = 0u; i < param.size(); ++i)
            result[i] = sf_float_erf(param[i]).result;

        return true;
    }
}; /* class FloatErrorFunctionProtocol { */

class __attribute__ ((visibility("internal"))) FloatFloorProtocol {
public: /* Methods: */

    FloatFloorProtocol(Shared3pPDPI & pdpi) { (void) pdpi; }

    template <typename T, typename U>
    bool invoke(const s3p_vec<T> & param, s3p_vec<U> & result,
                float_numeric_value_tag)
    {
        if (param.size() != result.size())
            return false;

        for (size_t i = 0u; i < param.size(); ++i) {
            const auto rv = sf_float_floor(param[i]);
            const auto iv = sf_float_to_int(rv.result);
            result[i] = iv.result;
        }

        return true;
    }
}; /* class FloatFloorProtocol { */

class __attribute__ ((visibility("internal"))) FloatInverseProtocol {
public: /* Methods: */

    FloatInverseProtocol(Shared3pPDPI & pdpi) { (void) pdpi; }

    template <typename T>
    bool invoke(const s3p_vec<T> & param, s3p_vec<T> & result,
                float_numeric_value_tag)
    {
        if (param.size() != result.size())
            return false;

        for (size_t i = 0u; i < param.size(); ++i)
            result[i] = sf_float_inv(param[i]).result;

        return true;
    }

}; /* class FloatInverseProtocol { */

class __attribute__ ((visibility("internal"))) FloatIsNegligibleProtocol {
public: /* Methods: */

    FloatIsNegligibleProtocol(Shared3pPDPI & pdpi) { (void) pdpi; }

    template <typename T>
    bool invoke(const s3p_vec<T> & param, s3p_vec<s3p_bool_t> & result,
                float_numeric_value_tag)
    {
        if (param.size() != result.size())
            return false;

        for (size_t i = 0u; i < param.size(); ++i) {
            const auto exp = static_cast<size_t>(sf_float_exponent(param[i]));
            result[i] = exp <= (T::bias - 20u); /// \todo
        }

        return true;
    }
}; /* class FloatIsNegligibleProtocol { */

class __attribute__ ((visibility("internal"))) FloatNaturalLogarithmProtocol {
public: /* Methods: */

    FloatNaturalLogarithmProtocol(Shared3pPDPI & pdpi) { (void) pdpi; }

    template <typename T>
    bool invoke(const s3p_vec<T> & param, s3p_vec<T> & result,
                float_numeric_value_tag)
    {
        if (param.size() != result.size())
            return false;

        for (size_t i = 0u; i < param.size(); ++i)
            result[i] = sf_float_log(param[i]).result;

        return true;
    }
}; /* class FloatNaturalLogarithmProtocol { */

class __attribute__ ((visibility("internal"))) FloatPowerOfEProtocol {
public: /* Methods: */

    FloatPowerOfEProtocol(Shared3pPDPI & pdpi) { (void) pdpi; }

    template <typename T>
    bool invoke(const s3p_vec<T> & param, s3p_vec<T> & result,
                float_numeric_value_tag)
    {
        if (param.size() != result.size())
            return false;

        for (size_t i = 0u; i < param.size(); ++i)
            result[i] = sf_float_exp(param[i]).result;

        return true;
    }
}; /* class FloatPowerOfEProtocol { */

class __attribute__ ((visibility("internal"))) FloatSineProtocol {
public: /* Methods: */

    FloatSineProtocol(Shared3pPDPI & pdpi) { (void) pdpi; }

    template <typename T>
    bool invoke(const s3p_vec<T> & param, s3p_vec<T> & result,
                float_numeric_value_tag)
    {
        if (param.size() != result.size())
            return false;

        for (size_t i = 0u; i < param.size(); ++i)
            result[i] = sf_float_sin(param[i]).result;

        return true;
    }
}; /* class FloatSineProtocol { */

class __attribute__ ((visibility("internal"))) FloatSquareRootProtocol {
public: /* Methods: */

    FloatSquareRootProtocol(Shared3pPDPI & pdpi) { (void) pdpi; }

    template <typename T>
    bool invoke(const s3p_vec<T> & param, s3p_vec<T> & result,
                float_numeric_value_tag)
    {
        if (param.size() != result.size())
            return false;

        for (size_t i = 0u; i < param.size(); ++i)
            result[i] = sf_float_sqrt(param[i]).result;

        return true;
    }
}; /* class FloatSquareRootProtocol { */

enum MinimumMaximumMode { /// \todo
    ModeMin,
    ModeMax
};

template <MinimumMaximumMode mode>
class __attribute__ ((visibility("internal"))) MinimumMaximumProtocol {
public: /* Methods: */

    MinimumMaximumProtocol(Shared3pPDPI & pdpi) { (void) pdpi; }

    template <typename T>
    bool invoke(const s3p_vec<T> & param, s3p_vec<T> & result,
                any_value_tag)
    {
        const size_t result_size = result.size();
        const size_t param_size = param.size();

        if (result_size == 0u)
            return false;

        if (param_size % result_size != 0u)
            return false;

        const size_t subarr_len = param_size / result_size;

        auto offset = param.cbegin();
        for (size_t i = 0u; i < result_size; ++i) {
            if (mode == ModeMin) {
                result[i] = *std::min_element(offset, offset + subarr_len);
            } else {
                result[i] = *std::max_element(offset, offset + subarr_len);
            }

            offset += subarr_len;
        }

        return true;
    }

    template <typename T>
    bool invoke(const s3p_vec<T> & param, s3p_vec<T> & result,
                float_numeric_value_tag)
    {
        const size_t result_size = result.size();
        const size_t param_size = param.size();

        if (result_size == 0u)
            return false;

        if (param_size % result_size != 0u)
            return false;

        const size_t subarr_len = param_size / result_size;

        auto comp = [](typename s3p_vec<T>::value_type const & a,
                       typename s3p_vec<T>::value_type const & b)
            { return sf_float_lt(a, b).result != 0u; };

        auto offset = param.cbegin();
        for (size_t i = 0u; i < result_size; ++i) {
            if (mode == ModeMin) {
                result[i] =
                    *std::min_element(offset, offset + subarr_len, comp);
            } else {
                result[i] =
                    *std::max_element(offset, offset + subarr_len, comp);
            }

            offset += subarr_len;
        }

        return true;
    }

}; /* class MinimumMaximumProtocol { */

class __attribute__ ((visibility("internal"))) MostSignificantNonZeroBitProtocol {
public: /* Methods: */

    MostSignificantNonZeroBitProtocol(Shared3pPDPI & pdpi) { (void) pdpi; }

    template <typename T>
    bool invoke(const s3p_vec<T> & param, s3p_vec<T> & result,
                 xored_numeric_value_tag)
    {
        if (param.size() != result.size())
            return false;

        typedef typename value_traits<T>::share_type share_type;

        for (size_t i = 0u; i < param.size(); ++i) {
            share_type value = param[i];
            size_t count = 0;

            for (size_t j = 1u; j <= T::log_of_bits; ++j) {
                share_type bit = (1u << (T::num_of_bits >>  j));
                share_type mask = bit - 1u;
                share_type relmask = (mask << (T::num_of_bits - (T::num_of_bits >> j)));

                if ((value & relmask) == 0u) {
                    count += (1 << (T::log_of_bits -j));
                    value <<= (1 << (T::log_of_bits -j));
                }
            }

            result[i] = 1u << (T::num_of_bits - count - 1u);
        }

        return true;
    }

}; /* class MostSignificantNonZeroBitProtocol { */

class __attribute__ ((visibility("internal"))) NegProtocol {
public: /* Methods: */

    NegProtocol(Shared3pPDPI & pdpi) { (void) pdpi; }

    template <typename T, typename U>
    bool invoke(const s3p_vec<T> & param, s3p_vec<U> & result,
                any_value_tag)
    {
        if (param.size() != result.size())
            return false;

        for (size_t i = 0u; i < param.size(); ++i)
            result[i] = -param[i];

        return true;
    }

    template <typename T>
    bool invoke(const s3p_vec<T> & param, s3p_vec<T> & result,
                float_numeric_value_tag)
    {
        if (param.size() != result.size())
            return false;

        for (size_t i = 0u; i < param.size(); ++i)
            result[i] = sf_float_neg(param[i]);

        return true;
    }

}; /* class NegProtocol { */

class __attribute__ ((visibility("internal"))) NotProtocol {
public: /* Methods: */

    NotProtocol(Shared3pPDPI & pdpi) { (void) pdpi; }

    bool invoke(const s3p_vec<s3p_bool_t> & param,
                s3p_vec<s3p_bool_t> & result,
                any_value_tag)
    {
        if (param.size() != result.size())
            return false;

        for (size_t i = 0u; i < param.size(); ++i)
            result[i] = !param[i];

        return true;
    }

}; /* NotProtocol { */

class __attribute__ ((visibility("internal"))) ProductProtocol {
public: /* Methods: */
    ProductProtocol(Shared3pPDPI & pdpi) { (void) pdpi; }

    template <typename T, typename U>
    bool invoke(const s3p_vec<T> & param, s3p_vec<U> & result, any_value_tag) {
        const size_t param_size = param.size ();
        const size_t result_size = result.size ();
        if (result_size == 0u)
            return false;

        if (param_size == 0u) {
            if (result_size != 1)
                return false;
            result[0] = 0;
            return true;
        }

        if (param_size % result_size != 0u)
            return false;

        for (size_t i = 0u; i < result_size; ++i) {
            result[i] = 0;
        }

        const size_t subarr_len = param_size / result_size;
        for (size_t i = 0u; i < param_size; ++i) {
            result[i / subarr_len] *= param[i];
        }

        return true;
    }

}; /* class ProductProtocol { */

class __attribute__ ((visibility("internal"))) SignProtocol {
public: /* Methods: */

    SignProtocol(Shared3pPDPI & pdpi) { (void) pdpi; }

    template <typename T>
    bool invoke(const s3p_vec<T> & param, s3p_vec<T> & result,
                numeric_value_tag)
    {
        if (param.size() != result.size())
            return false;

        for (size_t i = 0u; i < param.size(); ++i)
            result[i] = (param[i] > 0) ? 1 : ((param[i] < 0) ? -1 : 0);

        return true;
    }

}; /* class SignProtocol { */

class __attribute__ ((visibility("internal"))) SumProtocol {
public: /* Methods: */

    SumProtocol(Shared3pPDPI & pdpi) { (void) pdpi; }

    template <typename T, typename U>
    bool invoke(const s3p_vec<T> & param, s3p_vec<U> & result,
                any_value_tag)
    {
        const size_t param_size = param.size ();
        const size_t result_size = result.size ();
        if (result_size == 0u)
            return false;

        if (param_size == 0u) {
            if (result_size != 1)
                return false;
            result[0] = 0;
            return true;
        }

        if (param_size % result_size != 0u)
            return false;

        for (size_t i = 0u; i < result_size; ++i) {
            result[i] = 0;
        }

        const size_t subarr_len = param_size / result_size;
        for (size_t i = 0u; i < param_size; ++i) {
            result[i / subarr_len] += param[i];
        }

        return true;
    }

    template <typename T>
    bool invoke(const s3p_vec<T> & param, s3p_vec<T> & result,
                float_numeric_value_tag)
    {
        const size_t param_size = param.size ();
        const size_t result_size = result.size ();
        if (result_size == 0u)
            return false;

        if (param_size == 0u) {
            if (result_size != 1)
                return false;
            result[0] = 0;
            return true;
        }

        if (param_size % result_size != 0u)
            return false;

        for (size_t i = 0u; i < result_size; ++i)
            result[i] = 0;

        const size_t subarr_len = param_size / result_size;
        for (size_t i = 0u; i < param_size; ++i)
            result[i / subarr_len] = sf_float_add(result[i / subarr_len], param[i]).result;

        return true;
    }

}; /* class SumProtocol { */

} /* namespace sharemind { */

#endif /* MOD_SHARED3P_EMU_PROTOCOLS_UNARY_H */
