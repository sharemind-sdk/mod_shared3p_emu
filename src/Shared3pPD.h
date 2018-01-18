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

#ifndef MOD_SHARED3P_EMU_SHARED3PPD_H
#define MOD_SHARED3P_EMU_SHARED3PPD_H

#include <memory>
#include <sharemind/Exception.h>
#include <sharemind/ExceptionMacros.h>
#include "Facilities/CxxRandomEngine.h"


namespace sharemind {

class ExecutionProfiler;
class ExecutionModelEvaluator;
class Shared3pModule;

class __attribute__ ((visibility("internal"))) Shared3pPD {

public: /* Types: */

    SHAREMIND_DECLARE_EXCEPTION_NOINLINE(sharemind::Exception, Exception);
    SHAREMIND_DECLARE_EXCEPTION_CONST_MSG_NOINLINE(Exception,
                                                   ConfigurationException);

public: /* Methods: */

    Shared3pPD(const std::string & pdName,
               const std::string & pdConfiguration,
               Shared3pModule & module);
    ~Shared3pPD() noexcept;

    inline ExecutionModelEvaluator & modelEvaluator() noexcept
    { return *m_modelEvaluator; }

    inline const ExecutionModelEvaluator & modelEvaluator() const noexcept
    { return *m_modelEvaluator; }

    inline ExecutionProfiler & profiler() noexcept
    { return m_profiler; }

    inline const ExecutionProfiler & profiler() const noexcept
    { return m_profiler; }

    inline CxxRandomEngine & rng() noexcept
    { return m_rng; }

    inline const CxxRandomEngine & rng() const noexcept
    { return m_rng; }

    inline const std::string & name() const noexcept
    { return m_name; }

private: /* Fields: */

    std::string m_name;

    ExecutionProfiler & m_profiler;

    std::unique_ptr<ExecutionModelEvaluator> m_modelEvaluator;
    CxxRandomEngine m_rng;

}; /* class Shared3pPD { */

} /* namespace sharemind { */

#endif /* MOD_SHARED3P_EMU_SHARED3PPD_H */
