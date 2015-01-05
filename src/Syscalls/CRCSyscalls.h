/*
 * This file is a part of the Sharemind framework.
 * Copyright (C) Cybernetica AS
 *
 * All rights are reserved. Reproduction in whole or part is prohibited
 * without the written consent of the copyright owner. The usage of this
 * code is subject to the appropriate license agreement.
 */

#ifndef MOD_ADDITIVE3P_SYSCALLS_CRCSYSCALLS_H
#define MOD_ADDITIVE3P_SYSCALLS_CRCSYSCALLS_H

#include <sharemind/libmodapi/api_0x1.h>
#include "Common.h"

namespace sharemind {

enum CRCMode {
    CRCMode16,
    CRCMode32
};

/**
 * SysCall: crc_xor_vec
 * Args:
 *      0) uint64[0]     pd index
 *      1) p[0]          input text vector handle
 *      2) p[0]          output handle
 */
template <CRCMode mode>
NAMED_SYSCALL(crc_xor_vec, name, args, num_args, refs, crefs, returnValue, c)
    __attribute__ ((visibility("internal")));

} /* namespace sharemind */

#endif /* MOD_ADDITIVE3P_SYSCALLS_CRCSYSCALLS_H */
