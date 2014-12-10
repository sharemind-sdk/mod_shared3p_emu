/*
 * This file is a part of the Sharemind framework.
 * Copyright (C) Cybernetica AS
 *
 * All rights are reserved. Reproduction in whole or part is prohibited
 * without the written consent of the copyright owner. The usage of this
 * code is subject to the appropriate license agreement.
 */

#include "CRCProtocol.h"

namespace sharemind {

namespace /* anonymous */ {

template <CRCMode mode>
void init_crc_table (typename CRCModeInfo<mode>::share_type table[256]) {
    typedef typename CRCModeInfo<mode>::share_type share_type;
    for (size_t n = 0; n < 256; ++ n) {
        share_type c = static_cast<share_type>(n);
        for (size_t k = 0; k < 8; ++ k) {
            if (c & 1)
                c = CRCModeInfo<mode>::POLY ^ (c >> 1);
            else
                c = c >> 1;
        }

        table[n] = c;
    }
}

class CRCTableInitializer {
private: /* Methods: */

    CRCTableInitializer () {
        init_crc_table<CRCMode16>(CRCModeInfo<CRCMode16>::table);
        init_crc_table<CRCMode32>(CRCModeInfo<CRCMode32>::table);
    }

private: /* Fields: */
    static CRCTableInitializer m_instance;
};

CRCTableInitializer CRCTableInitializer::m_instance;

} // namespace anonymous

typename CRCModeInfo<CRCMode16>::share_type CRCModeInfo<CRCMode16>::table[256];
typename CRCModeInfo<CRCMode32>::share_type CRCModeInfo<CRCMode32>::table[256];

} // namespace sharemind
