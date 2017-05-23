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

#ifndef MOD_SHARED3P_EMU_SHARED3PCONFIGURATION_H
#define MOD_SHARED3P_EMU_SHARED3PCONFIGURATION_H

#include <sharemind/ConfigurationInterpolation.h>
#include <string>


namespace LogHard { class Logger; }

namespace sharemind {

class __attribute__ ((visibility("internal"))) Shared3pConfiguration {

public: /* Methods: */

    Shared3pConfiguration(const LogHard::Logger & logger);

    bool load(const std::string & filename);

    const std::string & modelEvaluatorConfiguration() const noexcept
    { return m_modelEvaluatorConfiguration; }

private: /* Fields: */

    ConfigurationInterpolation m_interpolate;
    const LogHard::Logger & m_logger;
    std::string m_modelEvaluatorConfiguration;

}; /* class Shared3pConfiguration { */

} /* namespace sharemind { */

#endif /* MOD_SHARED3P_EMU_SHARED3PCONFIGURATION_H */
