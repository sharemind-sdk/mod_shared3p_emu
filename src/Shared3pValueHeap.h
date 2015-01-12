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

#ifndef MOD_SHARED3P_EMU_SHARED3PVALUEHEAP_H
#define MOD_SHARED3P_EMU_SHARED3PVALUEHEAP_H

#include <cstdint>
#include <unordered_map>
#include "Shared3pValueTraits.h"
#include "Shared3pVector.h"


namespace sharemind {

/**
 * \brief Heap of share vectors.
 * This class tracks in a type safe manner all of the allocated share vectors.
 */
class __attribute__ ((visibility("internal"))) s3p_heap {

private: /* Types: */

    typedef std::unordered_map<s3p_vec_base *, uint8_t> impl_t;

public: /* Methods: */

    s3p_heap() = default;
    s3p_heap(const s3p_heap &) = delete;
    s3p_heap & operator=(const s3p_heap &) = delete;

    /**
     * Frees all stored share vectors.
     */
    ~s3p_heap() {
        for (impl_t::value_type & pair : m_pointers)
            delete pair.first;
    }

    /**
     * Insert vector share vector of type T into the heap.
     * \param[in] vec Vector to be inserted into the heap.
     * \retval true If vector was inserted into the heap successfully, and it wasn't stored in the heap before.
     * \retval false If vec was null pointer, or if the vector was already stored in the heap.
     */
    template <typename T>
    bool insert(s3p_vec<T>* vec) {
        uint8_t heap_type_id = value_traits<T>::heap_type_id;
        if (vec != 0)
            return m_pointers.emplace(static_cast<s3p_vec_base *>(vec), heap_type_id).second;

        return false;
    }

    /**
     * Erases a share vector of given type from the heap.
     * \param[in] vec The vector to be erased.
     * \retval true If vector was successfully freed from the heap.
     * \retval false If the vector was not stored, or was stored with incorrect type.
     */
    template <typename T>
    bool erase(s3p_vec<T>* vec) {
        impl_t::iterator i = m_pointers.find(vec);
        if (i != m_pointers.end()) {
            if (i->second == value_traits<T>::heap_type_id) {
                delete i->first;
                m_pointers.erase(i);
                return true;
            }
        }

        return false;
    }

    /**
     * Checks if given handle is stored in the heap with given type.
     * \param[in] hndl A handle to a share vector.
     * \retval true If the \a hndl was stored in the heap with type \a T.
     * \retval false If the handle is not stored in the heap, or is stored with incorrect type.
     */
    template <typename T>
    bool check(void* hndl) const {
        impl_t::const_iterator i = m_pointers.find(static_cast<s3p_vec_base*>(hndl));
        if (i != m_pointers.end())
            return (i->second == value_traits<T>::heap_type_id);

        return false;
    }

private: /* Fields: */

    impl_t m_pointers;

}; /* class s3p_heap { */

} /* namespace sharemind */

#endif /* MOD_SHARED3P_EMU_SHARED3PVALUEHEAP_H */
