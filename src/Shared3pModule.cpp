/*
 * This file is a part of the Sharemind framework.
 * Copyright (C) Cybernetica AS
 *
 * All rights are reserved. Reproduction in whole or part is prohibited
 * without the written consent of the copyright owner. The usage of this
 * code is subject to the appropriate license agreement.
 */

#include <LogHard/Backend.h>
#include <LogHard/Logger.h>
#include <LogHard/Priority.h>
#include <sharemind/Exception.h>
#include <sharemind/ExecutionProfiler.h>
#include "Shared3pModule.h"


namespace {

class StdErrAppender: public LogHard::Backend::Appender {

private: /* Types: */

    using Appender = LogHard::Backend::Appender;
    using Appenders = LogHard::Backend::Appenders;
    using CFileAppender = LogHard::Backend::CFileAppender;
    using Priority = LogHard::Priority;

public: /* Types: */

    SHAREMIND_DEFINE_EXCEPTION(std::exception, Exception);
    SHAREMIND_DEFINE_EXCEPTION_CONST_MSG(
            Exception,
            MultipleStdErrAppenderException,
            "Multiple Std Err appenders not allowed!");

public: /* Methods: */

    void activate(const Appenders & appenders) override {
        for (Appender * const a : appenders)
            if (a != this && dynamic_cast<StdErrAppender *>(a) != nullptr)
                throw MultipleStdErrAppenderException();
    }

    inline void log(timeval time,
                    const Priority priority,
                    const char * message) noexcept override
    {
        CFileAppender::logToFile(STDERR_FILENO, time, priority, message,
                                 m_mutex);
    }

    sharemind::QueueingMutex m_mutex;

}; /* class StdErrAppender { */

} /* namespace { */

namespace sharemind {

Shared3pModule::Shared3pModule(ExecutionProfiler & profiler) : m_profiler(profiler) {
    m_logBackend.reset(new LogHard::Backend());
    m_logBackend->addAppender(new StdErrAppender());
    m_logger.reset(new LogHard::Logger(*m_logBackend));
    m_profiler.startLog("emulator-profile.csv");
}

} /* namespace sharemind { */
