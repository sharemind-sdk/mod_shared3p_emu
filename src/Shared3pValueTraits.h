/*
 * This file is a part of the Sharemind framework.
 * Copyright (C) Cybernetica AS
 *
 * All rights are reserved. Reproduction in whole or part is prohibited
 * without the written consent of the copyright owner. The usage of this
 * code is subject to the appropriate license agreement.
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
struct __attribute__ ((visibility("internal"))) fix_numeric_value_tag : public numeric_value_tag { };
struct __attribute__ ((visibility("internal"))) fix32_numeric_value_tag : public fix_numeric_value_tag { };
struct __attribute__ ((visibility("internal"))) fix64_numeric_value_tag : public fix_numeric_value_tag { };
struct __attribute__ ((visibility("internal"))) fix128_numeric_value_tag : public fix_numeric_value_tag { };
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
    static constexpr uint8_t heap_type_id = 0x5u;
    static constexpr size_t num_of_bits = 1u;
    static constexpr size_t log_of_bits = 0u;
};

struct __attribute__ ((visibility("internal"))) s3p_uint8_t {
    typedef unsigned_value_tag value_category;
    typedef uint8_t share_type;
    typedef uint8_t public_type;
    static constexpr uint8_t heap_type_id = 0x1u;
    static constexpr size_t num_of_bits = 8u;
    static constexpr size_t log_of_bits = 3u;
};

struct __attribute__ ((visibility("internal"))) s3p_uint16_t {
    typedef unsigned_value_tag value_category;
    typedef uint16_t share_type;
    typedef uint16_t public_type;
    static constexpr uint8_t heap_type_id = 0x2u;
    static constexpr size_t num_of_bits = 16u;
    static constexpr size_t log_of_bits = 4u;
};

struct __attribute__ ((visibility("internal"))) s3p_uint32_t {
    typedef unsigned_value_tag value_category;
    typedef uint32_t share_type;
    typedef uint32_t public_type;
    static constexpr uint8_t heap_type_id = 0x3u;
    static constexpr size_t num_of_bits = 32u;
    static constexpr size_t log_of_bits = 5u;
};

struct __attribute__ ((visibility("internal"))) s3p_uint64_t {
    typedef unsigned_value_tag value_category;
    typedef uint64_t share_type;
    typedef uint64_t public_type;
    static constexpr uint8_t heap_type_id = 0x4u;
    static constexpr size_t num_of_bits = 64u;
    static constexpr size_t log_of_bits = 6u;
};

struct __attribute__ ((visibility("internal"))) s3p_uint128_t {
    typedef unsigned_value_tag value_category;
    typedef uint128_t share_type;
    typedef uint128_t public_type;
    static constexpr uint8_t heap_type_id = 0xcu;
    static constexpr size_t num_of_bits = 128u;
    static constexpr size_t log_of_bits = 7u;
};

struct __attribute__ ((visibility("internal"))) s3p_uint256_t {
    typedef unsigned_value_tag value_category;
    typedef uint_t<256> share_type;
    typedef uint_t<256> public_type;
    static constexpr uint8_t heap_type_id = 0xdu;
    static constexpr size_t num_of_bits = 256u;
    static constexpr size_t log_of_bits = 8u;
};


struct __attribute__ ((visibility("internal"))) s3p_int8_t {
    typedef signed_value_tag value_category;
    typedef uint8_t share_type;
    typedef int8_t public_type;
    static constexpr uint8_t heap_type_id = 0xdu;
    static constexpr size_t num_of_bits = 8u;
    static constexpr size_t log_of_bits = 3u;
};

struct __attribute__ ((visibility("internal"))) s3p_int16_t {
    typedef signed_value_tag value_category;
    typedef uint16_t share_type;
    typedef int16_t public_type;
    static constexpr uint8_t heap_type_id = 0xeu;
    static constexpr size_t num_of_bits = 16u;
    static constexpr size_t log_of_bits = 4u;
};

struct __attribute__ ((visibility("internal"))) s3p_int32_t {
    typedef signed_value_tag value_category;
    typedef uint32_t share_type;
    typedef int32_t public_type;
    static constexpr uint8_t heap_type_id = 0xfu;
    static constexpr size_t num_of_bits = 32u;
    static constexpr size_t log_of_bits = 5u;
};

struct __attribute__ ((visibility("internal"))) s3p_int64_t {
    typedef signed_value_tag value_category;
    typedef uint64_t share_type;
    typedef int64_t public_type;
    static constexpr uint8_t heap_type_id = 0x10u;
    static constexpr size_t num_of_bits = 64u;
    static constexpr size_t log_of_bits = 6u;
};


struct __attribute__ ((visibility("internal"))) s3p_xor_uint8_t {
    typedef xored_numeric_value_tag value_category;
    typedef uint8_t share_type;
    typedef uint8_t public_type;
    static constexpr uint8_t heap_type_id = 0x6u;
    static constexpr size_t num_of_bits = 8u;
    static constexpr size_t log_of_bits = 3u;
};

struct __attribute__ ((visibility("internal"))) s3p_xor_uint16_t {
    typedef xored_numeric_value_tag value_category;
    typedef uint16_t share_type;
    typedef uint16_t public_type;
    static constexpr uint8_t heap_type_id = 0x7u;
    static constexpr size_t num_of_bits = 16u;
    static constexpr size_t log_of_bits = 4u;
};

struct __attribute__ ((visibility("internal"))) s3p_xor_uint32_t {
    typedef xored_numeric_value_tag value_category;
    typedef uint32_t share_type;
    typedef uint32_t public_type;
    static constexpr uint8_t heap_type_id = 0x8u;
    static constexpr size_t num_of_bits = 32u;
    static constexpr size_t log_of_bits = 5u;
};

struct __attribute__ ((visibility("internal"))) s3p_xor_uint64_t {
    typedef xored_numeric_value_tag value_category;
    typedef uint64_t share_type;
    typedef uint64_t public_type;
    static constexpr uint8_t heap_type_id = 0x9u;
    static constexpr size_t num_of_bits = 64u;
    static constexpr size_t log_of_bits = 6u;
};

typedef uint32_t s3p_fix32_value;
typedef uint64_t s3p_fix64_value;
typedef uint128_t s3p_fix128_value;

struct __attribute__ ((visibility("internal"))) s3p_fix32_t {
    typedef fix32_numeric_value_tag value_category;
    typedef s3p_fix32_value share_type;
    typedef sf_float32 public_type;
    typedef s3p_uint32_t value_type;
    static constexpr uint8_t heap_type_id = 0xau;
    static constexpr size_t num_of_bits = 32u;
    static constexpr size_t log_of_bits = 5u;
    static constexpr size_t radix_point = 23u;
    static constexpr size_t erf_radix_point = 26u;
    static constexpr size_t exp_radix_point = 30u;
    static constexpr size_t inv_radix_point = 25u;
    static constexpr size_t sqrt_radix_point = 31u;
};

struct __attribute__ ((visibility("internal"))) s3p_fix64_t {
    typedef fix64_numeric_value_tag value_category;
    typedef s3p_fix64_value share_type;
    typedef sf_float64 public_type;
    typedef s3p_uint64_t value_type;
    static constexpr uint8_t heap_type_id = 0xbu;
    static constexpr size_t num_of_bits = 64u;
    static constexpr size_t log_of_bits = 6u;
    static constexpr size_t radix_point = 51u;
    static constexpr size_t erf_radix_point = 51u;
    static constexpr size_t exp_radix_point = 62u;
    static constexpr size_t inv_radix_point = 52u;
    static constexpr size_t sqrt_radix_point = 52u;
};

struct __attribute__ ((visibility("internal"))) s3p_fix128_t {
    typedef fix128_numeric_value_tag value_category;
    typedef s3p_fix128_value share_type;
    typedef sf_float64 public_type;
    typedef s3p_uint128_t value_type;
    static constexpr uint8_t heap_type_id = 0xcu;
    static constexpr size_t num_of_bits = 128u;
    static constexpr size_t log_of_bits = 7u;
    static constexpr size_t radix_point = 85u;
    static constexpr size_t erf_radix_point = 85u;
    static constexpr size_t exp_radix_point = 85u;
    static constexpr size_t inv_radix_point = 85u;
    static constexpr size_t sqrt_radix_point = 85u;
};

struct __attribute__ ((visibility("internal"))) __attribute__ ((packed)) s3p_float32_value {
    s3p_uint8_t::share_type sign;
    s3p_uint32_t::share_type significand;
    s3p_uint16_t::share_type exponent;

    s3p_float32_value ()
    : sign (0)
    , significand (0)
    , exponent (0)
    { }
};

struct __attribute__ ((visibility("internal"))) __attribute__ ((packed)) s3p_float64_value {
    s3p_uint8_t::share_type sign;
    s3p_uint64_t::share_type significand;
    s3p_uint16_t::share_type exponent;

    s3p_float64_value ()
    : sign (0)
    , significand (0)
    , exponent (0)
    { }
};

struct __attribute__ ((visibility("internal"))) s3p_float32_t {
    typedef float32_numeric_value_tag value_category;
    typedef s3p_float32_value share_type;
    typedef s3p_uint8_t sign_type;
    typedef s3p_uint32_t significand_type;
    // TODO Not nice
    typedef s3p_xor_uint32_t bitwise_significand_type;
    // TODO Not nice
    typedef s3p_xor_uint16_t bitwise_exponent_type;
    typedef s3p_uint16_t exponent_type;
    typedef sf_float32 public_type;
    static constexpr uint8_t heap_type_id = 0xcu;
    static constexpr size_t num_of_bits = 32u;
    static constexpr size_t log_of_bits = 5u;
    // Originally, bias was 2**15 - 1,
    // but we needed to compare exponents in the erf protocol
    // and current a3p comparisons can't compare the highest bit
    // Bias: 2**14 - 1
    static constexpr size_t bias = (1u << 14u) - 1u;
    // See comment about bias
    // zero_exp: 2**14 - 127u
    static constexpr size_t zero_exp = bias - 126u;
    // This is used in public multiplication and division
    static constexpr size_t exp_range = 256u;
    static constexpr size_t sine_precision = 5u;
    static constexpr size_t natural_log_precision = 6u;
    static constexpr size_t square_root_precision = 8u;
    static constexpr size_t erf_precision = 16u;

    static constexpr significand_type::share_type
            natural_log_ln2_significand = 2977044480ul,
            square_root_sqrt2_significand = 3037000448ul,
            two_over_sqrt_pi_significand = 2423175936ul,
            two_over_sqrt_pi_softfloat = 1066430139ul,
            one_significand = 2147483648ul,
            error_function_significand1 = 2188969472ul,
            error_function_significand2 = 2443808256ul,
            error_function_significand3 = 3052462848ul,
            error_function_significand4 = 3120620288ul,
            error_function_significand5 = 2188969472ul,
            error_function_p_significand = 2279350784ul;
};


struct __attribute__ ((visibility("internal"))) s3p_float64_t {
    typedef float64_numeric_value_tag value_category;
    typedef s3p_float64_value share_type;
    typedef s3p_uint8_t sign_type;
    typedef s3p_uint64_t significand_type;
    // TODO Not nice
    typedef s3p_xor_uint64_t bitwise_significand_type;
    // TODO Not nice
    typedef s3p_xor_uint16_t bitwise_exponent_type;
    typedef s3p_uint16_t exponent_type;
    typedef sf_float64 public_type;
    static constexpr uint8_t heap_type_id = 0xdu;
    static constexpr size_t num_of_bits = 64u;
    static constexpr size_t log_of_bits = 6u;
    // Originally, bias was 2**15 - 1,
    // but we needed to compare exponents in the erf protocol
    // and current a3p comparisons can't compare the highest bit
    // Bias: 2**14 - 1
    static constexpr size_t bias = (1u << 14u) - 1u;
    // zero_exp: 2**14 - 1022u
    // See comment about bias
    static constexpr size_t zero_exp = bias - 1022u;
    // This is used in public multiplication and division
    static constexpr size_t exp_range = 2048u;
    static constexpr size_t sine_precision = 5u;
    static constexpr size_t natural_log_precision = 6u;
    static constexpr size_t square_root_precision = 32u;
    static constexpr size_t erf_precision = 16u;

    static constexpr significand_type::share_type
            natural_log_ln2_significand = 12786308645202649088ull,
            square_root_sqrt2_significand = 13043817825332781056ull,
            two_over_sqrt_pi_significand = 10407460856758233088ull,
            two_over_sqrt_pi_softfloat = 4607760587169110893ull,
            one_significand = 9223372036854775808ull,
            error_function_significand1 = 9401552532063645696ull,
            error_function_significand2 = 10496076957595420672ull,
            error_function_significand3 = 13110227751540537344ull,
            error_function_significand4 = 13402961771130636288ull,
            error_function_significand5 = 9789737153604446208ull,
            error_function_p_significand = 12085978365049987072ull;
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

template <typename T> struct __attribute__ ((visibility("internal"))) fix_of_float;
template <> struct __attribute__ ((visibility("internal"))) fix_of_float<s3p_float32_t> { typedef s3p_fix32_t type; };
template <> struct __attribute__ ((visibility("internal"))) fix_of_float<s3p_float64_t> { typedef s3p_fix64_t type; };


} /* namespace sharemind */

#endif /* MOD_SHARED3P_EMU_SHARED3PVALUETRAITS_H */
