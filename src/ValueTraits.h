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

#ifndef MOD_SHARED3P_EMU_VALUETRAITS_H
#define MOD_SHARED3P_EMU_VALUETRAITS_H

#include <cstdint>
#include <sharemind/3rdparty/libsoftfloat/softfloat.h>
#include <sharemind/libemulator_protocols/ValueTraits.h>
#include <sharemind/uint128_t.h>
#include <sharemind/uint_t.h>
#include <type_traits>


namespace sharemind {

/*
 * Hierarchy of sharemind shared3p types.
 */
struct __attribute__ ((visibility("internal"))) bool_value_tag : public any_value_tag { };
struct __attribute__ ((visibility("internal"))) numeric_value_tag : public any_value_tag { };
struct __attribute__ ((visibility("internal"))) xored_numeric_value_tag : public any_value_tag { };
struct __attribute__ ((visibility("internal"))) float_numeric_value_tag : public any_value_tag { };
struct __attribute__ ((visibility("internal"))) float32_numeric_value_tag : public float_numeric_value_tag { };
struct __attribute__ ((visibility("internal"))) float64_numeric_value_tag : public float_numeric_value_tag { };
struct __attribute__ ((visibility("internal"))) signed_value_tag : public numeric_value_tag { };
struct __attribute__ ((visibility("internal"))) unsigned_value_tag : public numeric_value_tag { };

template <typename T>
struct __attribute__ ((visibility("internal"))) is_bool_value_tag :
    std::is_base_of<bool_value_tag, typename value_traits<T>::value_category>
{ };

template <typename T>
struct __attribute__ ((visibility("internal"))) is_unsigned_value_tag :
    std::is_base_of<unsigned_value_tag, typename value_traits<T>::value_category>
{ };

template <typename T>
struct __attribute__ ((visibility("internal"))) is_signed_value_tag :
    std::is_base_of<signed_value_tag, typename value_traits<T>::value_category>
{ };

template <typename T>
struct __attribute__ ((visibility("internal"))) is_integral_value_tag :
    std::integral_constant<bool, (is_unsigned_value_tag<T>::value || is_signed_value_tag<T>::value)>
{ };

template <typename T>
struct __attribute__ ((visibility("internal"))) is_xor_value_tag :
    std::is_base_of<xored_numeric_value_tag, typename value_traits<T>::value_category>
{ };

template <typename T>
struct __attribute__ ((visibility("internal"))) is_float_value_tag :
    std::is_base_of<float_numeric_value_tag, typename value_traits<T>::value_category>
{ };

struct __attribute__ ((visibility("internal"))) s3p_bool_t {
    using value_category = bool_value_tag;
    using share_type = bool;
    using public_type = uint8_t;
    static constexpr uint8_t heap_type_id = 0x5u;
    static constexpr size_t num_of_bits = 1u;
    static constexpr size_t log_of_bits = 0u;
};

struct __attribute__ ((visibility("internal"))) s3p_uint8_t {
    using value_category = unsigned_value_tag;
    using share_type = uint8_t;
    using public_type = uint8_t;
    static constexpr uint8_t heap_type_id = 0x1u;
    static constexpr size_t num_of_bits = 8u;
    static constexpr size_t log_of_bits = 3u;
};

struct __attribute__ ((visibility("internal"))) s3p_uint16_t {
    using value_category = unsigned_value_tag;
    using share_type = uint16_t;
    using public_type = uint16_t;
    static constexpr uint8_t heap_type_id = 0x2u;
    static constexpr size_t num_of_bits = 16u;
    static constexpr size_t log_of_bits = 4u;
};

struct __attribute__ ((visibility("internal"))) s3p_uint32_t {
    using value_category = unsigned_value_tag;
    using share_type = uint32_t;
    using public_type = uint32_t;
    static constexpr uint8_t heap_type_id = 0x3u;
    static constexpr size_t num_of_bits = 32u;
    static constexpr size_t log_of_bits = 5u;
};

struct __attribute__ ((visibility("internal"))) s3p_uint64_t {
    using value_category = unsigned_value_tag;
    using share_type = uint64_t;
    using public_type = uint64_t;
    static constexpr uint8_t heap_type_id = 0x4u;
    static constexpr size_t num_of_bits = 64u;
    static constexpr size_t log_of_bits = 6u;
};

struct __attribute__ ((visibility("internal"))) s3p_uint128_t {
    using value_category = unsigned_value_tag;
    using share_type = uint128_t;
    using public_type = uint128_t;
    static constexpr uint8_t heap_type_id = 0xcu;
    static constexpr size_t num_of_bits = 128u;
    static constexpr size_t log_of_bits = 7u;
};

struct __attribute__ ((visibility("internal"))) s3p_uint256_t {
    using value_category = unsigned_value_tag;
    using share_type = uint_t<256>;
    using public_type = uint_t<256>;
    static constexpr uint8_t heap_type_id = 0xdu;
    static constexpr size_t num_of_bits = 256u;
    static constexpr size_t log_of_bits = 8u;
};

struct __attribute__ ((visibility("internal"))) s3p_int8_t {
    using value_category = signed_value_tag;
    using share_type = int8_t;
    using public_type = int8_t;
    static constexpr uint8_t heap_type_id = 0xdu;
    static constexpr size_t num_of_bits = 8u;
    static constexpr size_t log_of_bits = 3u;
};

struct __attribute__ ((visibility("internal"))) s3p_int16_t {
    using value_category = signed_value_tag;
    using share_type = int16_t;
    using public_type = int16_t;
    static constexpr uint8_t heap_type_id = 0xeu;
    static constexpr size_t num_of_bits = 16u;
    static constexpr size_t log_of_bits = 4u;
};

struct __attribute__ ((visibility("internal"))) s3p_int32_t {
    using value_category = signed_value_tag;
    using share_type = int32_t;
    using public_type = int32_t;
    static constexpr uint8_t heap_type_id = 0xfu;
    static constexpr size_t num_of_bits = 32u;
    static constexpr size_t log_of_bits = 5u;
};

struct __attribute__ ((visibility("internal"))) s3p_int64_t {
    using value_category = signed_value_tag;
    using share_type = int64_t;
    using public_type = int64_t;
    static constexpr uint8_t heap_type_id = 0x10u;
    static constexpr size_t num_of_bits = 64u;
    static constexpr size_t log_of_bits = 6u;
};

struct __attribute__ ((visibility("internal"))) s3p_xor_uint8_t {
    using value_category = xored_numeric_value_tag;
    using share_type = uint8_t;
    using public_type = uint8_t;
    static constexpr uint8_t heap_type_id = 0x6u;
    static constexpr size_t num_of_bits = 8u;
    static constexpr size_t log_of_bits = 3u;
};

struct __attribute__ ((visibility("internal"))) s3p_xor_uint16_t {
    using value_category = xored_numeric_value_tag;
    using share_type = uint16_t;
    using public_type = uint16_t;
    static constexpr uint8_t heap_type_id = 0x7u;
    static constexpr size_t num_of_bits = 16u;
    static constexpr size_t log_of_bits = 4u;
};

struct __attribute__ ((visibility("internal"))) s3p_xor_uint32_t {
    using value_category = xored_numeric_value_tag;
    using share_type = uint32_t;
    using public_type = uint32_t;
    static constexpr uint8_t heap_type_id = 0x8u;
    static constexpr size_t num_of_bits = 32u;
    static constexpr size_t log_of_bits = 5u;
};

struct __attribute__ ((visibility("internal"))) s3p_xor_uint64_t {
    using value_category = xored_numeric_value_tag;
    using share_type = uint64_t;
    using public_type = uint64_t;
    static constexpr uint8_t heap_type_id = 0x9u;
    static constexpr size_t num_of_bits = 64u;
    static constexpr size_t log_of_bits = 6u;
};

struct __attribute__ ((visibility("internal"))) s3p_float32_t {
    using value_category = float32_numeric_value_tag;
    using share_type = sf_float32;
    using public_type = sf_float32;
    static constexpr uint8_t heap_type_id = 0xcu;
    static constexpr size_t num_of_bits = 32u;
    static constexpr size_t log_of_bits = 5u;
    static constexpr size_t bias = (1u << 7u) - 1u;
};

struct __attribute__ ((visibility("internal"))) s3p_float64_t {
    using value_category = float64_numeric_value_tag;
    using share_type = sf_float64;
    using public_type = sf_float64;
    static constexpr uint8_t heap_type_id = 0xdu;
    static constexpr size_t num_of_bits = 64u;
    static constexpr size_t log_of_bits = 6u;
    static constexpr size_t bias = (1u << 10u) - 1u;
};

// Define a type in a struct specialization
#define DEFINE_TYPE_IN(STRUCT_NAME,SPEC,VALUE) \
    template <> struct __attribute__ ((visibility("internal"))) STRUCT_NAME<SPEC> { using type = VALUE; };

template <typename T> struct __attribute__ ((visibility("internal"))) double_size;
DEFINE_TYPE_IN(double_size, s3p_uint8_t, s3p_uint16_t)
DEFINE_TYPE_IN(double_size, s3p_uint16_t, s3p_uint32_t)
DEFINE_TYPE_IN(double_size, s3p_uint32_t, s3p_uint64_t)
DEFINE_TYPE_IN(double_size, s3p_uint64_t, s3p_uint128_t)
DEFINE_TYPE_IN(double_size, s3p_uint128_t, s3p_uint256_t)
DEFINE_TYPE_IN(double_size, s3p_int8_t, s3p_int16_t)
DEFINE_TYPE_IN(double_size, s3p_int16_t, s3p_int32_t)
DEFINE_TYPE_IN(double_size, s3p_int32_t, s3p_int64_t)

template <typename T> struct __attribute__ ((visibility("internal"))) half_size;
DEFINE_TYPE_IN(half_size, s3p_uint16_t, s3p_uint8_t)
DEFINE_TYPE_IN(half_size, s3p_uint32_t, s3p_uint16_t)
DEFINE_TYPE_IN(half_size, s3p_uint64_t, s3p_uint32_t)
DEFINE_TYPE_IN(half_size, s3p_uint128_t, s3p_uint64_t)
DEFINE_TYPE_IN(half_size, s3p_uint256_t, s3p_uint128_t)

template <typename T> struct __attribute__ ((visibility("internal"))) respective_unsigned_type;
DEFINE_TYPE_IN(respective_unsigned_type, s3p_int8_t, s3p_uint8_t)
DEFINE_TYPE_IN(respective_unsigned_type, s3p_int16_t, s3p_uint16_t)
DEFINE_TYPE_IN(respective_unsigned_type, s3p_int32_t, s3p_uint32_t)
DEFINE_TYPE_IN(respective_unsigned_type, s3p_int64_t, s3p_uint64_t)
DEFINE_TYPE_IN(respective_unsigned_type, s3p_xor_uint8_t, s3p_uint8_t)
DEFINE_TYPE_IN(respective_unsigned_type, s3p_xor_uint16_t, s3p_uint16_t)
DEFINE_TYPE_IN(respective_unsigned_type, s3p_xor_uint32_t, s3p_uint32_t)
DEFINE_TYPE_IN(respective_unsigned_type, s3p_xor_uint64_t, s3p_uint64_t)

template <typename T> struct __attribute__ ((visibility("internal"))) respective_signed_type;
DEFINE_TYPE_IN(respective_signed_type, s3p_uint8_t, s3p_int8_t)
DEFINE_TYPE_IN(respective_signed_type, s3p_uint16_t, s3p_int16_t)
DEFINE_TYPE_IN(respective_signed_type, s3p_uint32_t, s3p_int32_t)
DEFINE_TYPE_IN(respective_signed_type, s3p_uint64_t, s3p_int64_t)
DEFINE_TYPE_IN(respective_signed_type, s3p_xor_uint8_t, s3p_int8_t)
DEFINE_TYPE_IN(respective_signed_type, s3p_xor_uint16_t, s3p_int16_t)
DEFINE_TYPE_IN(respective_signed_type, s3p_xor_uint32_t, s3p_int32_t)
DEFINE_TYPE_IN(respective_signed_type, s3p_xor_uint64_t, s3p_int64_t)

template <typename T> struct __attribute__ ((visibility("internal"))) respective_xor_type;
DEFINE_TYPE_IN(respective_xor_type, s3p_uint8_t, s3p_xor_uint8_t)
DEFINE_TYPE_IN(respective_xor_type, s3p_uint16_t, s3p_xor_uint16_t)
DEFINE_TYPE_IN(respective_xor_type, s3p_uint32_t, s3p_xor_uint32_t)
DEFINE_TYPE_IN(respective_xor_type, s3p_uint64_t, s3p_xor_uint64_t)
DEFINE_TYPE_IN(respective_xor_type, s3p_int8_t, s3p_xor_uint8_t)
DEFINE_TYPE_IN(respective_xor_type, s3p_int16_t, s3p_xor_uint16_t)
DEFINE_TYPE_IN(respective_xor_type, s3p_int32_t, s3p_xor_uint32_t)
DEFINE_TYPE_IN(respective_xor_type, s3p_int64_t, s3p_xor_uint64_t)

#undef DEFINE_TYPE_IN

} /* namespace sharemind */

#endif /* MOD_SHARED3P_EMU_VALUETRAITS_H */
