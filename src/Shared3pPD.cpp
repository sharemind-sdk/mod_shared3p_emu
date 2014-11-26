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
#include "Shared3pModule.h"
#include "Shared3pPD.h"


namespace sharemind {

Shared3pPD::Shared3pPD(const std::string & pdName,
                       const std::string & pdConfiguration,
                       Shared3pModule & module)
    : m_configuration(module.logger())
    , m_name(pdName)
    , m_profiler(module.profiler())
{
    if (!m_configuration.load(pdConfiguration))
        throw ConfigurationException();

    try {
        m_modelEvaluator.reset(
                new ExecutionModelEvaluator(module.logger(),
                    m_configuration.modelEvaluatorConfiguration()));
    } catch (const ExecutionModelEvaluator::ConfigurationException &) {
        throw ConfigurationException();
    }
}

Shared3pPD::~Shared3pPD() = default;

} /* namespace sharemind { */
