/*
 * This file is a part of the Sharemind framework.
 * Copyright (C) Cybernetica AS
 *
 * All rights are reserved. Reproduction in whole or part is prohibited
 * without the written consent of the copyright owner. The usage of this
 * code is subject to the appropriate license agreement.
 */

#ifndef MOD_SHARED3P_EMU_PROTOCOLHELPERMACROS_H
#define MOD_SHARED3P_EMU_PROTOCOLHELPERMACROS_H

#include <sharemind/likely.h>
#include <sharemind/visibility.h>

/**
 * Evaluates a boolean expression. Returns false from the current function if the expression evaluates to false.
 */
#define GUARD(expr) do { if (unlikely (! (expr))) { return false; } } while (0)

/**
 * Macro to iterate code over [0, sizeExpr) using variable VAR.
 * \param sizeExpr An expression of type size_t, will only be evaluated once.
 * \param VAR A variable to iterate over with.
 */
#define CYCLE(VAR,sizeExpr) \
    if (const auto & sizeExpr_ = sharemind::impl::auto_any<size_t>(sizeExpr)) {} else \
    for (size_t VAR = 0; (VAR) < sizeExpr_.m_value; ++ (VAR))


#endif /* MOD_SHARED3P_EMU_PROTOCOLHELPERMACROS_H */
