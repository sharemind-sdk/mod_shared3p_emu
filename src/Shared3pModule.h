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

#ifndef MOD_SHARED3P_EMU_SHARED3PMODULE_H
#define MOD_SHARED3P_EMU_SHARED3PMODULE_H

#include <memory>


namespace LogHard {

class Backend;
class Logger;

} /* namespace LogHard { */

namespace sharemind {

class ExecutionProfiler;

class __attribute__ ((visibility("internal"))) Shared3pModule {

public: /* Methods: */

    Shared3pModule(ExecutionProfiler & m_profiler);

    const LogHard::Logger & logger() const noexcept
    { return *m_logger; }

    inline ExecutionProfiler & profiler() noexcept
    { return m_profiler; }

    inline const ExecutionProfiler & profiler() const noexcept
    { return m_profiler; }

private:

    /// \todo Logger and ExecutionProfiler should be in a separate module.
    std::unique_ptr<LogHard::Backend> m_logBackend;
    std::unique_ptr<LogHard::Logger> m_logger;
    ExecutionProfiler & m_profiler;

}; /* class Shared3pModule { */

} /* namespace sharemind { */

#endif /* MOD_SHARED3P_EMU_SHARED3PMODULE_H */
