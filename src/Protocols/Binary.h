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

#ifndef MOD_SHARED3P_EMU_PROTOCOLS_BINARY_H
#define MOD_SHARED3P_EMU_PROTOCOLS_BINARY_H

#include "SoftFloatUtility.h"
#include "../Shared3pValueTraits.h"
#include "../VMReferences.h"


namespace sharemind {

class __attribute__ ((visibility("internal"))) AdditionProtocol {
public: /* Methods: */

    AdditionProtocol(Shared3pPDPI & pdpi) { (void) pdpi; }

    template <typename T>
    bool invoke(const s3p_vec<T> & param1, const s3p_vec<T> & param2,
                s3p_vec<T> & result, numeric_value_tag)
    {
        if (param1.size() != param2.size() || param1.size() != result.size())
            return false;

        for (size_t i = 0u; i < param1.size(); ++i)
            result[i] = param1[i] + param2[i];

        return true;
    }

    template <typename T>
    bool invoke(const s3p_vec<T> & param1, const s3p_vec<T> & param2,
                s3p_vec<T> & result, float_numeric_value_tag)
    {
        if (param1.size() != param2.size() || param1.size() != result.size())
            return false;

        for (size_t i = 0u; i < param1.size(); ++i)
            result[i] = sf_float_add(param1[i], param2[i]).result;

        return true;
    }

}; /* class AdditionProtocol { */

class __attribute__ ((visibility("internal"))) BitwiseAndProtocol {
public: /* Methods: */

    BitwiseAndProtocol(Shared3pPDPI & pdpi) { (void) pdpi; }

    template <typename T>
    bool invoke(const s3p_vec<T> & param1, const s3p_vec<T> & param2,
                s3p_vec<T> & result, any_value_tag)
    {
        if (param1.size() != param2.size() || param1.size() != result.size())
            return false;

        for (size_t i = 0u; i < param1.size(); ++i)
            result[i] = param1[i] & param2[i];

        return true;
    }

    template <typename T>
    bool invoke(const s3p_vec<T> & param1, const s3p_vec<T> & param2,
                s3p_vec<T> & result, bool_value_tag)
    {
        if (param1.size() != param2.size() || param1.size() != result.size())
            return false;

        for (size_t i = 0u; i < param1.size(); ++i)
            result[i] = param1[i] && param2[i];

        return true;
    }

}; /* class BitwiseAndProtocol { */

class __attribute__ ((visibility("internal"))) BitwiseOrProtocol {
public: /* Methods: */

    BitwiseOrProtocol(Shared3pPDPI & pdpi) { (void) pdpi; }

    template <typename T>
    bool invoke(const s3p_vec<T> & param1, const s3p_vec<T> & param2,
                s3p_vec<T> & result, any_value_tag)
    {
        if (param1.size() != param2.size() || param1.size() != result.size())
            return false;

        for (size_t i = 0u; i < param1.size(); ++i)
            result[i] = param1[i] | param2[i];

        return true;
    }

    template <typename T>
    bool invoke(const s3p_vec<T> & param1, const s3p_vec<T> & param2,
                s3p_vec<T> & result, bool_value_tag)
    {
        if (param1.size() != param2.size() || param1.size() != result.size())
            return false;

        for (size_t i = 0u; i < param1.size(); ++i)
            result[i] = param1[i] || param2[i];

        return true;
    }

}; /* class BitwiseOrProtocol { */

class __attribute__ ((visibility("internal"))) BitwiseXorProtocol {
public: /* Methods: */

    BitwiseXorProtocol(Shared3pPDPI & pdpi) { (void) pdpi; }

    template <typename T>
    bool invoke(const s3p_vec<T> & param1, const s3p_vec<T> & param2,
                s3p_vec<T> & result, any_value_tag)
    {
        if (param1.size() != param2.size() || param1.size() != result.size())
            return false;

        for (size_t i = 0u; i < param1.size(); ++i)
            result[i] = param1[i] ^ param2[i];

        return true;
    }

}; /* class BitwiseXorProtocol { */

class __attribute__ ((visibility("internal"))) DivisionProtocol {
public: /* Methods: */

