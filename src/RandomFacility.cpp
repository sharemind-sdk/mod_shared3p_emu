/*
 * This file is a part of the Sharemind framework.
 * Copyright (C) Cybernetica AS
 *
 * All rights are reserved. Reproduction in whole or part is prohibited
 * without the written consent of the copyright owner. The usage of this
 * code is subject to the appropriate license agreement.
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
