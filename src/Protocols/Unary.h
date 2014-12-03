/*
 * This file is a part of the Sharemind framework.
 * Copyright (C) Cybernetica AS
 *
 * All rights are reserved. Reproduction in whole or part is prohibited
 * without the written consent of the copyright owner. The usage of this
 * code is subject to the appropriate license agreement.
 */

#ifndef MOD_SHARED3P_EMU_PROTOCOLS_UNARY_H
#define MOD_SHARED3P_EMU_PROTOCOLS_UNARY_H

#include <algorithm>
#include "../Shared3pValueTraits.h"

namespace sharemind {

class __attribute__ ((visibility("internal"))) ConversionProtocol {
public: /* Methods: */

    ConversionProtocol (Shared3pPDPI &pdpi) { (void) pdpi; }

    template <typename DestT, typename SourceT>
    bool invoke (const s3p_vec<SourceT>& param, s3p_vec<DestT>& result, any_value_tag)
    {
        if (param.size() != result.size())
            return false;
        for (size_t i = 0u; i < param.size(); ++i)
            result[i] = static_cast<typename value_traits<DestT>::public_type> (param[i]);
        return true;
    }

    template <typename DestT, typename SourceT>
    bool invoke (const s3p_vec<SourceT>& param, s3p_vec<s3p_bool_t>& result, bool_value_tag)
    {
        if (param.size() != result.size())
            return false;
        for (size_t i = 0u; i < param.size(); ++i)
            result[i] = param[i];
        return true;
    }
}; /* class ConversionProtocol { */

class __attribute__ ((visibility("internal"))) SumProtocol {
public: /* Methods: */

    SumProtocol (Shared3pPDPI &pdpi) { (void) pdpi; }

    template <typename DestT, typename SourceT>
    bool invoke (const s3p_vec<SourceT>& param, s3p_vec<DestT>& result, any_value_tag)
    {
        const size_t param_size = param.size ();
        const size_t result_size = result.size ();
        if (result_size == 0)
            return false;

        if ((s3p_vec<DestT>*)&param == &result)
            return true;

        if (param_size == 0) {
            if (result_size != 1)
                return false;
            result[0] = 0;
            return true;
        }

        if (param_size % result_size != 0)
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

}; /* class SumProtocol { */

class __attribute__ ((visibility("internal"))) ProductProtocol {
public: /* Methods: */
    ProductProtocol (Shared3pPDPI &pdpi) { (void) pdpi; }

