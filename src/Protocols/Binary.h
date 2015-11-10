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

#include <sharemind/libemulator_protocols/Binary.h>
#include <sharemind/libemulator_protocols/VmVector.h>
#include <type_traits>
#include "../Shared3pPDPI.h"
#include "../ShareVector.h"
#include "../ValueTraits.h"
#include "SoftFloatUtility.h"


namespace sharemind {

template<>
class __attribute__ ((visibility("internal"))) AdditionProtocol<Shared3pPDPI> {
public: /* Methods: */

    AdditionProtocol(Shared3pPDPI & pdpi) { (void) pdpi; }

    template <typename T>
    typename std::enable_if<is_integral_value_tag<T>::value, bool>::type
    invoke(const share_vec<T> & param1,
           const share_vec<T> & param2,
           share_vec<T> & result)
    {
        if (param1.size() != param2.size() || param1.size() != result.size())
            return false;

        for (size_t i = 0u; i < param1.size(); ++i)
            result[i] = param1[i] + param2[i];

        return true;
    }

    template <typename T>
    typename std::enable_if<is_float_value_tag<T>::value, bool>::type
    invoke(const share_vec<T> & param1,
           const share_vec<T> & param2,
           share_vec<T> & result)
    {
        if (param1.size() != param2.size() || param1.size() != result.size())
            return false;

        for (size_t i = 0u; i < param1.size(); ++i)
            result[i] = sf_float_add(param1[i], param2[i]).result;

        return true;
    }

}; /* class AdditionProtocol { */

template<>
class __attribute__ ((visibility("internal"))) BitwiseAndProtocol<Shared3pPDPI> {
public: /* Methods: */

    BitwiseAndProtocol(Shared3pPDPI & pdpi) { (void) pdpi; }

    template <typename T>
    typename std::enable_if<
        is_any_value_tag<T>::value &&
        ! is_bool_value_tag<T>::value
    , bool>::type
    invoke(const share_vec<T> & param1,
           const share_vec<T> & param2,
           share_vec<T> & result)
    {
        if (param1.size() != param2.size() || param1.size() != result.size())
            return false;

        for (size_t i = 0u; i < param1.size(); ++i)
            result[i] = param1[i] & param2[i];

        return true;
    }

    template <typename T>
    typename std::enable_if<is_bool_value_tag<T>::value, bool>::type
    invoke(const share_vec<T> & param1,
           const share_vec<T> & param2,
           share_vec<T> & result)
    {
        if (param1.size() != param2.size() || param1.size() != result.size())
            return false;

        for (size_t i = 0u; i < param1.size(); ++i)
            result[i] = param1[i] && param2[i];

        return true;
    }

}; /* class BitwiseAndProtocol { */

template<>
class __attribute__ ((visibility("internal"))) BitwiseOrProtocol<Shared3pPDPI> {
public: /* Methods: */

    BitwiseOrProtocol(Shared3pPDPI & pdpi) { (void) pdpi; }

    template <typename T>
    typename std::enable_if<
        is_any_value_tag<T>::value &&
        ! is_bool_value_tag<T>::value
    , bool>::type
    invoke(const share_vec<T> & param1,
           const share_vec<T> & param2,
           share_vec<T> & result)
    {
        if (param1.size() != param2.size() || param1.size() != result.size())
            return false;

        for (size_t i = 0u; i < param1.size(); ++i)
            result[i] = param1[i] | param2[i];

        return true;
    }

    template <typename T>
    typename std::enable_if<is_bool_value_tag<T>::value, bool>::type
    invoke(const share_vec<T> & param1,
           const share_vec<T> & param2,
           share_vec<T> & result)
    {
        if (param1.size() != param2.size() || param1.size() != result.size())
            return false;

        for (size_t i = 0u; i < param1.size(); ++i)
            result[i] = param1[i] || param2[i];

        return true;
    }

}; /* class BitwiseOrProtocol { */

template<>
class __attribute__ ((visibility("internal"))) DivisionProtocol<Shared3pPDPI> {
public: /* Methods: */

    DivisionProtocol(Shared3pPDPI & pdpi) { (void) pdpi; }

