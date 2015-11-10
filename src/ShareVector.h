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

#ifndef MOD_SHARED3P_EMU_SHAREVECTOR_H
#define MOD_SHARED3P_EMU_SHAREVECTOR_H

#include <cassert>
#include <cstdint>
#include <iterator>
#include <sharemind/libemulator_protocols/BitVector.h>
#include <sharemind/libemulator_protocols/ShareVector.h>
#include <sharemind/Random/RandomEngine.h>
#include "ValueTraits.h"


namespace sharemind {

/* Just a facade for bit vectors, extend as new requirements emerge */
template <>
class __attribute__ ((visibility("internal"))) share_vec <s3p_bool_t> : public share_vec_base {
public: /* Types: */

    typedef s3p_bool_t::share_type value_type;
    typedef bit_vector<uint64_t> impl_t;
    typedef impl_t::reference reference;
    typedef impl_t::const_reference const_reference;
    typedef impl_t::size_type size_type;

public: /* Methods: */

    inline share_vec () {}
    share_vec(const share_vec &) = delete;
    share_vec & operator=(const share_vec &) = delete;

    explicit inline share_vec (size_type size, const value_type& defaultValue = value_type ())
        : m_vector (size, defaultValue)
    { }

    ~share_vec () { }

    inline size_type size() const { return m_vector.size (); }
    inline void resize (size_type sz) { m_vector.resize (sz); }
    inline bool empty () const { return m_vector.empty (); }
    inline void assign (const share_vec& vec) { m_vector.assign (vec.m_vector); }
    template <typename T>
    inline void assignBits (const share_vec<T>& vec) {
        return assignBits_ (vec, typename T::value_category ());
    }

    inline const_reference operator [] (size_type i) const {
        assert (i < size () && "operator[]: Index out of bounds.");
        return m_vector[i];
    }

    inline reference operator [] (size_type i) {
        assert (i < size () && "operator[]: Index out of bounds.");
        return m_vector[i];
    }

    template <typename InputIterator>
    inline void assign(InputIterator begin, InputIterator end) {
        m_vector.assign (begin, end);
    }

    inline void clear_and_release () {
        m_vector.clear_and_release ();
    }

    inline void randomize (RandomEngine & rng) { m_vector.randomize (rng); }

    share_vec& operator &= (const share_vec& other) { m_vector &= other.m_vector; return *this; }
    share_vec& operator ^= (const share_vec& other) { m_vector ^= other.m_vector; return *this; }

    share_vec& flip () { m_vector.flip (); return *this; }


private: /* Methods: */

    template <typename T>
    void assignBits_ (const share_vec<T>& vec, bool_value_tag) {
        assign (vec);
    }

    template <typename T>
    void assignBits_ (const share_vec<T>& vec, xored_numeric_value_tag) {
        m_vector.assignBits (vec.begin (), vec.end ());
    }

private: /* Fields: */

    impl_t m_vector;

}; /* class share_vec <s3p_bool_t> { */

} /* namespace sharemind { */

#endif /* MOD_SHARED3P_EMU_SHAREVECTOR_H */
