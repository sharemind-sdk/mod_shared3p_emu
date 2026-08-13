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

#ifndef MOD_SHARED3P_EMU_PROTOCOLS_MATRIXMULTIPLICATIONPROTOCOL_H
#define MOD_SHARED3P_EMU_PROTOCOLS_MATRIXMULTIPLICATIONPROTOCOL_H

#include "../Shared3pValueTraits.h"
#include "FixedPoint.h"
#include "ProtocolHelperMacros.h"
#include <sharemind/VmVector.h>

#include <Eigen/Dense>
#include <omp.h>


namespace sharemind {

class __attribute__ ((visibility("internal"))) MatrixMultiplicationProtocol {
public: /* Methods: */

    template <typename T>
    typename std::enable_if<is_integral_value_tag<T>::value, bool>::type
    invoke (const ShareVec<T>& mat1,
            const ShareVec<T>& mat2,
            const ImmutableVmVec<s3p_uint64_t>& dim1,
            const ImmutableVmVec<s3p_uint64_t>& dim2,
            const ImmutableVmVec<s3p_uint64_t>& dim3,
            ShareVec<T>& result,
            numeric_value_tag)
    {
        // Transpose mat2 for better memory access later.
        ShareVec<T> mat2T (mat2.size());

        size_t s = 0;
        for (size_t i = 0; i < dim1.size(); ++ i) {
            for (size_t j = 0; j < dim2[i]; ++ j) {
                for (size_t k = 0; k < dim3[i]; ++ k) {
                    mat2T[s + k * dim2[i] + j] = mat2[s + j * dim3[i] + k];
                }
            }

            s += dim2[i] * dim3[i];
        }

        size_t s1 = 0;
        size_t s2 = 0;
        size_t s3 = 0;
        for (size_t i = 0; i < dim1.size(); ++ i) {
            for (size_t j = 0; j < dim1[i]; ++ j) {
                for (size_t k = 0; k < dim3[i]; ++ k) {
                    const size_t t3 = s3 + j * dim3[i] + k;
                    result[t3] = 0;
                    for (size_t l = 0; l < dim2[i]; ++ l) {
                        const size_t t1 = s1 + j * dim2[i] + l;
                        const size_t t2 = s2 + k * dim2[i] + l;
                        result[t3] += mat1[t1] * mat2T[t2];
                    }
                }
            }

            s1 += dim1[i] * dim2[i];
            s2 += dim2[i] * dim3[i];
            s3 += dim1[i] * dim3[i];
        }

        return true;
    }

    /*
     * Does not perform normal algebraic matrix multiplication!
     * Addition is replaced by xor and multiplication by bitwise and.
     */
    template <typename T>
    typename std::enable_if<is_xor_value_tag<T>::value, bool>::type
    invoke (const ShareVec<T>& mat1,
            const ShareVec<T>& mat2,
            const ImmutableVmVec<s3p_uint64_t>& dim1,
            const ImmutableVmVec<s3p_uint64_t>& dim2,
            const ImmutableVmVec<s3p_uint64_t>& dim3,
            ShareVec<T>& result,
            xored_numeric_value_tag)
    {
        // Transpose mat2 for better memory access later.
        ShareVec<T> mat2T (mat2.size());

        size_t s = 0;
        for (size_t i = 0; i < dim1.size(); ++ i) {
            for (size_t j = 0; j < dim2[i]; ++ j) {
                for (size_t k = 0; k < dim3[i]; ++ k) {
                    mat2T[s + k * dim2[i] + j] = mat2[s + j * dim3[i] + k];
                }
            }

            s += dim2[i] * dim3[i];
        }

        size_t s1 = 0;
        size_t s2 = 0;
        size_t s3 = 0;
        for (size_t i = 0; i < dim1.size(); ++ i) {
            for (size_t j = 0; j < dim1[i]; ++ j) {
                for (size_t k = 0; k < dim3[i]; ++ k) {
                    const size_t t3 = s3 + j * dim3[i] + k;
                    result[t3] = 0;
                    for (size_t l = 0; l < dim2[i]; ++ l) {
                        const size_t t1 = s1 + j * dim2[i] + l;
                        const size_t t2 = s2 + k * dim2[i] + l;
                        result[t3] ^= mat1[t1] & mat2T[t2];
                    }
                }
            }

            s1 += dim1[i] * dim2[i];
            s2 += dim2[i] * dim3[i];
            s3 += dim1[i] * dim3[i];
        }

        return true;
    }


