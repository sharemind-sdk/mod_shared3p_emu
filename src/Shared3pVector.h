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

#include <boost/static_assert.hpp>
#include <cassert>
#include <cstdint>
#include <iterator>
#include <sharemind/Random/IRandom.h>
#include <vector>
#include "BitVector.h"
#include "Shared3pValueTraits.h"


namespace sharemind {

template <typename T> class s3p_vec;

struct __attribute__ ((visibility("internal"))) s3p_vec_base {
public: /* Types: */
    virtual ~s3p_vec_base () {}
};

template <typename Iter, class DerivedIter>
struct share_iterator_base : std::iterator_traits<Iter> {
    typedef Iter inner_iterator_type;
    typedef std::iterator_traits<Iter> Base;
    typedef typename Base::value_type value_type;
    typedef typename Base::pointer pointer;
    typedef typename Base::reference reference;
    typedef typename Base::difference_type difference_type;

    inner_iterator_type m_it;

    inline share_iterator_base() {}
    inline explicit share_iterator_base(const inner_iterator_type & copy) : m_it(copy) {}
    inline share_iterator_base(const DerivedIter & copy) : m_it(copy.m_it) {}

    inline DerivedIter & self () { return *static_cast<DerivedIter*>(this); }
    inline const DerivedIter & self () const { return *static_cast<const DerivedIter*>(this); }

    inline DerivedIter & operator=(const DerivedIter & rhs) { m_it = rhs.m_it; return self(); }
    inline DerivedIter & operator+=(const difference_type v) { m_it += v; return self(); }
    inline DerivedIter & operator-=(const difference_type v) { m_it -= v; return self(); }

    inline DerivedIter & operator++() { ++m_it; return self(); }
    inline DerivedIter operator++(int) { DerivedIter r(self()); ++m_it; return r; }
    inline DerivedIter & operator--() { --m_it; return self(); }
    inline DerivedIter operator--(int) { DerivedIter r(self()); --m_it; return r; }

    inline DerivedIter operator-(const difference_type v) const { return DerivedIter(m_it - v); }
    inline DerivedIter operator+(const difference_type v) const { return DerivedIter(m_it + v); }
    inline difference_type operator-(const DerivedIter & rhs) const { return m_it - rhs.m_it; }
    inline difference_type operator+(const DerivedIter & rhs) const { return m_it + rhs.m_it; }

    inline bool operator==(const DerivedIter & rhs) const { return m_it == rhs.m_it; }
    inline bool operator!=(const DerivedIter & rhs) const { return m_it != rhs.m_it; }
    inline bool operator<=(const DerivedIter & rhs) const { return m_it <= rhs.m_it; }
    inline bool operator>=(const DerivedIter & rhs) const { return m_it >= rhs.m_it; }
    inline bool operator< (const DerivedIter & rhs) const { return m_it <  rhs.m_it; }
    inline bool operator> (const DerivedIter & rhs) const { return m_it >  rhs.m_it; }

    inline pointer operator->() const { return m_it.operator->(); }
    inline reference operator*() const { return m_it.operator*(); }
    inline reference operator[](const difference_type v) { return m_it[v]; }

    inline const inner_iterator_type & getInner() const { return m_it; }
}; /* struct share_iterator_base { */

template <typename T, class C = s3p_vec<T> >
struct share_iterator: share_iterator_base<typename C::impl_t::iterator, share_iterator<T, C> >
{
    typedef C container_type;
    typedef share_iterator_base<typename C::impl_t::iterator, share_iterator<T, C> > Base;
    typedef typename Base::inner_iterator_type inner_iterator_type;
    typedef typename Base::value_type value_type;
    typedef typename Base::pointer pointer;
    typedef typename Base::reference reference;
    typedef typename Base::difference_type difference_type;

    inline share_iterator() {}
    inline explicit share_iterator(const inner_iterator_type & copy) : Base(copy) {}
}; /* struct share_iterator { */

template <typename T, class C = s3p_vec<T> >
struct share_const_iterator: share_iterator_base<typename C::impl_t::const_iterator, share_const_iterator<T, C> >
{
    typedef C container_type;
    typedef typename C::impl_t::iterator inner_iterator_type;
    typedef typename C::impl_t::const_iterator inner_const_iterator_type;

