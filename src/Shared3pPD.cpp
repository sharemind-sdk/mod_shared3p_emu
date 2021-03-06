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
#include "Shared3pConfiguration.h"
#include "Shared3pModule.h"
#include "Shared3pPD.h"


namespace sharemind {

SHAREMIND_DEFINE_EXCEPTION_NOINLINE(sharemind::Exception,
                                    Shared3pPD::,
                                    Exception);
SHAREMIND_DEFINE_EXCEPTION_CONST_MSG_NOINLINE(
        Exception,
        Shared3pPD::,
        ConfigurationException,
        "Error in protection domain configuration!");


Shared3pPD::Shared3pPD(const std::string & pdName,
                       const std::string & pdConfiguration,
                       Shared3pModule & module)
    : m_name(pdName)
{
    try {
        Shared3pConfiguration const config(pdConfiguration);
        m_modelEvaluator =
                std::make_unique<ExecutionModelEvaluator>(
                    module.logger(),
                    config.modelEvaluatorConfiguration());
    } catch (Shared3pConfiguration::ConfigurationException const &) {
        std::throw_with_nested(ConfigurationException());
    } catch (ExecutionModelEvaluator::ConfigurationException const &) {
        std::throw_with_nested(ConfigurationException());
    }
}

Shared3pPD::~Shared3pPD() noexcept = default;

} /* namespace sharemind { */
