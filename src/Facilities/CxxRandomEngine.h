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

#ifndef MOD_SHARED3P_EMU_CXXRANDOMENGINE_H
#define MOD_SHARED3P_EMU_CXXRANDOMENGINE_H

#include <random>
#include <sharemind/Random/IRandom.h>

namespace sharemind {

class CxxRandomEngine : public IRandom {
public: /* Methods */

    virtual void Seed() noexcept;

    virtual void Seed (const void* memptr, size_t size) noexcept;

    virtual void fillBytes (void* memptr, size_t size) noexcept;

private: /* Fields */

    std::default_random_engine m_rng;
    std::uniform_int_distribution<uint8_t> m_dist;

}; /* class CxxRandomEngine { */

} /* namespace sharemind { */

#endif /* MOD_SHARED3P_EMU_RANDOMFACILITY_H */