    typedef share_iterator_base<typename C::impl_t::const_iterator, share_const_iterator<T, C> > Base;
    using typename Base::value_type;
    using typename Base::pointer;
    using typename Base::reference;
    using typename Base::difference_type;

    inline share_const_iterator() {}
    inline explicit share_const_iterator(const inner_iterator_type & copy) : Base(copy) {}
    inline explicit share_const_iterator(const inner_const_iterator_type & copy) : Base(copy) {}
    inline share_const_iterator(const share_iterator<T> & copy) : Base(copy.m_it) {}

}; /* struct share_iterator { */

template <typename Iter>
struct iterator_chain_item {
    Iter begin;
    Iter end;
    iterator_chain_item<Iter> * next;
    iterator_chain_item<Iter> * prev;
}; /* struct iterator_chain_item { */

template <typename Iter>
class chained_iterator: std::iterator_traits<Iter> {
public: /* Types: */

    typedef Iter inner_iterator_type;
    typedef typename inner_iterator_type::container_type container_type;

    typedef std::iterator_traits<Iter> Base;
    typedef typename Base::value_type value_type;
    typedef typename Base::pointer pointer;
    typedef typename Base::reference reference;
    typedef typename Base::difference_type difference_type;

public: /* Methods: */

    inline chained_iterator()
        : m_chain(nullptr) {}

    inline chained_iterator(const chained_iterator<Iter> & copy)
        : m_it(copy.m_it)
        , m_chain(copy.m_chain) {}

    inline chained_iterator<Iter> & operator=(const chained_iterator<Iter> & rhs) {
        m_it = rhs.m_it;
        m_chain = rhs.m_chain;
    }

    inline chained_iterator(const inner_iterator_type pos,
                            const iterator_chain_item<Iter> * const chain)
        : m_it(pos)
        , m_chain(chain) {}

    inline chained_iterator<Iter> & operator++() { increaseInner(); return *this; }
    inline chained_iterator<Iter> operator++(int) { chained_iterator<Iter> r(*this); increaseInner(); return r; }
    inline chained_iterator<Iter> & operator--() { decreaseInner(); return *this; }
    inline chained_iterator<Iter> operator--(int) { chained_iterator<Iter> r(*this); decreaseInner(); return r; }

    inline bool operator==(const chained_iterator<Iter> & rhs) const { return m_chain == rhs.m_chain && m_it == rhs.m_it; }
    inline bool operator!=(const chained_iterator<Iter> & rhs) const { return m_chain != rhs.m_chain || m_it != rhs.m_it; }

    inline pointer operator->() const { return m_it.operator->(); }
    inline reference operator*() const { return m_it.operator*(); }

    inline const inner_iterator_type & getInner() const { return m_it; }

private: /* Methods: */

    inline void increaseInner() {
        ++m_it;
        if (m_chain && m_it == m_chain->end && m_chain->next) {
            m_chain = m_chain->next;
            m_it = m_chain->begin;
        }
    }

    inline void decreaseInner() {
        if (m_chain && m_it == m_chain->begin && m_chain->prev) {
            m_chain = m_chain->prev;
            m_it = m_chain->end;
        }
        --m_it;
    }

private: /* Fields: */

    inner_iterator_type m_it;
    const iterator_chain_item<Iter> * m_chain;

}; /* struct chained_iterator { */

template <typename Iter>
class iterator_chain {

public: /* Methods: */

    inline iterator_chain()
        : m_begin(nullptr)
        , m_end(nullptr) {}

    inline iterator_chain(const iterator_chain<Iter> & copy) {
        assignInner(copy);
    }

    inline ~iterator_chain() {
        destroyInner(m_begin);
    }

    iterator_chain<Iter> & operator=(const iterator_chain<Iter> & rhs) {
        iterator_chain_item<Iter> * oldBegin = m_begin;
        iterator_chain_item<Iter> * oldEnd = m_end;
        try {
            assignInner(rhs);
        } catch (...) {
            m_begin = oldBegin;
            m_end = oldEnd;
            throw;
        }
    }

    chained_iterator<Iter> begin() {
        if (m_begin)
            return chained_iterator<Iter>(m_begin->begin, m_begin);
        return chained_iterator<Iter>();
    }

    chained_iterator<Iter> end() {
        if (m_end)
            return chained_iterator<Iter>(m_end->end, m_end);
        return chained_iterator<Iter>();
    }