    DivisionProtocol(Shared3pPDPI & pdpi) { (void) pdpi; }

    template <typename T>
    bool invoke(const s3p_vec<T> & param1, const s3p_vec<T> & param2,
                s3p_vec<T> & result, numeric_value_tag)
    {
        if (param1.size() != param2.size() || param1.size() != result.size())
            return false;

        for (size_t i = 0u; i < param2.size(); ++i) {
            if (param2[i] == 0)
                return false;
        }

        for (size_t i = 0u; i < param1.size(); ++i)
            result[i] = param1[i] / param2[i];

        return true;
    }

    template <typename T>
    bool invoke(const s3p_vec<T> & param1, const s3p_vec<T> & param2,
                s3p_vec<T> & result, float_numeric_value_tag)
    {
        if (param1.size() != param2.size() || param1.size() != result.size())
            return false;

        for (size_t i = 0u; i < param2.size(); ++i) {
            if (param2[i] == 0x0)
                return false;
        }

        for (size_t i = 0u; i < param1.size(); ++i)
            result[i] = sf_float_div(param1[i], param2[i]).result;

        return true;
    }

    template <typename T>
    bool invoke(const s3p_vec<T> & param1, const immutable_vm_vec<T> & param2,
                s3p_vec<T> & result, numeric_value_tag)
    {
        if (param1.size() > param2.size() || param1.size() != result.size())
            return false;

        for (size_t i = 0u; i < param2.size(); ++i) {
            if (param2[i] == 0)
                return false;
        }

        for (size_t i = 0u; i < param1.size(); ++i)
            result[i] = param1[i] / param2[i];

        return true;
    }

    template <typename T>
    bool invoke(const s3p_vec<T> & param1, const immutable_vm_vec<T> & param2,
                s3p_vec<T> & result, float_numeric_value_tag)
    {
        if (param1.size() > param2.size() || param1.size() != result.size())
            return false;

        for (size_t i = 0u; i < param2.size(); ++i) {
            if (param2[i] == 0x0)
                return false;
        }

        for (size_t i = 0u; i < param1.size(); ++i)
            result[i] = sf_float_div(param1[i], param2[i]).result;

        return true;
    }

}; /* class DivisionProtocol { */

class __attribute__ ((visibility("internal"))) EqualityProtocol {
public: /* Methods: */

    EqualityProtocol(Shared3pPDPI & pdpi) { (void) pdpi; }

    template <typename T>
    bool invoke(const s3p_vec<T> & param1, const s3p_vec<T> & param2,
                s3p_vec<s3p_bool_t> & result, any_value_tag)
    {
        if (param1.size() != param2.size() || param1.size() != result.size())
            return false;

        for (size_t i = 0u; i < param1.size(); ++i)
            result[i] = param1[i] == param2[i];

        return true;
    }

    template <typename T>
    bool invoke(const s3p_vec<T> & param1, const s3p_vec<T> & param2,
                s3p_vec<s3p_bool_t> & result, float_numeric_value_tag)
    {
        if (param1.size() != param2.size() || param1.size() != result.size())
            return false;

        for (size_t i = 0u; i < param1.size(); ++i)
            result[i] = sf_float_eq(param1[i], param2[i]).result;

        return true;
    }

}; /* class EqualityProtocol { */

class __attribute__ ((visibility("internal"))) GreaterThanProtocol {
public: /* Methods: */

    GreaterThanProtocol(Shared3pPDPI & pdpi) { (void) pdpi; }

    template <typename T>
    bool invoke(const s3p_vec<T> & param1, const s3p_vec<T> & param2,
                s3p_vec<s3p_bool_t> & result, any_value_tag)
    {
        if (param1.size() != param2.size() || param1.size() != result.size())
            return false;

        for (size_t i = 0u; i < param1.size(); ++i)
            result[i] = param1[i] > param2[i];

        return true;
    }

