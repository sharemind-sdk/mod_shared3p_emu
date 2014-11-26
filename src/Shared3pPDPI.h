/*
 * This file is a part of the Sharemind framework.
 * Copyright (C) Cybernetica AS
 *
 * All rights are reserved. Reproduction in whole or part is prohibited
 * without the written consent of the copyright owner. The usage of this
 * code is subject to the appropriate license agreement.
 */

#ifndef MOD_SHARED3P_EMU_SHARED3PPDPI_H
#define MOD_SHARED3P_EMU_SHARED3PPDPI_H

#include "Shared3pPD.h"
#include "Shared3pValueHeap.h"
#include "Shared3pVector.h"


namespace sharemind {

class ExecutionModelEvaluator;
class ExecutionProfiler;
class Shared3pConfiguration;

class __attribute__ ((visibility("internal"))) Shared3pPDPI {

public: /* Methods: */

    Shared3pPDPI(Shared3pPD & pd);

    inline const std::string & pdName() const
    { return m_pd.name(); }

    inline const Shared3pConfiguration & pdConfiguration() const
    { return m_pd.configuration(); }

    inline ExecutionModelEvaluator & modelEvaluator()
    { return m_modelEvaluator; }

    inline const ExecutionModelEvaluator & modelEvaluator() const
    { return m_modelEvaluator; }

    inline ExecutionProfiler & profiler()
    { return m_profiler; }

    inline const ExecutionProfiler & profiler() const
    { return m_profiler; }

    template <typename T>
    inline bool isValidHandle(void * hndl) const {
        return m_heap.check<T>(hndl);
    }

    template <typename T>
    inline bool registerVector(s3p_vec<T> * vec) {
        return m_heap.insert(vec);
    }

    template <typename T>
    inline bool freeRegisteredVector(s3p_vec<T> * vec) {
        return m_heap.erase(vec);
    }

private: /* Fields: */

    Shared3pPD & m_pd;
    Shared3pConfiguration & m_pdConfiguration;

    ExecutionModelEvaluator & m_modelEvaluator;
    ExecutionProfiler & m_profiler;

    s3p_heap m_heap;

}; /* class Shared3pPDPI { */

} /* namespace sharemind { */

#endif /* MOD_SHARED3P_EMU_SHARED3PPDPI_H */
