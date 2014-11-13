/*
 * This file is a part of the Sharemind framework.
 * Copyright (C) Cybernetica AS
 *
 * All rights are reserved. Reproduction in whole or part is prohibited
 * without the written consent of the copyright owner. The usage of this
 * code is subject to the appropriate license agreement.
 */

#ifndef MOD_SHARED3P_EMU_SHARED3PPD_H
#define MOD_SHARED3P_EMU_SHARED3PPD_H

#include <sharemind/Exception.h>
#include "Shared3pConfiguration.h"


namespace sharemind {

class Shared3pModule;

class __attribute__ ((visibility("internal"))) Shared3pPD {

public: /* Types: */

    SHAREMIND_DEFINE_EXCEPTION(std::exception, Exception);

    SHAREMIND_DEFINE_EXCEPTION_CONST_MSG(
            Exception,
            ConfigurationException,
            "Error in protection domain configuration!");

public: /* Methods: */

    Shared3pPD(const std::string & pdName,
               const std::string & pdConfiguration,
               Shared3pModule & module);

    inline Shared3pConfiguration & configuration()
    { return m_configuration; }

    inline const Shared3pConfiguration & configuration() const
    { return m_configuration; }

    inline const std::string & name() const { return m_name; }

private: /* Fields: */

    Shared3pConfiguration m_configuration;
    std::string m_name;
}; /* class Shared3pPD { */

} /* namespace sharemind { */

#endif /* MOD_SHARED3P_EMU_SHARED3PPD_H */
