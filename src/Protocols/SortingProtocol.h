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

#ifndef MOD_SHARED3P_EMU_PROTOCOLS_SORTINGPROTOCOL_H
#define MOD_SHARED3P_EMU_PROTOCOLS_SORTINGPROTOCOL_H

#include <algorithm>
#include <sharemind/VmVector.h>
#include <tuple>

#include "../Shared3pPDPI.h"
#include "../Shared3pValueTraits.h"
#include "../Shared3pVector.h"
#include "SoftFloatUtility.h"

namespace sharemind {

namespace {

template<typename T>
typename std::enable_if<! is_float_value_tag<T>::value, bool>::type
lt(const typename sharemind::ValueTraits<T>::share_type & a,
   const typename sharemind::ValueTraits<T>::share_type & b)
{
    return a < b;
}

template<typename T>
typename std::enable_if<is_float_value_tag<T>::value, bool>::type
lt(const typename sharemind::ValueTraits<T>::share_type & a,
   const typename sharemind::ValueTraits<T>::share_type & b)
{
    return sf_float_lt(a, b).result;
}

template<typename T>
typename std::enable_if<! is_float_value_tag<T>::value, bool>::type
eq(const typename sharemind::ValueTraits<T>::share_type & a,
   const typename sharemind::ValueTraits<T>::share_type & b)
{
    return a == b;
}

template<typename T>
typename std::enable_if<is_float_value_tag<T>::value, bool>::type
eq(const typename sharemind::ValueTraits<T>::share_type & a,
   const typename sharemind::ValueTraits<T>::share_type & b)
{
    return sf_float_eq(a, b).result;
}

template<typename T>
using Triple =
    std::tuple<typename sharemind::ValueTraits<T>::share_type,
               typename sharemind::ValueTraits<sharemind::s3p_xor_uint64_t>::share_type,
               uint64_t>;

template<typename T>
struct Compare {
    Compare(bool ascending)
        : m_ascending(ascending)
        {}

    bool operator()(const Triple<T>& atrip, const Triple<T>& btrip) {
        using Value = typename sharemind::ValueTraits<T>::share_type;
        using Idx = typename sharemind::ValueTraits<sharemind::s3p_xor_uint64_t>::share_type;
        Value a = std::get<0>(atrip), b = std::get<0>(btrip);
        Idx i = std::get<1>(atrip), j = std::get<1>(btrip);
        if (m_ascending)
            return lt<T>(a, b) || (eq<T>(a, b) && i < j);
        else
            return lt<T>(b, a) || (eq<T>(a, b) && i < j);
    }

    /*
    typename std::enable_if<is_float_value_tag<T>::value, bool>::type
    operator()(const Triple<T>& atrip, const Triple<T>& btrip) {
        using Value = typename sharemind::ValueTraits<T>::share_type;
        using Idx = typename sharemind::ValueTraits<sharemind::s3p_xor_uint64_t>::share_type;
        Value a = std::get<0>(atrip), b = std::get<0>(btrip);
        Idx i = std::get<1>(atrip), j = std::get<1>(btrip);
        if (m_ascending)
            return sf_float_lt(a, b).result || (sf_float_eq(a, b).result && i < j);
        else
            return sf_float_gt(a, b).result || (sf_float_eq(a, b).result && i < j);
    }
    */

    const bool m_ascending;
};

} /* anonymous namespace */

class __attribute__ ((visibility("internal"))) StableSortingProtocol {
public: /* Methods: */

    StableSortingProtocol(Shared3pPDPI & pdpi) { (void) pdpi; }

    template<typename T>
    bool invoke(const ShareVec<T>& param,
                const ShareVec<s3p_xor_uint64_t>& indices,
                MutableVmVec<s3p_uint64_t>& perm,
                bool ascending=true)
    {
        if (param.size() != indices.size()) return false;
        if (param.size() != perm.size()) return false;

        std::vector<uint64_t> blocks;
        blocks.push_back(0);
        blocks.push_back(param.size());
        return invokeImpl(param, indices, blocks, perm, ascending);
    }

    template <typename T>
    bool invoke(const ShareVec<T>& param,
                const ShareVec<s3p_xor_uint64_t>& indices,
                const ImmutableVmVec<s3p_uint64_t>& blocks,
                MutableVmVec<s3p_uint64_t>& perm,
                bool ascending=true)
    {
        if (param.size() != indices.size()) return false;
        if (param.size() != perm.size()) return false;
        if (blocks.size() < 2) return false;

        std::vector<uint64_t> blocksVec = {blocks[0]};
        for (size_t i = 1; i < blocks.size(); ++i) {
            uint64_t start = blocks[i - 1];
            uint64_t end = blocks[i];

            if (start > end) return false;
            if (end > param.size()) return false;

            blocksVec.push_back(end);
        }

        return invokeImpl(param, indices, blocksVec, perm, ascending);
    }

private: /* Methods: */

    template<typename T>
    bool invokeImpl(const ShareVec<T>& param,
                    const ShareVec<s3p_xor_uint64_t>& indices,
                    const std::vector<uint64_t>& blocks,
                    MutableVmVec<s3p_uint64_t>& perm,
                    bool ascending=true)
    {
        if (param.size() <= 1)
            return true;

        std::vector<Triple<T>> vec(param.size());
        for (uint64_t i = 0; i < param.size(); ++i) {
            vec[i] = std::make_tuple(param[i], indices[i], i);
        }

        Compare<T> cmp(ascending);

        for (size_t i = 1; i < blocks.size(); ++i) {
            std::sort(vec.begin() + blocks[i - 1],
                      vec.begin() + blocks[i],
                      cmp);
        }

        for (size_t i = 0; i < param.size(); ++i) {
            perm[i] = std::get<2>(vec[i]);
        }

        return true;
    }

};

}

#endif /* MOD_SHARED3P_EMU_PROTOCOLS_SORTINGPROTOCOL_H */
