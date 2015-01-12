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
}

} /* namespace sharemind { */