    void push_back(const Iter begin, const Iter end) {
        iterator_chain_item<Iter> ** op;
        if (m_end) {
            assert(m_begin);
            op = &(m_end->next);
        } else {
            assert(!m_begin);
            op = &m_begin;
        }
        (*op) = new iterator_chain_item<Iter>;
        (*op)->begin = begin;
        (*op)->end = end;
        (*op)->prev = m_end;
        (*op)->next = nullptr;
        m_end = (*op);
    }

private: /* Methods: */

    inline void assignInner(const iterator_chain<Iter> & v) {
        if (v.m_begin) {
            assert(v.m_end);
            assert(!v.m_begin->prev);
            iterator_chain_item<Iter> ** op = &m_begin;
            try {
                iterator_chain_item<Iter> ** ip = &v.m_begin;
                for (;;) {
                    // Exceptions in this method are only generated by this memory allocation:
                    (*op) = new iterator_chain_item<Iter>(**ip);
                    ip = &((*ip)->next);
                    if (!*ip) {
                        (*op)->next = nullptr;
                        break;
                    }
                    op = &((*op)->next);
                }
                m_end = (*op);
            } catch (...) {
                destroyInner(*op);
                throw;
            }
        } else {
            assert(!v.m_end);
            m_begin = m_end = nullptr;
        }
    }

    static inline void destroyInner(iterator_chain_item<Iter> * chainBegin) {
        while (chainBegin) {
            iterator_chain_item<Iter> * const i = chainBegin;
            chainBegin = chainBegin->next;
            delete i;
        }
    }

private: /* Fields: */

    iterator_chain_item<Iter> * m_begin;
    iterator_chain_item<Iter> * m_end;

};

template <typename Iter >
struct share_iterator_unwrap {
    inline const Iter & operator()(const Iter & it) { return it; }
};

template <class T, class C >
struct share_iterator_unwrap<share_iterator<T, C> > {
    inline const typename share_iterator<T, C>::inner_iterator_type & operator()(const share_iterator<T, C> & it) { return it.getInner(); }
};

template <class T, class C >
struct share_iterator_unwrap<share_const_iterator<T, C> > {
    inline const typename share_const_iterator<T, C>::inner_const_iterator_type & operator()(const share_const_iterator<T, C> & it) { return it.getInner(); }
};

/**
 * Flat vector of shares.
 */
template <typename T>
class __attribute__ ((visibility("internal"))) s3p_vec : public s3p_vec_base {
public: /* Types: */

    typedef T value_traits;
    typedef typename T::share_type value_type;
    typedef typename std::vector<value_type> impl_t;
    typedef typename impl_t::reference reference;
    typedef typename impl_t::const_reference const_reference;
    typedef typename impl_t::size_type size_type;

    typedef share_iterator<T, s3p_vec<T> > iterator;
    typedef share_const_iterator<T, s3p_vec<T> > const_iterator;

public: /* Methods: */

    inline s3p_vec () {}
    s3p_vec(const s3p_vec &) = delete;
    s3p_vec & operator=(const s3p_vec &) = delete;

    explicit inline s3p_vec (size_type size, const value_type& defaultValue = value_type ())
        : m_vector (size, defaultValue)
    { }

    template <typename InputIterator>
    s3p_vec (InputIterator begin, InputIterator end)
        : m_vector (begin, end)
    { }

    inline value_type * data() { return m_vector.data(); }
    inline const value_type * data() const { return m_vector.data(); }
    inline const value_type * cdata() const { return m_vector.data(); }
    inline size_type size() const { return m_vector.size (); }
    inline iterator begin() { return iterator(m_vector.begin()); }
    inline iterator end() { return iterator(m_vector.end()); }
    inline const_iterator cbegin() const { return const_iterator(m_vector.begin()); }
    inline const_iterator cend() const { return const_iterator(m_vector.end()); }
    inline const_iterator begin() const { return cbegin(); }
    inline const_iterator end() const { return cend(); }
    inline void resize (size_type sz) { m_vector.resize (sz); }
    inline bool empty () const { return m_vector.empty (); }

    inline const_reference operator [] (size_type i) const {
        assert (i < size () && "operator[]: Index out of bounds.");
        return m_vector[i];
    }

    inline reference operator [] (size_type i) {
        assert (i < size () && "operator[]: Index out of bounds.");
        return m_vector[i];
    }

