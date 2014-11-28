/*
 * This file is a part of the Sharemind framework.
 * Copyright (C) Cybernetica AS
 *
 * All rights are reserved. Reproduction in whole or part is prohibited
 * without the written consent of the copyright owner. The usage of this
 * code is subject to the appropriate license agreement.
 */

#ifndef MOD_SHARED3P_EMU_EXECUTIONMODELEVALUATOR_H
#define MOD_SHARED3P_EMU_EXECUTIONMODELEVALUATOR_H

#include <limits>
#include <sharemind/Exception.h>
#include <sharemind/ScopedObjectMap.h>


namespace LogHard { class Logger; }

namespace sharemind {

/// \todo This class should be moved to a separate facility module
class ExecutionModelEvaluator {

public: /* Types: */

    SHAREMIND_DEFINE_EXCEPTION(std::exception, Exception);
    SHAREMIND_DEFINE_EXCEPTION_CONST_MSG(
            Exception,
            ConfigurationException,
            "Error in profiler model loader configuration!");

    class Model {

    public: /* Types: */

        SHAREMIND_DEFINE_EXCEPTION(std::exception, Exception);
        SHAREMIND_DEFINE_EXCEPTION_CONST_MSG(
                Exception,
                LossOfPrecisionException,
                "Cannot cast input value without loss of precision!");
        SHAREMIND_DEFINE_EXCEPTION_CONST_MSG(
                Exception,
                ExpressionCompileException,
                "Failed to compile expression!");

    public: /* Methods: */

        inline virtual ~Model() noexcept {}
        virtual double evaluate(size_t inputSize) const = 0;

    }; /* class Model { */

private: /* Types: */

    using ModelMap = ScopedObjectMap<std::string, Model>;
    using ModelTypeMap = ScopedObjectMap<std::string, ModelMap>;

public: /* Methods: */

    ExecutionModelEvaluator(const LogHard::Logger & logger,
                        const std::string & configFilename);

    inline Model * model(const std::string & modelType,
                         const std::string & modelName) const
    {
        const ModelMap * const models = m_modelTypes.maybeAt(modelType);
        return models ? models->maybeAt(modelName) : nullptr;
    }

private: /* Fields: */

    ModelTypeMap m_modelTypes;

}; /* class ExecutionModelEvaluator { */

} /* namespace sharemind { */

#endif /* MOD_SHARED3P_EMU_EXECUTIONMODELEVALUATOR_H */