    template <typename T>
    bool invoke(const s3p_vec<T> & param1, const s3p_vec<T> & param2,
                s3p_vec<s3p_bool_t> & result, float_numeric_value_tag)
    {
        if (param1.size() != param2.size() || param1.size() != result.size())
            return false;

        for (size_t i = 0u; i < param1.size(); ++i)
            result[i] = sf_float_lt(param2[i], param1[i]).result;

        return true;
    }

}; /* class GreaterThanProtocol { */

class __attribute__ ((visibility("internal"))) GreaterThanOrEqualProtocol {
public: /* Methods: */

    GreaterThanOrEqualProtocol(Shared3pPDPI & pdpi) { (void) pdpi; }

    template <typename T>
    bool invoke(const s3p_vec<T> & param1, const s3p_vec<T> & param2,
                s3p_vec<s3p_bool_t> & result, any_value_tag)
    {
        if (param1.size() != param2.size() || param1.size() != result.size())
            return false;

        for (size_t i = 0u; i < param1.size(); ++i)
            result[i] = param1[i] >= param2[i];

        return true;
    }

    template <typename T>
    bool invoke(const s3p_vec<T> & param1, const s3p_vec<T> & param2,
                s3p_vec<s3p_bool_t> & result, float_numeric_value_tag)
    {
        if (param1.size() != param2.size() || param1.size() != result.size())
            return false;

        for (size_t i = 0u; i < param1.size(); ++i)
            result[i] = sf_float_le(param2[i], param1[i]).result;

        return true;
    }

}; /* class GreaterThanOrEqualProtocol { */

class __attribute__ ((visibility("internal"))) LeftRotationProtocol {
public: /* Methods: */

    LeftRotationProtocol(Shared3pPDPI & pdpi) { (void) pdpi; }

    template <typename T>
    bool invoke(const s3p_vec<T> & param1,
                const immutable_vm_vec<s3p_int64_t> & param2,
                s3p_vec<T> & result,
                xored_numeric_value_tag)
    {
        if (param1.size() > param2.size() || param1.size() != result.size())
            return false;

        constexpr const size_t n =
            8u * sizeof(typename value_traits<T>::share_type);

        for (size_t i = 0u; i < param1.size(); ++i) {
            const auto val = param1[i];
            const auto k = param2[i];

            if (k < 0) {
                const auto kmod = (-k) % n;
                result[i] = (val >> kmod) | (val << (n - kmod));
            } else {
                const auto kmod = k % n;
                result[i] = (val << kmod) | (val >> (n - kmod));
            }
        }

        return true;
    }

}; /* class LeftRotationProtocol { */

class __attribute__ ((visibility("internal"))) LeftShiftProtocol {
public: /* Methods: */

    LeftShiftProtocol(Shared3pPDPI & pdpi) { (void) pdpi; }

    template <typename T>
    bool invoke(const s3p_vec<T> & param1,
                const immutable_vm_vec<s3p_int64_t> & param2,
                s3p_vec<T> & result,
                xored_numeric_value_tag)
    {
        if (param1.size() > param2.size() || param1.size() != result.size())
            return false;

        constexpr const size_t n =
            8u * sizeof(typename value_traits<T>::share_type);

        for (size_t i = 0u; i < param1.size(); ++i) {
            const auto val = param1[i];
            const auto k = param2[i];

            if (k < 0) {
                if (static_cast<size_t>(-k) < n)
                    result[i] = val >> (-k);
            } else {
                if (static_cast<size_t>(k) < n)
                    result[i] = val << k;
            }
        }

        return true;
    }

}; /* class LeftShiftProtocol { */

class __attribute__ ((visibility("internal"))) LessThanProtocol {
public: /* Methods: */

    LessThanProtocol(Shared3pPDPI & pdpi) { (void) pdpi; }

    template <typename T>
    bool invoke(const s3p_vec<T> & param1, const s3p_vec<T> & param2,
                s3p_vec<s3p_bool_t> & result, any_value_tag)
    {
        if (param1.size() != param2.size() || param1.size() != result.size())
            return false;

        for (size_t i = 0u; i < param1.size(); ++i)
            result[i] = param1[i] < param2[i];

        return true;
    }

