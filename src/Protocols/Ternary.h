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
