/*
 * Copyright (C) 2018 Cybernetica
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

#ifndef MOD_SHARED3P_EMU_SYSCALLS_CARTERWEGMANSYSCALL_H
#define MOD_SHARED3P_EMU_SYSCALLS_CARTERWEGMANSYSCALL_H

#include <sharemind/module-apis/api_0x1.h>
#include "Common.h"

namespace sharemind {

/**
 * Syscall: carter_wegman128
 * Args:
 *      0) uint64[0] pd index
 *      1) p[0] key vector handle
 *      2) p[0] data vector handle
 *      3) p[0] result vector handle
 */
NAMED_SYSCALL(carter_wegman128, name, args, num_args, refs, crefs, returnValue, c)
    __attribute__ ((visibility("internal")));

} /* namespace sharemind */

#endif /* MOD_SHARED3P_EMU_SYSCALLS_CARTERWEGMANSYSCALL_H */
