/*
 * This file is a part of the Sharemind framework.
 * Copyright (C) Cybernetica AS
 *
 * All rights are reserved. Reproduction in whole or part is prohibited
 * without the written consent of the copyright owner. The usage of this
 * code is subject to the appropriate license agreement.
 */

#ifndef MOD_SHARED3P_EMU_PROTOCOLS_BINARY_H
#define MOD_SHARED3P_EMU_PROTOCOLS_BINARY_H

#include "../Shared3pValueTraits.h"
#include "../VMReferences.h"


namespace sharemind {

class __attribute__ ((visibility("internal"))) AdditionProtocol {
public: /* Methods: */

    AdditionProtocol(Shared3pPDPI &pdpi) { (void) pdpi; }

    template <typename T>
    bool invoke(const s3p_vec<T> & param1, const s3p_vec<T> & param2,
                s3p_vec<T> & result, any_value_tag)
    {
        if (param1.size() != param2.size() || param1.size() != result.size())
            return false;

        for (size_t i = 0u; i < param1.size(); ++i)
            result[i] = param1[i] + param2[i];

        return true;
    }

}; /* class AdditionProtocol { */

class __attribute__ ((visibility("internal"))) BitwiseAndProtocol {
public: /* Methods: */

    BitwiseAndProtocol(Shared3pPDPI &pdpi) { (void) pdpi; }

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

}; /* class BitwiseAndProtocol { */

class __attribute__ ((visibility("internal"))) BitwiseOrProtocol {
public: /* Methods: */

    BitwiseOrProtocol(Shared3pPDPI &pdpi) { (void) pdpi; }

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

}; /* class BitwiseOrProtocol { */

class __attribute__ ((visibility("internal"))) BitwiseXorProtocol {
public: /* Methods: */

    BitwiseXorProtocol(Shared3pPDPI &pdpi) { (void) pdpi; }

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

    DivisionProtocol(Shared3pPDPI &pdpi) { (void) pdpi; }

    template <typename T>
    bool invoke(const s3p_vec<T> & param1, const s3p_vec<T> & param2,
                s3p_vec<T> & result, any_value_tag)
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
    bool invoke(const s3p_vec<T> & param1, const immutable_vm_vec<T> & param2,
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

}; /* class DivisionProtocol { */

class __attribute__ ((visibility("internal"))) MaximumProtocol {
public: /* Methods: */

    MaximumProtocol(Shared3pPDPI &pdpi) { (void) pdpi; }

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

}; /* class MaximumProtocol{ */

class __attribute__ ((visibility("internal"))) MinimumProtocol {
public: /* Methods: */

    MinimumProtocol(Shared3pPDPI &pdpi) { (void) pdpi; }

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

}; /* class MinimumProtocol{ */

class __attribute__ ((visibility("internal"))) MultiplicationProtocol {
public: /* Methods: */

    MultiplicationProtocol(Shared3pPDPI &pdpi) { (void) pdpi; }

    template <typename T>
    bool invoke(const s3p_vec<T> & param1, const s3p_vec<T> & param2,
                s3p_vec<T> & result, any_value_tag)
    {
        if (param1.size() != param2.size() || param1.size() != result.size())
            return false;

        for (size_t i = 0u; i < param1.size(); ++i)
            result[i] = param1[i] * param2[i];

        return true;
    }

    template <typename T>
    bool invoke(const s3p_vec<T> & param1, const immutable_vm_vec<T> & param2,
                s3p_vec<T> & result, numeric_value_tag)
    {
        if (param1.size() != param2.size() || param1.size() != result.size())
            return false;

        for (size_t i = 0u; i < param1.size(); ++i)
            result[i] = param1[i] * param2[i];

        return true;
    }

}; /* class MultiplicationProtocol { */

class __attribute__ ((visibility("internal"))) RemainderProtocol {
public: /* Methods: */

    RemainderProtocol(Shared3pPDPI &pdpi) { (void) pdpi; }

    template <typename T>
    bool invoke(const s3p_vec<T> & param1, const s3p_vec<T> & param2,
                s3p_vec<T> & result, any_value_tag)
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

}; /* class RemainderProtocol { */

class __attribute__ ((visibility("internal"))) SubtractionProtocol {
public: /* Methods: */

    SubtractionProtocol(Shared3pPDPI &pdpi) { (void) pdpi; }

    template <typename T>
    bool invoke(const s3p_vec<T> & param1, const s3p_vec<T> & param2,
                s3p_vec<T> & result, any_value_tag)
    {
        if (param1.size() != param2.size() || param1.size() != result.size())
            return false;

        for (size_t i = 0u; i < param1.size(); ++i)
            result[i] = param1[i] - param2[i];

        return true;
    }

}; /* class SubtractionProtocol { */

} /* namespace sharemind { */

#endif /* MOD_SHARED3P_EMU_PROTOCOLS_BINARY_H */
