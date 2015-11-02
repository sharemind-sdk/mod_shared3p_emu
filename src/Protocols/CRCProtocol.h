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

#ifndef MOD_SHARED3P_EMU_PROTOCOLS_CRCPROTOCOL_H
#define MOD_SHARED3P_EMU_PROTOCOLS_CRCPROTOCOL_H

#include "../Shared3pValueTraits.h"
#include "../Shared3pVector.h"


namespace sharemind {

template <CRCMode mode>
struct __attribute__ ((visibility("internal"))) CRCModeInfo {};

template <>
struct __attribute__ ((visibility("internal"))) CRCModeInfo<CRCMode16> {
    typedef s3p_xor_uint16_t value_t;
    typedef ValueTraits<value_t>::share_type share_type;
    enum { POLY = static_cast<share_type>(0x8408) };
    static share_type table[256];
};

template <>
struct __attribute__ ((visibility("internal"))) CRCModeInfo<CRCMode32> {
    typedef s3p_xor_uint32_t value_t;
    typedef ValueTraits<value_t>::share_type share_type;
    enum { POLY = static_cast<share_type>(0xedb88320) };
    static share_type table[256];
};

template <CRCMode mode>
class __attribute__ ((visibility("internal"))) CRCProtocol {};

template <CRCMode mode>
class __attribute__ ((visibility("internal"))) CRCProtocolBase {

public: /* Methods: */

    bool invoke(const ShareVec<s3p_xor_uint8_t> & src,
                typename CRCModeInfo<mode>::share_type & dest)
    {
        typedef typename CRCModeInfo<mode>::share_type share_type;
        share_type crc = ~dest;
        for (size_t i = 0; i < src.size(); ++ i) {
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

#endif /* MOD_SHARED3P_EMU_PROTOCOLS_CRCPROTOCOL_H */
