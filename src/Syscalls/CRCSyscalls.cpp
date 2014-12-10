/*
 * This file is a part of the Sharemind framework.
 * Copyright (C) Cybernetica AS
 *
 * All rights are reserved. Reproduction in whole or part is prohibited
 * without the written consent of the copyright owner. The usage of this
 * code is subject to the appropriate license agreement.
 */

#include "CRCSyscalls.h"

#include "Common.h"
#include "../Shared3pPDPI.h"
#include "../Protocols/CRCProtocol.h"

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

} /* namespace anonymous */


CRCModeInfo<CRCMode16>::share_type CRCModeInfo<CRCMode16>::table[256];
CRCModeInfo<CRCMode32>::share_type CRCModeInfo<CRCMode32>::table[256];

template <CRCMode mode>
SHAREMIND_MODULE_API_0x1_SYSCALL(crc_xor_vec,
                                 args, num_args, refs, crefs,
                                 returnValue, c)
{
    VMHandles handles;
    if (!SyscallArgs<3>::check(num_args, refs, crefs, returnValue) ||
        !handles.get(c, args))
    {
        return SHAREMIND_MODULE_API_0x1_INVALID_CALL;
    }

    try {
        Shared3pPDPI & pdpi =
                *static_cast<Shared3pPDPI *>(handles.pdpiHandle);

        void * inputVecHandle = args[1].p[0];
        void * outputHandle = args[2].p[0];
        typedef typename CRCModeInfo<mode>::value_t value_t;

        if (!pdpi.isValidHandle<s3p_xor_uint8_t>(inputVecHandle) ||
            !pdpi.isValidHandle<value_t>(outputHandle))
        {
            return SHAREMIND_MODULE_API_0x1_GENERAL_ERROR;
        }

        const s3p_vec<s3p_xor_uint8_t> & inputVec = *static_cast<s3p_vec<s3p_xor_uint8_t> *>(inputVecHandle);
        s3p_vec<value_t> & output = *static_cast<s3p_vec<value_t> *>(outputHandle);
        if (output.size () != 1) {
            return SHAREMIND_MODULE_API_0x1_INVALID_CALL;
        }

        if (! CRCProtocol<mode>().invoke (inputVec, output[0])) {
            return SHAREMIND_MODULE_API_0x1_GENERAL_ERROR;
        }

        return SHAREMIND_MODULE_API_0x1_OK;
    } catch (const std::bad_alloc &) {
        return SHAREMIND_MODULE_API_0x1_OUT_OF_MEMORY;
    } catch (...) {
        return SHAREMIND_MODULE_API_0x1_GENERAL_ERROR;
    }
    return SHAREMIND_MODULE_API_0x1_GENERAL_ERROR;
}

/*
 * Explicitly instantiate system calls:
 */

template SHAREMIND_MODULE_API_0x1_SYSCALL(crc_xor_vec<CRCMode16>,
                                          args, num_args, refs, crefs,
                                          returnValue, c);

template SHAREMIND_MODULE_API_0x1_SYSCALL(crc_xor_vec<CRCMode32>,
                                          args, num_args, refs, crefs,
                                          returnValue, c);


} /* namespace sharemind */
