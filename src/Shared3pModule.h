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

#include <LogHard/Backend.h>
#include <LogHard/Logger.h>
#include <memory>


namespace sharemind {

class __attribute__ ((visibility("internal"))) Shared3pModule {

public: /* Methods: */

    Shared3pModule();

    const LogHard::Logger & logger() const noexcept
    { return *m_logger; }

private:

    /// \todo Actually, we need to use a single logger over all the pd's
    /// \todo Also, a logger may be unnecessary anyways, just log to stderr
    std::unique_ptr<LogHard::Backend> m_logBackend;
    std::unique_ptr<LogHard::Logger> m_logger;

}; /* class Shared3pModule { */

} /* namespace sharemind { */

#endif /* MOD_SHARED3P_EMU_SHARED3PMODULE_H */