    template <typename T>
    typename std::enable_if<is_integral_value_tag<T>::value, bool>::type
    invokeN (const ShareVec<T>& X,
             const ShareVec<T>& Y,
             ShareVec<T>& Z,
             const size_t N, const size_t H, const size_t K, const size_t W)
    {
        using share = typename T::share_type;

        using RowMatrixXi = Eigen::Matrix<share, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor>;

        if (Z.empty())
            return true;

        GUARD (X.size() == (N * H * K));
        GUARD (Y.size() == (N * K * W));
        GUARD (Z.size() == (N * H * W));

        // If batch size is over 8, parallelize across the batch dimension
        // Otherwise, let Eigen parallelize at the matrix level.

        bool parallelizeBatch = (N > 8);
        Eigen::setNbThreads(parallelizeBatch ? 1 : omp_get_max_threads());

        #pragma omp parallel for if(parallelizeBatch)
        for (size_t i = 0; i < N; ++i) {

            size_t x_offset = i * H * K;
            size_t y_offset = i * K * W;
            size_t z_offset = i * H * W;

            // Map data pointers to Eigen matrices (no copy)
            Eigen::Map<const RowMatrixXi> tX(X.data() + x_offset, H, K);
            Eigen::Map<const RowMatrixXi> tY(Y.data() + y_offset, K, W);
            Eigen::Map<RowMatrixXi> tZ(Z.data() + z_offset, H, W);

            // .noalias() tells Eigen the output doesn't overlap with inputs,
            // preventing it from allocating temporary arrays under the hood.
            tZ.noalias() = tX * tY;
        }

        return true;
    }


}; /* class MatrixMultiplicationProtocol { */

class __attribute__ ((visibility("internal"))) FixMatrixMultiplicationProtocol {
public: /* Methods: */


    uint64_t demote(sharemind::uint128_t x) {
        return static_cast<uint64_t>(x);
    }

    uint32_t demote(uint64_t x) {
        return static_cast<uint32_t>(x);
    }

    template <typename T>
    typename std::enable_if<is_unsigned_value_tag<T>::value, bool>::type
    invoke (const ShareVec<T>& mat1,
            const ShareVec<T>& mat2,
            const ImmutableVmVec<s3p_uint64_t>& dim1,
            const ImmutableVmVec<s3p_uint64_t>& dim2,
            const ImmutableVmVec<s3p_uint64_t>& dim3,
            ShareVec<T>& result)
    {

        static constexpr uint64_t radix_point = FixedPointHelper::Radix<typename T::share_type>::value;
        typedef typename double_size<T>::type BigT;

        typename T::public_type bias = 1ul<<(ValueTraits<T>::num_of_bits-1u);
        typename BigT::public_type big_bias = 1ul<<(ValueTraits<T>::num_of_bits-1u);


        ShareVec<T> mat1_biased (mat1.size());
        ShareVec<T> mat2_biased (mat2.size());

        for (size_t i = 0; i < mat1.size(); ++ i) {
            mat1_biased[i] = mat1[i]+bias;
        }
        for (size_t i = 0; i < mat2.size(); ++ i) {
            mat2_biased[i] = mat2[i]+bias;
        }


        ShareVec<BigT> mat2T (mat2.size());

        size_t s = 0;
        for (size_t i = 0; i < dim1.size(); ++ i) {
            for (size_t j = 0; j < dim2[i]; ++ j) {
                for (size_t k = 0; k < dim3[i]; ++ k) {
                    mat2T[s + k * dim2[i] + j] = mat2_biased[s + j * dim3[i] + k];
                }
            }
            s += dim2[i] * dim3[i];
        }

        ShareVec<BigT> mat1_big (mat1.size());

        for (size_t i = 0; i < mat1.size(); ++ i) {
            mat1_big[i] = mat1_biased[i];
        }

        for (size_t i = 0; i < mat1.size(); ++ i) {
            mat1_big[i] = mat1_big[i] - big_bias;
        }
        for (size_t i = 0; i < mat2.size(); ++ i) {
            mat2T[i] = mat2T[i] - big_bias;
        }

        ShareVec<BigT> big_result(result.size());

        size_t s1 = 0;
        size_t s2 = 0;
        size_t s3 = 0;
        for (size_t i = 0; i < dim1.size(); ++ i) {
            for (size_t j = 0; j < dim1[i]; ++ j) {
                for (size_t k = 0; k < dim3[i]; ++ k) {
                    const size_t t3 = s3 + j * dim3[i] + k;
                    big_result[t3] = 0;
                    for (size_t l = 0; l < dim2[i]; ++ l) {
                        const size_t t1 = s1 + j * dim2[i] + l;
                        const size_t t2 = s2 + k * dim2[i] + l;
                        big_result[t3] += mat1_big[t1] * mat2T[t2];
                    }
                }
            }

            s1 += dim1[i] * dim2[i];
            s2 += dim2[i] * dim3[i];
            s3 += dim1[i] * dim3[i];
        }

        for (size_t i = 0; i < result.size(); ++ i){
            big_result[i] >>= radix_point;
            result[i] = demote(big_result[i]);
        }

        return true;
    }

