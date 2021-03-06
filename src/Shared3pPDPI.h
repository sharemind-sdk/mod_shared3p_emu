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

#ifndef MOD_SHARED3P_EMU_SHARED3PPDPI_H
#define MOD_SHARED3P_EMU_SHARED3PPDPI_H

#include <sharemind/SharedValueHeap.h>

#include "Shared3pPD.h"
#include "Shared3pVector.h"

namespace sharemind {

class ExecutionModelEvaluator;
class CxxRandomEngine;
class Shared3pConfiguration;

class __attribute__ ((visibility("internal"))) Shared3pPDPI {

public: /* Methods: */

    Shared3pPDPI(Shared3pPD & pd);
    inline ~Shared3pPDPI() noexcept {}

    inline const std::string & pdName() const noexcept
    { return m_pd.name(); }

    inline ExecutionModelEvaluator & modelEvaluator() noexcept
    { return m_modelEvaluator; }

    inline const ExecutionModelEvaluator & modelEvaluator() const noexcept
    { return m_modelEvaluator; }

    inline CxxRandomEngine & rng() noexcept
    { return m_rng; }

    inline const CxxRandomEngine & rng() const noexcept
    { return m_rng; }

    template <typename T>
    inline bool isValidHandle(void * hndl) const {
        return m_heap.check<T>(hndl);
    }

    template <typename T>
    inline bool registerVector(ShareVec<T> * vec) {
        return m_heap.insert(vec);
    }

    template <typename T>
    inline bool freeRegisteredVector(ShareVec<T> * vec) {
        return m_heap.erase(vec);
    }

private: /* Fields: */

    Shared3pPD & m_pd;
    ExecutionModelEvaluator & m_modelEvaluator;
    CxxRandomEngine & m_rng;
    SharedValueHeap m_heap;

}; /* class Shared3pPDPI { */

} /* namespace sharemind { */

#endif /* MOD_SHARED3P_EMU_SHARED3PPDPI_H */
