/*
 * This file is a part of the Sharemind framework.
 * Copyright (C) Cybernetica AS
 *
 * All rights are reserved. Reproduction in whole or part is prohibited
 * without the written consent of the copyright owner. The usage of this
 * code is subject to the appropriate license agreement.
 */

#include <boost/property_tree/ini_parser.hpp>
#include <boost/property_tree/ptree.hpp>
#include <LogHard/Logger.h>
#include "Shared3pConfiguration.h"


namespace pt = boost::property_tree;

namespace sharemind {

Shared3pConfiguration::Shared3pConfiguration(const LogHard::Logger & logger)
    : m_logger(logger)
{}

bool Shared3pConfiguration::load(const std::string & filename) {

    pt::ptree config;

    try {
        pt::read_ini(filename, config);
        m_profilingModelsFilename = config.get<std::string>("ProtectionDomain.ProfilingModels");
    } catch (const pt::ini_parser_error & e) {
        m_logger.error() << "Error while parsing configuration file. "
            << e.message() << " [" << e.filename() << ":"
            << e.line() << "].";
        return false;
    } catch (const pt::ptree_bad_data & e) {
        m_logger.error() << "Error while parsing configuration file. Bad data: "
            << e.what();
        return false;
    } catch (const pt::ptree_bad_path & e) {
        m_logger.error() << "Error while parsing configuration file. Bad path: "
            << e.what();
        return false;
    }

    return true;
}

} /* namespace sharemind { */
