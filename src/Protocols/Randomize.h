/*
 * This file is a part of the Sharemind framework.
 * Copyright (C) Cybernetica AS
 *
 * All rights are reserved. Reproduction in whole or part is prohibited
 * without the written consent of the copyright owner. The usage of this
 * code is subject to the appropriate license agreement.
 */

#ifndef MOD_SHARED3P_EMU_PROTOCOLS_RANDOMIZE_H
#define MOD_SHARED3P_EMU_PROTOCOLS_RANDOMIZE_H

#include <random>
#include <algorithm>

namespace sharemind {

class __attribute__ ((visibility("internal"))) RandomizeProtocol {
public: /* Methods: */

    RandomizeProtocol (Shared3pPDPI &pdpi)
        : m_pdpi(pdpi)
    { }

    template <typename T>
    bool invoke (s3p_vec<T>& result, any_value_tag = typename value_traits<T>::value_category ()) {
        result.randomize(m_pdpi.rng());
        return true;
    }

private: /* Fields: */

    Shared3pPDPI & m_pdpi;

}; /* class RandomizeProtocol { */

} /* namespace sharemind { */

#endif /* MOD_SHARED3P_EMU_PROTOCOLS_RANDOMIZE_H */
