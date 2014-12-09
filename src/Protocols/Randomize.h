/*
 * This file is a part of the Sharemind framework.
 * Copyright (C) Cybernetica AS
 *
 * All rights are reserved. Reproduction in whole or part is prohibited
 * without the written consent of the copyright owner. The usage of this
 * code is subject to the appropriate license agreement.
 */

#ifndef MOD_SHARED3P_EMU_PROTOCOLS_RANDOMIZE_H
#define MOD_SHARED3P_EMU_PROTOCOLS_RANDOMIZE_H

#include <random>
#include <algorithm>
#include <numeric>

namespace sharemind {

class __attribute__ ((visibility("internal"))) RandomizeProtocol {
public: /* Methods: */

    RandomizeProtocol (Shared3pPDPI &pdpi)
        : m_pdpi(pdpi)
    { }

    template <typename T>
    bool invoke (s3p_vec<T>& result, any_value_tag = typename value_traits<T>::value_category ()) {
        result.randomize(m_pdpi.rng());
        return true;
    }

private: /* Fields: */

    Shared3pPDPI & m_pdpi;

}; /* class RandomizeProtocol { */

class __attribute__ ((visibility("internal"))) MatrixShufflingProtocol {
public: /* Methods: */

    MatrixShufflingProtocol (Shared3pPDPI &pdpi)
        : m_pdpi(pdpi)
    { }

    template <typename T>
    void invoke (s3p_vec<T> & inOut, const size_t rowSize, const s3p_vec<s3p_uint8_t> & rand) {
        invoke(inOut, rowSize, rand, true);
    }

    template <typename T>
    void invokeInverse (s3p_vec<T> & inOut, const size_t rowSize, const s3p_vec<s3p_uint8_t> & rand) {
        invoke(inOut, rowSize, rand, false);
    }

    template <typename T>
    void invoke (s3p_vec<T> & inOut, const size_t rowSize) {
        s3p_vec<s3p_uint8_t> rand(32);
        rand.randomize(m_pdpi.rng());
        invoke(inOut, rowSize, rand, false);
    }

private: /* Methods: */

    template <typename T>
    void invoke (s3p_vec<T> & inOut, const size_t rowSize, const s3p_vec<s3p_uint8_t> & rand,
                 bool dir) {
        std::vector<size_t> permut(inOut.size() / rowSize);
        getPermutationMatrix(permut, rand);
        shuffle(inOut, rowSize, permut, dir);
    }

    void getPermutationMatrix(std::vector<size_t> & perm, const s3p_vec<s3p_uint8_t> & key) {
        std::default_random_engine rng;
        std::seed_seq seed(key.begin(), key.end());
        rng.seed(seed);
        iota(perm.begin(), perm.end(), 0);
        std::shuffle(perm.begin(), perm.end(), rng);
    }

    template <typename T>
    void shuffle (s3p_vec<T> & inOut, const size_t rowSize, const std::vector<size_t> & perm,                             bool dir) {
        s3p_vec<T> copy;
        copy.assign(inOut);
        for (size_t i = 0; i < perm.size(); ++i) {
            size_t to, from;
            if (dir) {
                to = perm[i] * rowSize;
                from = i * rowSize;
            } else {
                to = i * rowSize;
                from = perm[i] * rowSize;
            }
            for (size_t j = 0; j < rowSize; ++j)
                inOut[to + j] = copy[from + j];
        }
    }


private: /* Fields: */

    Shared3pPDPI & m_pdpi;

}; /* class MatrixShufflingProtocol { */

} /* namespace sharemind { */

#endif /* MOD_SHARED3P_EMU_PROTOCOLS_RANDOMIZE_H */
