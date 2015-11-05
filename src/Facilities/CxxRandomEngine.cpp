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

#include <cassert>
#include <random>

#include "CxxRandomEngine.h"

using namespace sharemind;

namespace /* anonymouse */ {

extern "C" {
void CxxRandomEngine_fill_bytes(SharemindRandomEngine* rng_, void * memptr_, size_t size);
void CxxRandomEngine_free(SharemindRandomEngine* rng_);
}

class CxxRandomEngine : public SharemindRandomEngine {
public: /* Methods */

    inline CxxRandomEngine()
        : SharemindRandomEngine {
              CxxRandomEngine_fill_bytes,
              CxxRandomEngine_free
          }
    {
        std::random_device rd;
        m_rng.seed(rd());
    }

    inline static CxxRandomEngine& fromWrapper(SharemindRandomEngine& base) noexcept {
        return static_cast<CxxRandomEngine&>(base);
    }

public: /* Fields */
    std::default_random_engine m_rng;
    std::uniform_int_distribution<uint8_t> m_dist;
}; /* class CxxRandomEngine { */

extern "C"
void CxxRandomEngine_fill_bytes(SharemindRandomEngine* rng_, void * memptr_, size_t size) {
    assert (rng_ != nullptr);
    auto& rng = CxxRandomEngine::fromWrapper(*rng_).m_rng;
    auto& dist = CxxRandomEngine::fromWrapper(*rng_).m_dist;
    auto mem = static_cast<uint8_t*>(memptr_);
    for (size_t i = 0; i < size; ++i) {
        mem[i] = dist(rng);
    }
}

extern "C"
void CxxRandomEngine_free(SharemindRandomEngine* rng_) {
    if (rng_ != nullptr) {
        delete &CxxRandomEngine::fromWrapper(*rng_);
    }
}

} /* namespace anonymous { */

namespace sharemind {

SharemindRandomEngine* make_cxx_random_engine() {
    return new CxxRandomEngine {};
}

} /* namespace sharemind { */