    template <typename T>
    typename std::enable_if<is_integral_value_tag<T>::value, bool>::type
    invoke(const share_vec<T> & param1,
           const share_vec<T> & param2,
           share_vec<T> & result)
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
    typename std::enable_if<is_float_value_tag<T>::value, bool>::type
    invoke(const share_vec<T> & param1,
           const share_vec<T> & param2,
           share_vec<T> & result)
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
    typename std::enable_if<is_integral_value_tag<T>::value, bool>::type
    invoke(const share_vec<T> & param1,
           const immutable_vm_vec<T> & param2,
           share_vec<T> & result)
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
    typename std::enable_if<is_float_value_tag<T>::value, bool>::type
    invoke(const share_vec<T> & param1,
           const immutable_vm_vec<T> & param2,
           share_vec<T> & result)
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

template<>
class __attribute__ ((visibility("internal"))) EqualityProtocol<Shared3pPDPI> {
public: /* Methods: */

    EqualityProtocol(Shared3pPDPI & pdpi) { (void) pdpi; }

    template <typename T>
    typename std::enable_if<
        is_any_value_tag<T>::value &&
        ! is_float_value_tag<T>::value
    , bool>::type
    invoke(const share_vec<T> & param1,
           const share_vec<T> & param2,
           share_vec<s3p_bool_t> & result)
    {
        if (param1.size() != param2.size() || param1.size() != result.size())
            return false;

        for (size_t i = 0u; i < param1.size(); ++i)
            result[i] = param1[i] == param2[i];

        return true;
    }

    template <typename T>
    typename std::enable_if<is_float_value_tag<T>::value, bool>::type
    invoke(const share_vec<T> & param1,
           const share_vec<T> & param2,
           share_vec<s3p_bool_t> & result)
    {
        if (param1.size() != param2.size() || param1.size() != result.size())
            return false;

        for (size_t i = 0u; i < param1.size(); ++i)
            result[i] = sf_float_eq(param1[i], param2[i]).result;

        return true;
    }

}; /* class EqualityProtocol { */

template<>
class __attribute__ ((visibility("internal"))) GreaterThanProtocol<Shared3pPDPI> {
public: /* Methods: */

    GreaterThanProtocol(Shared3pPDPI & pdpi) { (void) pdpi; }

    template <typename T>
    typename std::enable_if<
        is_any_value_tag<T>::value &&
        ! is_float_value_tag<T>::value
    , bool>::type
    invoke(const share_vec<T> & param1,
           const share_vec<T> & param2,
           share_vec<s3p_bool_t> & result)
    {
        if (param1.size() != param2.size() || param1.size() != result.size())
            return false;

        for (size_t i = 0u; i < param1.size(); ++i)
            result[i] = param1[i] > param2[i];

        return true;
    }

    template <typename T>
    typename std::enable_if<is_float_value_tag<T>::value, bool>::type
    invoke(const share_vec<T> & param1,
           const share_vec<T> & param2,
           share_vec<s3p_bool_t> & result)
    {
        if (param1.size() != param2.size() || param1.size() != result.size())
            return false;

        for (size_t i = 0u; i < param1.size(); ++i)
            result[i] = sf_float_lt(param2[i], param1[i]).result;

        return true;
    }

}; /* class GreaterThanProtocol { */

template<>
class __attribute__ ((visibility("internal"))) GreaterThanOrEqualProtocol<Shared3pPDPI> {
public: /* Methods: */

    GreaterThanOrEqualProtocol(Shared3pPDPI & pdpi) { (void) pdpi; }

    template <typename T>
    typename std::enable_if<
        is_any_value_tag<T>::value &&
        ! is_float_value_tag<T>::value
    , bool>::type
    invoke(const share_vec<T> & param1,
           const share_vec<T> & param2,
           share_vec<s3p_bool_t> & result)
    {
        if (param1.size() != param2.size() || param1.size() != result.size())
            return false;

        for (size_t i = 0u; i < param1.size(); ++i)
            result[i] = param1[i] >= param2[i];

        return true;
    }

    template <typename T>
    typename std::enable_if<is_float_value_tag<T>::value, bool>::type
    invoke(const share_vec<T> & param1,
           const share_vec<T> & param2,
           share_vec<s3p_bool_t> & result)
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
    typename std::enable_if<is_xor_value_tag<T>::value, bool>::type
    invoke(const share_vec<T> & param1,
           const immutable_vm_vec<s3p_int64_t> & param2,
           share_vec<T> & result)
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
    typename std::enable_if<is_xor_value_tag<T>::value, bool>::type
    invoke(const share_vec<T> & param1,
           const immutable_vm_vec<s3p_int64_t> & param2,
           share_vec<T> & result)
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

template<>
class __attribute__ ((visibility("internal"))) LessThanProtocol<Shared3pPDPI> {
public: /* Methods: */

