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

#include <cassert>
#include <random>

namespace sharemind {

class CxxRandomEngine {
public: /* Methods: */

    inline CxxRandomEngine() {
        std::random_device rd;
        m_rng.seed(rd());
    }

    CxxRandomEngine(const CxxRandomEngine &) = delete;
    CxxRandomEngine & operator=(const CxxRandomEngine &) = delete;
    CxxRandomEngine(CxxRandomEngine && other) = default;
    CxxRandomEngine & operator=(CxxRandomEngine && other) = default;

    void fillBytes(void * memptr, size_t size) noexcept {
        auto mem = static_cast<uint8_t *>(memptr);
        for (size_t i = 0; i < size; ++i) {
            mem[i] = m_dist(m_rng);
        }
    }

    template <typename T>
    inline void fillBlock(T * begin, T * end) noexcept {
        assert (begin <= end);
        if (begin < end) {
            fillBytes(begin, sizeof(T) * (end - begin));
        }
    }

    template <typename T>
    inline T randomValue() noexcept(noexcept(T(T()))) {
        T value;
        fillBytes(&value, sizeof(T));
        return value;
    }

private: /* Fields: */
    std::default_random_engine m_rng;
    std::uniform_int_distribution<uint8_t> m_dist;

}; /* class CxxRandomEngine { */

} /* namespace sharemind { */

#endif /* MOD_SHARED3P_EMU_CXXRANDOMENGINE_H */