    template <typename T>
    bool invoke(const s3p_vec<T> & param1, const s3p_vec<T> & param2,
                s3p_vec<s3p_bool_t> & result, float_numeric_value_tag)
    {
        if (param1.size() != param2.size() || param1.size() != result.size())
            return false;

        for (size_t i = 0u; i < param1.size(); ++i)
            result[i] = sf_float_lt(param1[i], param2[i]).result;

        return true;
    }

}; /* class LessThanProtocol { */

class __attribute__ ((visibility("internal"))) LessThanOrEqualProtocol {
public: /* Methods: */

    LessThanOrEqualProtocol(Shared3pPDPI & pdpi) { (void) pdpi; }

    template <typename T>
    bool invoke(const s3p_vec<T> & param1, const s3p_vec<T> & param2,
                s3p_vec<s3p_bool_t> & result, any_value_tag)
    {
        if (param1.size() != param2.size() || param1.size() != result.size())
            return false;

        for (size_t i = 0u; i < param1.size(); ++i)
            result[i] = param1[i] <= param2[i];

        return true;
    }

    template <typename T>
    bool invoke(const s3p_vec<T> & param1, const s3p_vec<T> & param2,
                s3p_vec<s3p_bool_t> & result, float_numeric_value_tag)
    {
        if (param1.size() != param2.size() || param1.size() != result.size())
            return false;

        for (size_t i = 0u; i < param1.size(); ++i)
            result[i] = sf_float_le(param1[i], param2[i]).result;

        return true;
    }

}; /* class LessThanOrEqualProtocol { */

class __attribute__ ((visibility("internal"))) MaximumProtocol {
public: /* Methods: */

    MaximumProtocol(Shared3pPDPI & pdpi) { (void) pdpi; }

    template <typename T>
    bool invoke(const s3p_vec<T> & param1, const s3p_vec<T> & param2,
                s3p_vec<T> & result, any_value_tag)
    {
        if (param1.size() != param2.size() || param1.size() != result.size())
            return false;

        for (size_t i = 0u; i < param1.size(); ++i)
            result[i] = param1[i] > param2[i] ? param1[i] : param2[i];

        return true;
    }

    template <typename T>
    bool invoke(const s3p_vec<T> & param1, const s3p_vec<T> & param2,
                s3p_vec<T> & result, float_numeric_value_tag)
    {
        if (param1.size() != param2.size() || param1.size() != result.size())
            return false;

        for (size_t i = 0u; i < param1.size(); ++i)
            result[i] = sf_float_lt(param2[i], param1[i]).result ? param1[i] : param2[i];

        return true;
    }

}; /* class MaximumProtocol { */

class __attribute__ ((visibility("internal"))) MinimumProtocol {
public: /* Methods: */

    MinimumProtocol(Shared3pPDPI & pdpi) { (void) pdpi; }

    template <typename T>
    bool invoke(const s3p_vec<T> & param1, const s3p_vec<T> & param2,
                s3p_vec<T> & result, any_value_tag)
    {
        if (param1.size() != param2.size() || param1.size() != result.size())
            return false;

        for (size_t i = 0u; i < param1.size(); ++i)
            result[i] = param1[i] < param2[i] ? param1[i] : param2[i];

        return true;
    }

    template <typename T>
    bool invoke(const s3p_vec<T> & param1, const s3p_vec<T> & param2,
                s3p_vec<T> & result, float_numeric_value_tag)
    {
        if (param1.size() != param2.size() || param1.size() != result.size())
            return false;

        for (size_t i = 0u; i < param1.size(); ++i)
            result[i] = sf_float_lt(param1[i], param2[i]).result ? param1[i] : param2[i];

        return true;
    }

}; /* class MinimumProtocol { */

class __attribute__ ((visibility("internal"))) MultiplicationProtocol {
public: /* Methods: */

    MultiplicationProtocol(Shared3pPDPI & pdpi) { (void) pdpi; }

