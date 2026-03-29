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

#ifndef MOD_SHARED3P_EMU_SHARED3PVECTOR_H
#define MOD_SHARED3P_EMU_SHARED3PVECTOR_H

#include <sharemind/ShareVector.h>

#include "Shared3pValueTraits.h"


namespace sharemind {

/* Just a facade for bit vectors, extend as new requirements emerge */
template <>
class __attribute__ ((visibility("internal"))) ShareVec <s3p_bool_t> : public BitShareVec <s3p_bool_t> {

public: /* Methods: */

    using BitShareVec<s3p_bool_t>::BitShareVec;

    template <typename T>
    inline void assignBits (const ShareVec<T>& vec) {
        return assignBits_ (vec, vec.value_category ());
    }

private: /* Methods: */

    template <typename T>
    void assignBits_ (const ShareVec<T>& vec, bool_value_tag) {
        assign (vec);
    }

    template <typename T>
    void assignBits_ (const ShareVec<T>& vec, xored_numeric_value_tag) {
        const size_t input_size = vec.size();
        const size_t bits_per_input_element = ValueTraits<T>::num_of_bits;
        const size_t total_bits = input_size * bits_per_input_element;

        resize(total_bits);

        for (size_t i = 0; i < input_size; ++i) {
            const auto& input_element = vec[i];
            for (size_t bit_pos = 0; bit_pos < bits_per_input_element; ++bit_pos) {
                const bool bit_value = (input_element & (static_cast<typename T::share_type>(1) << bit_pos)) != 0;
                const size_t output_index = i * bits_per_input_element + bit_pos;
                (*this)[output_index] = bit_value;
            }
        }
    }

}; /* class ShareVec <s3p_bool_t> { */


} /* namespace sharemind { */

#endif /* MOD_SHARED3P_EMU_SHARED3PVECTOR_H */