    template <typename T>
    typename std::enable_if<is_unsigned_value_tag<T>::value, bool>::type
    invokeN (const ShareVec<T>& X,
             const ShareVec<T>& Y,
             ShareVec<T>& Z,
             const size_t N, const size_t H, const size_t K, const size_t W)
    {
        static constexpr uint64_t radix_point = FixedPointHelper::Radix<typename T::share_type>::value;
        typedef typename double_size<T>::type BigT;
        using share = typename BigT::share_type;

        typename T::public_type bias = 1ul<<(ValueTraits<T>::num_of_bits-1u);
        typename BigT::public_type big_bias = 1ul<<(ValueTraits<T>::num_of_bits-1u);


        ShareVec<T> X_biased (X.size());
        ShareVec<T> Y_biased (Y.size());

        for (size_t i = 0; i < X.size(); ++ i) {
            X_biased[i] = X[i]+bias;
        }
        for (size_t i = 0; i < Y.size(); ++ i) {
            Y_biased[i] = Y[i]+bias;
        }
        using RowMatrixXi = Eigen::Matrix<share, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor>;

        if (Z.empty())
            return true;

        GUARD (X.size() == (N * H * K));
        GUARD (Y.size() == (N * K * W));
        GUARD (Z.size() == (N * H * W));

        ShareVec<BigT> bigX(X.size());
        ShareVec<BigT> bigY(Y.size());
        ShareVec<BigT> bigZ(Z.size());

        for (size_t i = 0; i < X.size(); ++ i) {
            bigX[i] = X_biased[i] - big_bias;
        }

        for (size_t i = 0; i < Y.size(); ++ i) {
            bigY[i] = Y_biased[i] - big_bias;
        }

        // If batch size is over 8, parallelize across the batch dimension
        // Otherwise, let Eigen parallelize at the matrix level.

        bool parallelizeBatch = (N > 8);
        Eigen::setNbThreads(parallelizeBatch ? 1 : omp_get_max_threads());

        #pragma omp parallel for if(parallelizeBatch)
        for (size_t i = 0; i < N; ++i) {

            size_t x_offset = i * H * K;
            size_t y_offset = i * K * W;
            size_t z_offset = i * H * W;

            // Map data pointers to Eigen matrices (no copy)
            Eigen::Map<const RowMatrixXi> tX(bigX.data() + x_offset, H, K);
            Eigen::Map<const RowMatrixXi> tY(bigY.data() + y_offset, K, W);
            Eigen::Map<RowMatrixXi> tZ(bigZ.data() + z_offset, H, W);

            // .noalias() tells Eigen the output doesn't overlap with inputs,
            // preventing it from allocating temporary arrays under the hood.
            tZ.noalias() = tX * tY;
        }


        for (size_t i = 0; i < Z.size(); ++ i) {
            bigZ[i] >>= radix_point;
            Z[i] = demote(bigZ[i]);
        }

        return true;
    }
};

} /* namespace sharemind { */

#endif /* MOD_SHARED3P_EMU_PROTOCOLS_MATRIXMULTIPLICATIONPROTOCOL_H */
