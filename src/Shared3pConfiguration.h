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

#include <sharemind/Exception.h>
#include <sharemind/ExceptionMacros.h>
#include <string>


namespace sharemind {

class __attribute__ ((visibility("internal"))) Shared3pConfiguration {

public: /* Types: */

    SHAREMIND_DECLARE_EXCEPTION_NOINLINE(sharemind::Exception, Exception);

public: /* Methods: */

    Shared3pConfiguration(std::string const & filename);

    const std::string & modelEvaluatorConfiguration() const noexcept
    { return m_modelEvaluatorConfiguration; }

private: /* Fields: */

    std::string m_modelEvaluatorConfiguration;

}; /* class Shared3pConfiguration { */

} /* namespace sharemind { */

#endif /* MOD_SHARED3P_EMU_SHARED3PCONFIGURATION_H */
