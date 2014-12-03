/*
 * This file is a part of the Sharemind framework.
 * Copyright (C) Cybernetica AS
 *
 * All rights are reserved. Reproduction in whole or part is prohibited
 * without the written consent of the copyright owner. The usage of this
 * code is subject to the appropriate license agreement.
 */

#ifndef MOD_SHARED3P_EMU_PROTOCOLS_TERNARY_H
#define MOD_SHARED3P_EMU_PROTOCOLS_TERNARY_H

#include "../Shared3pValueTraits.h"


namespace sharemind {

class __attribute__ ((visibility("internal"))) ObliviousChoiceProtocol {
public: /* Methods: */

    ObliviousChoiceProtocol(Shared3pPDPI & pdpi) { (void) pdpi; }

    template <typename T>
    bool invoke(const s3p_vec<s3p_bool_t> & param1,
                const s3p_vec<T> & param2,
                const s3p_vec<T> & param3,
                s3p_vec<T> & result,
                any_value_tag)
    {
        if (param1.size() != param2.size() ||
                param1.size() != param3.size() ||
                param1.size() != result.size())
        {
            return false;
        }

        for (size_t i = 0u; i < param1.size(); ++i)
            result[i] = param1[i] ? param2[i] : param3[i];

        return true;
    }

}; /* class ObliviousChoiceProtocol { */

} /* namespace sharemind { */

#endif /* MOD_SHARED3P_EMU_PROTOCOLS_TERNARY_H */
