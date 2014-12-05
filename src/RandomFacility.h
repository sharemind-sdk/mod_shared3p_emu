/*
 * This file is a part of the Sharemind framework.
 * Copyright (C) Cybernetica AS
 *
 * All rights are reserved. Reproduction in whole or part is prohibited
 * without the written consent of the copyright owner. The usage of this
 * code is subject to the appropriate license agreement.
 */

#ifndef MOD_SHARED3P_EMU_RANDOMFACILITY_H
#define MOD_SHARED3P_EMU_RANDOMFACILITY_H

#include <random>
#include <sharemind/Random/IRandom.h>

namespace sharemind {

class RandomFacility : public IRandom {
public: /* Methods */

    virtual void Seed() noexcept;

    virtual void Seed (const void* memptr, size_t size) noexcept;

    virtual void fillBytes (void* memptr, size_t size) noexcept;

private: /* Fields */
    std::default_random_engine m_rng;
    std::uniform_int_distribution<uint8_t> m_dist;

}; /* class RandomFacility { */

} /* namespace sharemind { */

#endif /* MOD_SHARED3P_EMU_RANDOMFACILITY_H */
