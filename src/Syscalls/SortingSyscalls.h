/*
 * Copyright (C) 2017 Cybernetica
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

#ifndef MOD_SHARED3P_EMU_SYSCALLS_SORTINGSYSCALLS_H
#define MOD_SHARED3P_EMU_SYSCALLS_SORTINGSYSCALLS_H

#include <sharemind/module-apis/api_0x1.h>
#include "Common.h"

namespace sharemind {

/**
 * Syscall: stable_sort
 * Args:
 *      0) uint64[0] pd index
 *      1) p[0] input vector handle
 *      2) p[0] index vector handle
 *      3) uint8[0] direction (true = ascending)
 * Refs:
 *      0) permutation vector handle
 */
template<typename T>
NAMED_SYSCALL(stable_sort, name, args, num_args, refs, crefs, returnValue, c)
    __attribute__ ((visibility("internal")));

} /* namespace sharemind */

#endif /* MOD_SHARED3P_EMU_SYSCALLS_SORTINGSYSCALLS_H */
