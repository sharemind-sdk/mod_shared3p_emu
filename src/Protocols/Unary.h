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
        public:
            ConversionProtocol (Shared3pPDPI &pdpi) { (void) pdpi; }
        public:
            template <typename DestT, typename SourceT>
                bool invoke (const s3p_vec<SourceT>& param, s3p_vec<DestT>& result,
                        any_value_tag)
                {
                    if (param.size() != result.size())
                        return false;
                    for (size_t i = 0u; i < param.size(); ++i)
                        result[i] = static_cast<typename value_traits<DestT>::public_type> (param[i]);
                    return true;
                }

            template <typename DestT, typename SourceT>
                bool invoke (const s3p_vec<SourceT>& param, s3p_vec<s3p_bool_t>& result,
                        bool_value_tag)
                {
                    if (param.size() != result.size())
                        return false;
                    for (size_t i = 0u; i < param.size(); ++i)
                        result[i] = param[i];
                    return true;
                }
    };

    class __attribute__ ((visibility("internal"))) SumProtocol {
        public:
            SumProtocol (Shared3pPDPI &pdpi) { (void) pdpi; }
        public:
            template <typename DestT, typename SourceT>
                bool invoke (const s3p_vec<SourceT>& param, s3p_vec<DestT>& result,
                        any_value_tag)
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

    };

    class __attribute__ ((visibility("internal"))) ProductProtocol {
        public:
            ProductProtocol (Shared3pPDPI &pdpi) { (void) pdpi; }
        public:
            template <typename DestT, typename SourceT>
                bool invoke (const s3p_vec<SourceT>& param, s3p_vec<DestT>& result,
                        any_value_tag)
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

    };

    class __attribute__ ((visibility("internal"))) NegProtocol {
        public:
            NegProtocol (Shared3pPDPI &pdpi) { (void) pdpi; }
        public:
            template <typename DestT, typename SourceT>
                bool invoke (const s3p_vec<SourceT>& param, s3p_vec<DestT>& result,
                        any_value_tag)
                {
                    if (param.size() != result.size())
                        return false;
                    for (size_t i = 0u; i < param.size(); ++i)
                        result[i] = static_cast<typename value_traits<DestT>::public_type> (- param[i]);
                    return true;
                }
    };

    class __attribute__ ((visibility("internal"))) NotProtocol {
        public:
            NotProtocol (Shared3pPDPI &pdpi) { (void) pdpi; }
        public:
            bool invoke (const s3p_vec<s3p_bool_t>& param, s3p_vec<s3p_bool_t>& result,
                    any_value_tag)
            {
                if (param.size() != result.size())
                    return false;
                for (size_t i = 0u; i < param.size(); ++i)
                    result[i] = !param[i];
                return true;
            }
    };

    class __attribute__ ((visibility("internal"))) BitExtractionProtocol {
        public:
            BitExtractionProtocol (Shared3pPDPI &pdpi) { (void) pdpi; }
        public:
            template <typename SourceT>
                bool invoke (const s3p_vec<SourceT>& param, s3p_vec<s3p_bool_t>& result,
                        any_value_tag)
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
    };

    class __attribute__ ((visibility("internal"))) SignProtocol {
        public:
            SignProtocol (Shared3pPDPI &pdpi) { (void) pdpi; }
        public:
            template <typename SourceT>
                bool invoke (const s3p_vec<SourceT>& param, s3p_vec<SourceT>& result,
                        numeric_value_tag)
                {
                    if (param.size() != result.size())
                        return false;

                    for (size_t i = 0u; i < param.size(); ++i)
                        result[i] = (param[i] > 0) ? 1 : ((param[i] < 0) ? -1 : 0);
                    return true;
                }
    };

    class __attribute__ ((visibility("internal"))) AbsoluteValueProtocol {
        public:
            AbsoluteValueProtocol (Shared3pPDPI &pdpi) { (void) pdpi; }
        public:
            template <typename SourceT>
                bool invoke (const s3p_vec<SourceT>& param,
                                s3p_vec<typename respective_unsigned_type<SourceT>::type>& result,
                                any_value_tag)
                {
                    if (param.size() != result.size())
                        return false;

                    for (size_t i = 0u; i < param.size(); ++i)
                        result[i] = abs(param[i]);
                    return true;
                }
    };

    enum MinimumMaximumMode {
        ModeMin,
        ModeMax
    };

template <MinimumMaximumMode mode>
    class __attribute__ ((visibility("internal"))) MinimumMaximumProtocol {
        public:
            MinimumMaximumProtocol (Shared3pPDPI &pdpi) {}
        public:
            template <typename T>
                bool invoke (const s3p_vec<T>& param,
                                s3p_vec<T>& result,
                                any_value_tag)
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
    };
}

#endif /* MOD_SHARED3P_EMU_PROTOCOLS_UNARY_H */