    LessThanProtocol(Shared3pPDPI & pdpi) { (void) pdpi; }

    template <typename T>
    typename std::enable_if<
        is_any_value_tag<T>::value &&
        ! is_float_value_tag<T>::value
    , bool>::type
    invoke(const share_vec<T> & param1,
           const share_vec<T> & param2,
           share_vec<s3p_bool_t> & result)
    {
        if (param1.size() != param2.size() || param1.size() != result.size())
            return false;

        for (size_t i = 0u; i < param1.size(); ++i)
            result[i] = param1[i] < param2[i];

        return true;
    }

    template <typename T>
    typename std::enable_if<is_float_value_tag<T>::value, bool>::type
    invoke(const share_vec<T> & param1,
           const share_vec<T> & param2,
           share_vec<s3p_bool_t> & result)
    {
        if (param1.size() != param2.size() || param1.size() != result.size())
            return false;

        for (size_t i = 0u; i < param1.size(); ++i)
            result[i] = sf_float_lt(param1[i], param2[i]).result;

        return true;
    }

}; /* class LessThanProtocol { */

template<>
class __attribute__ ((visibility("internal"))) LessThanOrEqualProtocol<Shared3pPDPI> {
public: /* Methods: */

    LessThanOrEqualProtocol(Shared3pPDPI & pdpi) { (void) pdpi; }

    template <typename T>
    typename std::enable_if<
        is_any_value_tag<T>::value &&
        ! is_float_value_tag<T>::value
    , bool>::type
    invoke(const share_vec<T> & param1,
           const share_vec<T> & param2,
           share_vec<s3p_bool_t> & result)
    {
        if (param1.size() != param2.size() || param1.size() != result.size())
            return false;

        for (size_t i = 0u; i < param1.size(); ++i)
            result[i] = param1[i] <= param2[i];

        return true;
    }

    template <typename T>
    typename std::enable_if<is_float_value_tag<T>::value, bool>::type
    invoke(const share_vec<T> & param1,
           const share_vec<T> & param2,
           share_vec<s3p_bool_t> & result)
    {
        if (param1.size() != param2.size() || param1.size() != result.size())
            return false;

        for (size_t i = 0u; i < param1.size(); ++i)
            result[i] = sf_float_le(param1[i], param2[i]).result;

        return true;
    }

}; /* class LessThanOrEqualProtocol { */

template<>
class __attribute__ ((visibility("internal"))) MaximumProtocol<Shared3pPDPI> {
public: /* Methods: */

    MaximumProtocol(Shared3pPDPI & pdpi) { (void) pdpi; }

    template <typename T>
    typename std::enable_if<
        is_any_value_tag<T>::value &&
        ! is_float_value_tag<T>::value
    , bool>::type
    invoke(const share_vec<T> & param1,
           const share_vec<T> & param2,
           share_vec<T> & result)
    {
        if (param1.size() != param2.size() || param1.size() != result.size())
            return false;

        for (size_t i = 0u; i < param1.size(); ++i)
            result[i] = param1[i] > param2[i] ? param1[i] : param2[i];

        return true;
    }

    template <typename T>
    typename std::enable_if<is_float_value_tag<T>::value, bool>::type
    invoke(const share_vec<T> & param1,
           const share_vec<T> & param2,
           share_vec<T> & result)
    {
        if (param1.size() != param2.size() || param1.size() != result.size())
            return false;

        for (size_t i = 0u; i < param1.size(); ++i)
            result[i] = sf_float_lt(param2[i], param1[i]).result ? param1[i] : param2[i];

        return true;
    }

}; /* class MaximumProtocol { */

template<>
class __attribute__ ((visibility("internal"))) MinimumProtocol<Shared3pPDPI> {
public: /* Methods: */

    MinimumProtocol(Shared3pPDPI & pdpi) { (void) pdpi; }

