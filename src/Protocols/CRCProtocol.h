/*
 * This file is a part of the Sharemind framework.
 * Copyright (C) Cybernetica AS
 *
 * All rights are reserved. Reproduction in whole or part is prohibited
 * without the written consent of the copyright owner. The usage of this
 * code is subject to the appropriate license agreement.
 */

#ifndef MOD_ADDITIVE3P_CRCPROTOCOL_H
#define MOD_ADDITIVE3P_CRCPROTOCOL_H

#include "../Shared3pValueTraits.h"
#include "../Shared3pVector.h"

namespace sharemind {

template <CRCMode mode> struct __attribute__ ((visibility("internal"))) CRCModeInfo {};

template <> struct __attribute__ ((visibility("internal"))) CRCModeInfo<CRCMode16> {
    typedef s3p_xor_uint16_t value_t;
    typedef value_traits<value_t>::share_type share_type;
    enum { POLY = static_cast<share_type>(0x8408) };
    static share_type table[256];
};

template <> struct __attribute__ ((visibility("internal"))) CRCModeInfo<CRCMode32> {
    typedef s3p_xor_uint32_t value_t;
    typedef value_traits<value_t>::share_type share_type;
    enum { POLY = static_cast<share_type>(0xedb88320) };
    static share_type table[256];
};

template <CRCMode mode>
class __attribute__ ((visibility("internal"))) CRCProtocol {};

template <CRCMode mode>
class __attribute__ ((visibility("internal"))) CRCProtocolBase {

public: /* Methods: */

    bool invoke (const s3p_vec<s3p_xor_uint8_t>& src, typename CRCModeInfo<mode>::share_type& dest) {
        typedef typename CRCModeInfo<mode>::share_type share_type;
        share_type crc = ~dest;
        for (size_t i = 0; i < src.size (); ++ i) {
            crc = (crc >> 8) ^ CRCModeInfo<mode>::table[(crc ^ static_cast<share_type>(src[i])) & 0xff];
        }

        dest = ~crc;
        return true;
    }
}; /* class CRCProtocolBase { */

template <>
class __attribute__ ((visibility("internal"))) CRCProtocol<CRCMode16>
    : public CRCProtocolBase<CRCMode16>
{

public: /* Methods: */

}; /* class CRCProtocol<CRCMode16> { */

template <>
class __attribute__ ((visibility("internal"))) CRCProtocol<CRCMode32>
    : public CRCProtocolBase<CRCMode32>
{

public: /* Methods: */

}; /* class CRCProtocol<CRCMode32> { */


} /* namespace sharemind */

#endif /* MOD_ADDITIVE3P_CRCPROTOCOL_H */
