/*
 * This file is a part of the Sharemind framework.
 * Copyright (C) Cybernetica AS
 *
 * All rights are reserved. Reproduction in whole or part is prohibited
 * without the written consent of the copyright owner. The usage of this
 * code is subject to the appropriate license agreement.
 */

#ifndef MOD_SHARED3P_EMU_SECRETSHARING_H
#define MOD_SHARED3P_EMU_SECRETSHARING_H

/*
 * There are no shares in mod_shared3p_emu but in order to be
 * consistent with similar classes in mod_shared3p and
 * mod_shared3p_dev this class is named SecretSharing.
 */

#include "Shared3pValueTraits.h"

namespace sharemind {

class __attribute__ ((visibility("internal"))) SecretSharing {

public: /* Methods: */

    template <typename Share>
    static Share combine (const Share& value, const Share& share, numeric_value_tag) {
        return value + share;
    }

    static bool combine (bool value, bool share, bool_value_tag) {
        return value ^ share;
    }

    template <typename Share>
    static Share combine (const Share& value, const Share& share, xored_numeric_value_tag) {
        return value ^ share;
    }

    template <typename Share>
    static Share inverse (const Share& value, numeric_value_tag) { return 0 - value; }

    template <typename Share>
    static Share inverse (const Share& value, bool_value_tag) { return value; }

    template <typename Share>
    static Share inverse (const Share& value, xored_numeric_value_tag) { return value; }

}; /* class SecretSharing { */

} /* namespace sharemind { */

#endif // MOD_SHARED3P_EMU_SECRETSHARING_H
