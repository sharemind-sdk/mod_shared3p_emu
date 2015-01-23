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

#ifndef MOD_SHARED3P_EMU_PROTOCOLS_MATRIXSHUFFLINGPROTOCOL_H
#define MOD_SHARED3P_EMU_PROTOCOLS_MATRIXSHUFFLINGPROTOCOL_H

#include <algorithm>
#include <numeric>
#include <random>

namespace sharemind {

class __attribute__ ((visibility("internal"))) MatrixShufflingProtocol {
public: /* Methods: */

    MatrixShufflingProtocol(Shared3pPDPI &pdpi)
        : m_pdpi(pdpi)
    { }

    template <typename T>
    void invoke(s3p_vec<T> & inOut, const size_t rowSize,
                const s3p_vec<s3p_uint8_t> & rand)
    {
        invoke(inOut, rowSize, rand, true);
    }

    template <typename T>
    void invokeInverse(s3p_vec<T> & inOut, const size_t rowSize,
                       const s3p_vec<s3p_uint8_t> & rand)
    {
        invoke(inOut, rowSize, rand, false);
    }

    template <typename T>
    void invoke(s3p_vec<T> & inOut, const size_t rowSize) {
        s3p_vec<s3p_uint8_t> rand(32);
        rand.randomize(m_pdpi.rng());
        invoke(inOut, rowSize, rand, false);
    }

private: /* Methods: */

    template <typename T>
    void invoke(s3p_vec<T> & inOut, const size_t rowSize,
                 const s3p_vec<s3p_uint8_t> & rand, bool dir)
    {
        std::vector<size_t> permut(inOut.size() / rowSize);
        getPermutationMatrix(permut, rand);
        shuffle(inOut, rowSize, permut, dir);
    }

    void getPermutationMatrix(std::vector<size_t> & perm,
                              const s3p_vec<s3p_uint8_t> & key)
    {
        std::default_random_engine rng;
        std::seed_seq seed(key.begin(), key.end());
        rng.seed(seed);
        std::iota(perm.begin(), perm.end(), 0u);
        std::shuffle(perm.begin(), perm.end(), rng);
    }

    template <typename T>
    void shuffle(s3p_vec<T> & inOut, const size_t rowSize,
                 const std::vector<size_t> & perm, bool dir)
    {
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
            for (size_t j = 0u; j < rowSize; ++j)
                inOut[to + j] = copy[from + j];
        }
    }

private: /* Fields: */

    Shared3pPDPI & m_pdpi;

}; /* class MatrixShufflingProtocol { */

} /* namespace sharemind { */

#endif /* MOD_SHARED3P_EMU_PROTOCOLS_MATRIXSHUFFLINGPROTOCOL_H */