    template <typename T>
    bool invoke(const s3p_vec<T> & param1, const s3p_vec<T> & param2,
                s3p_vec<T> & result, numeric_value_tag)
    {
        if (param1.size() != param2.size() || param1.size() != result.size())
            return false;

        for (size_t i = 0u; i < param1.size(); ++i)
            result[i] = param1[i] * param2[i];

        return true;
    }

    template <typename T>
    bool invoke(const s3p_vec<T> & param1, const s3p_vec<T> & param2,
                s3p_vec<T> & result, float_numeric_value_tag)
    {
        if (param1.size() != param2.size() || param1.size() != result.size())
            return false;

        for (size_t i = 0u; i < param1.size(); ++i)
            result[i] = sf_float_mul(param1[i], param2[i]).result;

        return true;
    }

    template <typename T>
    bool invoke(const s3p_vec<T> & param1, const immutable_vm_vec<T> & param2,
                s3p_vec<T> & result, numeric_value_tag)
    {
        if (param1.size() > param2.size() || param1.size() != result.size())
            return false;

        for (size_t i = 0u; i < param1.size(); ++i)
            result[i] = param1[i] * param2[i];

        return true;
    }

    template <typename T>
    bool invoke(const s3p_vec<T> & param1, const immutable_vm_vec<T> & param2,
                s3p_vec<T> & result, float_numeric_value_tag)
    {
        if (param1.size() > param2.size() || param1.size() != result.size())
            return false;

        for (size_t i = 0u; i < param1.size(); ++i)
            result[i] = sf_float_mul(param1[i], param2[i]).result;

        return true;
    }

}; /* class MultiplicationProtocol { */

class __attribute__ ((visibility("internal"))) RemainderProtocol {
public: /* Methods: */

    RemainderProtocol(Shared3pPDPI & pdpi) { (void) pdpi; }

    template <typename T>
    bool invoke(const s3p_vec<T> & param1, const s3p_vec<T> & param2,
                s3p_vec<T> & result, numeric_value_tag)
    {
        if (param1.size() != param2.size() || param1.size() != result.size())
            return false;

        for (size_t i = 0u; i < param2.size(); ++i) {
            if (param2[i] == 0)
                return false;
        }

        for (size_t i = 0u; i < param1.size(); ++i)
            result[i] = param1[i] % param2[i];

        return true;
    }

    template <typename T>
    bool invoke(const s3p_vec<T> & param1, const immutable_vm_vec<T> & param2,
                s3p_vec<T> & result, numeric_value_tag)
    {
        if (param1.size() > param2.size() || param1.size() != result.size())
            return false;

        for (size_t i = 0u; i < param2.size(); ++i) {
            if (param2[i] == 0)
                return false;
        }

        for (size_t i = 0u; i < param1.size(); ++i)
            result[i] = param1[i] % param2[i];

        return true;
    }

}; /* class RemainderProtocol { */

class __attribute__ ((visibility("internal"))) SubtractionProtocol {
public: /* Methods: */

    SubtractionProtocol(Shared3pPDPI & pdpi) { (void) pdpi; }

    template <typename T>
    bool invoke(const s3p_vec<T> & param1, const s3p_vec<T> & param2,
                s3p_vec<T> & result, numeric_value_tag)
    {
        if (param1.size() != param2.size() || param1.size() != result.size())
            return false;

        for (size_t i = 0u; i < param1.size(); ++i)
            result[i] = param1[i] - param2[i];

        return true;
    }

    template <typename T>
    bool invoke(const s3p_vec<T> & param1, const s3p_vec<T> & param2,
                s3p_vec<T> & result, float_numeric_value_tag)
    {
        if (param1.size() != param2.size() || param1.size() != result.size())
            return false;

        for (size_t i = 0u; i < param1.size(); ++i)
            result[i] = sf_float_sub(param1[i], param2[i]).result;

        return true;
    }

}; /* class SubtractionProtocol { */

} /* namespace sharemind { */

#endif /* MOD_SHARED3P_EMU_PROTOCOLS_BINARY_H */
