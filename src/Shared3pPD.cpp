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

#include <sharemind/ExecutionModelEvaluator.h>
#include <sharemind/ExecutionProfiler.h>
#include <sharemind/MakeUnique.h>
#include "Shared3pConfiguration.h"
#include "Shared3pModule.h"
#include "Shared3pPD.h"


namespace sharemind {

Shared3pPD::Shared3pPD(const std::string & pdName,
                       const std::string & pdConfiguration,
                       Shared3pModule & module)
    : m_name(pdName)
    , m_profiler(module.profiler())
{
    try {
        Shared3pConfiguration const config(pdConfiguration);
        m_modelEvaluator =
                makeUnique<ExecutionModelEvaluator>(
                    module.logger(),
                    config.modelEvaluatorConfiguration());
    } catch (ExecutionModelEvaluator::ConfigurationException const &) {
        throw ConfigurationException();
    }
}

Shared3pPD::~Shared3pPD() noexcept = default;

} /* namespace sharemind { */
