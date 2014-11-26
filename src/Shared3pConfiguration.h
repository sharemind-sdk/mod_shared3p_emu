/*
 * This file is a part of the Sharemind framework.
 * Copyright (C) Cybernetica AS
 *
 * All rights are reserved. Reproduction in whole or part is prohibited
 * without the written consent of the copyright owner. The usage of this
 * code is subject to the appropriate license agreement.
 */

#ifndef MOD_SHARED3P_EMU_SHARED3PCONFIGURATION_H
#define MOD_SHARED3P_EMU_SHARED3PCONFIGURATION_H

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

    const LogHard::Logger & m_logger;

    std::string m_modelEvaluatorConfiguration;

}; /* class Shared3pConfiguration { */

} /* namespace sharemind { */

#endif /* MOD_SHARED3P_EMU_SHARED3PCONFIGURATION_H */
