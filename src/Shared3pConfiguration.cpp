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

#include <boost/filesystem.hpp>
#include <boost/property_tree/ini_parser.hpp>
#include <boost/property_tree/ptree.hpp>
#include <LogHard/Logger.h>
#include "Shared3pConfiguration.h"


namespace fs = boost::filesystem;

namespace sharemind {

Shared3pConfiguration::Shared3pConfiguration(const LogHard::Logger & logger)
    : m_logger(logger)
{}

bool Shared3pConfiguration::load(const std::string & filename) {

    using boost::property_tree::ptree;

    ptree config;

    try {
        boost::property_tree::read_ini(filename, config);
        m_interpolate.addVar("CurrentFileDirectory",
                             fs::canonical(fs::path(filename)).parent_path().string());
        m_modelEvaluatorConfiguration =
            m_interpolate(config.get<std::string>("ProtectionDomain.ModelEvaluatorConfiguration"));
    } catch (const boost::property_tree::ini_parser_error & e) {
        m_logger.error() << "Error while parsing configuration file. "
            << e.message() << " [" << e.filename() << ":"
            << e.line() << "].";
        return false;
    } catch (const boost::property_tree::ptree_bad_data & e) {
        m_logger.error() << "Error while parsing configuration file. Bad data: "
            << e.what();
        return false;
    } catch (const boost::property_tree::ptree_bad_path & e) {
        m_logger.error() << "Error while parsing configuration file. Bad path: "
            << e.what();
        return false;
    }

    return true;
}

} /* namespace sharemind { */