    inline void clear_and_release () {
        impl_t ().swap (m_vector);
    }

    template <typename InputIterator>
    inline void insert(iterator pos, InputIterator first, InputIterator last) {
        m_vector.insert(pos.getInner(),
                        share_iterator_unwrap<InputIterator>()(first),
                        share_iterator_unwrap<InputIterator>()(last));
    }

    template <typename InputIterator>
    inline void assign(InputIterator first, InputIterator last) {
        m_vector.assign(share_iterator_unwrap<InputIterator>()(first),
                        share_iterator_unwrap<InputIterator>()(last));
    }

    inline void push_back (const_reference val) {
        m_vector.push_back(val);
    }

    inline void assign (const s3p_vec& vec) {
        assign (vec.begin (), vec.end ());
    }


    inline void randomize(IRandom & rng) {
        if (!empty()) {
            rng.fillBlock(begin_ptr (), end_ptr ());
        }
    }

    void setBit (size_type i, bool value) {
        BOOST_STATIC_ASSERT(T::num_of_bits != 0);
        const size_type block_index = i / T::num_of_bits;
        const value_type mask = value_type (1) << (i % T::num_of_bits);
        assert (block_index < m_vector.size ());
        value_type& block = m_vector[block_index];
        if (value)
            block |= mask;
        else
            block &= ~ mask;
    }

    inline bool getBit (size_type i) {
        BOOST_STATIC_ASSERT(T::num_of_bits != 0);
        const size_type bit_index = i % T::num_of_bits;
        const size_type block_index = i / T::num_of_bits;
        assert (block_index < m_vector.size ());
        return (m_vector[block_index] & (value_type (1) << bit_index)) != 0;
    }

private: /* Methods: */

    // \returns nullptr if the vector is empty.
    inline value_type * begin_ptr () { return empty() ? nullptr : &*begin(); }
    inline value_type* end_ptr () { return begin_ptr () + size (); }
    inline const value_type * begin_ptr () const { return empty() ? nullptr : &*begin(); }
    inline const value_type* end_ptr () const { return begin_ptr () + size (); }

private: /* Fields: */

    impl_t m_vector;

}; /* class s3p_vec { */

/* Just a facade for bit vectors, extend as new requirements emerge */
template <>
class __attribute__ ((visibility("internal"))) s3p_vec <s3p_bool_t> : public s3p_vec_base {
public: /* Types: */

    typedef s3p_bool_t::share_type value_type;
    typedef bit_vector<uint64_t> impl_t;
    typedef impl_t::reference reference;
    typedef impl_t::const_reference const_reference;
    typedef impl_t::size_type size_type;

public: /* Methods: */

    inline s3p_vec () {}
    s3p_vec(const s3p_vec &) = delete;
    s3p_vec & operator=(const s3p_vec &) = delete;

    explicit inline s3p_vec (size_type size, const value_type& defaultValue = value_type ())
        : m_vector (size, defaultValue)
    { }

    ~s3p_vec () { }

    inline size_type size() const { return m_vector.size (); }
    inline void resize (size_type sz) { m_vector.resize (sz); }
    inline bool empty () const { return m_vector.empty (); }
    inline void assign (const s3p_vec& vec) { m_vector.assign (vec.m_vector); }
    template <typename T>
    inline void assignBits (const s3p_vec<T>& vec) {
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

    inline void randomize (IRandom& rng) { m_vector.randomize (rng); }

    s3p_vec& operator &= (const s3p_vec& other) { m_vector &= other.m_vector; return *this; }
    s3p_vec& operator ^= (const s3p_vec& other) { m_vector ^= other.m_vector; return *this; }

    s3p_vec& flip () { m_vector.flip (); return *this; }


private: /* Methods: */

    template <typename T>
    void assignBits_ (const s3p_vec<T>& vec, bool_value_tag) {
        assign (vec);
    }

    template <typename T>
    void assignBits_ (const s3p_vec<T>& vec, xored_numeric_value_tag) {
        m_vector.assignBits (vec.begin (), vec.end ());
    }

private: /* Fields: */

    impl_t m_vector;

}; /* class s3p_vec <s3p_bool_t> { */


} /* namespace sharemind { */

#endif /* MOD_SHARED3P_EMU_SHARED3PVECTOR_H */
