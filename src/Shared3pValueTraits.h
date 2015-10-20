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

#ifndef MOD_SHARED3P_SHARED3PVALUETRAITS_H
#define MOD_SHARED3P_SHARED3PVALUETRAITS_H

#include <cstdint>
#include <sharemind/3rdparty/libsoftfloat/softfloat.h>
#include <sharemind/uint128_t.h>
#include <sharemind/uint_t.h>


namespace sharemind {

/*
 * Hierarchy of sharemind A3PP types.
 */
struct __attribute__ ((visibility("internal"))) any_value_tag { };
struct __attribute__ ((visibility("internal"))) bool_value_tag : public any_value_tag { };
struct __attribute__ ((visibility("internal"))) numeric_value_tag : public any_value_tag { };
struct __attribute__ ((visibility("internal"))) xored_numeric_value_tag : public any_value_tag { };
struct __attribute__ ((visibility("internal"))) float_numeric_value_tag : public any_value_tag { };
struct __attribute__ ((visibility("internal"))) float32_numeric_value_tag : public float_numeric_value_tag { };
struct __attribute__ ((visibility("internal"))) float64_numeric_value_tag : public float_numeric_value_tag { };
struct __attribute__ ((visibility("internal"))) signed_value_tag : public numeric_value_tag { };
struct __attribute__ ((visibility("internal"))) unsigned_value_tag : public numeric_value_tag { };


/**
 * \brief Type traits to track static information about sharemind types.
 * Instantiating the template with some type succeeds only for legal sharemind types.
 */
template <typename ValueType>
struct __attribute__ ((visibility("internal"))) value_traits {

    /**
     * \brief Static hierarchy of types.
     * Splitting values into various categories allows for simpler overloading
     * for protocol implementations. For example one might have different
     * implementations for signed and unsigned values.
     * \code
     * template <typename T>
     * bool invoke (share_vec<T>& arr, unsigned_value_tag) { ... }
     * template <typename T>
     * bool invoke (share_vec<T>& arr, signed_value_tag) { ... }
     * \endcode
     * The correct one is automatically selected via a single call:
     * \code
     * invoke (arr, typename value_traits<T>::value_category ());
     * \endcode
     */
    typedef typename ValueType::value_category value_category;

    /**
     * \brief Type of shares.
     */
    typedef typename ValueType::share_type share_type;

    /**
     * \brief Type of public representation.
     * The public values are handled by the VM.
     */
    typedef typename ValueType::public_type public_type;

    /**
     * \brief Type identifier.
     * This value is used by the heap to track types.
     */
    static constexpr uint8_t heap_type_id = ValueType::heap_type_id;

    /**
     * \brief Number of bits.
     * This value gives the number of bits used for the type
     */
    static constexpr size_t num_of_bits = ValueType::num_of_bits;