    template <typename T>
    typename std::enable_if<
        is_any_value_tag<T>::value &&
        ! is_float_value_tag<T>::value
    , bool>::type
    invoke(const share_vec<T> & param1,
           const share_vec<T> & param2,
           share_vec<T> & result)
    {
        if (param1.size() != param2.size() || param1.size() != result.size())
            return false;

        for (size_t i = 0u; i < param1.size(); ++i)
            result[i] = param1[i] < param2[i] ? param1[i] : param2[i];

        return true;
    }

    template <typename T>
    typename std::enable_if<is_float_value_tag<T>::value, bool>::type
    invoke(const share_vec<T> & param1,
           const share_vec<T> & param2,
           share_vec<T> & result)
    {
        if (param1.size() != param2.size() || param1.size() != result.size())
            return false;

        for (size_t i = 0u; i < param1.size(); ++i)
            result[i] = sf_float_lt(param1[i], param2[i]).result ? param1[i] : param2[i];

        return true;
    }

}; /* class MinimumProtocol { */

template<>
class __attribute__ ((visibility("internal"))) MultiplicationProtocol<Shared3pPDPI> {
public: /* Methods: */

    MultiplicationProtocol(Shared3pPDPI & pdpi) { (void) pdpi; }

    template <typename T>
    typename std::enable_if<is_integral_value_tag<T>::value, bool>::type
    invoke(const share_vec<T> & param1,
           const share_vec<T> & param2,
           share_vec<T> & result)
    {
        if (param1.size() != param2.size() || param1.size() != result.size())
            return false;

        for (size_t i = 0u; i < param1.size(); ++i)
            result[i] = param1[i] * param2[i];

        return true;
    }

    template <typename T>
    typename std::enable_if<is_float_value_tag<T>::value, bool>::type
    invoke(const share_vec<T> & param1,
           const share_vec<T> & param2,
           share_vec<T> & result)
    {
        if (param1.size() != param2.size() || param1.size() != result.size())
            return false;

        for (size_t i = 0u; i < param1.size(); ++i)
            result[i] = sf_float_mul(param1[i], param2[i]).result;

        return true;
    }

    template <typename T>
    typename std::enable_if<is_integral_value_tag<T>::value, bool>::type
    invoke(const share_vec<T> & param1,
           const immutable_vm_vec<T> & param2,
           share_vec<T> & result)
    {
        if (param1.size() > param2.size() || param1.size() != result.size())
            return false;

        for (size_t i = 0u; i < param1.size(); ++i)
            result[i] = param1[i] * param2[i];

        return true;
    }

    template <typename T>
    typename std::enable_if<is_float_value_tag<T>::value, bool>::type
    invoke(const share_vec<T> & param1,
           const immutable_vm_vec<T> & param2,
           share_vec<T> & result)
    {
        if (param1.size() > param2.size() || param1.size() != result.size())
            return false;

        for (size_t i = 0u; i < param1.size(); ++i)
            result[i] = sf_float_mul(param1[i], param2[i]).result;

        return true;
    }

}; /* class MultiplicationProtocol { */

template<>
class __attribute__ ((visibility("internal"))) RemainderProtocol<Shared3pPDPI> {
public: /* Methods: */

    RemainderProtocol(Shared3pPDPI & pdpi) { (void) pdpi; }

    template <typename T>
    typename std::enable_if<is_integral_value_tag<T>::value, bool>::type
    invoke(const share_vec<T> & param1,
           const share_vec<T> & param2,
           share_vec<T> & result)
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
    typename std::enable_if<is_integral_value_tag<T>::value, bool>::type
    invoke(const share_vec<T> & param1,
           const immutable_vm_vec<T> & param2,
           share_vec<T> & result)
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

template<>
class __attribute__ ((visibility("internal"))) SubtractionProtocol<Shared3pPDPI> {
public: /* Methods: */

    SubtractionProtocol(Shared3pPDPI & pdpi) { (void) pdpi; }

    template <typename T>
    typename std::enable_if<is_integral_value_tag<T>::value, bool>::type
    invoke(const share_vec<T> & param1,
           const share_vec<T> & param2,
           share_vec<T> & result)
    {
        if (param1.size() != param2.size() || param1.size() != result.size())
            return false;

        for (size_t i = 0u; i < param1.size(); ++i)
            result[i] = param1[i] - param2[i];

        return true;
    }

    template <typename T>
    typename std::enable_if<is_float_value_tag<T>::value, bool>::type
    invoke(const share_vec<T> & param1,
           const share_vec<T> & param2,
           share_vec<T> & result)
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
