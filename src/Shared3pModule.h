/*
 * This file is a part of the Sharemind framework.
 * Copyright (C) Cybernetica AS
 *
 * All rights are reserved. Reproduction in whole or part is prohibited
 * without the written consent of the copyright owner. The usage of this
 * code is subject to the appropriate license agreement.
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

    Shared3pModule();

    const LogHard::Logger & logger() const noexcept
    { return *m_logger; }

    inline ExecutionProfiler & profiler() noexcept
    { return *m_profiler; }

    inline const ExecutionProfiler & profiler() const noexcept
    { return *m_profiler; }

private:

    /// \todo Logger and ExecutionProfiler should be in a separate module.
    std::unique_ptr<LogHard::Backend> m_logBackend;
    std::unique_ptr<LogHard::Logger> m_logger;
    std::unique_ptr<ExecutionProfiler> m_profiler;

}; /* class Shared3pModule { */

} /* namespace sharemind { */

#endif /* MOD_SHARED3P_EMU_SHARED3PMODULE_H */