    /**
     * \brief Logarithm of the number of bits.
     * This value gives the logarithm of the number of bits
     */
    static constexpr size_t log_of_bits = ValueType::log_of_bits;

}; /* struct value_traits { */

struct __attribute__ ((visibility("internal"))) s3p_bool_t {
    typedef bool_value_tag value_category;
    typedef bool share_type;
    typedef uint8_t public_type;
    static constexpr uint8_t heap_type_id = 0x1u;
    static constexpr size_t num_of_bits = 1u;
    static constexpr size_t log_of_bits = 0u;
};

struct __attribute__ ((visibility("internal"))) s3p_uint8_t {
    typedef unsigned_value_tag value_category;
    typedef uint8_t share_type;
    typedef uint8_t public_type;
    static constexpr uint8_t heap_type_id = 0x2u;
    static constexpr size_t num_of_bits = 8u;
    static constexpr size_t log_of_bits = 3u;
};

struct __attribute__ ((visibility("internal"))) s3p_uint16_t {
    typedef unsigned_value_tag value_category;
    typedef uint16_t share_type;
    typedef uint16_t public_type;
    static constexpr uint8_t heap_type_id = 0x3u;
    static constexpr size_t num_of_bits = 16u;
    static constexpr size_t log_of_bits = 4u;
};

struct __attribute__ ((visibility("internal"))) s3p_uint32_t {
    typedef unsigned_value_tag value_category;
    typedef uint32_t share_type;
    typedef uint32_t public_type;
    static constexpr uint8_t heap_type_id = 0x4u;
    static constexpr size_t num_of_bits = 32u;
    static constexpr size_t log_of_bits = 5u;
};

struct __attribute__ ((visibility("internal"))) s3p_uint64_t {
    typedef unsigned_value_tag value_category;
    typedef uint64_t share_type;
    typedef uint64_t public_type;
    static constexpr uint8_t heap_type_id = 0x5u;
    static constexpr size_t num_of_bits = 64u;
    static constexpr size_t log_of_bits = 6u;
};

struct __attribute__ ((visibility("internal"))) s3p_uint128_t {
    typedef unsigned_value_tag value_category;
    typedef uint128_t share_type;
    typedef uint128_t public_type;
    static constexpr uint8_t heap_type_id = 0x6u;
    static constexpr size_t num_of_bits = 128u;
    static constexpr size_t log_of_bits = 7u;
};

struct __attribute__ ((visibility("internal"))) s3p_uint256_t {
    typedef unsigned_value_tag value_category;
    typedef uint_t<256> share_type;
    typedef uint_t<256> public_type;
    static constexpr uint8_t heap_type_id = 0x7u;
    static constexpr size_t num_of_bits = 256u;
    static constexpr size_t log_of_bits = 8u;
};


struct __attribute__ ((visibility("internal"))) s3p_int8_t {
    typedef signed_value_tag value_category;
    typedef int8_t share_type;
    typedef int8_t public_type;
    static constexpr uint8_t heap_type_id = 0x8u;
    static constexpr size_t num_of_bits = 8u;
    static constexpr size_t log_of_bits = 3u;
};

struct __attribute__ ((visibility("internal"))) s3p_int16_t {
    typedef signed_value_tag value_category;
    typedef int16_t share_type;
    typedef int16_t public_type;
    static constexpr uint8_t heap_type_id = 0x9u;
    static constexpr size_t num_of_bits = 16u;
    static constexpr size_t log_of_bits = 4u;
};

struct __attribute__ ((visibility("internal"))) s3p_int32_t {
    typedef signed_value_tag value_category;
    typedef int32_t share_type;
    typedef int32_t public_type;
    static constexpr uint8_t heap_type_id = 0xau;
    static constexpr size_t num_of_bits = 32u;
    static constexpr size_t log_of_bits = 5u;
};

struct __attribute__ ((visibility("internal"))) s3p_int64_t {
    typedef signed_value_tag value_category;
    typedef int64_t share_type;
    typedef int64_t public_type;
    static constexpr uint8_t heap_type_id = 0xbu;
    static constexpr size_t num_of_bits = 64u;
    static constexpr size_t log_of_bits = 6u;
};


struct __attribute__ ((visibility("internal"))) s3p_xor_uint8_t {
    typedef xored_numeric_value_tag value_category;
    typedef uint8_t share_type;
    typedef uint8_t public_type;
    static constexpr uint8_t heap_type_id = 0xcu;
    static constexpr size_t num_of_bits = 8u;
    static constexpr size_t log_of_bits = 3u;
};

struct __attribute__ ((visibility("internal"))) s3p_xor_uint16_t {
    typedef xored_numeric_value_tag value_category;
    typedef uint16_t share_type;
    typedef uint16_t public_type;
    static constexpr uint8_t heap_type_id = 0xdu;
    static constexpr size_t num_of_bits = 16u;
    static constexpr size_t log_of_bits = 4u;
};

struct __attribute__ ((visibility("internal"))) s3p_xor_uint32_t {
    typedef xored_numeric_value_tag value_category;
    typedef uint32_t share_type;
    typedef uint32_t public_type;
    static constexpr uint8_t heap_type_id = 0xeu;
    static constexpr size_t num_of_bits = 32u;
    static constexpr size_t log_of_bits = 5u;
};

struct __attribute__ ((visibility("internal"))) s3p_xor_uint64_t {
    typedef xored_numeric_value_tag value_category;
    typedef uint64_t share_type;
    typedef uint64_t public_type;
    static constexpr uint8_t heap_type_id = 0xfu;
    static constexpr size_t num_of_bits = 64u;
    static constexpr size_t log_of_bits = 6u;
};


struct __attribute__ ((visibility("internal"))) s3p_float32_t {
    typedef float32_numeric_value_tag value_category;
    typedef sf_float32 share_type;
    typedef sf_float32 public_type;
    static constexpr uint8_t heap_type_id = 0x10u;
    static constexpr size_t num_of_bits = 32u;
    static constexpr size_t log_of_bits = 5u;
    static constexpr size_t bias = (1u << 7u) - 1u;
};

struct __attribute__ ((visibility("internal"))) s3p_float64_t {
    typedef float64_numeric_value_tag value_category;
    typedef sf_float64 share_type;
    typedef sf_float64 public_type;
    static constexpr uint8_t heap_type_id = 0x11u;
    static constexpr size_t num_of_bits = 64u;
    static constexpr size_t log_of_bits = 6u;
    static constexpr size_t bias = (1u << 10u) - 1u;
};




template <typename T> struct __attribute__ ((visibility("internal"))) double_size;
template <> struct __attribute__ ((visibility("internal"))) double_size<s3p_uint8_t> { typedef s3p_uint16_t type; };
template <> struct __attribute__ ((visibility("internal"))) double_size<s3p_uint16_t> { typedef s3p_uint32_t type; };
template <> struct __attribute__ ((visibility("internal"))) double_size<s3p_uint32_t> { typedef s3p_uint64_t type; };
template <> struct __attribute__ ((visibility("internal"))) double_size<s3p_uint64_t> { typedef s3p_uint128_t type; };
template <> struct __attribute__ ((visibility("internal"))) double_size<s3p_uint128_t> { typedef s3p_uint256_t type; };

template <typename T> struct __attribute__ ((visibility("internal"))) half_size;
template <> struct __attribute__ ((visibility("internal"))) half_size<s3p_uint16_t> { typedef s3p_uint8_t type; };
template <> struct __attribute__ ((visibility("internal"))) half_size<s3p_uint32_t> { typedef s3p_uint16_t type; };
template <> struct __attribute__ ((visibility("internal"))) half_size<s3p_uint64_t> { typedef s3p_uint32_t type; };
template <> struct __attribute__ ((visibility("internal"))) half_size<s3p_uint128_t> { typedef s3p_uint64_t type; };
template <> struct __attribute__ ((visibility("internal"))) half_size<s3p_uint256_t> { typedef s3p_uint128_t type; };

template <typename T> struct respective_unsigned_type { };
template <> struct respective_unsigned_type<s3p_int8_t> { typedef s3p_uint8_t type; };
template <> struct respective_unsigned_type<s3p_int16_t> { typedef s3p_uint16_t type; };
template <> struct respective_unsigned_type<s3p_int32_t> { typedef s3p_uint32_t type; };
template <> struct respective_unsigned_type<s3p_int64_t> { typedef s3p_uint64_t type; };
template <> struct respective_unsigned_type<s3p_xor_uint8_t> { typedef s3p_uint8_t type; };
template <> struct respective_unsigned_type<s3p_xor_uint16_t> { typedef s3p_uint16_t type; };
template <> struct respective_unsigned_type<s3p_xor_uint32_t> { typedef s3p_uint32_t type; };
template <> struct respective_unsigned_type<s3p_xor_uint64_t> { typedef s3p_uint64_t type; };

template <typename T> struct respective_signed_type { };
template <> struct respective_signed_type<s3p_uint8_t> { typedef s3p_int8_t type; };
template <> struct respective_signed_type<s3p_uint16_t> { typedef s3p_int16_t type; };
template <> struct respective_signed_type<s3p_uint32_t> { typedef s3p_int32_t type; };
template <> struct respective_signed_type<s3p_uint64_t> { typedef s3p_int64_t type; };

template <typename T> struct respective_xor_type { };
template <> struct respective_xor_type<s3p_uint8_t> { typedef s3p_xor_uint8_t type; };
template <> struct respective_xor_type<s3p_uint16_t> { typedef s3p_xor_uint16_t type; };
template <> struct respective_xor_type<s3p_uint32_t> { typedef s3p_xor_uint32_t type; };
template <> struct respective_xor_type<s3p_uint64_t> { typedef s3p_xor_uint64_t type; };
template <> struct respective_xor_type<s3p_int8_t> { typedef s3p_xor_uint8_t type; };
template <> struct respective_xor_type<s3p_int16_t> { typedef s3p_xor_uint16_t type; };
template <> struct respective_xor_type<s3p_int32_t> { typedef s3p_xor_uint32_t type; };
template <> struct respective_xor_type<s3p_int64_t> { typedef s3p_xor_uint64_t type; };


} /* namespace sharemind */

#endif /* MOD_SHARED3P_EMU_SHARED3PVALUETRAITS_H */
