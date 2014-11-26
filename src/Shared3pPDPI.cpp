/*
 * This file is a part of the Sharemind framework.
 * Copyright (C) Cybernetica AS
 *
 * All rights are reserved. Reproduction in whole or part is prohibited
 * without the written consent of the copyright owner. The usage of this
 * code is subject to the appropriate license agreement.
 */

#include <sharemind/ExecutionProfiler.h>
#include "Facilities/ExecutionModelEvaluator.h"
#include "Shared3pPDPI.h"


namespace sharemind {

Shared3pPDPI::Shared3pPDPI(Shared3pPD & pd)
    : m_pd(pd)
    , m_pdConfiguration(pd.configuration())
    , m_modelEvaluator(pd.modelEvaluator())
    , m_profiler(pd.profiler())
{}

} /* namespace sharemind { */
