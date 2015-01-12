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

#include <random>
#include "RandomFacility.h"

namespace sharemind {

void RandomFacility::Seed () noexcept {
    std::random_device rd;
    m_rng.seed(rd());
}

void RandomFacility::Seed (const void* memptr, size_t size) noexcept {
    const uint8_t *mem = static_cast<const uint8_t*>(memptr);
    std::seed_seq seed(mem, mem + size);
    m_rng.seed(seed);
}

void RandomFacility::fillBytes (void* memptr, size_t size) noexcept {
    uint8_t *mem = static_cast<uint8_t*>(memptr);
    for (size_t i = 0; i < size; ++i) {
        mem[i] = m_dist(m_rng);
    }
}

} /* namespace sharemind { */