    template <typename DestT, typename SourceT>
    bool invoke (const s3p_vec<SourceT>& param, s3p_vec<DestT>& result, any_value_tag)
    {
        const size_t param_size = param.size ();
        const size_t result_size = result.size ();
        if (result_size == 0)
            return false;

        if ((s3p_vec<DestT>*)&param == &result)
            return true;

        if (param_size == 0) {
            if (result_size != 1)
                return false;
            result[0] = 0;
            return true;
        }

        if (param_size % result_size != 0)
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

class __attribute__ ((visibility("internal"))) NegProtocol {
public: /* Methods: */

    NegProtocol (Shared3pPDPI &pdpi) { (void) pdpi; }

    template <typename DestT, typename SourceT>
    bool invoke (const s3p_vec<SourceT>& param, s3p_vec<DestT>& result, any_value_tag)
    {
        if (param.size() != result.size())
            return false;
        for (size_t i = 0u; i < param.size(); ++i)
            result[i] = static_cast<typename value_traits<DestT>::public_type> (- param[i]);
        return true;
    }
}; /* class NegProtocol { */

class __attribute__ ((visibility("internal"))) NotProtocol {
public: /* Methods: */

    NotProtocol (Shared3pPDPI &pdpi) { (void) pdpi; }

    bool invoke (const s3p_vec<s3p_bool_t>& param, s3p_vec<s3p_bool_t>& result, any_value_tag)
    {
        if (param.size() != result.size())
            return false;
        for (size_t i = 0u; i < param.size(); ++i)
            result[i] = !param[i];
        return true;
    }
}; /* NotProtocol { */

class __attribute__ ((visibility("internal"))) BitwiseInvProtocol {
public: /* Methods: */

    BitwiseInvProtocol (Shared3pPDPI &pdpi) { (void) pdpi; }

    template <typename T>
    bool invoke (const s3p_vec<T>& param, s3p_vec<T>& result, xored_numeric_value_tag)
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

    BitExtractionProtocol (Shared3pPDPI &pdpi) { (void) pdpi; }

    template <typename SourceT>
    bool invoke (const s3p_vec<SourceT>& param, s3p_vec<s3p_bool_t>& result, any_value_tag)
    {
        enum { num_of_bits = value_traits<SourceT>::num_of_bits };
        SHAREMIND_STATIC_ASSERT (num_of_bits > 0);
        if (result.size () % num_of_bits != 0)
            return false;
        if (param.size () != result.size () / num_of_bits)
            return false;
        result.assignBits (param);
        return true;
    }
}; /* BitExtractionProtocol { */

class __attribute__ ((visibility("internal"))) SignProtocol {
public: /* Methods: */

    SignProtocol (Shared3pPDPI &pdpi) { (void) pdpi; }

    template <typename SourceT>
    bool invoke (const s3p_vec<SourceT>& param, s3p_vec<SourceT>& result, numeric_value_tag)
    {
        if (param.size() != result.size())
            return false;

        for (size_t i = 0u; i < param.size(); ++i)
            result[i] = (param[i] > 0) ? 1 : ((param[i] < 0) ? -1 : 0);
        return true;
    }
}; /* SignProtocol { */

class __attribute__ ((visibility("internal"))) AbsoluteValueProtocol {
public: /* Methods: */

    AbsoluteValueProtocol (Shared3pPDPI &pdpi) { (void) pdpi; }

    template <typename SourceT>
    bool invoke (const s3p_vec<SourceT>& param,
                 s3p_vec<typename respective_unsigned_type<SourceT>::type>& result, any_value_tag)
    {
        if (param.size() != result.size())
            return false;

        for (size_t i = 0u; i < param.size(); ++i)
            result[i] = abs(param[i]);
        return true;
    }
}; /* class AbsoluteValueProtocol { */

enum MinimumMaximumMode {
    ModeMin,
    ModeMax
};

template <MinimumMaximumMode mode>
class __attribute__ ((visibility("internal"))) MinimumMaximumProtocol {
public: /* Methods: */

        MinimumMaximumProtocol (Shared3pPDPI &pdpi) { (void) pdpi; }

        template <typename T>
        bool invoke (const s3p_vec<T>& param, s3p_vec<T>& result, any_value_tag)
        {
            const size_t result_size = result.size();
            const size_t param_size = param.size();
            if (result.size() == 0)
                return false;
            else if (param.size() % result.size() == 0) {
                const size_t subarr_len = param_size / result_size;
                for (size_t i = 0u; i < param_size; ++i) {
                    if (i == subarr_len)
                        continue;
                    if (mode == ModeMin)
                        result[i / subarr_len] = std::min(param[i], param[i+1]);
                    else
                        result[i / subarr_len] = std::max(param[i], param[i+1]);
                }
                return true;
            }
            return false;
        }
}; /* class MinimumMaximumProtocol { */

class __attribute__ ((visibility("internal"))) MostSignificantNonZeroBitProtocol {
public: /* Methods: */

    MostSignificantNonZeroBitProtocol (Shared3pPDPI &pdpi) { (void) pdpi; }

    //This should return an number with the most significant non zero bit set and all the rest unset.
    template <typename SourceT>
    bool invoke (const s3p_vec<SourceT>& param, s3p_vec<SourceT>& result,
                 xored_numeric_value_tag)
    {
        if (param.size() != result.size())
            return false;

        enum {
            log_of_bits = value_traits<SourceT>::log_of_bits,
            num_of_bits = value_traits<SourceT>::num_of_bits
        };
        typedef typename value_traits<SourceT>::share_type share_type;

        for (size_t i = 0u; i < param.size(); ++i) {
            share_type value = param[i];
            size_t count = 0;
            for (size_t j = 1u; j <= log_of_bits; ++j) {
                share_type bit = (1 << (num_of_bits >>  j));
                share_type mask = bit - 1;
                share_type relmask = (mask << (num_of_bits - (num_of_bits >> j)));
                if ((value & relmask) == 0) {
                    count += (1 << (log_of_bits -j));
                    value <<= (1 << (log_of_bits -j));
                }
            }
            result[i] = 1 << (num_of_bits - count - 1);
        }

        return true;
    }

}; /* class MostSignificantNonZeroBitProtocol { */

} /* namespace sharemind { */

#endif /* MOD_SHARED3P_EMU_PROTOCOLS_UNARY_H */
