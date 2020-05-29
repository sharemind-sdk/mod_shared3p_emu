/*
 * This file is a part of the Sharemind framework.
 * Copyright (C) Cybernetica AS
 *
 * All rights are reserved. Reproduction in whole or part is prohibited
 * without the written consent of the copyright owner. The usage of this
 * code is subject to the appropriate license agreement.
 */

#ifndef MOD_SHARED3P_EMU_PREFIXSUMSYSCALLS_H
#define MOD_SHARED3P_EMU_PREFIXSUMSYSCALLS_H

#include "../SecretSharing.h"
#include "../Shared3pValueTraits.h"
#include "../Shared3pVector.h"

namespace sharemind {

namespace {

template <typename T>
void addShares(typename ShareVec<T>::reference dest,
               const typename ValueTraits<T>::share_type& src1,
               const typename ValueTraits<T>::share_type& src2)
{
    dest = SecretSharing::combine(src1, src2, typename ValueTraits<T>::value_category{});
}

template <typename T>
void subShares(typename ShareVec<T>::reference dest,
               const typename ValueTraits<T>::share_type& src1,
               const typename ValueTraits<T>::share_type& src2)
{
    addShares<T>(dest, src1, SecretSharing::inverse(
                     src2, typename ValueTraits<T>::value_category{}));
}

template <typename T>
inline bool freePrefixSum(const ShareVec<T>& in, ShareVec<T>& out) {
    if (in.size() != out.size()) {
        return false;
    }

    if (out.size() > 0) {
        out[0] = in[0];
    }

    for (size_t i = 1; i < out.size(); ++i) {
        addShares<T>(out[i], in[i], out[i - 1]);
    }

    return true;
}

template <typename T>
inline bool freeInvPrefixSum(const ShareVec<T> & in, ShareVec<T>& out) {
    if (in.size() != out.size()) {
        return false;
    }

    if (out.size() > 0) {
        out[0] = in[0];
    }

    for (size_t i = out.size() - 1; i > 0; --i) {
        subShares<T>(out[i], in[i], in[i - 1]);
    }

    return true;
}

} /* namespace { */

class __attribute__ ((visibility("internal"))) PrefixSumProtocol {
public: /* Methods: */

    PrefixSumProtocol(Shared3pPDPI& pdpi) { (void) pdpi; }

    template <typename T>
    bool invoke(const ShareVec<T>& in, ShareVec<T>& out) {
        return freePrefixSum(in, out);
    }

}; /* class PrefixSumProtocol */

class __attribute__ ((visibility("internal"))) InvPrefixSumProtocol {
public: /* Methods: */

    InvPrefixSumProtocol(Shared3pPDPI& pdpi) { (void) pdpi; }

    template <typename T>
    bool invoke(const ShareVec<T>& in, ShareVec<T>& out) {
        return freeInvPrefixSum(in, out);
    }

}; /* class PrefixSumProtocol */

class __attribute__ ((visibility("internal"))) MatPrefixSumProtocol {
public: /* Methods: */

    MatPrefixSumProtocol(Shared3pPDPI& pdpi) { (void) pdpi; }

    template <typename T>
    bool invoke(ShareVec<T>& vec,
                const ImmutableVmVec<s3p_uint32_t>& start,
                const ImmutableVmVec<s3p_uint32_t>& step,
                const ImmutableVmVec<s3p_uint32_t>& count,
                bool direction)
    {
        if (start.size() != step.size() ||
            start.size() != count.size())
        {
            return false;
        }

        for (size_t i = 0; i < start.size(); ++i) {
            ShareVec<T> w(count[i]);
            ShareVec<T> out(count[i]);

            for (size_t j = 0; j < count[i]; ++j) {
                size_t idx = start[i] + j * step[i];
                if (idx >= vec.size()) {
                    return false;
                }
                w[j] = vec[idx];
            }

            if (direction) {
                if (!freePrefixSum(w, out)) {
                    return false;
                }
            } else {
                if (!freeInvPrefixSum(w, out)) {
                    return false;
                }
            }

            for (size_t j = 0; j < count[i]; ++j) {
                vec[start[i] + j * step[i]] = out[j];
            }
        }

        return true;
    }

}; /* class PrefixSumProtocol */

} /* namespace sharemind { */

#endif // MOD_SHARED3P_EMU_PREFIXSUMSYSCALLS_H
